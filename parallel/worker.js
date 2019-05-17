const { getConsequents, mine } = require('../build/Release/addon');
const fs = require('fs');
const _ = require('lodash');
const args = process.argv;
const START = parseInt(_.find(args, x => x.match('START')).replace('START=', ''));
const END = parseInt(_.find(args, x => x.match('END')).replace('END=', '')) + 1;
const WORKERID = parseInt(_.find(args, x => x.match('WORKERID')).replace('WORKERID=', ''));
const REQTIME = _.find(args, x => x.match('REQTIME')).replace('REQTIME=', '');
const TRANSACTIONS = _.find(args, x => x.match('TRANSACTIONS')).replace('TRANSACTIONS=', '');
const ANTECEDENT = parseInt(_.find(args, x => x.match('ANTECEDENT')).replace('ANTECEDENT=', ''));

// test
function getTrans(antecedent) {
  const givenTrans = require('../' + TRANSACTIONS);
  let trans = [];
  _.each(givenTrans, tran => {
    let items = [];
    _.each(_.uniq(tran), item => items.push(parseInt(item)));
    items.indexOf(antecedent) !== -1 ? trans.push(items) : null;
  });
  return trans;
}


const trans = getTrans(ANTECEDENT);
const { uniqs, associatedItems } = getConsequents(trans);
const assocs = mine(trans, ANTECEDENT, _.slice(uniqs, START, END), associatedItems);

const outputFile = `assocs_${WORKERID}_${REQTIME}.json`;
fs.writeFileSync(outputFile, JSON.stringify(assocs), 'UTF-8');
console.log('done=' + outputFile);
