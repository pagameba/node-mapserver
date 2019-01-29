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


/*jshint node:true, sub:true */
/*global beforeEach, describe, it */

"use strict";


var assert = require('assert');
var util = require('util');
var mapserver = require('../mapserver');
var fs = require('fs');
var path = require('path');
var datadir = path.join(__dirname, 'data');
var mapfile = path.join(datadir, 'test.map');
var badmapfile = path.join(datadir, 'bad.map');
var symbolfile = path.join(datadir, 'symbolset.txt');
var nomapfile = 'missing.map';
var map;
var layer;
var err;

console.log('running tests for mapserver %s : %s', mapserver.getVersionInt(), mapserver.getVersion());

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
    assert.ok(mapserver.getVersionInt(), 'mapserver should report its version number');
    assert.ok(mapserver.getVersion(), 'mapserver should report its version string');
    assert.ok(mapserver.supportsThreads(), 'mapserver compiled without thread support.');
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
    assert.equal(proj.projString, "+init=epsg:4326", 'getting projection string failed. Got ' + proj.projString);
    err = mapserver.getError();
    assert.equal(err.code, 0, 'should be no errors');
    proj.projString = "+init=epsg:3857";
    assert.equal(proj.projString, "+init=epsg:3857", 'setting projection object via string failed. Got ' +  proj.projString);
  });

  it('should have a functional point object', function() {
    var point = new mapserver.Point();
    assert.equal(point.x, -1, 'default point x should be -1');
    assert.equal(point.y, -1, 'default point y should be -1');

    point = new mapserver.Point(10.5, 20);
    assert.equal(point.x, 10.5, 'create a point with an x and y value');
    assert.equal(point.y, 20, 'create a point with an x and y value');

    var epsg4326 = new mapserver.Projection("+init=epsg:4326");
    var epsg3857 = new mapserver.Projection("+init=epsg:3857");
    point.project(epsg4326, epsg3857);

    var dx = Math.abs(point.x - 1168854.6533293733).toFixed(6);
    assert.equal(dx, 0, 'reprojected x was not correct, delta was ' + dx);
    var dy = Math.abs(point.y - 2273030.926987688).toFixed(6);
    assert.equal(dy, 0, 'reprojected y was not correct, delta was ' + dy);
    point = new mapserver.Point(0,0);
    assert.equal(point.distanceToPoint(new mapserver.Point(1,0)), 1, 'distanceToPoint should be 1');
  });

  it('should have a functional rect object', function() {
    var rect;

    assert.doesNotThrow(function() {
      rect = new mapserver.Rect();
    }, 'creating a rect with 0 arguments should work.');

    assert.equal(rect.minx, -1, 'default rect constructor minx should equal -1');
    assert.equal(rect.miny, -1, 'default rect constructor miny should equal -1');
    assert.equal(rect.maxx, -1, 'default rect constructor maxx should equal -1');
    assert.equal(rect.maxy, -1, 'default rect constructor maxy should equal -1');

    assert.doesNotThrow(function() {
      rect = new mapserver.Rect(1.2, 3.4, 5.6, 7.8);
    }, 'creating a rect with 4 arguments should work');

    assert.equal(rect.minx, 1.2, 'rect minx should equal 1.2');
    assert.equal(rect.miny, 3.4, 'rect miny should equal 3.4');
    assert.equal(rect.maxx, 5.6, 'rect maxx should equal 5.6');
    assert.equal(rect.maxy, 7.8, 'rect maxy should equal 7.8');

    assert['throws'](function() {
      rect = new mapserver.Rect(1,1);
    }, Error, 'rect constructor should throw an error with wrong number of arguments.');

    assert['throws'](function() {
      rect = new mapserver.Rect('a', 'b', 'c', 'd');
      console.log(rect);
    }, Error, 'rect constructor should throw an error with wrong argument type.');

    rect = new mapserver.Rect(3, 4, 1, 2);
    assert.equal(rect.minx, 1, 'rect minx should be minimum x value');
    assert.equal(rect.miny, 2, 'rect miny should be minimum y value');
    assert.equal(rect.maxx, 3, 'rect maxx should be maximum x value');
    assert.equal(rect.maxy, 4, 'rect maxy should be maximum y value');

    rect = new mapserver.Rect();
    rect.minx = 1;
    rect.miny = 2;
    rect.maxx = 3;
    rect.maxy = 4;
    assert.equal(rect.minx, 1, 'rect minx should assignable');
    assert.equal(rect.miny, 2, 'rect miny should assignable');
    assert.equal(rect.maxx, 3, 'rect maxx should assignable');
    assert.equal(rect.maxy, 4, 'rect maxy should assignable');

    assert['throws'](function() {
      rect = new mapserver.Rect(0, 0, 1, 1);
      rect.project('a','b');
    }, Error, 'rect project should throw an error with wrong argument type.');
    rect = new mapserver.Rect(0, 0, 1, 1);
    rect.project(new mapserver.Projection('+init=epsg:4326'), new mapserver.Projection('+init=epsg:3857'));

    var delta = Math.abs(rect.minx - 0).toFixed(6);
    assert.equal(delta, 0, 'reprojecting rect minx failed, delta was ' + delta);
    delta = Math.abs(rect.miny - 0).toFixed(6);
    assert.equal(delta, 0, 'reprojecting rect miny failed, delta was ' + delta);
    delta = Math.abs(rect.maxx - 111319.49079327231).toFixed(6);
    assert.equal(delta, 0, 'reprojecting rect maxx failed, delta was ' + delta);
    delta = Math.abs(rect.maxy - 111325.14286638486).toFixed(6);
    assert.equal(delta, 0, 'reprojecting rect maxy failed, delta was ' + delta);
  });

  it('should throw an error for a missing mapfile', function() {
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
    assert.equal(map.maxsize, 4096, 'getting map maxsize failed');
    assert.equal(map.units, mapserver.MS_DD, 'getting map units failed');
    assert.equal(map.resolution, 72, 'getting map resolution failed');
    assert.equal(map.defresolution, 72, 'getting map defresolution failed');
    assert.equal(map.shapepath, './', 'getting shapepath failed');
    assert.equal(map.imagetype, 'png', 'imagetype should be png, got ' + map.imagetype);
    assert.equal(map.mimetype, 'image/png', 'mimetype should be image/png, got ' + map.mimetype);
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

  it('should clone a map object', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');

    var clone = map.clone();

    ['name','width','height','maxsize','units','resolution','defresolution'].forEach(function(prop) {
      assert.equal(map[prop], clone[prop], 'it should clone the map ' + prop);
    });

  });

  it('should get the map projection', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');

    assert.equal(map.projection.projString, "+init=epsg:4326", 'getting map projection string failed. Got ' + map.projection.projString);

    var epsg3857 = new mapserver.Projection("+init=epsg:3857");
    var point = new mapserver.Point(10.5, 20);
    point.project(map.projection, epsg3857);
    var delta = Math.abs(point.x - 1168854.6533293733).toFixed(6);
    assert.equal(delta, 0, 'reprojected x was not correct, delta was ' + delta);
    delta = Math.abs(point.y - 2273030.926987688).toFixed(6);
    assert.equal(delta, 0, 'reprojected y was not correct, delta was ' + delta);

  });

  it('should set the map projection', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');
    map.projection = "+init=epsg:3857";
    assert.equal(map.projection.projString, "+init=epsg:3857", 'setting map projection string failed. Got ' + map.projection.projString);
  });

  it('should get a layer projection', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');

    assert.equal(map.layers.prov_bound.projection.projString, "+init=epsg:4326", 'getting layer projection string failed. Got ' + map.layers.prov_bound.projection.projString);
  });

  it('should set a layer projection', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');
    map.layers.prov_bound.projection = "+init=epsg:3857";
    assert.equal(map.layers.prov_bound.projection.projString, "+init=epsg:3857", 'setting map projection string failed. Got ' + map.projection.projString);
  });

  it('should set the map symbol set', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');
    map.setSymbolSet(symbolfile);
    assert.equal(map.setSymbolSet(symbolfile), 0, 'setting symbolset failed.');
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

    map.extent.project(map.projection, new mapserver.Projection("+init=epsg:3857"));

    var delta = Math.abs(map.extent.minx + 10018754.171394622).toFixed(6);
    assert.equal(delta, 0, 'reprojecting map extent minx failed, delta was ' + delta);
    delta = Math.abs(map.extent.miny + 5621521.486192066).toFixed(6);
    assert.equal(delta, 0, 'reprojecting map extent miny failed, delta was ' + delta);
    delta = Math.abs(map.extent.maxx -  10018754.171394622).toFixed(6);
    assert.equal(delta, 0, 'reprojecting map extent maxx failed, delta was ' + delta);
    delta = Math.abs(map.extent.maxy -  5621521.486192066).toFixed(6);
    assert.equal(delta, 0, 'reprojecting map extent maxy failed, delta was ' + delta);

  });

  it('should create a rect from the map extent', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');

    map.setExtent(-90, -45, 90, 45);

    var rect = new mapserver.Rect(map.extent);

    assert.equal(rect.minx, -90, 'rect does not match extent: minx failed');
    assert.equal(rect.miny, -45, 'rect does not match extent: miny failed');
    assert.equal(rect.maxx, 90, 'rect does not match extent: maxx failed');
    assert.equal(rect.maxy, 45, 'rect does not match extent: maxy failed');

    rect.minx = 0;
    assert.equal(map.extent.minx, -90, 'original rect should not be modified by copy modification');
    assert.equal(rect.minx, 0, 'copied rect should be modified');
  });

  it('should compute cell size and scaledenom', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');
    // test effect of recompute on cellsize and scaledenom
    map.recompute();
    var delta = Math.abs(map.cellsize - 0.6020066889632107).toFixed(6);
    assert.equal(delta, 0, 'recomputing map cellsize failed, got ' + map.cellsize);
    delta = Math.abs(map.scaledenom - 189621444.28093645).toFixed(6);
    assert.ok(delta, 0, 'recomputing map scaledenom failed, got '+ map.scaledemon);
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

    assert.equal(map.layers.prov_bound.connectiontype, mapserver.MS_SHAPEFILE, 'layer should have connectiontype MS_SHAPEFILE, got ' + map.layers.prov_bound.connectiontype);

    // layers should be accessible by name too
    assert.equal(map.layers['test'].name, 'test', 'layer should be accessible by name');

    assert.equal(map.layers['test'].status, mapserver.MS_ON, 'layer status should be ON');

    map.layers['test'].status = mapserver.MS_OFF;
    assert.equal(map.layers['test'].status, mapserver.MS_OFF, 'layer status should change to OFF');
    //test accessor for layer connection string
    assert.equal(map.layers['test'].connection, undefined, 'layer should have no connection on init');
    map.layers['test'].connection = 'myconnectionstring';
    assert.equal(map.layers['test'].connection, 'myconnectionstring', 'layer should have correct connection string after set');


    //should be able to replace layer with a valid string
    map.layers['test'].updateFromString('LAYER NAME "stringtest" TYPE LINE STATUS ON END');
    assert.equal(map.layers[1].name, 'stringtest', 'layer updated from string should have new name');
    assert.equal(map.layers['stringtest'].type, mapserver.MS_LAYER_LINE, 'layer updated from string should be type LINE');
    assert.equal(map.layers['stringtest'].status, mapserver.MS_ON, 'layer updated from string  should have status ON');
  });

  it('should create a new layer', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');

    assert.doesNotThrow(function() {
     layer = new mapserver.Layer('foo');
    }, Error, 'constructing a layer should not throw an error.');

    assert.ok(layer, 'layer should be an object');
    assert.equal(layer.name, 'foo', 'new layer should have a name');

    layer.name = 'bar';
    assert.equal(layer.name, 'bar', 'should be able to set a name on a new layer');

    assert.equal(layer.connectiontype, mapserver.MS_SHAPEFILE, 'layer should have connectiontype MS_SHAPEFILE');

    map.insertLayer(layer);
    assert.equal(map.layers['bar'].name, 'bar', 'the new layer should have a name');
    assert.equal(map.layers[map.layers.length-1].name, 'bar', 'the new layer should be at the end');

    layer = new mapserver.Layer('first');

    map.insertLayer(layer, 0);
    assert.equal(map.layers[0].name, 'first', 'the new layer should be at index 0');

    layer = new mapserver.Layer('middle');

    map.insertLayer(layer, 2);
    assert.equal(map.layers[2].name, 'middle', 'the new layer should be at index 2');
  });

  it('should get grid intersection coordinates', function() {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');

    assert.equal(map.layers.grid.connectiontype, mapserver.MS_GRATICULE, 'grid layer should have connectiontype MS_GRATICULE');

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

  if (mapserver.getVersionInt() >= 60400) {
    it('should get layer and map metadata', function() {
      assert.doesNotThrow(function() {
        map = new mapserver.Map(mapfile);
      }, Error, 'loading a valid map file should not throw an error.');

      assert.equal(map.metadata.foo, "bar", 'map should have metadata');

      map.metadata.bar = 'foo';
      assert.equal(map.metadata.bar, 'foo', 'should be able to set arbitrary metadata on the map');

      assert.doesNotThrow(function() {
       layer = new mapserver.Layer('metalayer');
      }, Error, 'constructing a layer should not throw an error.');
      map.insertLayer(layer);

      assert.equal(map.layers['metalayer'].metadata['foo'], undefined, 'layer should have empty metadata on init');

      map.layers['metalayer'].updateFromString('LAYER NAME "metatest" METADATA "foo" "bar" END END');
      assert.equal(map.layers['metatest'].metadata['foo'], "bar", 'layer updated from string containing metadata should have metadata');

      map.layers.metatest.metadata.foo = 'test';
      assert.equal(map.layers.metatest.metadata.foo, 'test', 'should be able to set metadata on a layer');

      map.layers.metatest.metadata.bar = 'foo';
      assert.equal(map.layers.metatest.metadata.bar, 'foo', 'should be able to create arbitrary metadata');

    });
  }

  it('should get and set layer properties', function() {
    assert.doesNotThrow(function() {
     layer = new mapserver.Layer('foo');
    }, Error, 'constructing a layer should not throw an error.');

    assert.equal(layer.minscaledenom, -1, 'default minscaledenom value should be -1');
    assert.equal(layer.maxscaledenom, -1, 'default maxscaledenom value should be -1');
    assert.equal(layer.units, mapserver.MS_METERS, "default units for layer should be " + mapserver.MS_METERS + ", got " + layer.units);

    layer.minscaledenom = 10.1;
    assert.equal(layer.minscaledenom, 10.1, 'setting minscaledenom to float should work');

    layer.minscaledenom = 9;
    assert.equal(layer.minscaledenom, 9, 'setting minscaledenom to int should work');

    layer.minscaledenom = -1;
    assert.equal(layer.minscaledenom, -1, 'setting minscaledenom to -1 should work');

    layer.maxscaledenom = 10.1;
    assert.equal(layer.maxscaledenom, 10.1, 'setting minscaledenom to float should work');

    layer.maxscaledenom = 10;
    assert.equal(layer.maxscaledenom, 10, 'setting minscaledenom to int should work');

    layer.maxscaledenom = -1;
    assert.equal(layer.maxscaledenom, -1, 'setting minscaledenom to -1 should work');

    layer.units = mapserver.MS_DD;
    assert.equal(layer.units, mapserver.MS_DD, "setting layer units should work");

  });

  if (mapserver.getVersionInt() >= 60400) {
    it('should convert a layer to a string', function() {
      assert.doesNotThrow(function() {
       layer = new mapserver.Layer('foo');
      }, Error, 'constructing a layer should not throw an error.');

      var defaultLayerText = 'LAYER\n  NAME "foo"\n  STATUS OFF\n  TILEITEM "location"\n  UNITS METERS\nEND # LAYER\n\n';

console.log(layer)

      assert.equal(layer.toString(), defaultLayerText, 'unexpected default layer text ' + layer.toString());
    });

    it('should be able to clone a layer using toString/updateFromString', function() {
      assert.doesNotThrow(function() {
        map = new mapserver.Map(mapfile);
      }, Error, 'loading a valid map file should not throw an error.');

      var grid = map.layers.grid.toString();

      var layer = new mapserver.Layer('grid2');
      layer.updateFromString(grid);
      assert.equal(map.layers.grid.toString(), layer.toString(), 'updateFromString produced different results');
    });
  }

  it('should draw a map', function(done) {
    var v = mapserver.getVersionInt();
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');
    fs.readFile(path.join(__dirname, 'data', 'test_buffer_'+v+'.png'), function(err, data) {
      map.drawMap(function(drawError, buffer) {
        if (drawError) {
          util.inspect(drawError, false, 0, true);
          assert.ok(false, 'Error drawing map.');
        } else {
          fs.writeFileSync(path.join(__dirname, 'data', 'test_out_'+v+'.png'), buffer);
          if (err) {
            throw new Error('failed to load test image');
          }
          assert.equal(data.toString('hex'), buffer.toString('hex'), 'map draw differed from sample image');
          done();
        }
      });
    });
  });

  it('should not segfault', function(done) {
    var v = mapserver.getVersionInt();
    assert.doesNotThrow(function() {
      map = new mapserver.Map(badmapfile);
    }, Error, 'loading a invalid map file should throw an error.');
    
    map.drawMap(function(drawError, buffer) {
      if (drawError) {
        console.log('error', drawError)
        assert.ok(true, 'Returned error from bad map')
        done();
      } else {
        fs.writeFileSync(path.join(__dirname, 'data', 'bad_out_'+v+'.png'), buffer);

        assert.ok(false, 'Did not receive error drawing map.');
      }
    });
  });


  it('should get the label cache', function(done) {
    assert.doesNotThrow(function() {
      map = new mapserver.Map(mapfile);
    }, Error, 'loading a valid map file should not throw an error.');

    map.setExtent(-150, 37, -50, 87);
    map.width = 1000;
    map.height = 600;

    var labels = map.getLabelCache();
    assert.equal(labels.length, 0, 'Is not empty before drawing.');
    map.drawMap(function(drawError) {
      if (drawError) {
        util.inspect(drawError, false, 0, true);
        assert.ok(false, 'Error drawing map.');
      } else {
        labels = map.getLabelCache();
        var numLabels = mapserver.getVersionInt() < 60500 ? 109 : 119;

        assert.equal(labels.length, numLabels, 'Does not have the right number of labels, expected (' + numLabels + ', got ' + labels.length);
        done();
      }
    });
  });

});
