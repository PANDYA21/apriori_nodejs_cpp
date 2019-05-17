const { getConsequents, mine } = require('../build/Release/addon');
const _ = require('lodash');
const args = process.argv;
const START = parseInt(_.find(args, x => x.match('START')).replace('START=', ''));
const END = parseInt(_.find(args, x => x.match('END')).replace('END=', ''));

// test
function getTrans() {
  const ans = require('../trans7.json');
  // console.log({ ntrans: ans.length });
  let finans = [];
  _.each(ans, x => {
    let prods = [];
    _.each(_.uniq(x.produkts), xx => prods.push(parseInt(xx)));
    finans.push(prods);
  });
  return finans;
}

const trans = getTrans();
const { uniqs, associatedItems } = getConsequents(trans);
// console.log({ START, END });

const assocs = mine(trans, 9000001, _.slice(uniqs, START, END), associatedItems);
console.log(_.slice(_.sortBy(assocs, x => -x.confidence), 0, 2));
console.log('done');