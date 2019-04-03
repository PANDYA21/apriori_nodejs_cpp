const addon = require('./build/Release/addon');
const _ = require('lodash');
const measures = require('./interestingmeasures');


module.exports = function mine(transactions, options) {
  const antecedent = options.antecedent;
  const sortingMeasure = options.sortingMeasure || 'confidence';
  if (measures[sortingMeasure] == undefined) {
    throw new Error(`Sorting measure "${sortingMeasure}" not supported`);
  }
  if (sortingMeasure !== 'confidence') {
    options.attachMeasures = options.attachMeasures || false;
  }

  let transactionsWithAntecedent = [];
  _.each(transactions, x => {
    if (x.indexOf(antecedent) !== -1) {
      transactionsWithAntecedent.push(x);
    }
  })
  const associations = addon.mine(transactionsWithAntecedent, antecedent);
  const counts = _.countBy(_.flattenDeep(transactions));
  let ans = associations;
  _.each(ans, x => {
    if (!options.attachMeasures) {
      return;
    }
    x.nB = counts[x.consequent.toString()];
    x.lift = measures.lift(x.nA, x.nB, x.nAB, transactions.length);
    x.improvedLift = measures.improvedLift(x.nA, x.nB, x.nAB, transactions.length);
    x.kulc = measures.kulc(x.nA, x.nB, x.nAB, transactions.length);
    x.imbalanceRatio = measures.imbalanceRatio(x.nA, x.nB, x.nAB, transactions.length);
    x.biConfidence = measures.biConfidence(x.nA, x.nB, x.nAB, transactions.length);
    x.biImprove = measures.biImprove(x.nA, x.nB, x.nAB, transactions.length);
    x.jaccard = measures.jaccard(x.nA, x.nB, x.nAB, transactions.length);
    x.sebagSchoenauer = measures.sebagSchoenauer(x.nA, x.nB, x.nAB, transactions.length);
    x.certaintyFactor = measures.certaintyFactor(x.nA, x.nB, x.nAB, transactions.length);
  });
  ans = _.sortBy(ans, x => -x[sortingMeasure]);
  return ans;
};
