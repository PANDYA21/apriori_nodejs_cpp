const _ = require('lodash');
const fs = require('fs');
const { execFile } = require('child_process');
const path = require('path');

// split data into required portions
function splitArray(arr, nSplits) {
  const nRows = Math.round(arr.length / nSplits);

  let arrSplitted = [];
  for (let i = 0; i < nSplits; i++) {
    const start = nRows * i;
    const end = i === nSplits - 1 ? arr.length : nRows * (i + 1);
    arrSplitted.push(_.slice(arr, start, end));
  }
  return arrSplitted;
}

// save splitted data onto file-system for workers to read later
function saveSplitData(arrSplitted, reqTime = Date.now()) {
  let splitFileNames = [];
  for (let i in arrSplitted) {
    const file = `split_${reqTime}_${i}.json`;
    fs.writeFileSync(file, JSON.stringify(arrSplitted[i]), 'UTF-8');
    splitFileNames.push(file);
  }
  return splitFileNames;
}


// options: {
//   antecedent: Number
//   sortingMeasure?: String
//   minsupp?: Number
//   minconf?: Number
//   attachMeasures?: Boolean
//   nCores?: Number
//   workerEnv?: Array of Object
//   cb?: 
// }
function main(transactions, options, cb) {
  // request time as constant
  const reqTime = Date.now();
  // wrap the options, check for caalback
  const nCores = options.nCores || numCPUs;
  options.nCores = nCores;
  cb = cb || options.cb;
  if (!cb) {
    throw new Error('callback is required');
  }
  options.cb = cb;

  // split data and save temp files
  const arrSplitted = splitArray(transactions, nCores);
  const splitFileNames = saveSplitData(arrSplitted, reqTime);
  options.splitFileNames = splitFileNames;
  options.nCores = splitFileNames.length; // just to be sure
  options.reqTime = reqTime;

  // start parallel cluster
  startCluster(options);
}


// start nodejs cluster as a forked process
function startCluster(options) {
  fs.writeFileSync('./options_' + options.reqTime + '.json', JSON.stringify(options, null, 2), 'UTF-8');
  const file = path.join(__dirname, 'core.js');
  execFile('node', [file, `REQTIME=${options.reqTime}`], (error, stdout, stderr) => {
    if (error) {
      console.error(error);
    }
    console.log(stdout, stdout.indexOf('done'));
    console.error(stderr);
    if (stdout.indexOf('done') !== -1) {
      console.log('This happened here');
      const assocs = require(path.join(__dirname, `/../assoc_${options.reqTime}.json`));
      options.cb(assocs);
    }
  });
}


// test
function getTrans() {
  const ans = require('../trans4.json');
  console.log({ ntrans: ans.length });
  let finans = [];
  _.each(ans, x => {
    let prods = [];
    _.each(_.uniq(x.produkts), xx => prods.push(parseInt(xx)));
    finans.push(prods);
  });
  return finans;
}

const trans = getTrans();
const t1 = Date.now();
main(_.slice(trans, 0, 1000), {
  antecedent: 9000001,
  nCores: 5
}, assoc => {
  const t2 = Date.now();
  console.log(assoc);
  console.log((t2 - t1) / 1000);
});