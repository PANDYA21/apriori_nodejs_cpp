const cluster = require('cluster');
const numCPUs = require('os').cpus().length - 1;
const addon = require('./build/Release/addon');
const _ = require('lodash');
const measures = require('./interestingmeasures');
const fs = require('fs');


function mineOriginal(transactions, options) {
  const antecedent = options.antecedent;
  const sortingMeasure = options.sortingMeasure || 'confidence';
  const minsupp = options.minsupp || 0;
  const minconf = options.minconf || 0;

  if (measures[sortingMeasure] == undefined) {
    throw new Error(`Sorting measure "${sortingMeasure}" not supported`);
  }
  if (sortingMeasure !== 'confidence') {
    options.attachMeasures = options.attachMeasures || false;
  }

  let transactionsWithAntecedent = [];
  if (!antecedent) {
    transactionsWithAntecedent = transactions;
  } else {
    _.each(transactions, x => {
      if (x.indexOf(antecedent) !== -1) {
        transactionsWithAntecedent.push(x);
      }
    });
  }
  const associations = addon.mine(transactionsWithAntecedent, antecedent, minsupp, minconf);
  // const associations = addon.mine(transactions, null, minsupp, minconf);
  const counts = _.countBy(_.flattenDeep(transactions));
  let ans = associations;
  _.each(ans, x => {
    if (!options.attachMeasures) {
      return;
    }
    x.nB = counts[x.consequent.toString()];
    x.lift = measures.lift(x.nA, x.nB, x.nAB, transactions.length);
    x.improvedLift = measures.improvedLift(x.nA, x.nB, x.nAB, transactions.length);
    x.kulc = measures.kulc(x.nA, x.nB, x.nAB, transactions.length);
    x.imbalanceRatio = measures.imbalanceRatio(x.nA, x.nB, x.nAB, transactions.length);
    x.biConfidence = measures.biConfidence(x.nA, x.nB, x.nAB, transactions.length);
    x.biImprove = measures.biImprove(x.nA, x.nB, x.nAB, transactions.length);
    x.jaccard = measures.jaccard(x.nA, x.nB, x.nAB, transactions.length);
    x.sebagSchoenauer = measures.sebagSchoenauer(x.nA, x.nB, x.nAB, transactions.length);
    x.certaintyFactor = measures.certaintyFactor(x.nA, x.nB, x.nAB, transactions.length);
  });
  ans = _.sortBy(ans, x => -x[sortingMeasure]);
  return ans;
}

function splitArray(arr, nSplits) {
  // if (arr.length / nSplits < 1) {
  //   console.warn('split nopt required, too less data');
  //   return arr;
  // }
  const nSplit = Math.round(arr.length / nSplits);

  let arrSplitted = [];
  for (let i = 0; i < nSplits; i++) {
    const start = nSplit * i;
    const end = i === nSplits - 1 ? arr.length : nSplit * (i + 1);
    arrSplitted.push(_.slice(arr, start, end));
  }
  return arrSplitted;
}


// options: {
//   antecedent: Number
//   sortingMeasure: String
//   minsupp: Number
//   minconf: Number
//   attachMeasures: Boolean
//   nCores: Number
//   workerEnv: Array of Object
// }
function mineParallel(transactions, options, cb) {
  const nCores = options.nCores || 5; // numCPUs;
  cb = cb || options.cb;
  if (!cb) {
    throw new Error('callback is required');
  }

  // split trans
  const transSplitted = splitArray(transactions, nCores);
  if (cluster.isMaster) {
    // console.log(`Master ${process.pid} is running`);

    // Fork workers.
    let workers = [];
    for (let i = 0; i < nCores; i++) {
      let thisWorkerEnv = {};
      if (options.workerEnv) {
        thisWorkerEnv = options.workerEnv[i];
      }
      thisWorkerEnv.INDEX = i;
      workers.push(cluster.fork(thisWorkerEnv));
    }

    let messages = [];
    cluster.on('message', (worker, msg) => {
      // messages.push(`worker ${worker.process.pid} sent a msg: ${msg}`);
      messages.push(msg);

      if (messages.length === workers.length) {
        // read the temp files
        let finans = [];
        for (let fil of messages) {
          finans = _.concat(finans, require(fil));
        }
        cb(finans);
        // delete the temp files
        for (let fil of messages) {
          fs.unlinkSync(fil);
        }
      }
    });

    cluster.on('exit', (worker, code, signal) => {
      // console.log(`worker ${worker.process.pid} died`);
    });
  } else {
    // console.log(`Worker ${process.pid} started`);
    const ans = mineOriginal(transSplitted[parseInt(process.env.INDEX)], options);
    const filename = './temp_' + Date.now() + '_assocs_' + process.env.INDEX + '.json';
    fs.writeFileSync(filename, JSON.stringify(ans), 'UTF-8');
    // process.send(`process ${process.pid} is done with its task.`);
    process.send(filename);
    process.exit(0);
  }
}



module.exports = mineParallel;
