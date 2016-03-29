
## Quickstart

1. `git clone https://github.com/buzzfeed-openlab/bumpy-lumpy.git`
2. `cd ./bumpy-lumpy/bumpy-bot`
3. `npm install`
4. `touch config.json`, fill it out
5. `node bumpy-bot.js`

### Sample config
```
{
    "deviceId": "xxxxx",
    "reallyActuallyTweet": false,

    "particleConfig": {
        "username": "xxxxx",
        "password": "xxxxx"
    },
    "twitterConfig": {
       "consumer_key":         "xxxxx",
       "consumer_secret":      "xxxxx",
       "access_token":         "xxxxx-xxxxx",
       "access_token_secret":  "xxxxx",
       "timeout_ms":           10000
    }
}
```
