/**
 *  @dev minakokojima
 *  @copyright Andoromeda
 */
#pragma once
#include <eosiolib/asset.hpp>
#include <eosiolib/currency.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/transaction.hpp>

#include "council.hpp"
#include "NFT.hpp"
// #include <cmath>
// #include <string>

#include "config.hpp"
#include "utils.hpp"
#include "kyubey.hpp"
// #include "eosio.token.hpp"
 
typedef double real_type;

using std::string;
using eosio::symbol_name;
using eosio::asset;
using eosio::symbol_type;
using eosio::permission_level;
using eosio::action;
using eosio::extended_asset;
using eosio::unpack_action_data;
using eosio::currency;

class sign : public council {
    public: sign(account_name self) :
        council(self),
        _global(_self, _self),
        _market(_self, _self),
        _sign(_self, _self){}

    // @abi action
    void init();
    // @abi action
    void clear();     
    // @abi action
    void test();
    // @abi action
    void unstake(account_name from, uint64_t amount);
    // @abi action
    void claim(account_name from);    

    // @abi action
    void transfer(account_name   from,
                  account_name   to,
                  asset          quantity,
                  string         memo);
    
    void onTransfer(account_name from, account_name to,
                    extended_asset quantity, string memo); 

    void create(account_name from, extended_asset in, const vector<string>& params);
    void sponsor(account_name from, extended_asset in, const vector<string>& params);    
    void buy(account_name from, extended_asset in, const vector<string>& params);
    void sell(account_name from, extended_asset in, const vector<string>& params);

    // @abi action
    void airdrop(account_name to, uint64_t amount);

    void apply(account_name code, action_name action);

    // @abi table signs
    struct sign_info {
        uint64_t id;
        account_name creator = 0;
        account_name owner = 0;
        uint64_t creator_fee;
        uint64_t ref_fee;
        uint64_t k;        
        uint64_t price;
        uint64_t anti_bot_fee;
        time anti_bot_timer;
        time last_buy_timer;        
        time st;
        uint64_t primary_key()const { return id; }
        uint64_t next_price() const {
            return price * k / 1000;
        }
    };    
    
    // @abi table players
    struct player_info {
        uint64_t ref_income;
        uint64_t staked_income;
        uint64_t article_income;
        uint64_t sponsor_income;
    };
        
    // @abi table global
    struct global_info {
        uint64_t defer_id;
        uint64_t total_staked;
        uint64_t global_fee;
        account_name last;
        time st, ed;
    };

    typedef eosio::multi_index<N(signs), sign_info> sign_index;
    sign_index _sign;

    typedef eosio::multi_index<N(market), kyubey::market> market_index;
    market_index _market;    

    typedef singleton<N(global), global_info> singleton_global;
    singleton_global _global;     

    typedef singleton<N(players), player_info> singleton_players;  
    
    // @abi action
    void receipt(const rec_reveal& reveal) {
        require_auth(_self);
    }

    uint64_t get_next_defer_id() {
        auto g = _global.get();    
        g.defer_id += 1;
        _global.set(g, _self);
        return g.defer_id;
    }

    template <typename... Args>
    void send_defer_action(Args&&... args) {
        transaction trx;
        trx.actions.emplace_back(std::forward<Args>(args)...);
        trx.send(get_next_defer_id(), _self, false);
    }
};


struct st_transfer {
    account_name from;
    account_name to;
    asset        quantity;
    string       memo;
};

void sign::apply(account_name code, action_name action) {   
    auto &thiscontract = *this;

    if (action == N(transfer)) {
        auto transfer_data = unpack_action_data<st_transfer>();
        onTransfer(transfer_data.from, transfer_data.to, extended_asset(transfer_data.quantity, code), transfer_data.memo);               
        return;
    }

    if (code != _self) return;
    switch (action) {
        EOSIO_API(sign, (init));
    };
}

extern "C" {
    [[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) 
    {
        sign p(receiver);
        p.apply(code, action);
        eosio_exit(0);
    }
}