const mine = require('..');
const _ = require('lodash');
const assert = require('assert');
const fs = require('fs');

function getRandomInt(min, max) {
  min = Math.ceil(min);
  max = Math.floor(max);
  return Math.floor(Math.random() * (max - min)) + min; //The maximum is exclusive and the minimum is inclusive
}

function generateTransactions(nTrans = getRandomInt(10, 100), nItems = getRandomInt(10, 100), maxItemsPerTrans) {
  maxItemsPerTrans = maxItemsPerTrans || nItems;
  let items = [];
  for (let i = 0; i < nItems; i++) {
    items.push(i);
  }
  let trans = [];
  for (let i = 0; i < nTrans; i++) {
    let thisTran = [];
    const rand = getRandomInt(1, maxItemsPerTrans)
    for (let j = 0; j < rand; j++) {
      const index = getRandomInt(0, items.length);
      thisTran.push(items[index]);
    }
    trans.push(_.uniq(thisTran));
  }
  return trans;
}

function cppMine(trans, antecedent) {
  const assocs = mine(trans, { antecedent, attachMeasures: false });
  return assocs;
}

function jsMine(trans, antecedent) {
  let associatedItems = [];
  let nA = 0;
  for (let tran of trans) {
    if (tran.indexOf(antecedent) !== -1) {
      associatedItems = _.concat(associatedItems, _.filter(tran, x => x !== antecedent));
      nA++;
    }
  }
  const counts = _.countBy(associatedItems);
  let assocs = [];
  for (let item in counts) {
    assocs.push({
      antecedent,
      consequent: item,
      nAB: counts[item],
      nA,
      confidence: counts[item] / nA
    });
  }
  assocs = _.filter(assocs, x => x.consequent != x.antecedent);
  return _.sortBy(assocs, x => -x.nAB);
}

function benchmarkOne(nTrans, nItems) {
  // generate synthetic transactions
  const trans = generateTransactions(nTrans, nItems);
  const antecedent = 1;

  // cpp mining
  const tc1 = Date.now();
  const assocsCpp = cppMine(trans, antecedent);
  const tc2 = Date.now();

  // js mining
  const tj1 = Date.now();
  const assocsJs = jsMine(trans, antecedent);
  const tj2 = Date.now();

  assert.deepEqual(assocsJs, assocsCpp);

  // ŕeturn the times
  return {
    jsTime: tj2 - tj1,
    cppTime: tc2 - tc1
  };
}

function benchmark(nTests = 5, nTrans = 1000, nItems = 100) {
  let times = [];
  for (let i = 0; i < nTests; i++) {
    times.push(benchmarkOne(nTrans, nItems));
  }
  // const meanJsTime = _.sum(_.map(times, 'jsTime')) / times.length;
  // const meanCppTime = _.sum(_.map(times, 'cppTime')) / times.length;
  // console.log({
  //   meanJsTime,
  //   meanCppTime,
  //   ratio: meanJsTime / meanCppTime
  // });
  return times;
}

function benchmarkForTransCount(nCounts = 5) {
  let ans = [];
  for (let i = 0; i < nCounts; i++) {
    const thisNTrans = 10000 + i*1000;
    const thisNItems = 1000;
    let thisBenchmark = benchmark(5, thisNTrans, thisNItems);
    _.each(thisBenchmark, x => {
      x.nTrans = thisNTrans;
      x.nItems = thisNItems;
    });
    ans = _.concat(ans, thisBenchmark);
  }
  return ans;
}

function benchmarkForItemsCount(nCounts = 5) {
  let ans = [];
  for (let i = 0; i < nCounts; i++) {
    const thisNTrans = 10000;
    const thisNItems = 1000 + i*100;
    let thisBenchmark = benchmark(5, thisNTrans, thisNItems);
    _.each(thisBenchmark, x => {
      x.nTrans = thisNTrans;
      x.nItems = thisNItems;
    });
    ans = _.concat(ans, thisBenchmark);
  }
  return ans;
}

(function main() {
  fs.writeFile('./benchmark_trans2.json', JSON.stringify(benchmarkForTransCount(), null, 2), 'utf8', err => err ? console.error(err): null);
  fs.writeFile('./benchmark_items2.json', JSON.stringify(benchmarkForItemsCount(), null, 2), 'utf8', err => err ? console.error(err): null);
})();
