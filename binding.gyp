
{
  "targets": [
    {
      "target_name": "rocketmq",
      "sources": [
        "src/rocketmq.cpp",
        "src/producer.cpp",
        "src/push_consumer.cpp",
        "src/consumer_ack.cpp",
        "src/consumer_ack_inner.cpp",
        "src/handle_message.cpp"
      ],
      "include_dirs": [
        "/Users/wuteng/Desktop/github/node/src",
        "deps/rocketmq/include",
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "defines": ["NAPI_VERSION=6"],
      "conditions": [
        ["OS==\"linux\"", {
          "libraries": [
            "<(module_root_dir)/deps/lib/librocketmq.a"
          ],
          "cflags_cc!": [ "-fno-exceptions", "-pthread", "-Wl,--no-as-needed", "-ldl" ],
          "cflags_cc": [ "-Wno-ignored-qualifiers" ],
          "cflags": [ "-std=c++11", "-g" ]
        }],
        ["OS==\"win\"", {
          "libraries": [
            "<(module_root_dir)/deps/lib/rocketmq-client-cpp.lib"
          ],
          "copies": [
            {
              "destination": "<(module_root_dir)/build/Release/",
              "files": [ "<(module_root_dir)/deps/lib/rocketmq-client-cpp.dll" ]
            }
          ]
        }],
        ["OS==\"mac\"", {
          "xcode_settings": {
            "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
          },
          "cflags!": [ "-fno-exceptions" ],
          "cflags_cc!": [ "-fno-exceptions", "-pthread", "-Wl,--no-as-needed", "-ldl" ],
          "cflags_cc": [ "-Wno-ignored-qualifiers" ],
          "cflags": [ "-std=c++11", "-stdlib=libc++" ]
        }]
      ]
    }
  ]
}
