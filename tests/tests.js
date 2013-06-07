/*
Copyright (c) 2010, Paul Spencer <pagameba@gmail.com>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

var assert = require('assert')
  , util = require('util')
  , mapserver = require('../mapserver')
  , fs = require('fs')
  , path = require('path')
  , datadir = path.normalize(path.join(path.dirname(__filename), './data'))
  , mapfile = 'test.map'
  , nomapfile = 'missing.map'
  , errormapfile = 'error.map'
  , map
  , err
  ;

  // test constants
assert.equal(mapserver.MS_OFF, 0, 'MS_OFF not defined');
assert.equal(mapserver.MS_ON, 1, 'MS_ON not defined');

// test accessing basic info about the mapserver build
console.log('mapserver version number: ' + mapserver.getVersionInt());
console.log('mapserver version information: ' + mapserver.getVersion());
console.log('Threading support: ' + (parseInt(mapserver.supportsThreads()) == 1 ? 'ON' : 'OFF'));

// quick error test
err = mapserver.getError();
assert.equal(err.code, 0, 'should be no errors');

// Test default mapfile pattern (must end in .map)
assert['throws'](function() { 
  mapserver.loadMap(path.join(datadir), nomapfile);  
}, Error, 'attempting to load a non-existent map should throw an error.');

// check error
err = mapserver.getError();
assert.equal(err.code, 5, 'Default mapfile pattern test failed')

// test resetErrorList
mapserver.resetErrorList();
err = mapserver.getError();
assert.equal(err.code, 0, 'should be no errors');

// test loadMap with missing file
assert['throws'](function() { 
  mapserver.loadMap(path.join(datadir, 'missing.map'), nomapfile);  
}, Error, 'attempting to load a non-existent map should throw an error.');

err = mapserver.getError();
assert.equal(err.code, 1, 'Mapserver should complain about a mapfile that does not exist.');

assert.doesNotThrow(function() {
  map = mapserver.loadMap(path.join(datadir, mapfile),datadir);
}, Error, 'loading a valid map file should not throw an error.');

// test getting map properties
assert.equal(map.name, "GMAP_DEMO", 'map name is incorrect');
assert.equal(map.status, mapserver.MS_ON, 'map status is not ON');
assert.equal(map.width, 600, 'getting map width failed');
assert.equal(map.height, 300, 'getting map height failed');
assert.equal(map.cellsize, 0, 'default map cellsize should be 0, got ' + map.cellsize);
assert.equal(map.scaledenom, -1, 'default map scaledenom should be -1, got ' + map.scaledenom);
assert.equal(map.maxsize, 2048, 'getting map maxsize failed');
assert.equal(map.units, mapserver.MS_DD, 'getting map units failed');
assert.equal(map.resolution, 72, 'getting map resolution failed');
assert.equal(map.defresolution, 72, 'getting map defresolution failed');
assert.equal(map.shapepath, './', 'getting shapepath failed');
assert.equal(path.normalize(map.mappath), path.normalize(path.join(__dirname, 'data')), 'getting mappath failed');
assert.equal(map.extent.minx, -180, 'getting map extent minx failed');
assert.equal(map.extent.miny, -90, 'getting map extent miny failed');
assert.equal(map.extent.maxx, 180, 'getting map extent maxx failed');
assert.equal(map.extent.maxy , 90, 'getting map extent maxy failed');

// test effect of recompute on cellsize and scaledenom
map.recompute();
assert.equal(map.cellsize, 0.6020066889632107, 'recomputing map cellsize failed, got ' + map.cellsize);
// assert.equal(map.scaledenom, 34533691.52101404, 'recomputing map scaledenom failed, got '+ map.scaledemon);

// test setting map properties
map.name = "test_set";
assert.equal(map.name, "test_set", "map name could not be set");  

map.width = 600;
assert.equal(map.width, 600, 'setting map width failed');

map.height = 600;
assert.equal(map.height, 600, 'setting map height failed');
map.height = 300;
assert.equal(map.height, 300, 'resetting map height failed');

map.maxsize = 4096;
assert.equal(map.maxsize, 4096, 'setting map maxsize failed');

map.units = mapserver.MS_FEET;
assert.equal(map.units, mapserver.MS_FEET, 'setting map units failed');

map.resolution = 96;
assert.equal(map.resolution, 96, 'setting map defresolution failed');

map.defresolution = 96;
assert.equal(map.defresolution, 96, 'setting map defresolution failed');

// test map.setExtent()
map.setExtent(-2500000, -402631, 1872800,3130000);
assert.equal(map.extent.minx, -2500000, 'setting map extent minx failed');
assert.equal(map.extent.miny, -402631, 'setting map extent miny failed');
assert.equal(map.extent.maxx, 1872800, 'setting map extent maxx failed');
assert.equal(map.extent.maxy, 3130000, 'setting map extent maxy failed');
map.setExtent(-180, -90, 180, 90);

// test indexed accessor to map.layers
assert.equal(map.layers.foo, undefined, 'map.layers should not expose arbitrary properties');
assert.equal(typeof map.layers.length, 'number', 'map.layers.length should be an integer');
assert.equal(map.layers.length, 2, 'map.layers.length should be 2');

// test accessor for layer name
assert.notEqual(map.layers[1].name,'test', 'layer name should not be test before we change it');
map.layers[1].name = 'test';
assert.equal(map.layers[1].name, 'test', 'layer name should have changed.');
// layers should be accessible by name too
assert.equal(map.layers['test'].name, 'test', 'layer should be accessible by name');

console.log('is ' + map.layers['grid'].connectiontype + ' = ' + mapserver.MS_GRATICULE);

console.log('intersection coordinates: ' + util.inspect(map.layers['grid'].getGridIntersectionCoordinates(), false, null, true));

map.drawMap(function(drawError, buffer) {
  if (drawError) {
    console.log(drawError);
    assert.ok(false, 'Error drawing map.');
  } else {
    buffer = buffer.slice(0,buffer.length);
    fs.writeFile(path.join(__dirname, 'test_buffer.gif'), buffer, function(err) {
      if (err) throw err;
    });
  }
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
