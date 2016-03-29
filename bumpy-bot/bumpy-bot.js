var Particle = require('particle-api-js'),
    Twit = require('twit'),
    config = require('./config.json');

var particleConfig = config.particleConfig,
    twitterConfig = config.twitterConfig;

var particle = new Particle(),
    twit = new Twit(twitterConfig);

// -------

particle.login(particleConfig).then(
    function(data) {
        console.log('connected to particle');
        var token = data.body.access_token;

        particle.getEventStream({ deviceId: config.deviceId, auth: token }).then(function(stream) {
            stream.on('event', function(event) {
                console.log("event: " + event.name, ' ', event.published_at, '\n', event.data);

                if (config.reallyActuallyTweet) {
                    twit.post('statuses/update', { status: 'pothole pothole pothole, bump bump bump!' }, function(err, data, response) {
                        if (err) {
                            console.log("ERROR TWEETING:", err);
                        }
                        console.log('tweeted:\n', data.id_str, '\n', data.text);
                    });
                }
            });
        });
    },

    function(err) {
        console.log('ERROR: FAILED TO LOGIN\n', err);
    }
);
