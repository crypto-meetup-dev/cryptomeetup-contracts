/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */
#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/transaction.hpp>
 
typedef double real_type;

using std::string;
using eosio::symbol_code;
using eosio::asset;
using eosio::extended_asset;
using eosio::symbol_code;
using eosio::permission_level;
using eosio::action;

namespace NFT {
    
    void create(name from) {
    }

    struct NFT {
        uint64_t id;
        name owner;
        uint64_t primary_key()const { return id; }
        void transfer(name to) {
            owner = to;
        }
    };    

    struct tradeable_NFT : public NFT {
        uint64_t price;        
    };
};