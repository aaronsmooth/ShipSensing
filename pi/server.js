/**
 *  @author - Pasang Sherpa
 *  @author - Aaron Nelson
 *  @author - Jonathan Forbes
 *  @author - Takatoshi Tomoyose
 */

var http = require('http'),
    fs = require('fs'),
    util = require('util'),
    mime = require('mime'),
    wpi = require('wiring-pi'),
    Parse = require('parse').Parse,
    RaspiCam = require('raspicam'),
    startStopDaemon = require('start-stop-daemon'),
    fd, serialAvailable, serialData,
    camera = new RaspiCam({
        mode: "timelapse",
        output: "./photo/img.jpg",
        encoding: "jpg",
        timelapse: 200,
        timeout: 86400000
    });

startStopDaemon(function() {
    Parse.initialize("GabAeJSBADI5LJzFSdTzBX7Ru4Ns2Kq2UMhtXaI8", "xwaDV6YKCz2oKfV6tw1jKeceTSXRbLH0mfgY2nP9");

    wpi.setup();
    fd = serialOpen("/dev/ttyAMA0", 9600); // Serial Open
    camera.start();
    http.createServer(function(req, resp) {
        resp.writeHead(200, {
            "Content-Type": "text/plain"
        });
        resp.write("Check the console bro..");
        resp.end();
    }).listen(8000);

    setInterval(function() {
        serialAvailable = serialDataAvail(fd);

        console.log(fd)
        console.log(serialAvailable)

        if (fd && serialAvailable) {

            serialData = serialGetchar (fd);
            console.log(serialData)

            // recordActivity('/home/pi/projects/rsas-pi/pi/photo/img.jpg', left, function() {
            //     console.log("activity saved")
            // });
        }

    }, 500);

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

}).on("stop", function() {
    this.stdout.write('Stopping at ' + new Date() + '\n');
});
