/**
 *  @author - Pasang Sherpa
 *  @author - Aaron Nelson
 *  @author - Jonathan Forbes
 *  @author - Takatoshi Tomoyose
 */

var express = require('express'),
    moment = require('moment'),
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

app.get('*', function(req, res) {
    Parse.Cloud.useMasterKey();
    var Activity = Parse.Object.extend("Activity"),
        activityQuery = new Parse.Query(Activity);
        activityQuery.descending("enteredAt");
    activityQuery.find({
        success: function(results) {
            res.render('content', {
                activities: results
            });
        },
        error: function(error) {
            console.log("Error: " + error.code + " " + error.message);
        }
    })
});


app.listen();
