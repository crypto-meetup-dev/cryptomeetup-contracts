cleos wallet unlock --password PW5JaGpLtoM1vtD1WxiAC4RDsr82FRUczmKgocw1KJZqVahB4LZ1u
/usr/local/eosio/bin/eosiocpp -g signature.bp/signature.bp.abi  signature.bp/signature.bp.cpp
/usr/local/eosio/bin/eosiocpp -o signature.bp/signature.bp.wast signature.bp/signature.bp.cpp
# cleos -u http://api.eosbeijing.one set contract signature.bp signature.bp -p signature.bp@active