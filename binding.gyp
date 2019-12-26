{
  "targets": [
    {
      "target_name": "airtrash",
      "sources": [
        "airtrash.cc",
        "api.cc",
        "net.cc",
        "core.cc",
        "util.cc",
        "pipe.cc",
        "socket.cc"
      ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ],
}
