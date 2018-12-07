/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */

#include "cryptomeetup.hpp"

void cryptomeetup::init() {
    require_auth(_self);   

    auto g = _global.get_or_create( _self, global{});    
    g.ed = now() + 30*24*60*60;
    _global.set(g, _self);

    /*
    auto st = _land.available_primary_key();
    for (int i=st;i<st+20;++i) {
        auto from = "eosotcbackup"_n;
        _land.emplace(from, [&](auto &p) {
            p.id = _land.available_primary_key();
            p.owner = from;
            p.price = 10000;
        });
    }
    */

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
    /* 
    auto itr = _portal.begin();
    _portal.erase( itr );
    */
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
    eosio_assert(creator_fee < 950, "illegal creator_fee");
    eosio_assert(ref_fee < 950, "illegal ref_fee");
    eosio_assert(creator_fee + ref_fee < 950, "illegal sum of fee");
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

    if ( exceed > 0) {
        action(
            permission_level{_self, "active"_n},
            "eosio.token"_n, "transfer"_n,
            make_tuple(_self, from, asset(exceed, EOS_SYMBOL),
                std::string("exceed eos transfer."))
        ).send();         
    }
   
    auto delta = itr->next_price() - itr->price;

    auto to_owner = delta * 60 / 100;
    if (itr->price + to_owner > 0) {
        action(
            permission_level{_self, "active"_n},
            "eosio.token"_n, "transfer"_n,
            make_tuple(_self, itr->owner, asset(itr->price + to_owner, EOS_SYMBOL),
                std::string("transfer ownership"))
        ).send();       
    } 

    auto to_dividend_pool = delta * 27 / 100;
    council::make_profit(to_dividend_pool);
    
    uint64_t to_ref = 0;
    if (params.size() >= 3) {
        to_ref = delta * 3 / 100;
        asset out_ref;
        _market.modify(_market.begin(), _self, [&](auto &m) {
            out_ref = m.buy(to_ref);
        });         
        auto ref = name(params[2]);
        if (is_account(ref) && ref != from) {   
            if (out_ref.amount > 0) {
            action(
                permission_level{_self, "active"_n},
                TOKEN_CONTRACT, "transfer"_n,
                make_tuple(_self, ref, out_ref,
                            std::string("mining token by reference")))
                .send();
            }
        } 
    }

    auto to_prize_pool = delta - to_owner - to_dividend_pool - to_ref;
    asset out_pool;
    _market.modify(_market.begin(), _self, [&](auto &m) {
        out_pool = m.buy(to_prize_pool);
    });
    g.pool += out_pool.amount;
    g.last = from;

    g.ed += itr->next_price() * 60 / 10000;
    _global.set(g, _self);       

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

void cryptomeetup::buy_portal(name from, extended_asset in, const vector<string>& params) {
    
    auto g = _global.get();
    eosio_assert( g.st <= now() && now() <= g.ed, "not correct time...");
    eosio_assert(in.contract == EOS_CONTRACT, "only true EOS token is allowed");
    eosio_assert(in.quantity.symbol == EOS_SYMBOL, "only true EOS token is allowed...");
    
    eosio_assert(params.size() >= 2, "No ID found.");
    auto id = string_to_price(params[1]);

    auto itr = _portal.find(id);
    eosio_assert(itr != _portal.end(), "no portal exist");
    eosio_assert(in.quantity.amount >= itr->next_price(), "no enough eos");
    eosio_assert(from != itr->owner, "cannot buy with yourself");

    auto exceed = in.quantity.amount - itr->next_price();

    if (exceed > 0) {
        action(
            permission_level{_self, "active"_n},
            "eosio.token"_n, "transfer"_n,
            make_tuple(_self, from, asset(exceed, EOS_SYMBOL),
                std::string("exceed eos transfer."))
        ).send();   
    }

    auto delta = itr->next_price() - itr->price;

    auto to_dividend_pool = delta * 5 / 100;
    council::make_profit(to_dividend_pool);

    uint64_t to_ref = 0; 
    if (params.size() >= 3) {
        to_ref = delta * itr->ref_fee / 1000;
        asset out_ref;
        _market.modify(_market.begin(), _self, [&](auto &m) {
            out_ref = m.buy(to_ref);
        });         
        auto ref = name(params[2]);
        if (is_account(ref) && ref != from) {   
            if (out_ref.amount > 0) {
            action(
                permission_level{_self, "active"_n},
                TOKEN_CONTRACT, "transfer"_n,
                make_tuple(_self, ref, out_ref,
                            std::string("mining token by reference")))
                .send();
            }
        } 
    }

    auto to_creator = delta * itr->creator_fee / 1000;
    if (to_creator > 0) {
        action(
            permission_level{_self, "active"_n},
            "eosio.token"_n, "transfer"_n,
            make_tuple(_self, itr->creator, asset(to_creator, EOS_SYMBOL),
                std::string("transfer ownership fee for creator"))
        ).send();         
    }

    auto to_owner = delta - to_dividend_pool - to_ref - to_creator;
    if (delta > 0) {
        action(
            permission_level{_self, "active"_n},
            "eosio.token"_n, "transfer"_n,
            make_tuple(_self, itr->owner, asset(itr->price + to_owner, EOS_SYMBOL),
                std::string("transfer ownership"))
        ).send();       
    } 


    _portal.modify(itr, get_self(), [&](auto &p) {
        p.owner = from;
        p.price = itr->next_price();
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

    if (params[0] == "buy_portal") {
        buy_portal(from, in, params);
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