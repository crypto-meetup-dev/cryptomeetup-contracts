/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */
#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/transaction.hpp>
 
#define TOKEN_CONTRACT N(dacincubator)
#define TOKEN_SYMBOL S(4, PLT)

typedef double real_type;
typedef uint8_t card ;

using std::string;
using eosio::symbol_name;
using eosio::asset;
using eosio::symbol_type;
using eosio::permission_level;
using eosio::action;

namespace meetup {
    
    void create(account_name from, string& memo) {
    }



    struct meetup {
        uint64_t id;
        uint64_t staked; 
        uint64_t parent;       
        string name;
        string hyperlink;      
        account_name owner;
        time st, ed;        
        uint64_t primary_key()const { return id; }
    };
};