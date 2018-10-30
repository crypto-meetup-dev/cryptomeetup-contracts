/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */

#include "cryptomeetup.hpp"

void cryptomeetup::init() {
    require_auth(_self);    

    
    while (_market.begin() != _market.end()) {
        _market.erase(_market.begin());
    }

    auto g = _global.get_or_create( _self, global{});    
    _global.set(g, _self);    

    if (_market.begin() == _market.end()) {
        const uint64_t init_dummy_supply = 40000000ll * 10000ll;
        const uint64_t init_dummy_balance = 80000ll * 10000ll;

        _market.emplace( get_self(), [&](auto &m) {
            m.supply.amount = init_dummy_supply;
            m.supply.symbol = CMU_SYMBOL;
            m.balance.amount = init_dummy_balance;
            m.balance.symbol = EOS_SYMBOL;
            m.progress = 0;
        });        
    }
}  

void cryptomeetup::newland(account_name &from, asset &eos) {
//    require_auth(_self);
    for (int i=0;i<45;++i) {
        _land.emplace( name( from ), [&](auto &p) {
            p.id = _land.available_primary_key();
            p.owner = from;
            p.price = eos.amount;
        });
    }
}

void cryptomeetup::buy_land(account_name from, extended_asset in, const vector<string>& params) {

    eosio_assert(in.contract == "eosio.token"_n, "only true EOS token is allowed");
    eosio_assert(in.quantity.symbol == EOS_SYMBOL, "only true EOS token is allowed");
   
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

    // mining...
    asset out;
    _market.modify(_market.begin(), get_self(), [&](auto &m) {
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
    }    

    _land.modify(itr, get_self(), [&](auto &t) {
        t.owner = from;
        t.price = itr->next_price();
    });    
}

void cryptomeetup::buy(account_name from, extended_asset in, const vector<string>& params) {

    eosio_assert(in.contract == "eosio.token"_n, "only true EOS token is allowed");
    eosio_assert(in.quantity.symbol == EOS_SYMBOL, "only true EOS token is allowed");

    asset out;
    _market.modify(_market.begin(), get_self(), [&](auto &m) {
        out = m.buy(in.quantity.amount);
    }); 

    if (out.amount > 0){
        /*
        action(
            permission_level{_self, "active"_n},
            "dacincubator"_n, "transfer"_n,
            make_tuple(_self, from, out, std::string("buy some new token"))
        ).send();    
        */
    }    
}

void cryptomeetup::sell(account_name from, extended_asset quantity, const vector<string>& params) {

    /*
    eosio_assert(params.size() >= 2, "No ID found.");
    auto id = string_to_price(params[1]);
   

    if (params.size() >= 3) {
       auto ref = eosio::string_to_name(params[2].c_str());
       if (is_account(ref) && ref != from) {   
          if (ref_b.amount > 0) {
            action( // winner winner chicken dinner
                permission_level{_self, "active"_n},
                N(eosio.token), "transfer"_n,
                make_tuple(_self, ref, ref_b,
                            std::string("ref bonus")))
                .send();
            }
        }    
    }*/
}

void cryptomeetup::onTransfer(account_name from, account_name to, extended_asset quantity, string& memo){
    if ( name(to) != _self) return;
    require_auth(from);

    eosio_assert(quantity.quantity.is_valid(), "Invalid token transfer");
    // eosio_assert(quantity.get_extended_symbol() == extended_symbol(EOS_SYMBOL, name() ), "only EOS token is allowed");
    eosio_assert(quantity.quantity.amount > 0, "must buy a positive amount");
    
    auto params = split(memo, ' ');
    eosio_assert(params.size() >= 1, "Error params");

    if (params[0] == "buy_land") {
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
        eosio_assert(quantity.contract == "dacincubator"_n, "must use CMU to stake");
        eosio_assert(quantity.quantity.symbol == CMU_SYMBOL, "must use CMU to stake");
        stake(from, quantity.quantity.amount);
        return;
    }
    
    /*
    
    if (memo.substr(0, 3) == "buy") {   

        auto g = _bagsglobal.get();
        eosio_assert( g.st <= now() && now() <= g.ed, "not correct time.");                                                      
        memo.erase(0, 4);
        std::size_t s = memo.find(' ');
        if (s == string::npos)
        {
            s = memo.size();
        }

        auto id = string_to_price(memo.substr(0, s));
        eosio_assert(id <= 100 || now() >= 1539403200 + 8*60*60, "no character exist");
        //  auto id = 0;
        memo.erase(0, s + 1);
        auto itr = bags.find(id);
        eosio_assert(itr != bags.end(), "no character exist");
        eosio_assert(eos.amount >= itr->next_price(), "no enough eos");
        eosio_assert(from != itr->owner, "cannot buy with yourself" );

        asset d(eos.amount - itr->next_price(), EOS_SYMBOL);

        if (d.amount > 0 && _self != from){

        action( // winner wi nner chicken dinner
            permission_level{_self, "active"_n},
            TOKEN_CONTRACT, "transfer"_n,
            make_tuple(_self, from, d,
                       std::string("refund")))
            .send();
        }

        d.amount = itr->next_price() - itr->price;

        auto ref_b = d;
        ref_b.amount /= 10;


        auto ref = eosio::string_to_name(memo.c_str());
        if (is_account(ref) && ref != from && _self != from)
        {   
            if (ref_b.amount > 0) {
            action( // winner winner chicken dinner
                permission_level{_self, "active"_n},
                N(eosio.token), "transfer"_n,
                make_tuple(_self, ref, ref_b,
                           std::string("ref bonus")))
                .send();
            }
        }
        else
        {
            g.team += ref_b.amount;
        }
        d.amount -= ref_b.amount * 4;

        g.team += ref_b.amount ;
        g.pool += ref_b.amount * 2;
        g.last = from;
        g.ed = now() + 60 * 60 * 24;

        _bagsglobal.set(g, _self);

        auto delta = d;
        delta.amount += itr->price;

    if(delta.amount > 0 &&  _self !=itr->owner){
        action( // winner winner chicken dinner
            permission_level{_self, "active"_n},
            N(eosio.token), "transfer"_n,
            make_tuple(_self, itr->owner, delta,
                       std::string("next hodl")))
            .send();
    }

        bags.modify(itr, 0, [&](auto &t) {
            t.owner = from;
            t.price = itr->next_price();
        });
        return;
    }*/
}