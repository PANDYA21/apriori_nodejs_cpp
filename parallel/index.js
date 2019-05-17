const { getConsequents, mine } = require('../build/Release/addon');
const _ = require('lodash');
const { execFile } = require('child_process');
const os = require('os');
const maxCores = os.cpus().length - 1;
const fs = require('fs');
const path = require('path');

// options: {
//   antecedent: Number,
//   parallel?: Boolean,
//   nCores?: Number
//   callback: Function
// }
function wrapDefaultOptions(options = {}) {
  const defaults = {
    parallel: false,
    nCores: 1
  };
  for (let opt in defaults) {
    if (typeof options[opt] === 'undefined') {
      options[opt] = defaults[opt];
    }
  }
  if (options.parallel) {
    options.nCores = options.nCores || maxCores;
  } else {
    options.nCores = defaults.nCores;
  }
  return options;
}

function mineParallel(transactions, options) {
  const reqTime = Date.now();
  options = wrapDefaultOptions(options);
  if (!options.callback) {
    throw new Error ('Callback required');
  }
  const len = transactions.length;

  // write temp treansactions file for the workers to read
  const file = `temp_trans_${reqTime}.json`;
  fs.writeFileSync(file, JSON.stringify(transactions), 'UTF-8');

  // start workers
  let associations = [];
  for (let i = 0; i < options.nCores; i++) {
    const args = [
      path.join(__dirname, 'worker.js'),
      `START=${i === 0 ? 0 : (len / options.nCores * i + 1)}`,
      `END=${(len / options.nCores * (i + 1))}`,
      `TRANSACTIONS=${file}`,
      `WORKERID=${i}`,
      `REQTIME=${reqTime}`,
      `ANTECEDENT=${options.antecedent}`
    ];
    execFile('node', args, (error, stdout, stderr) => {
      if (error) {
        return console.error(error);
      }
      // console.log(stdout);
      // console.error(stderr);
      if (stdout.indexOf('done') !== -1) {
        // const outputFile = stdout.replace('done=', '').replace('\n', '');
        const outputFile = `assocs_${i}_${reqTime}.json`;
        associations.push(require('../' + outputFile));
        if (associations.length === options.nCores) {
          // all workers are done
          const ans = _.slice(_.sortBy(_.flattenDeep(associations), x => -x.confidence), 0, 10);
          options.callback(ans);
          // clean-up
          fs.unlinkSync(file);
          fs.unlinkSync(outputFile);
          return;
        }
      }
    });
  }

}



module.exports = { mineParallel };
