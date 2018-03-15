{
    "targets": [
        {
            "target_name": "bbscoinHelper",
            "sources": [
                'src/main.cc',
                '<!@(ls -1 src/**/*.cc)',
            ],
            "include_dirs": [
                "src",
                "bbscoin/include",
                "bbscoin/src",
                "<!(node -e \"require('nan')\")",
            ],
            "conditions": [
                ["OS=='linux'", {
                    "libraries": [
                        "<!(pwd)/bbscoin/build/src/*.a",
                    ],
                    "include_dirs": [
                        "bbscoin/src/Platform/Linux",
                        "bbscoin/src/Platform/Posix"
                    ]
                }],
                ["OS=='mac'", {
                    "libraries": [
                        "<!(pwd)/bbscoin/build/release/src/*.a",
                    ],
                    "include_dirs": [ 
                        "bbscoin/src/Platform/OSX",
                        "bbscoin/src/Platform/Posix"
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
