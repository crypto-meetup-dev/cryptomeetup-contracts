/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */

#include "cryptomeetup.hpp"

void cryptomeetup::init() {
    require_auth(_self);   

    auto g = _global.get_or_create( _self, global{});
    g.st = now();
    _global.set(g, _self);
    /*
    auto g = _global.get_or_create( _self, global{});
    g.st = 1544356800;
    g.ed = 1544356800 + 30*24*60*60;
    _global.set(g, _self);

    while (_player.begin() != _player.end()) {
        _player.erase(_player.begin());
    }    
    */
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
   /*
   auto itr = _portal.find(9);
   
    _portal.modify(itr, get_self(), [&](auto &p) {
        p.parent = 45;
    });
    */
   for (uint64_t i = 0; i < 270; ++i) {
       auto itr = _land.find(i);
       _land.modify(itr, get_self(), [&](auto &l){
           l.price = 1000;
       });
   }
}

void cryptomeetup::test() {
    require_auth(_self);    
}

void cryptomeetup::checkin(name from, const checksum256 &hash) {
    require_auth(_self);    


    

}

void cryptomeetup::airdrop(name from, asset eos) {
    require_auth(_self);    
    /*
    auto itr = _player.find(from.value);
    _player.erase( itr );   
    */
   /*
    singleton_players _player_from(_self, from.value);
    _player_from.remove();
    */
}


void cryptomeetup::newportal(name from, uint64_t id, uint64_t parent_id, 
    uint64_t creator_fee, uint64_t ref_fee, 
    uint64_t k, uint64_t price, uint64_t st) {

    require_auth(_self);
    eosio_assert(creator_fee < 930, "illegal creator_fee");
    eosio_assert(ref_fee < 930, "illegal ref_fee");
    eosio_assert(creator_fee + ref_fee < 930, "illegal sum of fee");
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

    if (exceed > 0) {
        singleton_players _player_from(_self, from.value);
        auto p = _player_from.get_or_create(_self, players{});
        p.game_profit += exceed;
        _player_from.set(p, _self);   
    }
   
    auto delta = itr->next_price() - itr->price;
    auto to_owner = delta * LAND_TO_LAST / 100;
    singleton_players _player1(_self, itr->owner.value);
    auto p1 = _player1.get_or_create(_self, players{});
    p1.game_profit += itr->price + to_owner;
    _player1.set(p1, _self);

    delta -= to_owner;
    asset out;
    _market.modify(_market.begin(), _self, [&](auto &m) {
        out = m.buy(delta);
    });

    asset to_prize_pool = out * LAND_TO_POOL / (100 - LAND_TO_LAST);
    g.pool += to_prize_pool.amount;
    g.last = from;
    g.ed += in.quantity.amount * 60 / 10000;
    _global.set(g, _self);

    asset to_ref = asset(0, CMU_SYMBOL);
    if (params.size() >= 3) {
        auto ref = name(params[2]);
        if (is_account(ref) && ref != from) {
            to_ref = out * LAND_TO_REF / (100 - LAND_TO_LAST);
            singleton_players _player2(_self, ref.value);
            auto p2 = _player2.get_or_create(_self, players{});
            p2.ref_profit += to_ref.amount;
            _player2.set(p2,_self);
        } 
    }

    auto to_dividend_pool = out - to_prize_pool - to_ref;
    council::make_profit(to_dividend_pool.amount); 

    _land.modify(itr, get_self(), [&](auto &t) {
        t.owner = from;
        t.price = itr->next_price();
    });

    // council::claim(from);
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
        singleton_players _player_from(_self, from.value);
        auto p = _player_from.get_or_create(_self, players{});
        p.game_profit += exceed;
        _player_from.set(p, _self);          
    }

    auto delta = itr->next_price() - itr->price;
    
    eosio_assert(delta * (100 - PORTAL_TO_PARENT - PORTAL_TO_POOL) / 100 > delta * (itr->creator_fee + itr->ref_fee) / 1000, "error portal owner income.");
    auto to_owner = delta * (100 - PORTAL_TO_PARENT - PORTAL_TO_POOL) / 100 - delta * (itr->creator_fee + itr->ref_fee) / 1000;
    singleton_players _player1(_self, itr->owner.value);
    auto p1 = _player1.get_or_create(_self, players{});
    p1.game_profit += itr->price + to_owner;
    _player1.set(p1,_self);
    
    delta -= to_owner;
    asset out;
    _market.modify(_market.begin(), _self, [&](auto &m) {
        out = m.buy(delta);
    });

    asset to_ref = asset(0, CMU_SYMBOL);
    if (params.size() >= 3) {
        auto ref = name(params[2]);
        if (is_account(ref) && ref != from) { 
            to_ref = out * itr->ref_fee / (itr->ref_fee + 10*PORTAL_TO_POOL + 10*PORTAL_TO_PARENT + itr->creator_fee) ; 
            singleton_players _player2(_self, ref.value);
            auto p2 = _player2.get_or_create(_self, players{});
            p2.ref_profit += to_ref.amount;
            _player2.set(p2,_self);
        } 
    }

    auto to_creator = out * itr->creator_fee / (itr->ref_fee + 10*PORTAL_TO_POOL + 10*PORTAL_TO_PARENT + itr->creator_fee);
    singleton_players _player3(_self, itr->creator.value);
    auto p3 = _player3.get_or_create(_self, players{});
    p3.fee_profit +=  to_creator.amount;
    _player3.set(p3,_self);

    auto itr_parent = _land.find(itr->parent);
    auto to_parent = out * 10*PORTAL_TO_PARENT / (itr->ref_fee + 10*PORTAL_TO_POOL + 10*PORTAL_TO_PARENT + itr->creator_fee);
    singleton_players _player4(_self, itr_parent->owner.value);
    auto p4 = _player4.get_or_create(_self, players{});
    p4.fee_profit +=  to_parent.amount;
    _player4.set(p4,_self);


    auto to_dividend_pool = out - to_ref - to_creator - to_parent;
    council::make_profit(to_dividend_pool.amount);

    _portal.modify(itr, get_self(), [&](auto &p) {
        p.owner = from;
        p.price = itr->next_price();
    });

    g.ed += in.quantity.amount * 60 / 10000;
    _global.set(g, _self);
    // council::claim(from); 
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

    // eosio_assert(false, "not start yet.");
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