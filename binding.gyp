{
  "targets": [
    {
      "target_name": "airtrash",
      "sources": [ "airtrash.cc", "api.cc", "netsys.cc" ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ],
}
