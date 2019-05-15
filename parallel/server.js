const express = require('express');
let app = express();
const { execFile } = require('child_process');
const path = require('path');

function test2(cb) {
  const file = path.join(__dirname, 'index.js');
  execFile('node', [file], (error, stdout, stderr) => {
    if (error) {
      console.error(error);
    }
    // console.log(stdout);
    cb(stdout);
  });
}

app.get('/', (req, res, next) => {
  test2(ans => res.status(200).send(ans));
});
app.listen(8081);