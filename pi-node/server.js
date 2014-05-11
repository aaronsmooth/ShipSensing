/**
 *  @author - Pasang Sherpa
 *  @author - Aaron Nelson
 *  @author - Jonathan Forbes
 *  @author - Takatoshi Tomoyose
 */

var express = require('express'),
    fs = require('fs'),
    util = require('util'),
    mime = require('mime'),
    Parse = require('parse').Parse,
    RaspiCam = require('raspicam'),
    camera = new RaspiCam({
        mode: "timelapse",
        output: "./photo/img.jpg",
        encoding: "jpg",
        timelapse: 200,
        timeout: 86400000
    }),
    port = process.env.PORT || 8000,
    app = express();

Parse.initialize("YwcaugA0e48pvA2Rsmj7yIT9GbOHCPitW3LDPnlq", "KI3qObjuxgfRLTosry7qgmOOFErXEYfuzxgJkiEg");
camera.start();

function recordActivity(path, value, callback) {
    var Activity = Parse.Object.extend("Activity"),
        activity = new Activity(),
        image, fileData, now = new Date();
    filedata = Array.prototype.slice.call(new Buffer(fs.readFileSync('/home/pi/projects/rsas-pi/pi/photo/img.jpg')), 0)
    image = new Parse.File("image.jpg", filedata);
    image.save().then(function(file) {
        console.log("saving activity to parse");
        activity.set("enteredAt", now);
        activity.set("value", value);
        activity.set("photo", file);
        activity.set("photoUrl", file.url());
        activity.save();
        callback(activity);
    });
}

app.post("/activity/add", function(req, res) {
    /* some server side logic */
    console.log("add activity")
    res.send("OK");
});

app.get("*", function(req, res) {
    res.send("OK");
});

app.listen(port, function() {
    console.log("Listening on " + port);
});
