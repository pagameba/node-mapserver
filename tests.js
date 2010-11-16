var util = require('util'),
    mapserver = require('./mapserver'),
    fs = require('fs');
    
var map = new mapserver.loadMap('/ms4w/apps/gmap/htdocs/gmap75_debug.map','/ms4w/apps/gmap/htdocs/');

console.log("map size: (" + map.width + "," + map.height + ")");

var buffer = new Buffer(map.drawMapRaw(), 'binary');
fs.writeFile('test_raw.gif', buffer, function(err) {
  if (err) throw err;
  console.log('saved test_raw.gif');
});

var buffer = map.drawMapBuffer();

fs.writeFile('test_buffer.gif', buffer, function(err) {
  if (err) throw err;
  console.log('saved test_buffer.gif');
})