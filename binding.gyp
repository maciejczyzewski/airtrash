{
  "targets": [
    {
      "target_name": "airtrash",
      "sources": [
        "airtrash.cc",
        "api.cc",
        "netsys.cc",
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