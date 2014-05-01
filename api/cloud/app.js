/**
 *  @author - Pasang Sherpa
 *  @author - Aaron Nelson
 *  @author - Jonathan Forbes
 *  @author - Takatoshi Tomoyose
 */

var express = require('express'),
    app = express();

app.get('/lookup/:key/:val', function(req, res) {
    var key = req.params.key,
        val = req.params.val;

    Parse.Cloud.useMasterKey();
    var Vessel = Parse.Object.extend("Vessel"),
        vesselQuery = new Parse.Query(Vessel);
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
        queryParameter: {
            key: "Search key (mmsi, imo, id, name)",
            val: "Value of the key"
        }
    });
});

app.listen();
