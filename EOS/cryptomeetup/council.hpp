/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */
#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/transaction.hpp>
#include "utils.hpp"
 
// #define TOKEN_CONTRACT N(dacincubator)
// #define TOKEN_SYMBOL S(4, PLT)


typedef double real_type;

using namespace eosio ;

using std::string;
using eosio::symbol_code;
using eosio::asset;
using eosio::extended_asset;
using eosio::symbol_code;
using eosio::permission_level;
using eosio::action;
using eosio::time_point_sec;
using eosio::name;

CONTRACT council : public eosio::contract {
    public: 
        // using contract::contract;
        council( name receiver, name code, datastream<const char*> ds ) :
            contract( receiver, code, ds ),
            _voters( receiver, 1 ),
            _proxies( receiver, 1 ),
            _council( receiver, 1 ){}
    
    
    ACTION stake(account_name from) {
        auto itr = _voters.find(from);
        if (itr == _voters.end()) {
            
        } else {

        }
    }

    ACTION unstake(account_name from) {

    }    

    ACTION vote(account_name from) {

    }

    
    ACTION run_for_proxy(account_name from) {

    }    

    ACTION run_for_council(account_name from) {

    }        

   TABLE voter_info {
        account_name owner = 0; /// the voter
        account_name proxy = 0; /// the proxy set by the voter, if any
        account_name council;   /// the producers approved by this voter if no proxy set
        uint64_t     staked = 0;
        time_point_sec  last_vote_time ; 

        auto primary_key()const { return owner; }
    };       

    TABLE proxy_info {
        account_name owner = 0; /// the voter
        account_name council; /// the producers approved by this voter if no proxy set
        uint64_t     staked = 0;
        uint64_t     delegated_staked = 0;
        time_point_sec  last_vote_time ; 

        auto primary_key()const { return owner; }
    };

    TABLE council_info {
        account_name owner = 0; /// the voter
        account_name council; /// the producers approved by this voter if no proxy set
        uint64_t     total_votes = 0;
        uint64_t     unpaid = 0;

        auto primary_key()const { return owner; }
    };          

    typedef eosio::multi_index<"voters"_n,  voter_info>  voters_t;
    typedef eosio::multi_index<"proxies"_n, voter_info>  proxies_t;
    typedef eosio::multi_index<"council"_n, voter_info>  council_t;    
    voters_t _voters;
    proxies_t _proxies;
    council_t _council;
};