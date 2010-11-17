var assert = require('assert'),
    util = require('util'),
    mapserver = require('./mapserver'),
    fs = require('fs'),
    path = require('path'),
    datadir = './data',
    mapfile = 'test.map',
    nomapfile = 'missing.map',
    errormapfile = 'error.map',
    map,
    err;

console.log('mapserver version number: ' + mapserver.getVersionInt());
console.log('mapserver version information: ' + mapserver.getVersion());

try {
  map = mapserver.loadMap(path.join(datadir), nomapfile);
} catch (e) {
  printError(mapserver.getError());
}

map = mapserver.loadMap(path.join(datadir, mapfile),datadir);

assert.equal(map.width, 400, 'getting map width failed');
assert.equal(map.height, 300, 'getting map height failed');

map.width = 600;
assert.equal(map.width, 600, 'setting map width failed');

map.height = 600;
assert.equal(map.height, 600, 'setting map height failed');



var buffer = map.drawMap();
buffer = buffer.slice(0,buffer.length);
fs.writeFile('test_buffer.gif', buffer, function(err) {
  if (err) throw err;
});

function printError(err) {
  if (err != undefined) {
    try {
      console.log('Error code ' + err.code + ' (' + err.codeStr + ') in ' + err.routine + ': ' + err.message);
      return true;
    } catch (e) {
      return false;
    }
  }
}