/**
 *  @author - Pasang Sherpa
 *  @author - Aaron Nelson
 *  @author - Jonathan Forbes
 *  @author - Takatoshi Tomoyose
 */

var express = require('express'),
    js2xml = require('cloud/lib/js2xml'),
    expressLayouts = require('cloud/express-layouts'),
    _ = require('underscore'),
    app = express();

// Setup underscore to be available in all templates
app.locals._ = _;

app.set('views', 'cloud/views');
app.set('view engine', 'ejs');
app.use(expressLayouts);
app.use(express.bodyParser()); // Populate req.body
app.use(express.methodOverride());

// dockmonitor.parseapp.com/activity?limit=10&format=json
// dockmonitor.parseapp.com/activity?mmsi=636090647&limit=2&format=xml
app.get('/activity?', function(req, res) {
    var limit = req.query.limit || 10,
        format = req.query.format,
        mmsi = req.query.mmsi,
        vesselApiUrl = "http://vesselapi.parseapp.com/lookup";

    Parse.Cloud.useMasterKey();
    var Activity = Parse.Object.extend("Activity"),
        activityQuery = new Parse.Query(Activity),
        activities = [];
    activityQuery.limit(limit > 0 ? limit : 10);
    mmsi ? activityQuery.equalTo("vesselMmsi", mmsi) : "";
    activityQuery.descending("dockedAt");
    activityQuery.find().then(function(results) {
        var promise = Parse.Promise.as();
        _.each(results, function(result) {
            result = result.attributes;
            result.image = result.image ? result.image._url : undefined;
            promise = promise.then(function() {
                return Parse.Cloud.httpRequest({
                    url: vesselApiUrl,
                    params: {
                        key: "mmsi",
                        val: result.vesselMmsi,
                        format: "json"
                    }
                }).then(function(httpResponse) {
                        vesselInfo = JSON.parse(httpResponse.text);
                        result.vesselInfo = vesselInfo.result;
                        activities.push(result);
                    },
                    function(error) {
                        res.json({
                            error: error.code + ": " + error.message
                        });
                    });
            });
        });
        return promise;
    }).then(function() {
            format != "xml" ? (
                res.json({
                    activities: activities
                })) : res.header('Content-Type', 'text/xml').send(js2xml.toXML({
                activities: activities
            }, {
                header: true,
                indent: '  '
            }));
        },
        function(error) {
            res.json({
                error: error.code + ": " + error.message
            });
        });
});

app.get('*', function(req, res) {
    Parse.Cloud.useMasterKey();
    var Activity = Parse.Object.extend("Activity"),
        activityQuery = new Parse.Query(Activity);
    activityQuery.descending("dockedAt");
    activityQuery.find({
        success: function(activities) {
            res.render('content', {
                activities: activities
            });
        },
        error: function(error) {
            console.log("Error: " + error.code + " " + error.message);
        }
    })
});


app.listen();
