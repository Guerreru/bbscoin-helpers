{
    "targets": [
        {
            "target_name": "bbscoinHelper",
            "sources": [
                'src/main.cc',
                '<!@(ls -1 src/**/*.cc)',
                "bbscoin/src/Common/Base58.cpp",
                "bbscoin/src/Common/StringTools.cpp",
                "bbscoin/src/Common/MemoryInputStream.cpp",
                "bbscoin/src/Common/StreamTools.cpp",
                "bbscoin/src/Common/StringOutputStream.cpp",
                "bbscoin/src/Common/StringView.cpp",
                "bbscoin/src/Common/VectorOutputStream.cpp",
                "bbscoin/src/Common/ScopeExit.cpp",
                "<!@(ls -1 bbscoin/src/crypto/*.c)",
                "<!@(ls -1 bbscoin/src/crypto/*.cpp)",
                "bbscoin/src/CryptoNoteCore/CryptoNoteBasicImpl.cpp",
                "bbscoin/src/CryptoNoteCore/CryptoNoteFormatUtils.cpp",
                "bbscoin/src/CryptoNoteCore/CryptoNoteSerialization.cpp",
                "bbscoin/src/CryptoNoteCore/CryptoNoteBasic.cpp",
                "bbscoin/src/CryptoNoteCore/CryptoNoteTools.cpp",
                "bbscoin/src/CryptoNoteCore/TransactionExtra.cpp",
                "bbscoin/src/CryptoNoteCore/TransactionUtils.cpp",
                "bbscoin/src/CryptoNoteCore/Transaction.cpp",
                "bbscoin/src/Serialization/BinaryInputStreamSerializer.cpp",
                "bbscoin/src/Serialization/BinaryOutputStreamSerializer.cpp",
                "bbscoin/src/Wallet/WalletUtils.cpp",
                "bbscoin/src/Wallet/WalletErrors.cpp",
                "bbscoin/src/Logging/ConsoleLogger.cpp",
                "bbscoin/src/Logging/CommonLogger.cpp",
                "bbscoin/src/Logging/ILogger.cpp",
            ],
            "include_dirs": [
                "src",
                "bbscoin/include",
                "bbscoin/src",
                "<!(node -e \"require('nan')\")",
            ],
            "conditions": [
                ["OS=='linux'", {
                    "sources": [
                        "<!@(ls -1 bbscoin/src/Platform/Linux/**/*.cpp)",
                        "<!@(ls -1 bbscoin/src/Platform/Posix/**/*.cpp)"
                    ],
                    "include_dirs": [
                        "bbscoin/src/Platform/Linux",
                        "bbscoin/src/Platform/Posix"
                    ]
                }],
                ["OS=='mac'", {
                    "sources": [
                        "<!@(ls -1 bbscoin/src/Platform/OSX/**/*.cpp)",
                        "<!@(ls -1 bbscoin/src/Platform/Posix/**/*.cpp)"
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
                    "-lboost_program_options"
                ]
            },
            "cflags_cc!": [ "-fno-exceptions", "-fno-rtti" ],
            "cflags": [
                "-maes",
            ],
            "cflags_cc": [
                "-maes",
                "-std=c++11",
                "-fexceptions",
                "-frtti",
                "-Wno-unused-private-field",
                "-Wno-missing-field-initializers",
                "-Wno-unused-const-variable",
                "-Wno-uninitialized",
                "-Wno-reorder"
            ],
            "xcode_settings": {
                "MACOSX_DEPLOYMENT_TARGET": "10.13",
                "OTHER_CFLAGS": [
                    "-maes",
                    "-fexceptions",
                    "-frtti",
                    "-Wno-unused-private-field",
                    "-Wno-missing-field-initializers",
                    "-Wno-unused-const-variable",
                    "-Wno-uninitialized",
                    "-Wno-reorder"
                ],
            }
        }
    ]
}
