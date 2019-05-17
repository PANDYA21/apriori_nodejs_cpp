const { mineParallel } = require('./parallel');

// mineParallel([
//     [3, 10],
//     [3, 4, 5],
//     [3, 4, 5],
//     [3, 4],
//     [1, 2],
//     [3],
//     [3, 4, 1],
//     [1, 2],
//     [7, 8, 9]
//   ], {
//     antecedent: 3,
//     callback: console.log
//   }
// );

module.exports = mineParallel;
