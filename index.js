const { mineParallel } = require('./parallel');
const _ = require('lodash');
const measures = require('./interestingmeasures');

function mine(transactions, options) {
  const mainCallback = options.callback;
  options.callback = associations => {
    const counts = _.countBy(_.flattenDeep(transactions));
    let ans = associations;
    if (options.attachMeasures) {
      _.each(ans, x => {
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
    }
    ans = _.sortBy(ans, x => -x[options.sortingMeasure]);
    return mainCallback(ans);
  };
  mineParallel(transactions, options);
}

module.exports = mine;