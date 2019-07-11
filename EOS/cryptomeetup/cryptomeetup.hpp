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
    _portal(receiver, receiver.value) {}

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
        uint64_t creator_fee;
        uint64_t ref_fee;
        uint64_t k;        
        uint64_t last_anti_bot_fee;
        uint64_t anti_bot_init_fee;
        time anti_bot_timer;
        time last_buy_timer;        
        time st;
        uint64_t primary_key()const { return id; }
        uint64_t next_price() const {
            return price * (k + 1000) / 1000;
        }
    };    
    
    TABLE players {
        uint64_t portal_approved;
        uint64_t meetup_attended;
        uint64_t game_profit;   // 游戏收入(卖land/portal两种情况)以及exceed的 EOS
        uint64_t ref_profit;    // 拉人收入(land/portal两种情况) CMU
        uint64_t fee_profit;    // creator创建地标收入以及地标在哪个land的owner收入(仅portal) CMU
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

    typedef singleton<"players"_n, players> singleton_players;

    typedef eosio::multi_index<"market"_n, market> market_t;
    market_t _market;    

    typedef eosio::multi_index<"checkins"_n, checkin_info> checkin_index;
    
    ACTION init();
    ACTION clear();
    // ACTION test() { require_auth(_self); }
    /*
    ACTION unstake(name from, asset delta) {
        council::unstake(from, delta);
    } */
    ACTION claim(name from) {
        // eosio_assert(false, "not start yet.");
        council::claim(from);

        singleton_players _player(_self, from.value);
        auto p = _player.get_or_create(_self, players{});

        if (p.game_profit == 0 && p.ref_profit == 0 && p.fee_profit == 0) {
            return;
        }
        if (p.game_profit > 0) {
            action(
                permission_level{_self, "active"_n},
                EOS_CONTRACT, "transfer"_n,
                make_tuple(_self, from, asset(p.game_profit, EOS_SYMBOL),
                    std::string("land_profit & creator_profit & exceed EOS"))
            ).send();    
            p.game_profit = 0;
        }
        if (p.ref_profit > 0) {
            action(
                permission_level{_self, "active"_n},
                TOKEN_CONTRACT, "transfer"_n,
                make_tuple(_self, from, asset(p.ref_profit, CMU_SYMBOL),
                    std::string("refer profit CMU."))
            ).send();
            p.ref_profit = 0;
        }
        if (p.fee_profit > 0) {
            action(
                permission_level{_self, "active"_n},
                TOKEN_CONTRACT, "transfer"_n,
                make_tuple(_self, from, asset(p.fee_profit, CMU_SYMBOL),
                    std::string("creator & parent profit CMU."))
            ).send();
            p.fee_profit = 0;
        }

        _player.set(p,_self);
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

    ACTION newportal(name from, uint64_t id, uint64_t parent_id, uint64_t creator_fee, uint64_t ref_fee, uint64_t k, uint64_t price, uint64_t st);
    ACTION newland(name from, asset eos);
    ACTION airdrop(name to, asset eos);
    // ACTION checkin(name from, const checksum256 &hash) { require_auth(_self); }

    ACTION transfer(name from, name to, asset quantity, string memo);    
    void onTransfer(name from, name to, extended_asset in, string memo); 
    void buy_land(name from, extended_asset in, const vector<string>& params);
    void buy_portal(name from, extended_asset in, const vector<string>& params);
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
                (claim)
                (refund)
                (vote)
                (unvote)
                (newland)
                (newportal)
                (airdrop)
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