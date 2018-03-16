{
    "targets": [
        {
            "target_name": "bbscoinHelper",
            "sources": [
                'src/main.cc',
                '<!@(ls -1 src/**/*.cc)',
                # "bbscoin/external/miniupnpc/connecthostport.c",
                # "bbscoin/external/miniupnpc/igd_desc_parse.c",
                # "bbscoin/external/miniupnpc/minisoap.c",
                # "bbscoin/external/miniupnpc/miniupnpc.c",
                # "bbscoin/external/miniupnpc/miniwget.c",
                # "bbscoin/external/miniupnpc/minixml.c",
                # "bbscoin/external/miniupnpc/portlistingparse.c",
                # "bbscoin/external/miniupnpc/receivedata.c",
                # "bbscoin/external/miniupnpc/upnpcommands.c",
                # "bbscoin/external/miniupnpc/upnpreplyparse.c",
                "bbscoin/src/BlockchainExplorer/BlockchainExplorer.cpp",
                "bbscoin/src/BlockchainExplorer/BlockchainExplorerErrors.cpp",
                "bbscoin/src/Common/Base58.cpp",
                "bbscoin/src/Common/CommandLine.cpp",
                "bbscoin/src/Common/Util.cpp",
                "bbscoin/src/Common/StringTools.cpp",
                "bbscoin/src/Common/JsonValue.cpp",
                "bbscoin/src/Common/ConsoleTools.cpp",
                "bbscoin/src/Common/MemoryInputStream.cpp",
                "bbscoin/src/Common/PathTools.cpp",
                "bbscoin/src/Common/StdInputStream.cpp",
                "bbscoin/src/Common/StdOutputStream.cpp",
                "bbscoin/src/Common/StreamTools.cpp",
                "bbscoin/src/Common/StringOutputStream.cpp",
                "bbscoin/src/Common/StringView.cpp",
                "bbscoin/src/Common/VectorOutputStream.cpp",
                "bbscoin/src/Common/ScopeExit.cpp",
                # "bbscoin/src/ConnectivityTool/ConnectivityTool.cpp",
                "<!@(ls -1 bbscoin/src/crypto/*.c)",
                "<!@(ls -1 bbscoin/src/crypto/*.cpp)",
                "<!@(ls -1 bbscoin/src/CryptoNoteCore/*.cpp)",
                "<!@(ls -1 bbscoin/src/CryptoNoteProtocol/*.cpp)",
                # "bbscoin/src/Daemon/*.cpp",
                # "bbscoin/src/HTTP/*.cpp",
                "bbscoin/src/InProcessNode/InProcessNode.cpp",
                "bbscoin/src/InProcessNode/InProcessNodeErrors.cpp",
                "<!@(ls -1 bbscoin/src/Logging/*.cpp)",
                "bbscoin/src/NodeRpcProxy/NodeErrors.cpp",
                "bbscoin/src/NodeRpcProxy/NodeRpcProxy.cpp",
                # "bbscoin/src/P2p/*.cpp",
                # "bbscoin/src/Rpc/*.cpp",
                "<!@(ls -1 bbscoin/src/Serialization/*.cpp)",
                "bbscoin/src/Transfers/BlockchainSynchronizer.cpp",
                "bbscoin/src/Transfers/SynchronizationState.cpp",
                "bbscoin/src/Transfers/TransfersConsumer.cpp",
                "bbscoin/src/Transfers/TransfersContainer.cpp",
                "bbscoin/src/Transfers/TransfersSubscription.cpp",
                "bbscoin/src/Transfers/TransfersSynchronizer.cpp",
                "<!@(ls -1 bbscoin/src/Wallet/*.cpp)",
                "bbscoin/src/WalletLegacy/KeysStorage.cpp",
                "bbscoin/src/WalletLegacy/WalletLegacy.cpp",
                "bbscoin/src/WalletLegacy/WalletHelper.cpp",
                "bbscoin/src/WalletLegacy/WalletLegacySerializer.cpp",
                "bbscoin/src/WalletLegacy/WalletLegacySerialization.cpp",
                "bbscoin/src/WalletLegacy/WalletTransactionSender.cpp",
                "bbscoin/src/WalletLegacy/WalletUnconfirmedTransactions.cpp",
                "bbscoin/src/WalletLegacy/WalletUserTransactionsCache.cpp",
                "bbscoin/src/System/ContextGroup.cpp",
                "bbscoin/src/System/ContextGroupTimeout.cpp",
                "bbscoin/src/System/Event.cpp",
                "bbscoin/src/System/EventLock.cpp",
                "bbscoin/src/System/InterruptedException.cpp",
                "bbscoin/src/System/Ipv4Address.cpp",
                "bbscoin/src/System/TcpStream.cpp",
                "<!@(ls -1 bbscoin/src/JsonRpcServer/*.cpp)",
                "<!@(ls -1 bbscoin/src/PaymentGate/*.cpp)",
                # "<!@(ls -1 bbscoin/src/PaymentGateService/*.cpp)",
                # "bbscoin/src/Miner/*.cpp
            ],
            "include_dirs": [
                "src",
                "bbscoin/external/rocksdb/include",
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
