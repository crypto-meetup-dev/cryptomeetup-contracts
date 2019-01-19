#pragma once
#include <eosiolib/asset.hpp>
typedef uint32_t time;

const auto BOS_CONTRACT = "eosio.token"_n;
const auto BOS_SYMBOL = eosio::symbol("BOS", 4);
const auto TOKEN_CONTRACT = "ncldwqxpkgav"_n;
const auto TOKEN_SYMBOL = eosio::symbol("CMU", 4);
const auto CMU_SYMBOL = eosio::symbol("CMU", 4);

static constexpr uint128_t MAGNITUDE = 1ll<<32;
static constexpr time refund_delay = 10;

static constexpr uint128_t LAND_TO_LAST = 60;
static constexpr uint128_t LAND_TO_POOL = 10;
static constexpr uint128_t LAND_TO_REF = 3;

static constexpr uint128_t PORTAL_TO_POOL = 5;
static constexpr uint128_t PORTAL_TO_PARENT = 2;


