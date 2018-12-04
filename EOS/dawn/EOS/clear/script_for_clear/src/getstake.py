import pickle
import json

from log import loggingSetting
from ut import pushaction, getAccounts, getTestAccounts, getRawAccounts


def transferCMU(f, to, n, m=""):
    return pushaction("dacincubator", "transfer", [f, to, "%.4f CMU" % n, m], f)

if __name__ == "__main__":
    accounts = getAccounts()    
    RawAccounts = getRawAccounts()

    _totalAmount = 0
    for j in RawAccounts:
        s = json.dumps(j)
        _amount = json.loads(s)["price"]
        _totalAmount += _amount        

    print("total amount of price: %f" % _totalAmount)

    for acc in accounts:
        print(acc)
        for k in RawAccounts:
            d = json.dumps(k)
            _accountName = json.loads(d)["owner"]
            _amount = json.loads(d)["price"]
            _transferAmount = _amount * 3000 / _totalAmount
            print(k)
            print("price: %f" % _amount)
            print("transfer CMU amount: %f to %s" % (_transferAmount, _accountName))
            t = transferCMU(acc, _accountName, _transferAmount)
            logger.info(t)
            print("\n")
            
