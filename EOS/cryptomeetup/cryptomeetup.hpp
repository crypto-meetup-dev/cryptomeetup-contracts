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
    _player(receiver, receiver.value) {}

    TABLE global : public global_info {};
    TABLE voters : public voter_info {};
    TABLE refunds : public refund_request {};
//    TABLE council : public council_info {};

    TABLE market : public kyubey::_market {};

    TABLE land : public NFT::tradeable_NFT {
        //uint64_t parent;
        void tax() {
        }
        uint64_t next_price() const {
            return price * 1.35;
        }
    };
    
    TABLE player {
        name  account;
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

    typedef eosio::multi_index<"players"_n, player> player_t;
    player_t _player;  
  
    typedef eosio::multi_index<"market"_n, market> market_t;
    market_t _market;    

    typedef eosio::multi_index<"checkins"_n, checkin_info> checkin_index;

    
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