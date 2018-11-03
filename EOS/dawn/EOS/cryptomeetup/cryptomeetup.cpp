/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */

#include "cryptomeetup.hpp"

void cryptomeetup::init() {
    require_auth(_self);    

    for (int i=0;i<250;++i) {
        auto itr = _land.find(i);
        _land.modify(itr, 0, [&](auto &p) {
            p.owner = N(eosotcbackup);
            p.price = 100;
        });
    }
    auto g = _global.get_or_create( _self, global{});
    g.st = now();
    _global.set(g, _self);      
    
   // auto g = _global.get_or_create( _self, global{});
  //  g.st = 1540555220;
//    _global.set(g, _self); 
    
    /*
    uint64_t _totalPrice = 0;

    for (int i = 0; i < 250; ++i) {
        auto itr = _land.find(i);
        uint64_t _price = itr->price;
        _totalPrice += _price;
    }

    uint64_t amount = 3000 * 10000;

    for (int k = 0; k < 250; ++k) {
        auto itr = _land.find(k);
        uint64_t _price = itr->price;
        account_name _owner = itr->owner;

        uint64_t _amountCMU = _price * amount / _totalPrice;

        airdrop(_owner, _amountCMU);
    }

    airdrop(N(minakokojima), 120000);*/

    /*
    while (_market.begin() != _market.end()) {
        _market.erase(_market.begin());
    }*/

    /*
    auto g = _global.get_or_create( _self, global{});
    g.st = 1540555220;
    _global.set(g, _self);   */ 

    //_global.remove();

    /*
    if (_market.begin() == _market.end()) {
        const uint64_t init_dummy_supply = 40000000ll * 10000ll;
        const uint64_t init_dummy_balance = 80000ll * 10000ll;

        _market.emplace(_self, [&](auto &m) {
            m.supply.amount = init_dummy_supply;
            m.supply.symbol = CMU_SYMBOL;
            m.balance.amount = init_dummy_balance;
            m.balance.symbol = EOS_SYMBOL;
            m.progress = 0;
        });        
    }*/
}  

void cryptomeetup::newland(account_name &from, asset &eos) {
    require_auth(_self);

/*
    for (int i=339;i<=339;++i) {
        auto itr = _land.find(i);
        _land.erase(itr);
    }*/

    
    for (int i=0;i<250;++i) {
        auto itr = _land.find(i);
        _land.modify(itr, 0, [&](auto &p) {
            p.owner = from;
            p.price = eos.amount;
        });
    }

    auto g = _global.get_or_create( _self, global{});
    g.st = 1540555220 + 600;
    _global.set(g, _self);  
    
}

void cryptomeetup::claim(account_name from) {
    require_auth(from);

    singleton_players _players(_self, from);
    auto p = _players.get_or_create(_self, player_info{});

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
    _players.set(p, _self);    
}

void cryptomeetup::unstake(account_name from, uint64_t amount) {
    require_auth(_self);
    council::unstake(from, amount);
    auto g = _global.get();
    g.total_staked -= amount;
    _global.set(g, _self);             
}

void cryptomeetup::airdrop(account_name to, uint64_t amount) {
    require_auth(_self);

    singleton_players _players(_self, to);
    auto p = _players.get_or_create(_self, player_info{});

    p.pool_profit += amount;
    _players.set(p, _self);
}

void cryptomeetup::buy_land(account_name from, extended_asset in, const vector<string>& params) {

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


    auto delta = itr->next_price() - itr->price;
    delta /= 2;    

    if (delta > 0) {
        action( // winner winner chicken dinner
            permission_level{_self, N(active)},
            N(eosio.token), N(transfer),
            make_tuple(_self, itr->owner, asset(itr->price + delta, EOS_SYMBOL),
                std::string("transfer ownership"))
        ).send();        
    }

    // mining
    asset out;
    _market.modify(_market.begin(), 0, [&](auto &m) {
        out = m.buy(delta);
    }); 

    out.amount /= 10;
    if (params.size() >= 3) {
       auto ref = eosio::string_to_name(params[2].c_str());
       if (is_account(ref) && ref != from) {   
            if (out.amount > 0) {
            action( // winner winner chicken dinner
                permission_level{_self, N(active)},
                 N(dacincubator), N(transfer),
                make_tuple(_self, ref, out,
                            std::string("mining token by reference")))
                .send();
            }
        }    
    }   

    out.amount *= 2; 

    /*
    out.amount *= 2;
    if (out.amount > 0){
        if (in.amount <= 100000) {
            out.amount /= 5;
            action(
                permission_level{_self, N(active)},
                N(dacincubator), N(transfer),
                make_tuple(_self, itr->owner, out, std::string("mining token by play game"))
            ).send(); 
            out.amount *= 5;
        } else {
            action(
                permission_level{_self, N(active)},
                N(dacincubator), N(transfer),
                make_tuple(_self, itr->owner, out, std::string("mining token by play game"))
            ).send(); 
        }
    }  */

    _land.modify(itr, 0, [&](auto &t) {
        t.owner = from;
        t.price = itr->next_price();
    });

    auto g = _global.get();
    g.last = from;
    if (g.pool >= 2000*10000) {
        g.ed = now() + 60;
    } else {
        g.ed = now() + (24*60*60 - ((g.pool * (23*60*60 + 59*60)) / 2000*10000));
    }

    g.pool += out.amount;
    _global.set(g, _self);       
}

void cryptomeetup::buy(account_name from, extended_asset in, const vector<string>& params) {

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

void cryptomeetup::sell(account_name from, extended_asset in, const vector<string>& params) {
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

void cryptomeetup::onTransfer(account_name from, account_name to, extended_asset quantity, string memo){
    if (to != _self) return;
    require_auth(from);

    /*action(
        permission_level{_self, N(active)},
        quantity.contract, N(transfer),
        make_tuple(_self, from, asset(quantity.amount, quantity.symbol),
            memo)
    ).send();    */    
//    eosio_assert(quantity.amount == 1000, "not equal to 1000");
  //  return;
//    eosio_assert(quantity.amount == 1000, "not equal to 1000");

    eosio_assert(quantity.is_valid(), "Invalid token transfer");
    eosio_assert(quantity.amount > 0, "must buy a positive amount");
    auto params = split(memo, ' ');
    eosio_assert(params.size() >= 1, "error memo");

    if (params[0] == "buy_land") {
        auto g = _global.get();
        eosio_assert(now() >= g.st, "This round will be start at 10/26/2018 @ 12:10pm (UTC).");
        buy_land(from, quantity, params);
        return;
    }

    if (params[0] == "buy") {
        buy(from, quantity, params);
        return;
    }    

    if (params[0] == "sell") {
        sell(from, quantity, params);
        return;
    }

    if (params[0] == "stake") {        
        eosio_assert(quantity.contract == N(dacincubator), "must use CMU to stake");
        eosio_assert(quantity.symbol == S(4, CMU), "must use CMU to stake");
        stake(from, quantity.amount);
        auto g = _global.get();
        g.total_staked += quantity.amount;
        _global.set(g, _self);         
        return;
    }
}