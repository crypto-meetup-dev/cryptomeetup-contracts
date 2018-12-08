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
    auto itr_owner = _player.find((itr->owner).value);
    if (itr_owner == _player.end()) {
        _player.emplace(from, [&](auto &p) {
            p.account = itr->owner.value;
            p.land_profit = itr->price + to_owner;
        });
    } else {
        _player.modify(itr_owner, from, [&](auto &p) {
            p.land_profit += itr->price + to_owner;
        });
    }

    delta -= to_owner;
    asset out;
    _market.modify(_market.begin(), _self, [&](auto &m) {
        out = m.buy(delta);
    });

    asset to_prize_pool = out * 10 / 40;
    g.pool += to_prize_pool.amount;

    asset to_ref;
    if (params.size() >= 3) {
        auto ref = name(params[2]);
        if (is_account(ref) && ref != from) {
            to_ref = out * 3 / 40;
            auto itr_ref = _player.find(ref.value);
            if (itr_ref == _player.end()) {
                _player.emplace(from, [&](auto &p) {
                    p.account = ref.value;
                    p.ref_profit = to_ref.amount;
                });
            } else {
                _player.modify(itr_ref, from, [&](auto &p) {
                    p.ref_profit += to_ref.amount;
                });
            }
        } 
    }

    auto to_dividend_pool = out - to_prize_pool - to_ref;
    council::make_profit(to_dividend_pool.amount);

    g.last = from;
    g.ed += itr->next_price() * 60 / 10000;
    _global.set(g, _self);       

    _land.modify(itr, get_self(), [&](auto &t) {
        t.owner = from;
        t.price = itr->next_price();
    });
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

    auto to_creator = delta * itr->creator_fee / 1000;
    auto itr_creator = _player.find((itr->creator).value);
    if (itr_creator == _player.end()) {
        _player.emplace(from, [&](auto &p) {
            p.account = itr->creator.value;
            p.fee_profit =  to_creator;
        });
    } else {
        _player.modify(itr_creator, from, [&](auto &p) {
            p.fee_profit += to_creator;
        });
    }

    eosio_assert(delta * 95 / 100 > to_creator + delta * itr->ref_fee / 1000, "error portal owner income.");
    auto to_owner = delta * 95 / 100 - to_creator - delta * itr->ref_fee / 1000;
    auto itr_owner = _player.find((itr->owner).value);
    if (itr_owner == _player.end()) {
        _player.emplace(from, [&](auto &p) {
            p.account = itr->owner.value;
            p.land_profit = itr->price + to_owner;
        });
    } else {
        _player.modify(itr_owner, from, [&](auto &p) {
            p.land_profit += itr->price + to_owner;
        });
    }
    
    delta = delta * 5 / 100 + delta * itr->ref_fee / 1000;
    asset out;
    _market.modify(_market.begin(), _self, [&](auto &m) {
        out = m.buy(delta);
    });

    asset to_ref;
    if (params.size() >= 3) {
        auto ref = name(params[2]);
        if (is_account(ref) && ref != from) { 
            to_ref = out * itr->ref_fee / (itr->ref_fee + 50) ;  
            auto itr_ref = _player.find(ref.value);
            if (itr_ref == _player.end()) {
                _player.emplace(from, [&](auto &p) {
                    p.account = ref.value;
                    p.ref_profit = to_ref.amount;
                });
            } else {
                _player.modify(itr_ref, from, [&](auto &p) {
                    p.ref_profit += to_ref.amount;
                });
            }
        } 
    } 

    auto to_dividend_pool = out - to_ref;
    council::make_profit(to_dividend_pool.amount);

    _portal.modify(itr, get_self(), [&](auto &p) {
        p.owner = from;
        p.price = itr->next_price();
    });
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