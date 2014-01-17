// v8
#include <v8.h>

// node
#include <node.h>
#include <node_buffer.h>
#include <node_version.h>

#include <mapserver.h>
#include <assert.h>

#include "mapserver.hpp"

namespace node_mapserver {
  
  using namespace node;
  using namespace v8;
  
  extern "C" {
    
    static void Init(Handle<Object> target) {
      
      MSError::Initialize(target);
      MSLayer::Initialize(target);
      MSLayers::Initialize(target);
      MSMap::Initialize(target);
      MSOutputFormat::Initialize(target);
      MSPoint::Initialize(target);
      MSProjection::Initialize(target);
      MSRect::Initialize(target);

      NODE_SET_METHOD(target, "getVersionInt", node_mapserver::getVersionInt);
      NODE_SET_METHOD(target, "getVersion", node_mapserver::getVersion);
      NODE_SET_METHOD(target, "supportsThreads", node_mapserver::supportsThreads);
      NODE_SET_METHOD(target, "resetErrorList", node_mapserver::resetErrorList);
      NODE_SET_METHOD(target, "getError", node_mapserver::getError);
      //NODE_SET_METHOD(target, "projectPoint", node_mapserver::projectPoint);
      
      NODE_DEFINE_CONSTANT(target, MS_ALIGN_CENTER);
      NODE_DEFINE_CONSTANT(target, MS_ALIGN_LEFT);
      NODE_DEFINE_CONSTANT(target, MS_ALIGN_RIGHT);
      NODE_DEFINE_CONSTANT(target, MS_AUTO);
      NODE_DEFINE_CONSTANT(target, MS_BITMAP);
      NODE_DEFINE_CONSTANT(target, MS_BLUE);
      NODE_DEFINE_CONSTANT(target, MS_CC);
      NODE_DEFINE_CONSTANT(target, MS_CJC_BEVEL);
      NODE_DEFINE_CONSTANT(target, MS_CJC_BUTT);
      NODE_DEFINE_CONSTANT(target, MS_CJC_MITER);
      NODE_DEFINE_CONSTANT(target, MS_CJC_NONE);
      NODE_DEFINE_CONSTANT(target, MS_CJC_ROUND);
      NODE_DEFINE_CONSTANT(target, MS_CJC_SQUARE);
      NODE_DEFINE_CONSTANT(target, MS_CJC_TRIANGLE);
      NODE_DEFINE_CONSTANT(target, MS_CL);
      NODE_DEFINE_CONSTANT(target, MS_CR);
      NODE_DEFINE_CONSTANT(target, MS_DB_CSV);
      NODE_DEFINE_CONSTANT(target, MS_DB_MYSQL);
      NODE_DEFINE_CONSTANT(target, MS_DB_ORACLE);
      NODE_DEFINE_CONSTANT(target, MS_DB_POSTGRES);
      NODE_DEFINE_CONSTANT(target, MS_DB_XBASE);
      NODE_DEFINE_CONSTANT(target, MS_DD);
      NODE_DEFINE_CONSTANT(target, MS_DEFAULT);
      NODE_DEFINE_CONSTANT(target, MS_DELETE);
      NODE_DEFINE_CONSTANT(target, MS_DONE);
      NODE_DEFINE_CONSTANT(target, MS_EMBED);
      NODE_DEFINE_CONSTANT(target, MS_FAILURE);
      NODE_DEFINE_CONSTANT(target, MS_FALSE);
      NODE_DEFINE_CONSTANT(target, MS_FEET);
      NODE_DEFINE_CONSTANT(target, MS_FILE);
      NODE_DEFINE_CONSTANT(target, MS_FILE_MAP);
      NODE_DEFINE_CONSTANT(target, MS_FILE_SYMBOL);
      //NODE_DEFINE_CONSTANT(target, MS_FOLLOW);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_BEYOND);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_CONTAINS);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_CROSSES);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_DISJOINT);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_DWITHIN);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_EQUALS);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_INTERSECTS);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_OVERLAPS);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_TOUCHES);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_WITHIN);
      NODE_DEFINE_CONSTANT(target, MS_GIANT);
      NODE_DEFINE_CONSTANT(target, MS_GRATICULE);
      NODE_DEFINE_CONSTANT(target, MS_GREEN);
      NODE_DEFINE_CONSTANT(target, MS_HILITE);
      NODE_DEFINE_CONSTANT(target, MS_IMAGEMODE_BYTE);
      NODE_DEFINE_CONSTANT(target, MS_IMAGEMODE_FLOAT32);
      NODE_DEFINE_CONSTANT(target, MS_IMAGEMODE_INT16);
      NODE_DEFINE_CONSTANT(target, MS_IMAGEMODE_NULL);
      NODE_DEFINE_CONSTANT(target, MS_IMAGEMODE_PC256);
      NODE_DEFINE_CONSTANT(target, MS_IMAGEMODE_RGB);
      NODE_DEFINE_CONSTANT(target, MS_IMAGEMODE_RGBA);
      NODE_DEFINE_CONSTANT(target, MS_INCHES);
      NODE_DEFINE_CONSTANT(target, MS_INLINE);
      NODE_DEFINE_CONSTANT(target, MS_JOIN_ONE_TO_MANY);
      NODE_DEFINE_CONSTANT(target, MS_JOIN_ONE_TO_ONE);
      NODE_DEFINE_CONSTANT(target, MS_KILOMETERS);
      NODE_DEFINE_CONSTANT(target, MS_LARGE);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_ANNOTATION);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_CHART);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_CIRCLE);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_LINE);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_POINT);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_POLYGON);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_QUERY);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_RASTER);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_TILEINDEX);
      NODE_DEFINE_CONSTANT(target, MS_LC);
      NODE_DEFINE_CONSTANT(target, MS_LL);
      NODE_DEFINE_CONSTANT(target, MS_LR);
      NODE_DEFINE_CONSTANT(target, MS_MAXCOLORS);
      NODE_DEFINE_CONSTANT(target, MS_MEDIUM);
      NODE_DEFINE_CONSTANT(target, MS_METERS);
      NODE_DEFINE_CONSTANT(target, MS_MILES);
      NODE_DEFINE_CONSTANT(target, MS_MULTIPLE);
      NODE_DEFINE_CONSTANT(target, MS_NAUTICALMILES);
      NODE_DEFINE_CONSTANT(target, MS_NO);
      NODE_DEFINE_CONSTANT(target, MS_NORMAL);
      NODE_DEFINE_CONSTANT(target, MS_OFF);
      NODE_DEFINE_CONSTANT(target, MS_OGR);
      NODE_DEFINE_CONSTANT(target, MS_ON);
      NODE_DEFINE_CONSTANT(target, MS_ORACLESPATIAL);
      NODE_DEFINE_CONSTANT(target, MS_PERCENTAGES);
      NODE_DEFINE_CONSTANT(target, MS_PIXELS);
      NODE_DEFINE_CONSTANT(target, MS_PLUGIN);
      NODE_DEFINE_CONSTANT(target, MS_POSTGIS);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_BY_ATTRIBUTE);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_BY_INDEX);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_BY_POINT);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_BY_RECT);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_BY_SHAPE);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_IS_NULL);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_MULTIPLE);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_SINGLE);
      NODE_DEFINE_CONSTANT(target, MS_RASTER);
      NODE_DEFINE_CONSTANT(target, MS_RED);
      NODE_DEFINE_CONSTANT(target, MS_SDE);
      NODE_DEFINE_CONSTANT(target, MS_SELECTED);
      NODE_DEFINE_CONSTANT(target, MS_SHAPEFILE);
      NODE_DEFINE_CONSTANT(target, MS_SHAPE_LINE);
      NODE_DEFINE_CONSTANT(target, MS_SHAPE_NULL);
      NODE_DEFINE_CONSTANT(target, MS_SHAPE_POINT);
      NODE_DEFINE_CONSTANT(target, MS_SHAPE_POLYGON);
      NODE_DEFINE_CONSTANT(target, MS_SINGLE);
      NODE_DEFINE_CONSTANT(target, MS_SMALL);
      NODE_DEFINE_CONSTANT(target, MS_SUCCESS);
      NODE_DEFINE_CONSTANT(target, MS_TILED_SHAPEFILE);
      NODE_DEFINE_CONSTANT(target, MS_TINY );
      NODE_DEFINE_CONSTANT(target, MS_TRUE);
      NODE_DEFINE_CONSTANT(target, MS_TRUETYPE);
      NODE_DEFINE_CONSTANT(target, MS_UC);
      NODE_DEFINE_CONSTANT(target, MS_UL);
      NODE_DEFINE_CONSTANT(target, MS_UNKNOWN);
      NODE_DEFINE_CONSTANT(target, MS_UNUSED_1);
      NODE_DEFINE_CONSTANT(target, MS_UR);
      NODE_DEFINE_CONSTANT(target, MS_URL);
      NODE_DEFINE_CONSTANT(target, MS_WFS);
      NODE_DEFINE_CONSTANT(target, MS_WMS);
      NODE_DEFINE_CONSTANT(target, MS_XY);
      NODE_DEFINE_CONSTANT(target, MS_YES);
      
    }
  }
}

NODE_MODULE(mapserver, node_mapserver::Init);