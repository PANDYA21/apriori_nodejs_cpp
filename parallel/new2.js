const { getConsequents, mine } = require('../build/Release/addon');
const _ = require('lodash');

// test

function serial() {
  function getTrans() {
    const ans = require('../trans7.json');
    // console.log({ ntrans: ans.length });
    let finans = [];
    _.each(ans, x => {
      let prods = [];
      _.each(_.uniq(x.produkts), xx => prods.push(parseInt(xx)));
      finans.push(prods);
    });
    return finans;
  }

  const trans = getTrans();
  const { uniqs, associatedItems } = getConsequents(trans);

  const t1 = Date.now();
  const assocs = mine(trans, 9000001, uniqs, associatedItems);
  const t2 = Date.now();
  console.log(_.slice(_.sortBy(assocs, x => -x.confidence), 0, 4));

  return t2 - t1;
}


const nCores = 3;

function parallel(cb) {
  const { execFile } = require('child_process');
  const t1 = Date.now();
  const len = 45000;

  for (let i = 0; i < nCores; i++) {
    execFile('node', ['./worker2.js', 'START=' + (len / nCores * i + 1), 'END=' + (len / nCores * (i + 1))], (error, stdout, stderr) => {
      if (error) {
        console.error(error);
      }
      console.log(stdout);
      console.error(stderr);
      if (stdout.indexOf('done') !== -1) {
        cb(t1, Date.now());
      }
    });
  }
}

// let pars = [];
// function parallelDone(t1, t2) {
//   pars.push(t2 - t1);
//   if (pars.length === nCores) {
//     console.log('\n\n\n---------------------------------------');
//     console.log({ ser, par: _.max(pars), ratio: ser / _.max(pars) });
//   }
// }


// const ser = serial();
// const par = parallel();

/////////////////////////////// benchmarking
const nTests = 200;

const ser = serial;
const par = async function() {
  let pars = [];
  return new Promise((resolve, reject) => {
    parallel((t1, t2) => {
      pars.push(t2 - t1);
      if (pars.length === nCores) {
        resolve(_.max(pars));
      }
    });
  });
};

async function benchmark () {
  let serials = [];
  let parallels = [];

  let i = 0;
  for (; i < nTests; i++) {
    serials.push(ser());
    parallels.push(await par());
  }

  console.log('---------------------------------------------------');

  let ans = {
    nTests,
    serial_mean: _.sum(serials) / nTests,
    parallel_mean: _.sum(parallels) / nTests,
    serial_median: _.sortBy(serials)[Math.round(nTests / 2) - 1],
    parallel_median: _.sortBy(parallels)[Math.round(nTests / 2) - 1]
  }

  ans.ratio_mean = ans.serial_mean / ans.parallel_mean;
  ans.ratio_median = ans.serial_median / ans.parallel_median;
  return ans;
}

const log = console.log;
console.log = function () {};
const fs = require('fs');

benchmark()
  .then(ans => fs.writeFileSync('./benchmark.json', JSON.stringify(ans, null, 2), 'UTF-8'))
  .catch(console.error);
