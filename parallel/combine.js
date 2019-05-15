const _ = require('lodash');


function combineAssociations(arr2d) {
  const antecedents = _.uniq(_.map(_.flattenDeep(arr2d), 'antecedent'));
  const consequents = _.uniq(_.map(_.flattenDeep(arr2d), 'consequent'));
  let ant_na = [];
  for (let antecedent of antecedents) {
    let this_na = 0;
    for (let onecore of arr2d) {
      const this_assoc = _.find(onecore, { antecedent });
      if (this_assoc) {
        this_na += this_assoc.nA;
      }
    }
    ant_na.push({ antecedent, nA: this_na });
  }


  let finans = [];
  for (let antecedent of antecedents) {
    for (let consequent of consequents) {
      let this_nab = 0;
      for (let onecore of arr2d) {
        const this_assoc = _.find(onecore, { antecedent, consequent });
        if (this_assoc) {
          this_nab += this_assoc.nAB;
        }
      }
      if (this_nab) {
        finans.push({
          antecedent,
          consequent,
          nA: _.find(ant_na, { antecedent }).nA,
          nAB: this_nab
        })
      }
    }
  }
  
  return finans;
}

module.exports = combineAssociations;
