const { getConsequents, mine } = require('../build/Release/addon');
const _ = require('lodash');
const args = process.argv;
const START = parseInt(_.find(args, x => x.match('START')).replace('START=', ''));
const END = parseInt(_.find(args, x => x.match('END')).replace('END=', ''));

// test
function getTrans() {
  const ans = require('../trans4.json');
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
const consequents = getConsequents(trans);
// console.log({ START, END });

const assocs = mine(trans, 9000001, _.slice(consequents, START, END));
console.log(_.slice(_.sortBy(assocs, x => -x.confidence), 0, 2));
console.log('done');
