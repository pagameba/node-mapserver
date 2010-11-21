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
    path = require("path");

var port = 8080;

var maps = {};

var map_config = {
  gmap: '/ms4w/apps/gmap/htdocs/gmap75.map'
};

for (var key in map_config) {
  var mappath = map_config[key];
  maps[key] = mapserver.loadMap(mappath, path.dirname(mappath));
}

http.createServer(function(request, response) {
  var parsed = url.parse(request.url, true);
  var components = parsed.pathname.split('/').splice(1);
  console.log(util.inspect(components));
  if (components.length == 0) {
    // root
    response.writeHead(200, {
      'Content-Type':'text/plain'
    });
    response.end('ok');
  } else if (components.length == 1 && maps[components[0]] != undefined) {
    var map = maps[components[0]];
    if (parsed.query.mapsize) {
      var mapsize = parsed.query.mapsize.split(' ');
      map.width = mapsize[0];
      map.height = mapsize[1];
    }
    response.writeHead(200, {
      'Content-Type':'image/gif'
    });
    var buffer = map.drawMap();
    response.end(buffer);
  } else {
    response.writeHead(404, {});
    response.end('File not found.');
  }
}).listen(port);
util.log("mapserv.js running on port " + port );  
