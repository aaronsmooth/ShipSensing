/**
 *  @author - Pasang Sherpa
 *  @author - Aaron Nelson
 *  @author - Jonathan Forbes
 *  @author - Takatoshi Tomoyose
 */

var express = require('express'),
    js2xml = require('cloud/lib/js2xml'),
    app = express();

// vesselapi.parseapp.com/lookup?key=name&val=Isodora&format=json
// vesselapi.parseapp.com/lookup?key=name&val=Ore%20Mutuca&format=xml
// vesselapi.parseapp.com/lookup?key=mmsi&val=636090647&format=json
// vesselapi.parseapp.com/lookup?key=imo&val=9187863&format=xml
app.get('/lookup?', function(req, res) {
    var key = req.query.key,
        val = req.query.val,
        format = req.query.format;

    if (!key || !val) {
        res.redirect('/?format=' + format);
    }

    Parse.Cloud.useMasterKey();
    var Vessel = Parse.Object.extend("Vessel"),
        vesselQuery = new Parse.Query(Vessel);
    vesselQuery.equalTo(key, val);
    vesselQuery.first({
        success: function(result) {
            result = result ? result.attributes : {
                message: "No vessel found with [" + key + " = " + val + "]"
            }
            result.photo = result.photo ? result.photo._url : undefined;
            format != "xml" ? (
                res.json({
                    result: result
                })) : res.header('Content-Type', 'text/xml').send(js2xml.toXML({
                result: result
            }, {
                header: true,
                indent: '  '
            }));

        },
        error: function(error) {
            res.json({
                error: error.message
            });
        }
    });
});

app.get('*', function(req, res) {
    var errorJson = {
        error: "Invalid input value(s).",
        example: "http://vesselapi.parseapp.com/lookup?key=name&val=Isodora&format=json",
        queryParameter: {
            key: "Search key. [mmsi, imo, id, name]",
            val: "Value of the key.",
            format: "Format of the result. [json, xml]"
        }
    }, errorXml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><root><error>Invalid input value(s).</error>\n<example>http://vesselapi.parseapp.com/lookup?key=name&amp;val=Isodora&amp;format=xml</example>\n<queryParameter>\n\t<key>Search key. [mmsi, imo, id, name]</key>\n<val>Value of the key.</val>\n\t<format>Format of the result. [json, xml]</format>\n</queryParameter></root>";
    req.query.format != "xml" ? res.json(errorJson) : res.header('Content-Type', 'text/xml').send(errorXml);
});

app.listen();
