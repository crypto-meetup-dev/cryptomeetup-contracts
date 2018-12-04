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
#include "council.cpp"
#include "NFT.hpp"
// #include <cmath>

#include "config.hpp"
#include "kyubey.hpp"
 
using namespace eosio;
using namespace std;
using namespace kyubey;

CONTRACT cryptomeetup : public council {
    public:
        cryptomeetup( name receiver, name code, datastream<const char*> ds ) :
        //contract( receiver, code, ds ),
        council( receiver, code, ds ),
        _global( code, uint64_t(eosio::name::raw(code)) ),
        _market(receiver, receiver.value),
        _land(receiver, receiver.value),
        _player(receiver, receiver.value) {}

    /*
    TABLE land {
        uint64_t     id;
        name owner = 0;
        uint64_t primary_key()const { return id; }        
        uint64_t price;           
        uint64_t parent;
        void tax() {
        }
        uint64_t next_price() const {
            return price * 1.35;
        }
    };    
    */

    TABLE land : public NFT::tradeable_NFT {
        //uint64_t parent;
        void tax() {
        }
        uint64_t next_price() const {
            return price * 1.35;
        }
    };
    
         
    TABLE player {
        capi_name  account;
        uint64_t land_profit;
        uint64_t ref_profit;
        uint64_t fee_profit;
        uint64_t pool_profit;
        auto primary_key() const {return account;}        
        void withdraw() {
        }
    };
        
    TABLE global {       
        uint64_t team;
        uint64_t pool;
        name last;
        time st, ed;
    };

    typedef singleton<"global"_n, global> singleton_global;
    singleton_global _global;       
    typedef eosio::multi_index<"land"_n, land> land_t;
    land_t _land;   

    typedef eosio::multi_index<"player"_n, player> player_t;
    player_t _player;  
  
    typedef eosio::multi_index<"market"_n, market> market_t;
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
        name last;
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
  void setslogan(name &from, uint64_t id,string memo);
  */

 /*
private:
    void countdownrest() {
        require_auth(_self);
        auto g = _global.get();                     
        g.ed = now() + 60 * 60;
        _global.set(g, _self);
    }
};*/

    ACTION init();
    ACTION clear();
    ACTION test(); 
    ACTION newland(name &from, asset &eos);
    ACTION airdrop(name to, uint64_t amount);
    ACTION checkin(name from, const checksum256 &hash);

    ACTION transfer(name from, name to, asset quantity, string memo);    
    void onTransfer(name from, name to, extended_asset in, string memo); 
    void buy_land(name from, extended_asset in, const vector<string>& params);
    void buy(name from, extended_asset in, const vector<string>& params);
    void sell(name from, extended_asset in, const vector<string>& params);     

    void apply(uint64_t receiver, uint64_t code, uint64_t action) {
        auto &thiscontract = *this;
        if (action == name("transfer").value) {
            auto transfer_data = unpack_action_data<st_transfer>();
            onTransfer(transfer_data.from, transfer_data.to, extended_asset(transfer_data.quantity, name(code)), transfer_data.memo);
            return;
        }

        switch (action) {
            EOSIO_DISPATCH_HELPER(cryptomeetup, (init)(newland)(unstake)(claim)(refund))
        }
    }
};

extern "C" {
    [[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
        cryptomeetup p( name(receiver), name(code), datastream<const char*>(nullptr, 0) );
        p.apply(receiver, code, action);
        eosio_exit(0);
    }
}