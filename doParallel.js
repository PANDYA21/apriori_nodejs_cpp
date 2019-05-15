const cluster = require('cluster');
const numCPUs = require('os').cpus().length - 1;

// parallelize a function that accepts two args: env and callback
function doParallel(fun, options, cb) {
  const nCores = options.nCores || numCPUs;
  if (cluster.isMaster) {
    console.log(`Master ${process.pid} is running`);

    // Fork workers.
    let workers = [];
    for (let i = 0; i < nCores; i++) {
      let thisWorkerEnv = {};
      if (options.workerEnv) {
        thisWorkerEnv = options.workerEnv[i];
      }
      workers.push(cluster.fork(thisWorkerEnv));
    }

    let messages = [];
    cluster.on('message', (worker, msg) => {
      messages.push(`worker ${worker.process.pid} sent a msg: ${msg}`);

      if (messages.length === workers.length) {
        return cb(messages);
      }
    });

    cluster.on('exit', (worker, code, signal) => {
      console.log(`worker ${worker.process.pid} died`);
    });
  } else {
    console.log(`Worker ${process.pid} started`);
    fun(process.env, function workerCB() {
      process.send(`process ${process.pid} is done with its task.`);
      process.exit(0);
    });
  }
}

module.exports = doParallel;

// function task(env, cb) {
//   // console.log(env);
//   setTimeout(() => {
//     let j = 0;
//     // for (let i = 1e10 - 1; i >= 0; i--) {
//     // j++;
//     // }
//     // console.log(`counted till ${j} and waited 3 secs.`);
//     cb();
//   }, 3000);
// }
// doParallel(task, { nCores: 2, workerEnv: [{a: 11}, {a: 20}] }, console.log);
