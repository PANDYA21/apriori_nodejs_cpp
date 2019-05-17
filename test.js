const mine = require('.');
const assert = require('assert');

function test(cb) {
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
    sortingMeasure: 'lift',
    callback: assocs => {
      console.log(assocs);
      cb(assocs.length);
    }
  });
}

test(len => {
  assert.equal(len, 4);
});
