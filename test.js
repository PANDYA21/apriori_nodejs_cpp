const assert = require('assert');
const mine = require('.');

function test() {
  const assocs = mine([
    [3, 10],
    [3, 4, 5],
    [3, 4, 5],
    [3, 4],
    [1, 2],
    [3],
    [3, 4, 1],
    [1, 2],
    [7, 8, 9]
  ], { 
    antecedent: 3,
    sortingMeasure: 'lift'
  });

  console.log(assocs);
  return assocs.length;
}

assert.equal(test(), 4);