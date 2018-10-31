/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */

#include "cryptomeetup.hpp"

void cryptomeetup::init() {
    require_auth(_self);    
    
    /*
    while (_market.begin() != _market.end()) {
        _market.erase(_market.begin());
    }*/

    auto g = _global.get_or_create( _self, global{});
    g.st = 1540555220;
    _global.set(g, _self);    

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

void cryptomeetup::buy_land(account_name from, extended_asset in, const vector<string>& params) {

    eosio_assert(in.contract == N(eosio.token), "only true EOS token is allowed");
    eosio_assert(in.symbol == EOS_SYMBOL, "only true EOS token is allowed");
   
    eosio_assert(params.size() >= 2, "No ID found.");
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

    out.amount /= 5;
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
    }    

    _land.modify(itr, 0, [&](auto &t) {
        t.owner = from;
        t.price = itr->next_price();
    });    
}

void cryptomeetup::buy(account_name from, extended_asset in, const vector<string>& params) {

    eosio_assert(in.contract == N(eosio.token), "only true EOS token is allowed");
    eosio_assert(in.symbol == EOS_SYMBOL, "only true EOS token is allowed.");
   
    asset out;
    _market.modify(_market.begin(), 0, [&](auto &m) {
        out = m.buy(in.amount);
    }); 

    if (out.amount > 0){
        /*
        action(
            permission_level{_self, N(active)},
            N(dacincubator), N(transfer),
            make_tuple(_self, from, out, std::string("buy some new token"))
        ).send();    
        */
    }    
}

void cryptomeetup::sell(account_name from, extended_asset in, const vector<string>& params) {
    eosio_assert(in.contract == N(dacincubator), "only true CMU token is allowed");
    eosio_assert(in.symbol == CMU_SYMBOL, "only true CMU token is allowed");
   
    asset out;
    _market.modify(_market.begin(), 0, [&](auto &m) {
        out = m.sell(in.amount);
    }); 

    if (out.amount > 0){
        action(
            permission_level{_self, N(active)},
            N(eosio.token), N(transfer),
            make_tuple(_self, from, out, std::string("sell some new token"))
        ).send();        
    }
}

void cryptomeetup::onTransfer(account_name from, account_name to, extended_asset quantity, string& memo){
    if (to != _self) return;
    require_auth(from);

    eosio_assert(quantity.is_valid(), "Invalid token transfer");
    eosio_assert(quantity.amount > 0, "must buy a positive amount");
    
    auto params = split(memo, ' ');
    eosio_assert(params.size() >= 1, "Error params");

    if (params[0] == "buy_land") {
        auto g = _global.get();
        eosio_assert(now() >= g.st, "This round will be start at 10/26/2018 @ 12:10pm (UTC).");
        buy_land(from, quantity, params);
        return;
    }

    if (params[0] == "buy") {
//      buy(from, quantity, params);
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
            permission_level{_self, N(active)},
            TOKEN_CONTRACT, N(transfer),
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
                permission_level{_self, N(active)},
                N(eosio.token), N(transfer),
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
            permission_level{_self, N(active)},
            N(eosio.token), N(transfer),
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

// 需要先转in数量的CMU到该合约地址，然后再调用这个函数。
void cryptomeetup::setAirdrop(account_name from, extended_asset in){
    require_auth(_self);
    
    _airdrop.emplace(_self, [&](auto &airdrop) {
        airdrop.id = _airdrop.available_primary_key();
        airdrop.airdropPool = in.amount;
    });
}

// 希望能够维护一个 全局变量 totalPrice, 即所有的价格之和
void cryptomeetup::airdrop(account_name from) {
    
    // 根据owner查找land，是根据第二主键查找，我觉得我写错了。
    // 而且一个玩家可能有多个land, 所以还是得遍历所有land。所以整体还得套个大循环。
    auto itr = _land.get_owner(from);

    eosio_assert(itr == _land.end(), "no land you own");
    
    uint64_t amountCMU = 0; // 玩家能够领取的cmu

    auto p = _dropPlayer.find(from);
    if (p == _dropPlayer.end()) {
        // 玩家还没领过空投，那就所有空投都领一遍。


        for (int i = 0; i < _airdrop.end().id; i++) {
            auto a = _airdrop.find(i);
            amountCMU += itr.price * a.airdropPool / totalPrice;
        }

        _dropPlayer.emplace(_self, [&](auto &dropPlayer) {
            dropPlayer.owner = from;
            dropPlayer.dropGet = _airdrop.end().id - 1;
        });
    } else {
        // 玩家已经领过空投，从上次领了的轮次后开始

        for (int k = p.dropGet + 1; k < _airdrop.end().id; k++) {
            auto ai = _airdrop.find(k);
            amountCMU += itr.price * ai.airdropPool / totalPrice;
        }

        _dropPlayer.modify(p, 0, [&](auto &d) {
            d.dropGet = _airdrop.end().id - 1;
        })
    }

    eosio_assert(amountCMU == 0, "no airdrop can get");

    action(
        permission_level{_self, N(active)},
        N(dacincubator), N(transfer),
        make_tuple(_self, from, asset(amountCMU, CMU_SYMBOL),
            std::string("CMU airdrop"))
    ).send();


}
