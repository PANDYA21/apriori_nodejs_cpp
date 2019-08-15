const assert = require('assert');

function testNodeVersion() {
  const version = process.version;
  const numVersion = parseFloat(version.substr(1,version.length));
  const check = numVersion >= 8.0 && numVersion < 11;
  console.log('version: ', version);
  console.log('check: ', check);         
  return check;
}

try {
  assert.equal(testNodeVersion(), true);
} catch(e) {
  throw new Error('Node.JS version between 8.x.x and 10.x.x required, current version: ' + process.version);
}
