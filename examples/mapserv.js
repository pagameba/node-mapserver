/**
 * A simplified mapserv clone using node-mapserver. Only provides mode=map
 * support (and hence mode=map is not required in the url).  Maps are
 * preconfigured and so map= is ignored in the url.  Supported parameters are:
 * - layers=<layer list> - space-separated list of layers to turn on, or all
 *   for all layers
 * - mapext=<minx> <miny> <maxx> <maxy> - the geospatial extent of the map 
 *   image to draw
 * - mapsize=<width> <height> - the size of the map image to draw
 */

var util = require('util'),
    http = require("http"),  
    url = require("url"),  
    mapserver = require("../mapserver"),
    path = require("path"),
    fs = require('fs'),
    Cache = require('./cache').Cache,
    md5 = require('../deps/hashlib/build/default/hashlib').md5;

var port = 8080;

var maps = {};

var map_config = {
  gmap: '/ms4w/apps/gmap/htdocs/gmap75.map',
  premium: '/ms4w/data/maps4ms_premium_2009.2_mercator/map_print/maps4ms_premium_2009.2_mercator_canada.map'
};

for (var key in map_config) {
  var mappath = map_config[key];
  try {
    maps[key] = mapserver.loadMap(mappath, path.dirname(mappath));
  } catch (e) {
    console.log(mapserver.getError());
  }
}

var cache = new Cache(100000);

http.createServer(function(request, response) {
  var parsed = url.parse(request.url, true);
  var components = parsed.pathname.split('/').splice(1);
  var page = components[0];
  if (page == '') {
    // root, serve HTML up
    var html = path.normalize(path.join(__dirname,'mapserv.html'));
    fs.readFile(html, function(err, buffer) {
      if (!err) {
        response.writeHead(200, {
          'Content-Type':'text/html'
        });
        response.end(buffer);
      } else {
        response.writeHead(400, {
          'Content-type':'text/html'
        });
        response.end('File not found');
      }
    });
  } else if (page == 'favicon.ico') {
    console.log('serving up mapserv.ico');
    var html = path.normalize(path.join(__dirname,'mapserv.ico'));
    fs.readFile(html, function(err, buffer) {
      if (!err) {
        response.writeHead(200, {
          'Content-Type':'image/x-icon'
        });
        response.end(buffer);
      } else {
        response.writeHead(400, {
          'Content-type':'text/html'
        });
        response.end('File not found');
      }
    });
    
  } else if (maps[page] != undefined) {
    var key = md5(parsed.search);
    console.log(parsed.search+' : '+key);
    var o = cache.get(key);
    if (o) {
      console.log('cache-hit ' + key);
      response.writeHead(200, {
        'Content-Type': o.data.mimetype
      });
      response.end(o.data.buffer);
      
    } else {
      console.log('cache-miss ' + key);
      var map = maps[page].copy();
      if (parsed.query.map_size) {
        var mapsize = parsed.query.map_size.split(' ');
        map.width = mapsize[0];
        map.height = mapsize[1];
      }
      if (parsed.query.mapext) {
        var extent = parsed.query.mapext.split(' ');
        if (extent.length == 4) {
          map.setExtent(parseInt(extent[0]),
            parseInt(extent[1]),
            parseInt(extent[2]),
            parseInt(extent[3])
          );
        }
      }
      if (parsed.query.layers) {
        var layers = parsed.query.layers.split(' ');
        if (layers.length == 1 && layers[0] == 'all') {
          for (var i=0; i<map.layers.length; i++) {
            if (map.layers[i].status != mapserver.MS_DELETE) {
              map.layers[i].status = mapserver.MS_ON;
            }
          }
        } else {
          for (var i=0; i<map.layers.length; i++) {
            var layer = map.layers[i];
            if (layers.indexOf(layer.name) != -1 && layer.status != mapserver.MS_DELETE) {
              layer.status = mapserver.MS_ON;
            } else if (layer.status != mapserver.MS_DELETE && layer.status != mapserver.MS_DEFAULT) {
              layer.status = mapserver.MS_OFF;
            }
          }
        }
      }
    
      if (parsed.query.map_imagetype) {
        try {
          map.selectOutputFormat(parsed.query.map_imagetype);
          map.imagetype = parsed.query.map_imagetype;
        } catch(e) {
          console.log('Error selecting output format ' + parsed.query.map_imagetype + ' ' + e);
        }
      }
    
      map.drawMap(function(err, buffer) {
        if (err) {
          console.log(err);
          response.writeHead(200, {
            'Content-Type':'text/plain'
          });
          response.end("MapServer Error: " + err.code + " ("+err.codeStr+"): " + err.message + ' in ' + err.routine);
        } else {
          cache.set(key, {mimetype: map.mimetype, buffer: buffer});
          response.writeHead(200, {
            'Content-Type': map.mimetype
          });
          response.end(buffer);
        }
      });
    }
  } else {
    response.writeHead(404, {});
    response.end('File not found.');
  }
}).listen(port);
util.log("mapserv.js running on port " + port );  
