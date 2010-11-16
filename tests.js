var util = require('util'),
    mapserver = require('./mapserver'),
    fs = require('fs');
    
var map = new mapserver.loadMap('/ms4w/apps/gmap/htdocs/gmap75_debug.map','/ms4w/apps/gmap/htdocs/');

console.log("map size: (" + map.width + "," + map.height + ")");

// var buffer = new Buffer(map.drawMap(), 'binary');
var buffer = map.drawMap();

fs.writeFile('test.gif', buffer, function(err) {
  if (err) throw err;
  // console.log('drawmap done.');
});
