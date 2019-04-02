const addon = require('./build/Release/addon');
const _ = require('lodash');

module.exports = function mine(transactions, antecedent) {
  let transactionsWithAntecedent = [];
  _.each(transactions, x => {
    if (x.indexOf(antecedent) !== -1) {
      transactionsWithAntecedent.push(x);
    }
  })
  const associations = addon.mine(transactionsWithAntecedent, antecedent);
  const ans = _.sortBy(associations, x => -x.nAB);
  return ans;
};
