var util = require('util'),
    mapserver = require('./mapserver'),
    fs = require('fs'),
    path = require('path'),
    datadir = './data',
    mapfile = 'test.map';


var map = new mapserver.loadMap(path.join(datadir, mapfile),datadir);

console.log("map size: (" + map.width + "," + map.height + ")");
map.width = 600;
map.height = 600;
console.log("map size: (" + map.width + "," + map.height + ")");


var buffer = map.drawMap();
buffer = buffer.slice(0,buffer.length);
fs.writeFile('test_buffer.gif', buffer, function(err) {
  if (err) throw err;
  console.log('saved test_buffer.gif');
})