import pickle

from log import loggingSetting
from ut import pushaction, getAccounts, getTestAccounts

logger = loggingSetting("transferCMUairdrop")


def transferEos(f, to, n, m=""):
    return pushaction("eosio.token", "transfer", [f, to, "%.4f EOS" % n, m], f)


def tranferDice(f, to, n, m=""):
    return pushaction("betdicetoken", "transfer", [f, to, "%.4f DICE" % n, m], f)


def tranferAdd(f, to, n, m=""):
    return pushaction("eosadddddddd", "transfer", [f, to, "%.4f ADD" % n, m], f)


def tranferEsa(f, to, n, m=""):
    return pushaction("shadowbanker", "transfer", [f, to, "%.4f ESA" % n, m], f)

def transferCMU(f, to, n, m="test for batch transfer"):
    return pushaction("dacincubator", "transfer", [f, to, "%.4f CMU" % n, m], f)


if __name__ == "__main__":
    accounts = getAccounts()
    testAccounts = getTestAccounts()
    for i in accounts:
        print(i)
        for k in testAccounts:
            print(k)
            
            t = transferCMU(i, k, 0.0001)
            logger.info(t)
