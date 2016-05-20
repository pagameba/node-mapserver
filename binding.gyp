{
  "targets": [
    {
      "target_name": "mapserver",
      "sources": [
          "src/mapserver.cpp",
          "src/ms_error.cpp",
          "src/ms_layer.cpp",
          "src/ms_layers.cpp",
          "src/ms_map.cpp",
          "src/ms_outputformat.cpp",
          "src/ms_point.cpp",
          "src/ms_projection.cpp",
          "src/ms_hashtable.cpp",
          "src/ms_rect.cpp"
      ],
      "libraries":["-lmapserver"],
      "include_dirs": [
          "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
