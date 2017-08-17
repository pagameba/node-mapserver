#include <mapserver.h>
#include <assert.h>
#include "mapserver.hpp"

namespace node_mapserver {

  extern "C" {

    static void Init(v8::Local<v8::Object> target) {

      MSError::Initialize(target);
      MSLayer::Initialize(target);
      MSLayers::Initialize(target);
      MSMap::Initialize(target);
      MSOutputFormat::Initialize(target);
      MSPoint::Initialize(target);
      MSProjection::Initialize(target);
      MSRect::Initialize(target);
      MSHashTable::Initialize(target);

      Nan::SetMethod(target, "getVersionInt", node_mapserver::getVersionInt);
      Nan::SetMethod(target, "getVersion", node_mapserver::getVersion);
      Nan::SetMethod(target, "supportsThreads", node_mapserver::supportsThreads);
      Nan::SetMethod(target, "resetErrorList", node_mapserver::resetErrorList);
      Nan::SetMethod(target, "getError", node_mapserver::getError);

      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_ALIGN_CENTER", MS_ALIGN_CENTER);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_ALIGN_LEFT", MS_ALIGN_LEFT);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_ALIGN_RIGHT", MS_ALIGN_RIGHT);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_AUTO", MS_AUTO);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_BITMAP", MS_BITMAP);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_BLUE", MS_BLUE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_CC", MS_CC);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_CJC_BEVEL", MS_CJC_BEVEL);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_CJC_BUTT", MS_CJC_BUTT);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_CJC_MITER", MS_CJC_MITER);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_CJC_NONE", MS_CJC_NONE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_CJC_ROUND", MS_CJC_ROUND);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_CJC_SQUARE", MS_CJC_SQUARE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_CJC_TRIANGLE", MS_CJC_TRIANGLE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_CL", MS_CL);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_CR", MS_CR);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_DB_CSV", MS_DB_CSV);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_DB_MYSQL", MS_DB_MYSQL);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_DB_ORACLE", MS_DB_ORACLE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_DB_POSTGRES", MS_DB_POSTGRES);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_DB_XBASE", MS_DB_XBASE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_DD", MS_DD);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_DEFAULT", MS_DEFAULT);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_DELETE", MS_DELETE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_DONE", MS_DONE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_EMBED", MS_EMBED);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_FAILURE", MS_FAILURE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_FALSE", MS_FALSE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_FEET", MS_FEET);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_FILE", MS_FILE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_FILE_MAP", MS_FILE_MAP);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_FILE_SYMBOL", MS_FILE_SYMBOL);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_GEOS_BEYOND", MS_GEOS_BEYOND);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_GEOS_CONTAINS", MS_GEOS_CONTAINS);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_GEOS_CROSSES", MS_GEOS_CROSSES);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_GEOS_DISJOINT", MS_GEOS_DISJOINT);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_GEOS_DWITHIN", MS_GEOS_DWITHIN);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_GEOS_EQUALS", MS_GEOS_EQUALS);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_GEOS_INTERSECTS", MS_GEOS_INTERSECTS);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_GEOS_OVERLAPS", MS_GEOS_OVERLAPS);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_GEOS_TOUCHES", MS_GEOS_TOUCHES);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_GEOS_WITHIN", MS_GEOS_WITHIN);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_GIANT", MS_GIANT);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_GRATICULE", MS_GRATICULE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_GREEN", MS_GREEN);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_HILITE", MS_HILITE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_IMAGEMODE_BYTE", MS_IMAGEMODE_BYTE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_IMAGEMODE_FLOAT32", MS_IMAGEMODE_FLOAT32);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_IMAGEMODE_INT16", MS_IMAGEMODE_INT16);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_IMAGEMODE_NULL", MS_IMAGEMODE_NULL);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_IMAGEMODE_PC256", MS_IMAGEMODE_PC256);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_IMAGEMODE_RGB", MS_IMAGEMODE_RGB);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_IMAGEMODE_RGBA", MS_IMAGEMODE_RGBA);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_INCHES", MS_INCHES);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_INLINE", MS_INLINE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_JOIN_ONE_TO_MANY", MS_JOIN_ONE_TO_MANY);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_JOIN_ONE_TO_ONE", MS_JOIN_ONE_TO_ONE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_KILOMETERS", MS_KILOMETERS);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_LARGE", MS_LARGE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_LAYER_ANNOTATION", MS_LAYER_ANNOTATION);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_LAYER_CHART", MS_LAYER_CHART);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_LAYER_CIRCLE", MS_LAYER_CIRCLE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_LAYER_LINE", MS_LAYER_LINE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_LAYER_POINT", MS_LAYER_POINT);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_LAYER_POLYGON", MS_LAYER_POLYGON);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_LAYER_QUERY", MS_LAYER_QUERY);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_LAYER_RASTER", MS_LAYER_RASTER);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_LAYER_TILEINDEX", MS_LAYER_TILEINDEX);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_LC", MS_LC);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_LL", MS_LL);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_LR", MS_LR);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_MAXCOLORS", MS_MAXCOLORS);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_MEDIUM", MS_MEDIUM);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_METERS", MS_METERS);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_MILES", MS_MILES);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_MULTIPLE", MS_MULTIPLE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_NAUTICALMILES", MS_NAUTICALMILES);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_NO", MS_NO);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_NORMAL", MS_NORMAL);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_OFF", MS_OFF);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_OGR", MS_OGR);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_ON", MS_ON);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_ORACLESPATIAL", MS_ORACLESPATIAL);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_PERCENTAGES", MS_PERCENTAGES);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_PIXELS", MS_PIXELS);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_PLUGIN", MS_PLUGIN);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_POSTGIS", MS_POSTGIS);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_QUERY_BY_ATTRIBUTE", MS_QUERY_BY_ATTRIBUTE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_QUERY_BY_INDEX", MS_QUERY_BY_INDEX);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_QUERY_BY_POINT", MS_QUERY_BY_POINT);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_QUERY_BY_RECT", MS_QUERY_BY_RECT);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_QUERY_BY_SHAPE", MS_QUERY_BY_SHAPE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_QUERY_IS_NULL", MS_QUERY_IS_NULL);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_QUERY_MULTIPLE", MS_QUERY_MULTIPLE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_QUERY_SINGLE", MS_QUERY_SINGLE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_RASTER", MS_RASTER);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_RED", MS_RED);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_SELECTED", MS_SELECTED);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_SHAPEFILE", MS_SHAPEFILE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_SHAPE_LINE", MS_SHAPE_LINE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_SHAPE_NULL", MS_SHAPE_NULL);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_SHAPE_POINT", MS_SHAPE_POINT);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_SHAPE_POLYGON", MS_SHAPE_POLYGON);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_SINGLE", MS_SINGLE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_SMALL", MS_SMALL);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_SUCCESS", MS_SUCCESS);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_TILED_SHAPEFILE", MS_TILED_SHAPEFILE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_TINY ", MS_TINY );
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_TRUE", MS_TRUE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_TRUETYPE", MS_TRUETYPE);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_UC", MS_UC);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_UL", MS_UL);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_UNKNOWN", MS_UNKNOWN);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_UNUSED_1", MS_UNUSED_1);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_UR", MS_UR);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_URL", MS_URL);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_WFS", MS_WFS);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_WMS", MS_WMS);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_XY", MS_XY);
      NODE_MAPSERVER_DEFINE_CONSTANT(target, "MS_YES", MS_YES);

    }
  }
}

NODE_MODULE(mapserver, node_mapserver::Init);
