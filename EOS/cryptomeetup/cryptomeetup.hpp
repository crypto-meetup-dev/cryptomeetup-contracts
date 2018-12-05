/**
 *  @dev minakokojima, yukiexe
 *  env: EOSIO.CDT v1.3.2
 *  @copyright Andoromeda
 */
#pragma once
#include "council.cpp"
#include "NFT.hpp"
#include "kyubey.hpp"
 
CONTRACT cryptomeetup : public council {
public:
    cryptomeetup( name receiver, name code, datastream<const char*> ds ) :
    council( receiver, code, ds ),
    _market(receiver, receiver.value),
    _land(receiver, receiver.value),
    _portal(receiver, receiver.value),    
    _player(receiver, receiver.value) {}

    TABLE global : public global_info {};
    TABLE voters : public voter_info {};
    TABLE refunds : public refund_request {};
//    TABLE council : public council_info {};

    TABLE market : public kyubey::_market {};

    TABLE land : public NFT::tradeable_NFT {
        uint64_t next_price() const {
            return price * 1.35;
        }
    };

    TABLE portal : public land {
        uint64_t parent;
        name creator;
        name owner;
        uint64_t creator_fee;
        uint64_t ref_fee;
        uint64_t k;        
        uint64_t price;
        uint64_t last_anti_bot_fee;
        uint64_t anti_bot_init_fee;
        time anti_bot_timer;
        time last_buy_timer;        
        time st;
        uint64_t primary_key()const { return id; }
        uint64_t next_price() const {
            return price * k / 1000;
        }
    };    
    
    TABLE player {
        name  account;
        uint64_t portal_approved;
        uint64_t meetup_attended;
        uint64_t land_profit;
        uint64_t ref_profit;
        uint64_t fee_profit;
        uint64_t pool_profit;
        auto primary_key() const {return account;}        
        void withdraw() {
        }
    };

    TABLE checkin_info {
        uint64_t event_id;
        uint64_t primary_key()const { return event_id; }        
    };

    typedef eosio::multi_index<"land"_n, land> land_t;
    land_t _land;   
    typedef eosio::multi_index<"portal"_n, portal> portal_t;
    portal_t _portal;    

    typedef eosio::multi_index<"players"_n, player> player_t;
    player_t _player;  
  
    typedef eosio::multi_index<"market"_n, market> market_t;
    market_t _market;    

    typedef eosio::multi_index<"checkins"_n, checkin_info> checkin_index;

    
    ACTION init();
    ACTION clear();
    ACTION test(); 

    ACTION unstake(name from, asset delta) {
        council::unstake(from, delta);
    }
    ACTION claim(name from) {
        council::claim(from);
    }  
    ACTION refund(name from) {
        council::refund(from);
    }      
    ACTION vote(name from, name to) {
        council::vote(from, to);
    }
    ACTION unvote(name from) {
        council::unvote(from);
    }

    ACTION newportal(name from, uint64_t amount, uint64_t id, const vector<string>& params);
    ACTION newland(name from, asset eos);
    ACTION airdrop(name to, asset eos);
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
            EOSIO_DISPATCH_HELPER(cryptomeetup, 
                (init)
                (clear)
                (test)
                (unstake)
                (claim)
                (refund)
                (vote)
                (unvote)
                (newland)
                (newportal)
                (airdrop)
                (checkin)
            )
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