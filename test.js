const mine = require('.');
const assert = require('assert');

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
  ], { antecedent: 7 });

  console.log(assocs);
  return assocs.length;
}

assert.equal(test(), 2);