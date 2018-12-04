import pickle
import json

from log import loggingSetting
from ut import pushaction, getAccounts, getTestAccounts, getRawAccounts

def transferCMU(f, to, n, m=""):
    return pushaction("dacincubator", "transfer", [f, to, "%.4f CMU" % n, m], f)

if __name__ == "__main__":
    accounts = getAccounts()
    RawAccounts = getRawAccounts()
    RawAccounts = RawAccounts.sort(key=operator.itemgetter('price'),reverse=True)
    

    _totalAmount = 0;
    for j in RawAccounts:
        s = json.dumps(j)
        _amount = json.loads(s)["price"]
        _totalAmount += _amount
        
    
    print(_totalAmount)
    
    for i in RawAccounts:
        d = json.dumps(i)
        _accountName = json.loads(d)["owner"]
        _amount = json.loads(d)["price"]
        _transferAmount = _amount * 1000 / _totalAmount
        print(i)
        print(_transferAmount)
        
