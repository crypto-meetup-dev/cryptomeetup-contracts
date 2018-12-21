pragma solidity ^0.4.23;

contract Config {
    uint256 internal increaseLimit1 = 0.02  ether;
    uint256 internal increaseLimit2 = 0.5  ether;
    uint256 internal increaseLimit3 = 2.0  ether;
    uint256 internal increaseLimit4 = 5.0  ether;  

    uint256 internal initPrice = 0.01 ether;

    uint256 internal period = 3600 * 12;

    uint256 internal devCut1 = 5;
    uint256 internal devCut2 = 4;
    uint256 internal devCut3 = 3;
    uint256 internal devCut4 = 2;

    uint256 internal secondsPerHundredTron = 600; // 每以太坊加10min

    uint256 internal toPool = 2;

}

// tokenId 必须从1开始，否则新开一轮的时候有问题