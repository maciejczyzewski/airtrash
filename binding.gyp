{
  "targets": [
    {
      "target_name": "airtrash",
      "sources": [
        "airtrash.cc",
        "src/api.cc",
        "src/net.cc",
        "src/core.cc",
        "src/util.cc",
        "src/pipe.cc",
        "src/socket.cc"
      ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ],
}
