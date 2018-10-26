/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */
#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/transaction.hpp>
 
static constexpr time refund_delay = 3*24*3600;

#define TOKEN_CONTRACT N(dacincubator)
#define TOKEN_SYMBOL S(4, PLT)

typedef double real_type;
typedef uint8_t card ;

using std::string;
using eosio::symbol_name;
using eosio::asset;
using eosio::symbol_type;
using eosio::permission_level;
using eosio::action;

// TODO
// 还没有办法让council, proxy退出。
// 还没实现72小时后取token。


class council : public eosio::contract {
    public: council(account_name self) :
        contract(self),
        _voters(_self, _self),
        _proxies(_self, _self),
        _council(_self, _self){}

    // @abi table voter_table
    struct voter_info {
        account_name owner = 0;
        account_name to    = 0;
        uint64_t     staked = 0;
        uint64_t primary_key()const { return owner; }
    };       

    // @abi table proxy_table
    struct proxy_info {
        account_name owner = 0;
        account_name to = 0;
        uint64_t     delegated_staked = 0;
        uint64_t primary_key()const { return owner; }
    };

    // @abi table council_table
    struct council_info {
        account_name owner = 0;
        uint64_t     total_votes = 0;
        uint64_t     unpaid = 0;          // 距离上次领取分红后未领取的分红
        time         last_vote_time = 0;  // 上次领取分红的时间

        uint64_t primary_key()const { return owner; }
    };          

    typedef eosio::multi_index<N(voters),  voter_info>  voters_table;
    typedef eosio::multi_index<N(proxies), proxy_info>  proxies_table;
    typedef eosio::multi_index<N(council), council_info>  council_table;    
    voters_table _voters;
    proxies_table _proxies;
    council_table _council;
    
    void stake(account_name from, uint64_t delta) {
        require_auth(from);
        eosio_assert(delta == 0, "must stake a positive amount");

        auto itr = _voters.find(from);
        if (itr != _voters.end()) {    
            // voter already exist, update the staked information.
            unvote(itr);
            _voters.modify(itr, 0, [&](auto &v) {
                v.staked += delta;
            });
            vote(itr); 
        } else {
            // new voter.    
            _voters.emplace(_self, [&](auto &v) {
                v.owner = from;
                v.staked += delta;
            });
        }
    }

    void unstake(account_name from) {
        require_auth(from);
        auto itr = _voters.find(from);
        eosio_assert(itr != _voters.end(), "voter doesn't exist");
        unvote(itr); 
        _voters.modify(itr, 0, [&](auto &v) {
            v.staked = 0;
        });
        // todo(minakokojima): add unstake event.
    }    

    void unvote(voters_table::const_iterator itr) {
        auto p = _proxies.find(itr->to);
        if (p != _proxies.end()) { 
            _proxies.modify(p, 0, [&](auto &pp) {
                pp.delegated_staked -= itr->staked;
            });
            auto c = _council.find(p->to);
            if (c != _council.end()) {                 
                _council.modify(c, 0, [&](auto &cc) {
                    cc.total_votes -= itr->staked;
                });     
            }
            return;
        }        
        auto c = _council.find(itr->to);
        if (c != _council.end()) {             
            _council.modify(c, 0, [&](auto &cc) {
                cc.total_votes -= itr->staked;
            });
        }
        _voters.modify(itr, 0, [&](auto &v) {

        });
    }

    void unvote(proxies_table::const_iterator itr) {
        /*
        auto c = _council.find(itr->to);
        if (c != _council.end()) { 
            _council.modify(c, 0, [&](auto &cc) {
                cc.total_votes -= itr->delegated_staked;
            });
        }
        */
    }

    void unvote(account_name from) {
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
        }
    }

    void vote(voters_table::const_iterator itr) {
        unvote(itr);
        auto p = _proxies.find(itr->to);
        if (p != _proxies.end()) {             
            _proxies.modify(p, 0, [&](auto &pp) {
                pp.delegated_staked += itr->staked;
            });
            auto c = _council.find(p->to);
            if (c != _council.end()) { 
                _council.modify(c, 0, [&](auto &cc) {
                    cc.total_votes += itr->staked;
                });            
            }
            return;
        }        
        auto c = _council.find(itr->to);
        if (c != _council.end()) {             
            _council.modify(c, 0, [&](auto &cc) {
                cc.total_votes += itr->staked;
            });
            return;          
        }
    }

    void vote(proxies_table::const_iterator itr) {   
        unvote(itr);     
        auto c = _council.find(itr->to);
        if (c != _council.end()) { 
            _council.modify(c, 0, [&](auto &cc) {
                cc.total_votes += itr->delegated_staked;
            });
        }
    }    

    void vote(account_name from, account_name to) {        
        require_auth(from);
        auto v = _voters.find(from);
        if (v != _voters.end()) {                   
            _voters.modify(v, 0, [&](auto &vv) {
                vv.to = to;
            });    
            vote(v);
            return;
        }

        auto p = _proxies.find(from);
        if (p != _proxies.end()) {
            _voters.modify(v, 0, [&](auto &vv) {
                vv.to = to;
            });    
            vote(v);
            return;
        }
    }

    // 申明自己参与代理
    void runproxy(account_name from) {
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
    void runcouncil(account_name from) {
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
    void getToken(account_name from) {
        require_auth(from);        
        auto itr = _voters.find(from);
        eosio_assert(itr == _voters.end(), "this account didn't stake");        
   }
};