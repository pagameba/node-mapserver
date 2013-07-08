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
  , datadir = path.join(__dirname, 'data')
  , mapfile = path.join(datadir, 'test.map')
  , nomapfile = 'missing.map'
  , errormapfile = path.join(datadir, 'error.map')
  , map
  , err
  ;
  
beforeEach(function() {
  mapserver.resetErrorList();
});
  
describe('mapserver', function() {
  it('should exports constants', function() {
      // test constants
    assert.equal(mapserver.MS_OFF, 0, 'MS_OFF not defined');
    assert.equal(mapserver.MS_ON, 1, 'MS_ON not defined');
  });
  
  it('should get some basic info about mapserver', function() {
    assert.ok(mapserver.getVersionInt());
    assert.ok(mapserver.getVersion());
    assert.ok(mapserver.supportsThreads());
  });
  
  it('should have no errors yet', function() {
    // quick error test
    var err = mapserver.getError();
    assert.equal(err.code, 0, 'should be no errors');
  });
  
  it('should create a projection', function() {
    // quick error test
    var proj = new mapserver.Projection("+init=epsg:4326");
    assert.equal(proj.units, mapserver.MS_DD, 'projection units should be decimal degrees');
    err = mapserver.getError();
    assert.equal(err.code, 0, 'should be no errors');
  });
  
  it('missing mapfile should throw an error', function() {
    // Test default mapfile pattern (must end in .map)
    assert['throws'](function() { 
      new mapserver.Map(nomapfile);  
    }, Error, 'attempting to load a non-existent map should throw an error.');

    // check error
    err = mapserver.getError();
    assert.equal(err.code, 1, 'Missing map file error failed.');

    // test resetErrorList
    mapserver.resetErrorList();
    err = mapserver.getError();
    assert.equal(err.code, 0, 'should be able to reset the error list');
  });
  
  it('should load a valid map file', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile, __dirname);
    }, function() {
      err = mapserver.getError();
      console.log(util.inspect(err));
    }, 'loading a valid map file with a data path should not throw an error.');
    assert.equal(map.mappath, __dirname, 'map did not load with correct data directory');

    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, function() {
      err = mapserver.getError();
      console.log(util.inspect(err));
    }, 'loading a valid map file with no data path should not throw an error.');
    assert.equal(path.relative(map.mappath, path.dirname(mapfile)), '', 'map did not compute correct default data directory');

    assert.doesNotThrow(function() {
      map = new mapserver.Map();
    }, function() {
      err = mapserver.getError();
      console.log(util.inspect(err));
    }, 'creating a blank map should not throw an error');
  });
  
  it('should get basic information from the map object', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');

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
  });
  
  it('should set map properties', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');

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
  });

  it('should get the map extent', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');
    
    assert.equal(map.extent.minx, -180, 'getting map extent minx failed');
    assert.equal(map.extent.miny, -90, 'getting map extent miny failed');
    assert.equal(map.extent.maxx, 180, 'getting map extent maxx failed');
    assert.equal(map.extent.maxy , 90, 'getting map extent maxy failed');
  });

  it('should set the map extent', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');
    
    map.setExtent(-90, -45, 90, 45);
    assert.equal(map.extent.minx, -90, 'setting map extent minx failed');
    assert.equal(map.extent.miny, -45, 'setting map extent miny failed');
    assert.equal(map.extent.maxx, 90, 'setting map extent maxx failed');
    assert.equal(map.extent.maxy, 45, 'setting map extent maxy failed');
  });

  it('should compute cell size and scaledenom', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');
    // test effect of recompute on cellsize and scaledenom
    map.recompute();
    assert.equal(map.cellsize, 0.6020066889632107, 'recomputing map cellsize failed, got ' + map.cellsize);
    assert.equal(map.scaledenom, 189621444.28093645, 'recomputing map scaledenom failed, got '+ map.scaledemon);
  });

  it('should have access to map layers', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');
  
    // test indexed accessor to map.layers
    assert.equal(map.layers.foo, undefined, 'map.layers should not expose arbitrary properties');
    assert.equal(typeof map.layers.length, 'number', 'map.layers.length should be an integer');
    assert.equal(map.layers.length, 3, 'map.layers.length should be 3');
  
    // test accessor for layer name
    assert.notEqual(map.layers[1].name,'test', 'layer name should not be test before we change it');
    map.layers[1].name = 'test';
    assert.equal(map.layers[1].name, 'test', 'layer name should have changed.');
    // layers should be accessible by name too
    assert.equal(map.layers['test'].name, 'test', 'layer should be accessible by name');
    
    assert.equal(map.layers['test'].status, mapserver.MS_ON, 'layer status should be ON');
    
    map.layers['test'].status = mapserver.MS_OFF;
    assert.equal(map.layers['test'].status, mapserver.MS_OFF, 'layer status should change to OFF');
    
  });

  it('should get grid intersection coordinates', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');
  
    var values = map.layers['grid'].getGridIntersectionCoordinates();
    assert.ok(values, 'grid intersection result is not an object');
    
    assert.ok(Array.isArray(values.left), 'left is not an array');
    assert.ok(Array.isArray(values.top), 'top is not an array');
    assert.ok(Array.isArray(values.right), 'right is not an array');
    assert.ok(Array.isArray(values.bottom), 'bottom is not an array');
    
    assert.equal(values.left.length, 15, 'does not have 15 values for the left array');
    assert.equal(values.top.length, 15, 'does not have 15 values for the top array');
    assert.equal(values.right.length, 15, 'does not have 15 values for the right array');
    assert.equal(values.bottom.length, 15, 'does not have 15 values for the bottom array');
    
    assert.equal(values.left[0].x, 0, 'first left value is not 0');
  });
  
  it('should have an output format', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');
    assert.equal(map.outputformat.name, 'png', 'output format name is incorrect');
    assert.equal(map.outputformat.mimetype, 'image/png', 'output format mimetype is incorrect');
  });
  
  it('should draw a map', function(done) {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');
    fs.readFile(path.join(__dirname, 'data', 'test_buffer.png'), function(err, data) {
      if (err) {
        throw new Error('failed to load test image');
      }
      map.drawMap(function(drawError, buffer) {
        if (drawError) {
          util.inspect(drawError, false, 0, true);
          assert.ok(false, 'Error drawing map.');
        } else {
          fs.writeFileSync(path.join(__dirname, 'data', 'test_out.png'), buffer);
          assert.equal(data.toString('hex'), buffer.toString('hex'), 'map draw differed from sample image');
          done();
        }
      });
    });
  });
});
