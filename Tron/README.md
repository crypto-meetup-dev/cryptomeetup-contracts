# Cryptomeetup with TronBox 

## How to init it

First, install TronBox globally, if you didn't yet
```
npm i -g tronbox
```

Second, pull this repository
```
git clone git@github.com:flyq/troncryptomeetup.git
```

Third, new a tronbox.js
refer: https://developers.tron.network/docs/tron-box-contract-deployment#section-configuration

Fourth, compile the smart contract
```
tronbox compile
```

Fifth, deploy it
```
tronbox migrate --reset
```

Now you are ready to start developing your portal. Enjoy!




