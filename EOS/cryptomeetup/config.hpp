#pragma once
#include <eosiolib/asset.hpp>
typedef uint32_t time;

const auto EOS_CONTRACT = "eosio.token"_n;
const auto EOS_SYMBOL = eosio::symbol("EOS", 4);
const auto TOKEN_CONTRACT = "dacincubator"_n;
const auto TOKEN_SYMBOL = eosio::symbol("CMU", 4);
const auto CMU_SYMBOL = eosio::symbol("CMU", 4);

static constexpr uint128_t MAGNITUDE = 1ll<<32;
static constexpr time refund_delay = 60;