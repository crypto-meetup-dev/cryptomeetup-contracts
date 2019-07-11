#cleos -u http://api-direct.eosasia.one get table eosio minakokojima delband
#cleos -u http://api-direct.eosasia.one set account permission cryptomeetup active '{"threshold": 1,"keys": [{"key": "EOS5x2vQgnAfvn6571VUFu43xVHTWzoysuQ875npKacf7YJqDKF8H","weight": 1}],"accounts": [{"permission":{"actor":"cryptomeetup","permission":"eosio.code"},"weight":1}]}' owner -p cryptomeetup
#cleos -u http://api-direct.eosasia.one get table dacincubator 222222dragon accounts

cleos -u http://api.eosbeijing.one push action cryptomeetup clear '[]' -p cryptomeetup@active

curl --request POST \
  --url https://nodes.get-scatter.com:443/v1/chain/get_table_by_scope \
  --header 'accept: application/json' \
  --header 'content-type: application/json' \
  --data '{"code":"cryptomeetup","table":"checkins"}'

curl --request POST \
  --url https://nodes.get-scatter.com:443/v1/chain/get_table_by_scope \
  --header 'accept: application/json' \
  --header 'content-type: application/json' \
  --data '{"code":"cryptomeetup","table":"players"}'

curl --request POST \
  --url https://nodes.get-scatter.com:443/v1/chain/get_table_by_scope \
  --header 'accept: application/json' \
  --header 'content-type: application/json' \
  --data '{"code":"cryptomeetup","table":"refunds"}'

curl --request POST \
  --url https://nodes.get-scatter.com:443/v1/chain/get_table_by_scope \
  --header 'accept: application/json' \
  --header 'content-type: application/json' \
  --data '{"code":"cryptomeetup","table":"voters"}'

/usr/local/eosio/bin/eosiocpp -g cryptomeetup/cryptomeetup.abi  cryptomeetup/cryptomeetup.cpp
/usr/local/eosio/bin/eosiocpp -o cryptomeetup/cryptomeetup.wast cryptomeetup/cryptomeetup.cpp
#cleos -u http://api.eosbeijing.one set contract cryptomeetup cryptomeetup -p cryptomeetup@owner



cleos -u http://api.eosbeijing.one push action cryptomeetup refund '[ "minakokojima"]' -p minakokojima@active

#cleos -u http://api.eosbeijing.one push action dacincubator transfer '[ "cryptomeetup", "222222dragon", "1.9000 CMU" "unstake"]' -p cryptomeetup@owner


# cleos -u http://api.eosbeijing.one push action cryptomeetup checkin '[ "minakokojima", "0196d5b5d9ec1bc78ba927d2db2cb327d836f002601c77bd8c3f144a07ddc737"]' -p minakokojima@active
# cleos -u http://api-direct.eosasia.one get table cryptomeetup minakokojima checkins


cleos -u http://api-direct.eosasia.one get table cryptomeetup minakokojima refunds

#cleos -u http://api.eosbeijing.one push action dacincubator transfer '[ "minakokojima", "cryptomeetup", "1.0000 CMU" "stake"]' -p minakokojima@active

#cleos -u http://api.eosbeijing.one push action eosio.token transfer '[ "minakokojima", "suzebiao5555", "5.0000 EOS" ""]' -p minakokojima@active


#cleos -u http://api.eosbeijing.one get table -l 65536 dacincubator societieseos accounts

#cleos -u http://api.eosbeijing.one get table -l 65536 cryptomeetup minakokojima voter



#cleos -u http://api.eosbeijing.one push action eosio.token transfer '[ "minakokojima", "thanosjw1314", "5.1000 EOS" "Thank you for test cryptothrone. Have a nice day!"]' -p minakokojima@active
#

#cleos -u http://api.eosbeijing.one push action eosio.token transfer '[ "cryptomeetup", "wajueji12345", "98.0000 EOS" "congratuation."]' -p cryptomeetup@active


#

cleos -u http://api.eosbeijing.one push action cryptomeetup airdrop '[ "blackholebug", "1.0000 CMU"]' -p cryptomeetup@owner

#cleos -u http://api.eosbeijing.one push action cryptomeetup unstake '[ "minakokojima", "20000"]' -p minakokojima@active

#cleos -u http://api.eosbeijing.one push action cryptomeetup airdrop '[ "prajjmqyxcfz", "9000000"]' -p cryptomeetup@active

#cleos -u http://api.eosbeijing.one push action eosio.token transfer '[ "minakokojima", "cryptomeetup", "5.0000 EOS" "buy_land 45"]' -p minakokojima@active

#cleos -u http://api.eosbeijing.one push action cryptomeetup newland '[ "eosotcbackup", "1.0000 EOS" ]' -p cryptomeetup@owner eosotcbackup@active

#cleos -u http://api.eosbeijing.one get table -l 65536 crazytown.bp crazytown.bp bag

#cleos -u http://api.eosbeijing.one get table -l 65536 dacincubator cryptomeetup account

#cleos -u http://api.eosbeijing.one get table -l 3 cryptomeetup cryptomeetup land

#cleos -u http://api-direct.eosasia.one push action eosio.token transfer '[ "minakokojima", "knowncarried", "360.2802 EOS", "" ]' -p minakokojima@active

# push action eosio.token transfer '[ "eosotcbackup", "cryptomeetup", "0.0330 EOS", "buy 0" ]' -p eosotcbackup@active

#cleos -u http://api.eosbeijing.one push action cryptomeetup newbag '[ "minakokojima", "0.0100 EOS" ]' -p cryptomeetup@active minakokojima@active


#cleos -u http://api.eosbeijing.one push action cryptomeetup newbag '[ "cryptomeetup", "0.0100 EOS" ]' -p cryptomeetup@active cryptomeetup@active



#cleos -u http://api.eosbeijing.one get table cryptomeetup cryptomeetup bag
#cleos -u http://api.eosbeijing.one get table cryptomeetup cryptomeetup global

#cleos -u http://api.eosbeijing.one push action cryptomeetup init '[]' -p cryptomeetup@owner


#cleos -u http://api.eosbeijing.one push action cryptomeetup init '["0196d5b5d9ec1bc78ba927d2db2cb327d836f002601c77bd8c3f144a07ddc737"]' -p cryptomeetup@active

#cleos -u http://api-direct.eosasia.one push action cryptomeetup reveal '["0196d5b5d9ec1bc78ba927d2db2cb327d836f002601c77bd8c3f144a07ddc737","0196d5b5d9ec1bc78ba927d2db2cb327d836f002601c77bd8c3f144a07ddc737"]' -p cryptomeetup@active
#cleos -u http://api-direct.eosasia.one push action cryptomeetup reveal '["7a2dee663b23200c63fe195f76c9cb0f255ff623e578134060dd3fbc9dfd06a2","7f86a67f22e04c1ff4a21ade119ec95f8e618b06588b160ee68035544eb40014"]' -p cryptomeetup@active
#cleos -u https://api.franceos.fr:443 push action cryptomeetup reveal '["7a2dee663b23200c63fe195f76c9cb0f255ff623e578134060dd3fbc9dfd06a2","7f86a67f22e04c1ff4a21ade119ec95f8e618b06588b160ee68035544eb40014"]' -p cryptomeetup@active


#cleos -u http://api-direct.eosasia.one push action cryptomeetup clear '[]' -p cryptomeetup@active
 
#https://api.franceos.fr:443

#cleos -u http://api-direct.eosasia.one push action cryptomeetup setwhitelist '["PXL", "dacincubator"]' -p cryptomeetup@active

#cleos -u http://api-direct.eosasia.one push action cryptomeetup rmwhitelist '["PXL"]' -p cryptomeetup@active

#cleos -u http://api-direct.eosasia.one push action eosio.token transfer '[ "minakokojima", "cryptomeetup", "0.0010 EOS", "0.0010 PXL" ]' -p minakokojima@active

#cleos -u http://api.eosbeijing.one push action dacincubator transfer '[ "minakokojima", "cryptomeetup", "1.0000 CMU", "make_profit" ]' -p minakokojima@active

#cleos -u http://api-direct.eosasia.one push action eosio.token transfer '[ "minakokojima", "cryptomeetup", "0.1000 EOS", "buy_land 45" ]' -p minakokojima@active

#cleos -u http://api-direct.eosasia.one get table cryptomeetup cryptomeetup global

#cleos -u http://api-direct.eosasia.one get table cryptomeetup cryptomeetup player


#

#cleos -u http://api-direct.eosasia.one get table cryptomeetup cryptomeetup bags

#cleos -u http://api-direct.eosasia.one get table -l 65536 eosotcbackup eosio.token order


# cleos -u https://api-kylin.eosasia.one push action pomelodex111 test '[]' -p minakokojima@active


# cleos -u https://api-kylin.eosasia.one set contract pomelodex111 pomelo -p pomelodex111@active
# cleos -u https://api-kylin.eosasia.one push action eosio.token transfer '[ "minakokojima", "pomelodex111", "1.0000 EOS", "buy,happyeosslot,HPY,2.0000" ]' -p minakokojima@active
# cleos -u https://api-kylin.eosasia.one set contract pomelodex111 pomelo -p pomelodex111@active
# cleos -u https://api-kylin.eosasia.one push action eosio.token transfer '[ "minakokojima", "happyeosslot", "1.0000 EOS", "buy" ]' -p minakokojima@active

# cleos -u https://api-kylin.eosasia.one push action eosio.token transfer '[ "minakokojima", "pomelodex111", "1.0000 EOS", "buy,happyeosslot,HPY,2.0000" ]' -p minakokojima@active
# /usr/local/eosio/bin/eosiocpp -g pomelo/pomelo.abi pomelo/pomelo.cpp
# cleos -u https://api-kylin.eosasia.one set contract pomelodex111 pomelo -p pomelodex111@active
# cleos -u https://api-kylin.eosasia.one system newaccount --stake-net '5.0000 EOS' --stake-cpu '1.0000 EOS' --buy-ram '20.0000 EOS' minakokojima pomelodex111 EOS5fY2dmpfXmzXN1DEJ6VfvhhCr55ZpE9MyTd9eX7Cw2EmzJcnFM EOS781pFN6TgUkuCDqmCdyxcp1cnJdtg3DYfajbAba6mvHvrFNuDy


# buy,happyeosslot,HPY,2.0000



#cleos wallet unlock --password PW5JaGpLtoM1vtD1WxiAC4RDsr82FRUczmKgocw1KJZqVahB4LZ1u
#
#/usr/local/eosio/bin/eosiocpp -g pomelo/pomelo.abi pomelo/pomelo.cpp


#
#cleos -u http://api-direct.eosasia.one set contract kyubeydex.bp pomelo -p kyubeydex.bp@active

#cleos -u http://api-direct.eosasia.one set contract cryptomeetup cryptomeetup -p cryptomeetup@active

#cleos -u http://api-direct.eosasia.one push action cryptomeetup clear '' -p cryptomeetup@active

#
#cleos -u http://api-direct.eosasia.one push action cryptomeetup setslogan '[ "minakokojima", "0", "EOS Asia" ]' -p minakokojima@active


#cleos -u http://api-direct.eosasia.one push action kyubeydex.bp setwhitelist '["PXL", "dacincubator"]' -p kyubeydex.bp@active

#cleos -u http://api-direct.eosasia.one push action cryptomeetup rmwhitelist '["PXL"]' -p cryptomeetup@active

#cleos -u http://api-direct.eosasia.one push action eosio.token transfer '[ "minakokojima", "cryptomeetup", "0.0010 EOS", "0.0010 PXL" ]' -p minakokojima@active

#cleos -u http://api-direct.eosasia.one push action dacincubator transfer '[ "eosotcbackup", "cryptomeetup", "0.0010 PXL", "0.0010 EOS" ]' -p eosotcbackup@active

#cleos -u http://api-direct.eosasia.one push action eosio.token transfer '[ "minakokojima", "cryptomeetup", "0.0100 EOS", "0.0100 PUB" ]' -p minakokojima@active


#
#cleos -u http://api-direct.eosasia.one push action eosio.token transfer '[ "minakokojima", "kyubeydex.bp", "0.0100 EOS", "0.0100 KBY" ]' -p minakokojima@active
#
# 
#cleos -u http://api-direct.eosasia.one push action dacincubator transfer '[ "eosotcbackup", "kyubeydex.bp", "0.0050 PXL", "0.0050 EOS" ]' -p eosotcbackup@active kyubeydex.bp@active



#cryptomeetup PXL whitelist


#cleos -u http://api-direct.eosasia.one get table cryptomeetup minakokojima voter_table

#cleos -u http://api-direct.eosasia.one get table dacincubator cryptomeetup accounts

#cleos -u http://api-direct.eosasia.one get table dacincubator cryptomeetup accounts

#cleos -u http://api-direct.eosasia.one get table dacincubator 2222222dragon accounts


#cleos -u http://api-direct.eosasia.one get table kyubeydex.bp PXL buyorder

#


#cleos -u http://api-direct.eosasia.one get table cryptomeetup cryptomeetup buyorder

#cleos -u http://api-direct.eosasia.one get table tokendapppub PUB games


# cleos -u https://api-kylin.eosasia.one push action pomelodex111 test '[]' -p minakokojima@active


# cleos -u https://api-kylin.eosasia.one set contract pomelodex111 pomelo -p pomelodex111@active
# cleos -u https://api-kylin.eosasia.one push action eosio.token transfer '[ "minakokojima", "pomelodex111", "1.0000 EOS", "buy,happyeosslot,HPY,2.0000" ]' -p minakokojima@active
# cleos -u https://api-kylin.eosasia.one set contract pomelodex111 pomelo -p pomelodex111@active
# cleos -u https://api-kylin.eosasia.one push action eosio.token transfer '[ "minakokojima", "happyeosslot", "1.0000 EOS", "buy" ]' -p minakokojima@active

# cleos -u https://api-kylin.eosasia.one push action eosio.token transfer '[ "minakokojima", "pomelodex111", "1.0000 EOS", "buy,happyeosslot,HPY,2.0000" ]' -p minakokojima@active
# /usr/local/eosio/bin/eosiocpp -g pomelo/pomelo.abi pomelo/pomelo.cpp
# cleos -u https://api-kylin.eosasia.one set contract pomelodex111 pomelo -p pomelodex111@active
# cleos -u https://api-kylin.eosasia.one system newaccount --stake-net '5.0000 EOS' --stake-cpu '1.0000 EOS' --buy-ram '20.0000 EOS' minakokojima pomelodex111 EOS5fY2dmpfXmzXN1DEJ6VfvhhCr55ZpE9MyTd9eX7Cw2EmzJcnFM EOS781pFN6TgUkuCDqmCdyxcp1cnJdtg3DYfajbAba6mvHvrFNuDy


# buy,happyeosslot,HPY,2.0000
