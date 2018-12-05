/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */

#include "cryptomeetup.hpp"

void cryptomeetup::init() {
    require_auth(_self);   

/*    auto g = _global.get_or_create( _self, global{});    
    g.st = now();
    g.ed = now() + 7*24*60*60;
    g.total_staked = asset(0, TOKEN_SYMBOL);
    _global.set(g, _self);*/

    auto st = _land.available_primary_key();
    for (int i=st;i<st+20;++i) {
        auto from = "eosotcbackup"_n;
        _land.emplace(from, [&](auto &p) {
            p.id = _land.available_primary_key();
            p.owner = from;
            p.price = 10000;
        });
    }    

    /*    
    while (_market.begin() != _market.end()) {
        _market.erase(_market.begin());
    }



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

void cryptomeetup::clear() {
    require_auth(_self);    
}

void cryptomeetup::test() {
    require_auth(_self);    
}

void cryptomeetup::checkin(name from, const checksum256 &hash) {
    require_auth(_self);    
}

void cryptomeetup::airdrop(name from, asset eos) {
    require_auth(_self);    
}

void cryptomeetup::newportal(name from, uint64_t id, uint64_t parent_id, 
    uint64_t creator_fee, uint64_t ref_fee, 
    uint64_t k, uint64_t price, uint64_t st) {

    require_auth(_self);
    eosio_assert(creator_fee <= 1000, "illegal creator_fee");
    eosio_assert(ref_fee <= 1000, "illegal ref_fee");
    eosio_assert(creator_fee + ref_fee <= 1000, "illegal sum of fee");
    eosio_assert(k >= 10 && k <= 1000, "illegal k");
    eosio_assert(price >= 1000 && price <= 10000000, "illegal initial price");
    if (st < now()) {
        st = now();
    }
    eosio_assert(st >= now() && st <= now() + 3652460 * 60, "illegal st");

    _portal.emplace(_self, [&](auto &s) {
        s.id = id;
        s.parent = parent_id;
        s.creator = from;
        s.owner = from;
        s.creator_fee = creator_fee;
        s.ref_fee = ref_fee;
        s.k = k;
        s.price = price;
        s.st = st;
//        s.last_anti_bot_fee = 0;
//        s.anti_bot_fee = 500;
//        s.anti_bot_timer = 5*60*60;
//        s.last_buy_timer = 0;
    });
}


void cryptomeetup::newland(name from, asset eos) {
    require_auth(_self);
    auto st = _land.available_primary_key();
    for (int i=st;i<st+20;++i) {
        _land.emplace(from, [&](auto &p) {
            p.id = _land.available_primary_key();
            p.owner = from;
            p.price = eos.amount;
        });
    }
}

void cryptomeetup::buy_land(name from, extended_asset in, const vector<string>& params) {
    
    auto g = _global.get();
    eosio_assert( g.st <= now() && now() <= g.ed, "not correct time...");
    eosio_assert(in.contract == EOS_CONTRACT, "only true EOS token is allowed");
    eosio_assert(in.quantity.symbol == EOS_SYMBOL, "only true EOS token is allowed...");
    
    eosio_assert(params.size() >= 2, "No ID found.");
    auto id = string_to_price(params[1]);

    auto itr = _land.find(id);
    eosio_assert(itr != _land.end(), "no land exist");
    eosio_assert(in.quantity.amount >= itr->next_price(), "no enough eos");
    eosio_assert(from != itr->owner, "cannot buy with yourself");

    auto exceed = in.quantity.amount - itr->next_price();

    action(
        permission_level{_self, "active"_n},
        "eosio.token"_n, "transfer"_n,
        make_tuple(_self, from, asset(exceed, EOS_SYMBOL),
            std::string("transfer ownership"))
    ).send();    


    auto delta = itr->next_price() - itr->price;
    delta /= 2;    
    if (delta > 0) {
        action(
            permission_level{_self, "active"_n},
            "eosio.token"_n, "transfer"_n,
            make_tuple(_self, itr->owner, asset(itr->price + delta, EOS_SYMBOL),
                std::string("transfer ownership"))
        ).send();       
    } 

    council::make_profit(delta);
    
    // mining...
    /*
    asset out;
    _market.modify(_market.begin(), _self, [&](auto &m) {
        out = m.buy(delta);
    }); 

    out.amount /= 5;
    if (params.size() >= 3) {
       auto ref = name(params[2].c_str());
       if (is_account(ref) && ref != name(from)) {   
            if (out.amount > 0) {
                
            action( // winner winner chicken dinner
                permission_level{_self, "active"_n},
                 "dacincubator"_n, "transfer"_n,
                make_tuple(_self, ref, out,
                            std::string("mining token by reference")))
                .send(); 
            }            
        }    
    }    

    out.amount *= 2;
    if (out.amount > 0){        
        action(
            permission_level{_self, "active"_n},
            "dacincubator"_n, "transfer"_n,
            make_tuple(_self, itr->owner, out, std::string("mining token by play game"))
        ).send();       
    }    */


    _land.modify(itr, get_self(), [&](auto &t) {
        t.owner = from;
        t.price = itr->next_price();
    });

    // countdownrest() ;    
}

void cryptomeetup::buy(name from, extended_asset in, const vector<string>& params) {

    eosio_assert(in.contract == EOS_CONTRACT, "only true EOS token is allowed");
    eosio_assert(in.quantity.symbol == EOS_SYMBOL, "only true EOS token is allowed");

    asset out;
    _market.modify(_market.begin(), _self, [&](auto &m) {
        out = m.buy(in.quantity.amount);
    }); 

    if (out.amount > 0) {        
        action(
            permission_level{_self, "active"_n},
            TOKEN_CONTRACT, "transfer"_n,
            make_tuple(_self, from, out, std::string("buy some new token"))
        ).send();
    }
}

void cryptomeetup::sell(name from, extended_asset in, const vector<string>& params) {

    eosio_assert(in.contract == TOKEN_CONTRACT, "only true CMU token is allowed");
    eosio_assert(in.quantity.symbol == TOKEN_SYMBOL, "only true CMU token is allowed");
   
    asset out;
    _market.modify(_market.begin(), _self, [&](auto &m) {
        out = m.sell(in.quantity.amount * 95 / 100);
    }); 

    if (out.amount > 0){        
        action(
            permission_level{_self, "active"_n},
            EOS_CONTRACT, "transfer"_n,
            make_tuple(_self, from, out, std::string("sell some new token"))
        ).send();
    }
}

void cryptomeetup::onTransfer(name from, name to, extended_asset in, string memo){

    if (to != _self) return;
    require_auth(from);
    eosio_assert(in.quantity.is_valid(), "invalid token transfer");
    eosio_assert(in.quantity.amount > 0, "must transfer a positive amount");

    auto params = split(memo, ' ');
    eosio_assert(params.size() >= 1, "error memo");  

    if (params[0] == "buy_land") {
        buy_land(from, in, params);
        return;
    }

    if (params[0] == "buy") {
        buy(from, in, params);
        return;
    }    

    if (params[0] == "sell") {
        sell(from, in, params);
        return;
    } 
    
    council::onTransfer(from, to, in, memo);
}