{
  "targets": [
    {
      "target_name": "mapserver",
      "sources": [ 
          "src/mapserver.cpp", 
          "src/ms_error.cpp",
          "src/ms_map.cpp",
          "src/ms_projection.cpp",
          "src/ms_rect.cpp"
     ],
      "libraries":["-lmapserver"]
    }
  ]
}
