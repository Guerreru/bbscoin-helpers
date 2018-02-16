{
    "targets": [
        {
            "target_name": "bbscoinHelper",
            "sources": [
                "src/main.cc",
                "src/Bindings/WalletBinding.cc",
                "src/AsyncWorkers/GenerateWalletAsyncWorker.cc",
                "src/AsyncWorkers/FilterOutputsAsyncWorker.cc",
            ],
            "include_dirs": [
                "src",
                "bbscoin/include",
                "bbscoin/src",
                "<!(node -e \"require('nan')\")",
            ],
            "libraries": [
                "<!(pwd)/bbscoin/build/release/src/*.a",
            ],
            "conditions": [
                ["OS=='linux'", {
                    "include_dirs": [
                        "bbscoin/src/Platform/Linux",
                        "bbscoin/src/Platform/Posix"
                    ]
                }],
                ["OS=='mac'", {
                    "include_dirs": [ 
                        "bbscoin/src/Platform/OSX",
                        "bbscoin/src/Platform/Posix"
                    ]
                }],
                ["OS=='win'", {
                    "include_dirs": [ 
                        "bbscoin/src/Platform/Windows" 
                    ]
                }]
            ], 
            "link_settings": {
                "libraries": [
                    "-lboost_filesystem",
                    "-lboost_system",
                    "-lboost_date_time",
                ]
            },
            "cflags_cc!": [ "-fno-exceptions", "-fno-rtti" ],
            "cflags_cc": [
                  "-std=c++0x",
                  "-fexceptions",
                  "-frtti",
                  "-Wno-unused-private-field",
            ],
            "xcode_settings": {
                "MACOSX_DEPLOYMENT_TARGET": "10.13",
                "OTHER_CFLAGS": [
                    "-fexceptions",
                    "-frtti",
                    "-Wno-unused-private-field",
                ],
            }
        }
    ]
}
