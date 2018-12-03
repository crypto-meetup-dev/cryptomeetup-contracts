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
 
using namespace eosio;
using namespace std;

// https://github.com/EOSIO/eos/search?q=_voters&unscoped_q=_voters

using std::string;
// TODO
// 还没有办法让council, proxy退出。
// 还没实现72小时后取token。

CONTRACT council : public contract {
public:
    council( name receiver, name code, datastream<const char*> ds ) :
        contract(receiver, code, ds),
//        _proxies(receiver, receiver.value),
        _council(receiver, receiver.value) {
    }

    TABLE voter_info {
        name     to;
        asset    staked;
        int64_t  payout;        
    };

    TABLE refund_request {
        uint32_t request_time;
        asset    amount;
    };    

    TABLE global_info {
        uint64_t defer_id;
        asset    total_staked;
    };    

    /*
    TABLE proxy_info {
        name owner;
        name to;
        uint64_t     delegated_staked ;

        auto primary_key()const { return owner; }
    };*/

    TABLE council_info {
        name owner ; /// the voter
        name council; /// the producers approved by this voter if no proxy set
        uint64_t     total_votes ;
        uint64_t     unpaid ;          // 距离上次领取分红后未领取的分红
        time         last_vote_time;  // 上次领取分红的时间

        auto primary_key()const { return owner; }
    };          

    typedef singleton<"voters"_n, voter_info> singleton_voters;
    typedef singleton<"refunds"_n, refund_request> singleton_refunds;
    // typedef eosio::multi_index<"proxies"_n, proxy_info>  proxies_t;
    typedef eosio::multi_index<"council"_n, council_info>  council_t;    
//    proxies_t _proxies;
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

    /*
    void unvote( voters_t::const_iterator itr  ) {
        
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

        });
    }*/

    void unvote( /*proxies_t::const_iterator itr*/ ) {
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

/*
    void vote(voters_t::const_iterator itr) {
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
        }
    }
    */

    /*
    void vote(proxies_t::const_iterator itr) {   
        
        unvote(itr);     
        auto c = _council.find(itr->to);
        if (c != _council.end()) { 
            _council.modify(c, _code, [&](auto &cc) {
                cc.total_votes += itr->delegated_staked;
            });
        }
        
    }    
*/
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


    ACTION refund(name from) {
        require_auth( from );
        
        singleton_refunds refunds_tbl( _self, from.value );
        eosio_assert( refunds_tbl.exists(), "refund request not found" );
        auto req = refunds_tbl.get();
        eosio_assert( req.request_time + refund_delay <= now(), "refund is not available yet" );
        
        // Until now() becomes NOW, the fact that now() is the timestamp of the previous block could in theory
        // allow people to get their tokens earlier than the 1 day delay if the unstake happened immediately after many
        // consecutive missed blocks.

        action(
            permission_level{_self, "active"_n},
            EOS_CONTRACT, "transfer"_n,
            make_tuple(_self, from, req.amount, "unstake refund")
        ).send();

        refunds_tbl.remove();
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
};