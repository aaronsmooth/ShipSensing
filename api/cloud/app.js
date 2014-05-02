/**
 *  @author - Pasang Sherpa
 *  @author - Aaron Nelson
 *  @author - Jonathan Forbes
 *  @author - Takatoshi Tomoyose
 */

var express = require('express'),
    app = express();



// vesselapi.parseapp.com/lookup?key=name&val=Isodora
// vesselapi.parseapp.com/lookup?key=name&val=Ore%20Mutuca
// vesselapi.parseapp.com/lookup?key=mmsi&val=636090647
// vesselapi.parseapp.com/lookup?key=imo&val=9187863

app.get('/lookup?', function(req, res) {

    var key = req.query.key,
        val = req.query.val;
    if (!key || !val) {
        res.redirect('/');
    }

    Parse.Cloud.useMasterKey();
    var Vessel = Parse.Object.extend("Vessel"),
        vesselQuery = new Parse.Query(Vessel);
    console.log("key:" + key + " val:" + val);
    vesselQuery.equalTo(key, val);
    vesselQuery.first({
        success: function(result) {
            result = result ? result : {
                message: "No vessel found with [" + key + " = " + val + "]"
            }
            res.json({
                result: result
            });
        },
        error: function(error) {
            res.json({
                error: error.message
            });
        }
    });
});

app.get('*', function(req, res) {
    res.json({
        error: "Invalid input value(s)",
        example: "http://vesselapi.parseapp.com/lookup?key=name&val=Isodora",
        queryParameter: {
            key: "Search key (mmsi, imo, id, name)",
            val: "Value of the key"
        }
    });
});

app.listen();
