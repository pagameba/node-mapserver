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

assert.throws(function() { 
  mapserver.loadMap(path.join(datadir), nomapfile);  
}, Error, 'attempting to load a non-existent map should throw an error.');

assert.doesNotThrow(function() {
  map = mapserver.loadMap(path.join(datadir, mapfile),datadir);
}, Error, 'loading a valid map file should not throw an error.');

assert.equal(map.width, 400, 'getting map width failed');
assert.equal(map.height, 300, 'getting map height failed');

map.width = 600;
assert.equal(map.width, 600, 'setting map width failed');

map.height = 600;
assert.equal(map.height, 600, 'setting map height failed');

assert.equal(map.layers.foo, undefined, 'map.layers should not expose arbitrary properties');
assert.equal(typeof map.layers.length, 'number', 'map.layers.length should be an integer');
assert.equal(map.layers.length, 1, 'map.layers.length should be 1');

for (var i=0; i< map.layers.length; i++) {
  console.log('layer ' + i + ' name: ' + map.layers[i].name);
}

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