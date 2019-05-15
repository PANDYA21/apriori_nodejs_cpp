const { getConsequents, mine } = require('../build/Release/addon');
const _ = require('lodash');

// test

function serial() {
  function getTrans() {
    const ans = require('../trans4.json');
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
  const consequents = getConsequents(trans);
  // console.log({ nCosenquents: consequents.length });

  const t1 = Date.now();
  const assocs = mine(trans, 9000001, consequents);
  const t2 = Date.now();
  console.log(_.slice(_.sortBy(assocs, x => -x.confidence), 0, 4));

  return t2 - t1;
}

function parallel() {
  const { execFile } = require('child_process');
  const t1 = Date.now();

  execFile('node', ['./worker.js', 'START=0', 'END=9000'], (error, stdout, stderr) => {
    if (error) {
      console.error(error);
    }
    console.log(stdout);
    console.error(stderr);
    if (stdout.indexOf('done') !== -1) {
      parallelDone(t1, Date.now());
    }
  });

  execFile('node', ['./worker.js', 'START=9001', 'END=18000'], (error, stdout, stderr) => {
    if (error) {
      console.error(error);
    }
    console.log(stdout);
    console.error(stderr);
    if (stdout.indexOf('done') !== -1) {
      parallelDone(t1, Date.now());
    }
  });

  execFile('node', ['./worker.js', 'START=18001', 'END=27000'], (error, stdout, stderr) => {
    if (error) {
      console.error(error);
    }
    console.log(stdout);
    console.error(stderr);
    if (stdout.indexOf('done') !== -1) {
      parallelDone(t1, Date.now());
    }
  });

  execFile('node', ['./worker.js', 'START=27001', 'END=36000'], (error, stdout, stderr) => {
    if (error) {
      console.error(error);
    }
    console.log(stdout);
    console.error(stderr);
    if (stdout.indexOf('done') !== -1) {
      parallelDone(t1, Date.now());
    }
  });

  execFile('node', ['./worker.js', 'START=36001', 'END=45000'], (error, stdout, stderr) => {
    if (error) {
      console.error(error);
    }
    console.log(stdout);
    console.error(stderr);
    if (stdout.indexOf('done') !== -1) {
      parallelDone(t1, Date.now());
    }
  });
}

let pars = [];
function parallelDone(t1, t2) {
  pars.push(t2 - t1);
  if (pars.length === 5) {
    console.log('\n\n\n---------------------------------------');
    console.log({ ser, par: _.max(pars), ratio: ser / _.max(pars) });
  }
}


const ser = serial();
const par = parallel();
