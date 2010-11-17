Overview
========

node-mapserver is a node.js extension to the [mapserver](http://mapserver.org/ "MapServer") open source web mapping engine.  It allows you to access all the mapserver goodness for rendering map images in a node.js server environment, allowing you to build high-performance, low-latency map server.

Getting node-mapserver
======================


Building node-mapserver:
========================

on linux:

CXX="g++ -I/path/to/mapserver -L/path/to/mapserver" node-waf configure

on osx:

Using node-mapserver:
=====================

API Documentation
=================

Module Methods:
---------------

**getVersion** returns the version information for mapserver as a string value

Example:

    require('mapserver').getVersion();
    
    MapServer version 5.6.5 OUTPUT=GIF OUTPUT=PNG OUTPUT=WBMP OUTPUT=SVG SUPPORTS=PROJ SUPPORTS=AGG SUPPORTS=CAIRO SUPPORTS=FREETYPE SUPPORTS=ICONV SUPPORTS=WMS_SERVER SUPPORTS=WMS_CLIENT SUPPORTS=WFS_SERVER SUPPORTS=WFS_CLIENT SUPPORTS=WCS_SERVER SUPPORTS=SOS_SERVER SUPPORTS=GEOS INPUT=EPPL7 INPUT=POSTGIS INPUT=OGR INPUT=GDAL INPUT=SHAPEFILE

**getVersionInt** returns the version number for mapserver as an integer value.

Example:

    require('mapserver').getVersionInt();
    
    50605

**loadMap** returns a Map object for the map file at the specified path.

Required arguments are:

 * mapfile - path to the map file to open, relative to the process working directory
 * path - path for mapserver with relative paths defined in the map file, assumed to be the process working directory if not set
 
Returns a Map object.

Throws an Error exception if the map could not be loaded.  Use getError() to get specific details about the error that happened.

Example:

    var mapserver = require('mapserver');
    try {
        var map = mapserver.loadMap('test.map');
    } catch (e) {
        var err = mapserver.getError();
        console.log('Error ' + err.code + '('+ err.codeStr +') in ' + err.routine + ':' + err.message);
    }

**getError** returns the next error in the mapserver error stack or undefined if there are no more errors.

Example: see loadMap() example.

Error
-----

An object represents an error in the mapserver error stack.  The error object has the following read-only properties:

 * *code* - read-only integer, the mapserver error code
 * *codeStr* - read-only string, a human readable version of the error code
 * *message* - read-only string, a descriptive message associated with the error
 * *routine* - read-only string, the mapserver function that triggered the error
 
Example: see loadMap() example

Map
---

A map object represents a parsed mapserver map file and has the following methods and properties for manipulating it.

Properties:

 * *width* - read/write integer, the width in pixels of the image to generate for this map when it is drawn
 * *height* - read/write integer, the height in pixels of the image to generate for this map when it is drawn
 
Methods:

**drawMap** renders an image based on the Map's current state and returns a Buffer object.
