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
    
    void stake(account_name from, uint64_t delta) {
        require_auth(from);
        eosio_assert(delta == 0, "must stake a positive amount");

        auto itr = _voters.find(from);
        if (itr == _voters.end()) {    
            _voters.emplace(_self, [&](auto &v) {
                v.owner = from;
                v.staked += delta;
            });
        } else {  
            _voters.modify(itr, 0, [&](auto &v) {
                v.staked += delta;
            });
        }
    }

    void unstake(account_name from) {
        
        require_auth(from);

        /*

        auto itr = _voters.find(from);
        eosio_assert(itr == _voters.end(), "this account didn't stake");
        eosio_assert(itr->unstakeTime != 0, "this account already unstake");


        account_name proxy_temp = itr->proxy;
        if (proxy_temp != 0) { // 普通人委托代理投票情形
            eosio_assert(itr -> council != 0, "council should be 0"); // 如果设置了代理，那么就必须不要设置自己的token投给谁
            
            auto itr_proxy = _proxies.find(proxy_temp);
            eosio_assert(itr_proxy == _proxies.end(), "proxy don't exist"); // 这个情况应该不会出现，出现了表示合约的_proxies table维护出了问题
            
            _proxies.modify(itr_proxy, 0, [&](auto &p) {
                p.voted -= itr->voted;
            });

            auto itr_council = _council.find(itr_proxy->council);
            eosio_assert(itr_council == _council.end(), "council don't exist"); // 这个情况应该不会出现，出现了表示合约的_proxies table维护出了问题

            _council.modify(itr_council, 0, [&](auto &c) {
                c.total_votes -= itr->voted;
            });

        } else {    // 普通人自己投票或者代理人投票情形
            eosio_assert(itr -> council == 0, "council should not be 0"); // 如果没有设置了代理，那么就必须要设置自己的token投给谁
            
            auto itr_council_2 = _council.find(itr->council);
            eosio_assert(itr_council_2 == _council.end(), "council don't exist"); // 这个情况应该不会出现，出现了表示合约的_proxies table维护出了问题
            
            _council.modify(itr_council_2, 0, [&](auto &c) {
                c.total_votes -= itr->voted;
            });

        }

        _voters.modify(itr, 0, [&](auto &v) {
            v.unstakeTime = now();
            v.voted = 0;
        });

        // warning!!!
        // 打出对应event, 让前端知道
        // warning!!! 
        */
    }    

    void unvote(account_name from) {
        require_auth(from);
        
        auto v = _voters.find(from);
        if (v != _voters.end()) {
            return;
        }
        auto p = _proxies.find(from);
        if (p != _proxies.end()) {
            return;
        }
    }

    void vote(account_name from, account_name to) {
        
        require_auth(from);
        unvote(from);

        auto v = _voters.find(from);
        if (v != _voters.end()) {
            auto c = _council.find(to);
            if (c != _council.end()) {
                
            } else {
                auto p = _proxies.find(to);
                if (p != _proxies.end()) {

                }
            }
            return;
        }

        auto p = _proxies.find(from);
        if (p != _proxies.end()) {
            return;
        }

        /*
        auto itr = _voters.find(from);
        if (itr == _voters.end()) { // 此时from是代理账户投票
            auto itr_proxy = _proxies.find(from);
            eosio_assert(itr_proxy == _proxies.end(), "can't find this account in voter and proxy");

            eosio_assert(itr_proxy->council == councilAccount, "already vote to this account");

            _proxies.modify(itr_proxy, 0, [&](auto &p) {
                p.council = councilAccount;
                p.voted = p.staked + p.delegated_staked;
            });

            _council.modify(itr_council, 0, [&](auto &c) {
                c.total_votes += itr_proxy->voted;
            });

            // warning!!!
            // 打出对应event, 让前端知道
            // warning!!!

        } else { // 此时是玩家自己投票
            eosio_assert(itr->proxy != 0, "proxy account can't  vote");
            eosio.assert(itr->council == councilAccount, "already vote to this account");

            _voters.modify(itr, 0, [&](auto &v) {
                v.council = councilAccount;
                v.voted = v.staked;
            });

            _council.modify(itr_council, 0, [&](auto &c) {
                c.total_votes += itr->voted;
            });


            // warning!!!
            // 打出对应event, 让前端知道
            // warning!!!
        }        */
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


    // @abi table voter_table
    struct voter_info {
        account_name owner = 0; /// the voter
        account_name proxy = 0; /// the proxy set by the voter, if any
        account_name council;   /// the council approved by this voter if no proxy set
        uint64_t     staked = 0;    // 玩家抵押了多少CMU
        uint64_t     voted = 0;     // 玩家投了多少票（CMU）这个数据和staked在玩家执行完
                                    // unstake()后还没有取回CMU的时间内是不同的
        time         unstakeTime = 0;   /// 玩家执行unstake的时刻

        uint64_t primary_key()const { return owner; }
    };       

    // @abi table proxy_table
    struct proxy_info {
        account_name owner = 0; /// the voter
        account_name council; /// the council approved by this voter if no proxy set
        uint64_t     staked = 0;
        uint64_t     delegated_staked = 0;
        uint64_t     voted;     // 代理人投了多少票

        uint64_t primary_key()const { return owner; }
    };

    // @abi table council_table
    struct council_info {
        account_name owner = 0; /// the voter
        uint64_t     total_votes = 0;
        uint64_t     unpaid = 0;          // 距离上次领取分红后未领取的分红
        time         last_vote_time = 0;  // 上次领取分红的时间

        uint64_t primary_key()const { return owner; }
    };          

    typedef eosio::multi_index<N(voters),  voter_info>  voters_table;
    typedef eosio::multi_index<N(proxies), voter_info>  proxies_table;
    typedef eosio::multi_index<N(council), voter_info>  council_table;    
    voters_table _voters;
    proxies_table _proxies;
    council_table _council;

};