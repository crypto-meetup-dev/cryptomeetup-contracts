/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */

#include "signature.bp.hpp"

void sign::init() {
    require_auth(_self);
}  


void sign::claim(account_name from) {
    require_auth(from);

    singleton_players _players(_self, from);
    auto p = _players.get_or_create(_self, player_info{});

/*
    if (p.pool_profit > 0) {
        send_defer_action(
            permission_level{_self, N(active)},
            N(dacincubator), N(transfer),
            make_tuple(_self, from, 
                asset(p.pool_profit, CMU_SYMBOL),
                string("claim")
            )
        );
    }

    p.pool_profit = 0;
    _players.set(p, _self); */   
}

void sign::unstake(account_name from, uint64_t amount) {
    require_auth(from);
    council::unstake(from, amount);
    auto g = _global.get();
    g.total_staked -= amount;
    _global.set(g, _self);             
}

void sign::airdrop(account_name to, uint64_t amount) {
    require_auth(_self);

    singleton_players _players(_self, to);
    auto p = _players.get_or_create(_self, player_info{});
//    p.pool_profit += amount;
    _players.set(p, _self);
}

void sign::create(account_name from, extended_asset in, const vector<string>& params) {
    require_auth(from);
    eosio_assert(in.contract == N(eosio.token), "only true EOS token is allowed");
    eosio_assert(in.symbol == EOS_SYMBOL, "only true EOS token is allowed");    
    eosio_assert(in.amount >= 1000, "you need at least 0.1 EOS to create an new signature");
    _sign.emplace(_self, [&](auto &s) {
        s.id = _sign.available_primary_key();
        s.creator = from;
        s.owner = from;
        s.creator_fee = 20;
        s.ref_fee = 50;
        s.k = 350;
        s.price = 1000;
        s.anti_bot_fee = 500;
        s.anti_bot_timer = 5*60*60;
        s.last_buy_timer = 0;
        s.st = now();
    });    
}

void sign::sponsor(account_name from, extended_asset in, const vector<string>& params) {
    require_auth(from);
    /*
    eosio_assert(in.contract == N(eosio.token), "only true EOS token is allowed");
    eosio_assert(in.symbol == EOS_SYMBOL, "only true EOS token is allowed");
   
    eosio_assert(params.size() >= 2, "No ID found..");
    auto id = string_to_price(params[1]);

    auto itr = _land.find(id);
    eosio_assert(itr != _land.end(), "no land exist");
    eosio_assert(in.amount >= itr->next_price(), "no enough eos");
    eosio_assert(from != itr->owner, "cannot buy with yourself");

    auto exceed = in.amount - itr->next_price();

    action(
        permission_level{_self, N(active)},
        N(eosio.token), N(transfer),
        make_tuple(_self, from, asset(exceed, EOS_SYMBOL),
            std::string("exceed EOS refund"))
    ).send();
    */
}

void sign::buy(account_name from, extended_asset in, const vector<string>& params) {

    eosio_assert(in.contract == N(eosio.token), "only true EOS token is allowed");
    eosio_assert(in.symbol == EOS_SYMBOL, "only true EOS token is allowed.");
   
    asset out;
    _market.modify(_market.begin(), 0, [&](auto &m) {
        out = m.buy(in.amount * 95 / 100);
    }); 

    if (out.amount > 0){      
        action(
            permission_level{_self, N(active)},
            N(dacincubator), N(transfer),
            make_tuple(_self, from, out, std::string("buy some new token"))
        ).send();            
    }    
}

void sign::sell(account_name from, extended_asset in, const vector<string>& params) {
    eosio_assert(in.contract == N(dacincubator), "only true CMU token is allowed");
    eosio_assert(in.symbol == CMU_SYMBOL, "only true CMU token is allowed");
   
    asset out;
    _market.modify(_market.begin(), 0, [&](auto &m) {
        out = m.sell(in.amount * 95 / 100);
    });

    if (out.amount > 0){
        action(
            permission_level{_self, N(active)},
            N(eosio.token), N(transfer),
            make_tuple(_self, from, out, std::string("sell some new token"))
        ).send();        
    }
}

void sign::onTransfer(account_name from, account_name to, extended_asset quantity, string memo){
    if (to != _self) return;
    require_auth(from);

    eosio_assert(quantity.is_valid(), "Invalid token transfer");
    eosio_assert(quantity.amount > 0, "must buy a positive amount");
    auto params = split(memo, ' ');
    eosio_assert(params.size() >= 1, "error memo");

    if (params[0] == "sponsor") {
//        auto g = _global.get();     
//        eosio_assert(now() >= g.st, "This round will be start at 11/04/2018 @ 12:00pm (UTC).");
        //buy_land(from, quantity, params);
        return;
    }

    if (params[0] == "create") {
        create(from, quantity, params);
        return;
    }    

    if (params[0] == "buy") {
        //buy(from, quantity, params);
        return;
    }    

    if (params[0] == "sell") {
        //sell(from, quantity, params);
        return;
    }

    if (params[0] == "stake") {        
        eosio_assert(quantity.contract == N(dacincubator), "must use SST to stake");
        eosio_assert(quantity.symbol == S(4, SST), "must use SST to stake");
        stake(from, quantity.amount);
        auto g = _global.get();
        g.total_staked += quantity.amount;
        _global.set(g, _self);         
        return;
    }
}