/**
 *  @dev minakokojima, yukiexe
 *  env: EOSIO.CDT v1.3.2
 *  @copyright Andoromeda
 */
#pragma once
#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/transaction.hpp>

#include "utils.hpp"
#include "council.hpp"
#include "NFT.hpp"
// #include <cmath>
// #include <string>

#include "config.hpp"
#include "kyubey.hpp"
// #include "eosio.token.hpp"
 
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
using eosio::name ;

CONTRACT cryptomeetup : public council {
    public:
        cryptomeetup( name receiver, name code, datastream<const char*> ds ) :
        council( receiver, code, ds ),
        _market( receiver, uint64_t(eosio::name::raw(receiver)) ),
        _land( receiver, uint64_t(eosio::name::raw(receiver)) ),
        _player( receiver, uint64_t(eosio::name::raw(receiver)) ){}
        

    ACTION init();
    ACTION clear();
    ACTION test();
       
 
    ACTION transfer(account_name   from,
                  account_name   to,
                  asset          quantity,
                  string         memo);
    
    void onTransfer(account_name from, account_name to,
                    extended_asset quantity, string& memo); 

    void newland(account_name &from, asset &eos);

    ACTION buy_land(account_name from, extended_asset in, const vector<string>& params);
    ACTION buy(account_name from, extended_asset in, const vector<string>& params);
    ACTION sell(account_name from, extended_asset in, const vector<string>& params);    

    void apply(account_name code, action_name action);

    
    TABLE land : public NFT::tradeable_token {
        uint64_t parent;
        void tax() {
        }
        uint64_t next_price() const { return price * 1.35; }

    };
        
    TABLE player {
        capi_name  account;
        uint64_t land_profit;
        uint64_t ref_profit;
        uint64_t fee_profit;
        uint64_t pool_profit;
        uint64_t staked_income;
        uint64_t council_income;

        auto primary_key() const {return account;}        
        void withdraw() {
        }
    };
        
    typedef eosio::multi_index<"land"_n, land> land_t;
    land_t _land;   

    typedef eosio::multi_index<"player"_n, player> player_t;
    player_t _player;  
  
    typedef eosio::multi_index<"market"_n, kyubey::market> market_t;
    market_t _market;    
    
    /*
    // @abi action
    void receipt(const rec_reveal& reveal) {
        require_auth(_self);
    }

    // @abi table bagsglobal
    struct bagsglobal {      
        uint64_t team;
        uint64_t pool;
        account_name last;
        time st, ed;
    };
    typedef singleton<N(bagsglobal), bagsglobal> singleton_bagsglobal;
    singleton_bagsglobal _bagsglobal;   

    uint64_t get_next_defer_id() {
    auto g = _global.get();    
    g.defer_id += 1;
    _global.set(g,_self);
        return g.defer_id;
    }

    template <typename... Args>
    void send_defer_action(Args&&... args) {
        transaction trx;
        trx.actions.emplace_back(std::forward<Args>(args)...);
        trx.send(get_next_defer_id(), _self, false);
    }

  // @abi action
  void newbag(account_name &from, asset &eos);

  // @abi action
  void setslogan(account_name &from, uint64_t id,string memo);
  
private:
    */
};

void cryptomeetup::apply(capi_name code, capi_name action) {   
    auto &thiscontract = *this;
    
    if ( name(action) == "transfer"_n ) {
        if ( name(code) == "eosio.token"_n ) {
            auto transfer_data = eosio::unpack_action_data<st_transfer>();
            onTransfer(transfer_data.from, transfer_data.to,
                        extended_asset(transfer_data.quantity, name( code ) ),
                        transfer_data.memo);
        }
        return;
    }

    if (name(code) != _self) return;
    switch (action) {
        // EOSIO_DISPATCH(cryptomeetup, (init)(clear)(test)(buy)(transfer));
    };
}

extern "C" {
    [[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) 
    {
        cryptomeetup p( name(receiver), name(code), datastream<const char*>(nullptr, 0) );
        p.apply(code, action);
        eosio_exit(0);
    }
}