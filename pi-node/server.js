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

app.configure(function() {
    app.use(express.bodyParser());
    app.use(express.methodOverride());
});

app.get("/save/:mmsi/:utime/:st", function(req, res) {
    var mmsi = req.params.mmsi,
        utime = req.params.utime,
        st = req.params.st,
        status = st == "S" ? "Arrived" : "Departed";

    console.log("mmsi:" + mmsi)
    console.log("utime:" + utime)
    console.log("status:" + status)

    var Activity = Parse.Object.extend("Activity"),
        activity = new Activity(),
        image, fileData, dockedAt = new Date(utime * 1000);
    filedata = Array.prototype.slice.call(new Buffer(fs.readFileSync("/home/pi/projects/ShipSensing/pi-node/photo/img.jpg")), 0)
    image = new Parse.File("img.jpg", filedata);
    image.save().then(function(file) {
        console.log("saving activity to parse");
        activity.set("vesselMmsi", mmsi);
        activity.set("dockedAt", dockedAt);
        activity.set("status", status);
        activity.set("image", file);
        activity.save();
        return res.send(activity);
    });
});

app.get("/add/:mmsi/:utime/:st", function(req, res) {
    console.log("add activity requested");
    var mmsi = req.params.mmsi,
        utime = req.params.utime,
        st = req.params.st;
    if (!mmsi || !utime || !st) {
        return res.send(400, "invalid inputs");
    }
    camera.start();
    camera.on("read", function(err, timestamp, filename) {
        camera.stop();
        console.log("photo image captured with filename: " + filename);
        return res.redirect('save/' + mmsi + "/" + utime + "/" + st);
    });
});

function recordActivity(mmsi, utime, status, callback) {
    var Activity = Parse.Object.extend("Activity"),
        activity = new Activity(),
        image, fileData, dockedAt = new Date(utime * 1000);
    filedata = Array.prototype.slice.call(new Buffer(fs.readFileSync("/home/pi/projects/ShipSensing/pi-node/photo/img.jpg")), 0)
    image = new Parse.File("img.jpg", filedata);
    image.save().then(function(file) {
        console.log("saving activity to parse");
        activity.set("vesselMmsi", mmsi);
        activity.set("dockedAt", dockedAt);
        activity.set("status", status);
        activity.set("image", file);
        activity.save();
        callback(activity);
    });
}

app.post("/activity", function(req, res) {
    var mmsi = req.body.mmsi,
        utime = req.body.utime,
        st = req.body.st,
        status = st == "S" ? "Arrived" : "Departed";

    if (!mmsi || !utime || !st) {
        return res.send(400, "invalid inputs");
    }
    recordActivity(mmsi, utime, status, function(activity) {
        return res.send(activity);
    });
});

app.get("*", function(req, res) {
    res.send("OK");
});

app.listen(port, function() {
    console.log("Listening on " + port);
});
