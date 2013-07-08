#ifndef __NODE_MAPSERVER_UTILS_H__
#define __NODE_MAPSERVER_UTILS_H__

#define ATTR(t, name, get, set)                                         \
    t->InstanceTemplate()->SetAccessor(String::NewSymbol(name), get, set);
    
#endif