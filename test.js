const addon = require('./build/Release/addon');
const _ = require('lodash');

const assocs = addon.mine([
  [3, 4, 5],
  [3, 4],
  [1, 2],
  [3],
  [3, 4, 1],
  [1, 2],
  [7,8,9]
], 3);


const ans = _.sortBy(assocs, x => -x.frequency);
console.log(ans);
