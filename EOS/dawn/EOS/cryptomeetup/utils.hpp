#pragma once
#include <string>
#include <vector>


namespace  eosio {

   /**
    *  @defgroup singleton Singleton Table 
    *  @brief Defines EOSIO Singleton Table
    *  @ingroup databasecpp 
    *  @{
    */

   /**
    *  This wrapper uses a single table to store named objects various types.
    *
    *  @tparam SingletonName - the name of this singleton variable
    *  @tparam T - the type of the singleton
    */
   template<uint64_t SingletonName, typename T>
   class singleton
   {
      /**
       * Primary key of the data inside singleton table
       * 
       * @brief Primary key  of the data singleton table
       */
      constexpr static uint64_t pk_value = SingletonName;

      /**
       * Structure of data inside the singleton table
       * 
       * @brief Structure of data inside the singleton table
       */
      struct row {
         /**
          * Value to be stored inside the singleton table
          * 
          * @brief Value to be stored inside the singleton table
          */
         T value;

         /**
          * Get primary key of the data
          * 
          * @brief Get primary key of the data
          * @return uint64_t - Primary Key
          */
         uint64_t primary_key() const { return pk_value; }

         EOSLIB_SERIALIZE( row, (value) )
      };

      typedef eosio::multi_index<SingletonName, row> table;

      public:

         /**
          * Construct a new singleton object given the table's owner and the scope
          * 
          * @brief Construct a new singleton object
          * @param code - The table's owner
          * @param scope - The scope of the table
          */
         singleton( account_name code, scope_name scope ) : _t( code, scope ) {}

         /**
          *  Check if the singleton table exists
          * 
          * @brief Check if the singleton table exists
          * @return true - if exists
          * @return false - otherwise
          */
         bool exists() {
            return _t.find( pk_value ) != _t.end();
         }

         /**
          * Get the value stored inside the singleton table. Will throw an exception if it doesn't exist
          * 
          * @brief Get the value stored inside the singleton table
          * @return T - The value stored
          */
         T get() {
            auto itr = _t.find( pk_value );
            eosio_assert( itr != _t.end(), "singleton does not exist" );
            return itr->value;
         }

         /**
          * Get the value stored inside the singleton table. If it doesn't exist, it will return the specified default value
          * 
          * @brief Get the value stored inside the singleton table or return the specified default value if it doesn't exist
          * @param def - The default value to be returned in case the data doesn't exist
          * @return T - The value stored
          */
         T get_or_default( const T& def = T() ) {
            auto itr = _t.find( pk_value );
            return itr != _t.end() ? itr->value : def;
         }

         /**
          * Get the value stored inside the singleton table. If it doesn't exist, it will create a new one with the specified default value
          * 
          * @brief Get the value stored inside the singleton table or create a new one with the specified default value if it doesn't exist
          * @param bill_to_account - The account to bill for the newly created data if the data doesn't exist
          * @param def - The default value to be created in case the data doesn't exist
          * @return T - The value stored
          */
         T get_or_create( account_name bill_to_account, const T& def = T() ) {
            auto itr = _t.find( pk_value );
            return itr != _t.end() ? itr->value
               : _t.emplace(bill_to_account, [&](row& r) { r.value = def; })->value;
         }

         /**
          * Set new value to the singleton table
          * 
          * @brief Set new value to the singleton table
          * 
          * @param value - New value to be set
          * @param bill_to_account - Account to pay for the new value
          */
         void set( const T& value, account_name bill_to_account ) {
            auto itr = _t.find( pk_value );
            if( itr != _t.end() ) {
               _t.modify(itr, bill_to_account, [&](row& r) { r.value = value; });
            } else {
               _t.emplace(bill_to_account, [&](row& r) { r.value = value; });
            }
         }

         /**
          * Remove the only data inside singleton table
          * 
          * @brief Remove the only data inside singleton table
          */
         void remove( ) {
            auto itr = _t.find( pk_value );
            if( itr != _t.end() ) {
               _t.erase(itr);
            }
         }

     // private:
         table _t;
   };

/// @} singleton
} /// namespace eosio

using namespace eosio;
using namespace std;

struct st_transfer {
    account_name from;
    account_name to;
    asset        quantity;
    string       memo;

    EOSLIB_SERIALIZE( st_transfer, (from)(to)(quantity)(memo) )
};

struct account {
    asset    balance;
    uint64_t primary_key()const { return balance.amount; }
};

typedef eosio::multi_index<N(accounts), account> accounts;
struct rec_reveal {
    uint8_t dragon ;
    uint8_t tiger ;
    checksum256 server_hash;
    checksum256 client_seed;
};

const vector<string> split(const string& s, const char& t) {
    string buff;
    vector<string> z;

    for (auto c: s) {
        if (c != t) buff += c; 
        else { 
            z.push_back(buff); 
            buff.clear(); 
        }
    }
    if (!buff.empty()) z.push_back(buff);
    return z;
}

uint64_t string_to_price(string s) {
    uint64_t z = 0;
    for (int i=0;i<s.size();++i) {
        if ('0' <= s[i] && s[i] <= '9') {
            z *= 10; 
            z += s[i] - '0';
        }
    }
    return z;
}

uint64_t string_to_int(string s) {
    uint64_t z = 0;
    for (int i=0;i<s.size();++i) {
        z += s[i] - '0';
        z *= 10;
    }
    return z;
}


string int_to_string(uint64_t t) {
    if (t == 0) return "0";
    string z;
    while (t != 0) {
        z += char('0' + (t % 10));  
        t /= 10;
    }
    reverse(z.begin(), z.end());
    return z;
}

//bet 50 ludufutemp minakokojima

class stringSplitter {
    public:
      stringSplitter(const string& _str) : str(_str) {
          current_position = 0;
      }

      bool eof() {
          return current_position == str.length();
      }

      void skip_empty() {
          while (!eof() && str[current_position] == ' ') current_position ++;
      }

      bool get_char(char* ch) {
          if (!eof()) {
              *ch  = str[current_position++];
              if (*ch == ' ') return false;
              else return true;
          } else return false;
      }

      void get_string(string* result) {
          result->clear();
          skip_empty();
          // if (eof()) return -1;
          //eosio_assert(!eof(), "No enough chars.");
          char ch;
          while (get_char(&ch)) {
              *result+= ch;
              //current_position++;
          }
          skip_empty();
      }

      void get_uint(uint64_t* result) {
          skip_empty();
          *result = 0;
          char ch;
          while (get_char(&ch)) {
              eosio_assert(ch >= '0' && ch <= '9', "Should be a valid number");
              *result = *result * 10 + ch - '0';
          }
          skip_empty();
      }
      
    private:
      string str;
      int current_position;
};




