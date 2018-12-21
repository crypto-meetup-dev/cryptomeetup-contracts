pragma solidity ^0.4.23;

contract Config {
    uint256 internal increaseLimit1 = 20  trx;
    uint256 internal increaseLimit2 = 500  trx;
    uint256 internal increaseLimit3 = 2000  trx;
    uint256 internal increaseLimit4 = 5000  trx;  

    uint256 internal initPrice = 10 trx;

    uint256 internal period = 3600 * 24 * 30;

    uint256 internal devCut1 = 5;
    uint256 internal devCut2 = 4;
    uint256 internal devCut3 = 3;
    uint256 internal devCut4 = 2;

    uint256 internal secondsPerHundredTron = 60;

    uint256 internal toPool = 2;

}

// tokenId 必须从1开始，否则新开一轮的时候有问题