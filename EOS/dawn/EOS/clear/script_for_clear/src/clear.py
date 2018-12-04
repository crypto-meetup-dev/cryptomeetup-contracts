import pickle
import json
import os


from log import loggingSetting
from ut import pushaction, getAccounts, getTestAccounts, getRawAccounts, getTable

logger = loggingSetting("rawAccountAirdrop")

def transferCMU(f, to, n, m=""):
    return pushaction("dacincubator", "transfer", [f, to, "%.4f CMU" % n, m], f)

def clear(scope):
    return pushaction("cryptomeetup", "newland", [scope,"0.0000 EOS"], "cryptomeetup")

if __name__ == "__main__":
    """
    t = getTable("cryptomeetup", "tmonomonomon", "voters")
    print(type(t))
    print(t)
    h =  t.decode("utf-8")
    row  = json.loads(h)["rows"]
    print(row)
    staked_row = row[0]
    staked = staked_row['staked']
    print(staked)
    print(type(staked))
cleos -u http://api.eosbeijing.one get table cryptomeetup sungod123224 voters



    <class 'bytes'>
    b'{\n  "rows": [{\n      "to": "",\n      "staked": 401000\n    }\n  ],\n  "more": false\n}\n'
    [{'staked': 401000, 'to': ''}]
    401000
    <class 'int'>
"""

    RawAccounts = getRawAccounts()
    i = 0
    for j in RawAccounts:
        s = json.dumps(j)
        scope = json.loads(s)["scope"]
        print(scope)
        print(i)
        lo = clear(scope)
        logger.info(lo)
        i = i + 1
        print("\n")
