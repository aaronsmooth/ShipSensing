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

// http://dockmonitor.parseapp.com/activity?limit=2&format=json
// http://dockmonitor.parseapp.com/activity?limit=none&format=json
// http://dockmonitor.parseapp.com/activity?mmsi=248264000&limit=2&format=xml
app.get('/activity?', function(req, res) {
    var limit = req.query.limit || 10,
        format = req.query.format,
        mmsi = req.query.mmsi,
        vesselApiUrl = "http://vesselapi.parseapp.com/lookup";

    Parse.Cloud.useMasterKey();
    var Activity = Parse.Object.extend("Activity"),
        activityQuery = new Parse.Query(Activity),
        activities = [], activityCount  = 0;
    limit != "none" ? activityQuery.limit(limit > 0 ? limit : 10) : "";
    mmsi ? activityQuery.equalTo("vesselMmsi", mmsi) : "";
    activityQuery.descending("dockedAt");
    activityQuery.find().then(function(results) {
        var promise = Parse.Promise.as();
        _.each(results, function(result) {
            var id = result.id;
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
                        result.id = id;
                        activities.push(result);
                    },
                    function(error) {
                        res.json({
                            error: error.code + ": " + error.message
                        });
                    });
            });
        });
        activityCount = results.length;
        return promise;
    }).then(function() {
            var results = {};
            results.activities = activities;
            results.activityCount = activityCount;
            format != "xml" ? (
                res.json({
                    results: results
                })) : res.header('Content-Type', 'text/xml').send(js2xml.toXML({
                results: results
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
    var url = "http://dockmonitor.parseapp.com/activity";
    Parse.Cloud.httpRequest({
        url: url,
        params: {
            limit: "none",
            format: "json"
        }
    }).then(function(httpResponse) {
            var results = JSON.parse(httpResponse.text);
            res.render('content', {
                results: results.results
            });
        },
        function(error) {
            res.json({
                error: error.code + ": " + error.message
            });
        });
});

app.listen();
