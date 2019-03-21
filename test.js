const addon = require('./build/Release/addon');

console.log(addon.mine([
  [3, 4, 5],
  [3, 4],
  [3],
  [3, 4, 1],
  [1, 2],
  [7,8,9]
]));