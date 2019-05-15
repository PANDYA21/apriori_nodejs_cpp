// to be used only as a forked child_process
const _ = require('lodash');
const args = process.argv;
const REQTIME = _.find(args, x => x.match('REQTIME')).replace('REQTIME=', '');
const path = require('path');
const optionsPath = path.join(__dirname, '../options_' + REQTIME + '.json');
const options = require(optionsPath);
const cluster = require('cluster');
const numCPUs = require('os').cpus().length - 1;
const mine = require('..');
const fs = require('fs');
const combineAssociations = require('./combine');

// start a nodejs cluster
function startCluster(options) {
  if (cluster.isMaster) {
    console.log(`Master ${process.pid} is running`);

    // Fork workers.
    let workers = [];
    for (let i = 0; i < options.nCores; i++) {
      let thisWorkerEnv = {};
      if (options.workerEnv) {
        thisWorkerEnv = options.workerEnv[i];
      }
      thisWorkerEnv.INDEX = i;
      thisWorkerEnv.SPLITFILENAME = options.splitFileNames[i];
      thisWorkerEnv.REQTIME = options.reqTime;
      workers.push(cluster.fork(thisWorkerEnv));
    }

    let messages = [];
    // read the sent filenames from workers sent as a message
    cluster.on('message', (worker, msg) => {
      messages.push(msg);

      if (messages.length === workers.length) {
        // read the temp files
        let finans = [];
        for (let fil of messages) {
          // finans = _.concat(finans, require(path.join('..', fil)));
          finans.push(require(path.join('..', fil)));
        }

        // // here is room for optimization
        // finans = combineAssociations(finans);

        const assocFile = `assoc_${REQTIME}.json`;
        fs.writeFileSync(assocFile, JSON.stringify(finans), 'UTF-8');
        console.log('done');
        // delete the temp files
        for (let fil of _.concat(messages, options.splitFileNames, [ optionsPath ])) {
          fs.unlinkSync(fil);
        }
      }
    });

    cluster.on('exit', (worker, code, signal) => {
      console.log(`worker ${worker.process.pid} died`);
    });
  } else {
    console.log(`Worker ${process.pid} started`);
    const trans = require(path.join('..', process.env.SPLITFILENAME));
    const ans = mine(trans, options);
    const filename = `./temp_${process.env.REQTIME}_${process.env.INDEX}.json`;
    fs.writeFileSync(filename, JSON.stringify(ans), 'UTF-8');
    process.send(filename);
    process.exit(0);
  }
}

startCluster(options);