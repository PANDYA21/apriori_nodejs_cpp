const mine = require('.');

const assocs = mine([
    [3, 10],
    [3, 4, 5],
    [3, 4, 5],
    [3, 4],
    // [1, 2],
    [3],
    [3, 4, 1],
    // [1, 2]
  ], 3);

console.log(assocs);
