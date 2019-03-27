const addon = require('./build/Release/addon');
const _ = require('lodash');

module.exports = function mine(transactions, antecedent) {
  const associations = addon.mine(transactions, antecedent);
  const ans = _.sortBy(associations, x => -x.nAB);
  return ans;
};
