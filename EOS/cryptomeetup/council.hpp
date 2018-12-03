/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */
#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/transaction.hpp>
#include "utils.hpp"
 
using namespace eosio ;

using std::string;
using eosio::symbol_code;
using eosio::asset;
using eosio::extended_asset;
using eosio::permission_level;
using eosio::action;

// TODO
// 还没有办法让council, proxy退出。
// 还没实现72小时后取token。

CONTRACT council : public contract {
    public:
        council( name receiver, name code, datastream<const char*> ds ) :
            contract( receiver, code, ds ),
            _voters( code, uint64_t(eosio::name::raw(code)) ),
            _proxies( code, uint64_t(eosio::name::raw(code)) ),
            _council( code, uint64_t(eosio::name::raw(code)) ) {
                _code = code ;
            }

   TABLE voter_info {
        name owner; /// the voter
        name to; /// the proxy set by the voter, if any
        uint64_t     staked ;
        
        auto primary_key()const { return owner; }
    };       

    TABLE proxy_info {
        name owner;
        name to;
        uint64_t     delegated_staked ;

        auto primary_key()const { return owner; }
    };

    TABLE council_info {
        name owner ; /// the voter
        name council; /// the producers approved by this voter if no proxy set
        uint64_t     total_votes ;
        uint64_t     unpaid ;          // 距离上次领取分红后未领取的分红
        time         last_vote_time;  // 上次领取分红的时间

        auto primary_key()const { return owner; }
    };          

    typedef eosio::multi_index<"voters"_n,  voter_info>  voters_t;
    typedef eosio::multi_index<"proxies"_n, proxy_info>  proxies_t;
    typedef eosio::multi_index<"council"_n, council_info>  council_t;    
    voters_t _voters;
    proxies_t _proxies;
    council_t _council;


    void stake(name from, uint64_t delta) {
        /*
        require_auth(from);
        eosio_assert(delta == 0, "must stake a positive amount");

        auto itr = _voters.find(from);
        if (itr != _voters.end()) {    
            // voter already exist, update the staked information.
            unvote(itr);
            _voters.modify(itr, _code, [&](auto &v) {
                v.staked += delta;
            });
            vote(itr); 
        } else {
            // new voter.    
            
            _voters.emplace(_code, [&](auto &v) {
                v.owner = from;
                v.staked += delta;
            });
            
        }*/
    }

    void unstake(name from) {
        /*
        require_auth(from);
        auto itr = _voters.find(from);
        eosio_assert(itr != _voters.end(), "voter doesn't exist");
        unvote(itr); 
        _voters.modify(itr, _code, [&](auto &v) {
            v.staked = 0;
        });*/
        // todo(minakokojima): add unstake event.
    }    

    void unvote(voters_t::const_iterator itr) {
        /*
        auto p = _proxies.find(itr->to);
        if (p != _proxies.end()) { 
            _proxies.modify(p, _code, [&](auto &pp) {
                pp.delegated_staked -= itr->staked;
            });
            auto c = _council.find(p->to);
            if (c != _council.end()) {                 
                _council.modify(c, _code, [&](auto &cc) {
                    cc.total_votes -= itr->staked;
                });     
            }
            return;
        }        
        auto c = _council.find(itr->to);
        if (c != _council.end()) {             
            _council.modify(c, _code, [&](auto &cc) {
                cc.total_votes -= itr->staked;
            });
        }
        _voters.modify(itr, _code, [&](auto &v) {

        });*/
    }

    void unvote(proxies_t::const_iterator itr) {
        /*
        auto c = _council.find(itr->to);
        if (c != _council.end()) { 
            _council.modify(c, _code, [&](auto &cc) {
                cc.total_votes -= itr->delegated_staked;
            });
        }
        */
    }

    void unvote(name from) {
        /*
        require_auth(from);        
        auto v = _voters.find(from);
        if (v != _voters.end()) {
            unvote(v);
            return;
        }
        auto p = _proxies.find(from);
        if (p != _proxies.end()) {
            unvote(p);     
            return;
        }*/
    }

    void vote(voters_t::const_iterator itr) {/*
        unvote(itr);
        auto p = _proxies.find(itr->to);
        if (p != _proxies.end()) {             
            _proxies.modify(p, _code, [&](auto &pp) {
                pp.delegated_staked += itr->staked;
            });
            auto c = _council.find(p->to);
            if (c != _council.end()) { 
                _council.modify(c, _code, [&](auto &cc) {
                    cc.total_votes += itr->staked;
                });            
            }
            return;
        }        
        auto c = _council.find(itr->to);
        if (c != _council.end()) {             
            _council.modify(c, _code, [&](auto &cc) {
                cc.total_votes += itr->staked;
            });
            return;          
        }*/
    }

    void vote(proxies_t::const_iterator itr) {   
        /*
        unvote(itr);     
        auto c = _council.find(itr->to);
        if (c != _council.end()) { 
            _council.modify(c, _code, [&](auto &cc) {
                cc.total_votes += itr->delegated_staked;
            });
        }
        */
    }    

    void vote(name from, name to) {    
        /*    
        require_auth(from);
        auto v = _voters.find(from);
        if (v != _voters.end()) {                   
            _voters.modify(v, _code, [&](auto &vv) {
                vv.to = to;
            });    
            vote(v);
            return;
        }

        auto p = _proxies.find(from);
        if (p != _proxies.end()) {
            _voters.modify(v, _code, [&](auto &vv) {
                vv.to = to;
            });    
            vote(v);
            return;
        }*/
    }

    // 申明自己参与代理
    void runproxy(name from) {
        /*
        require_auth(from);

        // warning!!!
        // 申明成为代理需要哪些条件？
        // warning!!!

        auto itr_proxy = _proxies.find(from);
        eosio_assert(itr_proxy != _proxies.end(), "already be proxy");
        _proxies.emplace(_self, [&](auto &p) {
            p.owner = from;
        });

        // warning!!!
        // 打出event, 让前端知道
        // warning!!!
    }    

    // 申明自己参与委员会
    void runcouncil(name from) {
        require_auth(from);

        // warning!!!
        // 申明成为委员会需要哪些条件？
        // warning!!!
        
        auto itr_council = _council.find(from);
        eosio_assert(itr_council != _council.end(), "already be council");
        _council.emplace(_self, [&](auto &c) {
            c.owner = from;
        });


        // warning!!!
        // 打出event, 让前端知道
        // warning!!!
        */
    }

    // unstake 72小时后可以取回token
    void getToken(name from) {
        /*
        require_auth(from);        
        auto itr = _voters.find(from);
        eosio_assert(itr == _voters.end(), "this account didn't stake");     */   
   }
};




/*
#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/transaction.hpp>

#include "config.hpp"
#include "utils.hpp"

using namespace eosio;
using namespace std;

struct st_transfer {
    name   from;
    name   to;
    asset  quantity;
    string memo;
};

static constexpr uint32_t refund_delay = 1*24*3600;

CONTRACT payout : public contract {
public:
    payout(name receiver, name code, datastream<const char*> ds): 
        contract(receiver, code, ds),
        _global(receiver, receiver.value) {
    }

    ACTION init();
    ACTION unstake(name from, asset delta);
    ACTION claim(name from);    
    ACTION transfer(name from, name to, asset quantity, string memo);
    void onTransfer(name from, name to, extended_asset in, string memo);
    void stake(name from, asset delta);
    void make_profit(uint64_t delta);

    struct [[eosio::table]] voter_info {
        name     to;
        asset    staked;
        int64_t  payout;        
    };

    struct [[eosio::table]] refund_request {
        name     owner;
        uint32_t request_time;
        asset    amount;

        uint64_t  primary_key()const { return owner.value; }
    };

    struct [[eosio::table]] global_info {
        uint64_t defer_id;
        asset    total_staked;
        int128_t earnings_per_share;
    };

    typedef singleton<"voters"_n, voter_info> singleton_voters;
    typedef singleton<"global"_n, global_info> singleton_global;
    typedef multi_index<"refunds"_n, refund_request> refunds_table;

    singleton_global _global;

    uint64_t get_next_defer_id() {
        auto g = _global.get();    
        g.defer_id += 1;
        _global.set(g,_self);
        return g.defer_id;
    }

    template <typename... Args>
    void send_defer_action(Args&&... args) {
        transaction trx;
        trx.actions.emplace_back(std::forward<Args>(args)...);
        trx.send(get_next_defer_id(), _self, false);
    }    

    ACTION refund(name owner) {
        require_auth( owner );
        
        refunds_table refunds_tbl( _self, owner.value );
        auto req = refunds_tbl.find( owner.value );
        eosio_assert( req != refunds_tbl.end(), "refund request not found" );
        eosio_assert( req->request_time + refund_delay <= now(), "refund is not available yet" );
        
        // Until now() becomes NOW, the fact that now() is the timestamp of the previous block could in theory
        // allow people to get their tokens earlier than the 3 day delay if the unstake happened immediately after many
        // consecutive missed blocks.

        action(
            permission_level{_self, "active"_n},
            EOS_CONTRACT, "transfer"_n,
            make_tuple(_self, owner, req->amount, "unstake refund")
        ).send();

      //  INLINE_ACTION_SENDER(eosio::token, transfer)( N(eosio.token), {N(eosio.stake),N(active)},
        //                                            { N(eosio.stake), req->owner, req->net_amount + req->cpu_amount, std::string("unstake") } );
        refunds_tbl.erase( req );
    }

    void apply(uint64_t receiver, uint64_t code, uint64_t action) {
        auto &thiscontract = *this;
        if (action == name("transfer").value) {
            auto transfer_data = unpack_action_data<st_transfer>();
            onTransfer(transfer_data.from, transfer_data.to, extended_asset(transfer_data.quantity, name(code)), transfer_data.memo);
            return;
        }

        switch (action) {
            EOSIO_DISPATCH_HELPER(payout, (unstake)(refund)(claim) )
        }
    }
};

extern "C" {
    [[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
        payout p( name(receiver), name(code), datastream<const char*>(nullptr, 0) );
        p.apply(receiver, code, action);
        eosio_exit(0);
    }
}
*/