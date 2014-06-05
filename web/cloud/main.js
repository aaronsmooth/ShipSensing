/**
 *  @author - Pasang Sherpa
 *  @author - Aaron Nelson
 *  @author - Jonathan Forbes
 *  @author - Takatoshi Tomoyose
 */

require('cloud/app');

Parse.Cloud.afterSave("Activity", function(request, response) {
    var query = new Parse.Query(Parse.Installation),
        activity = request.object,
        status = activity.get("status"),
        smiley = status == "Arrived" ? " :)" : " :(",
        message = "Activity in EB1. [ Vessel " + status.toLowerCase() + smiley + " ]";

    Parse.Push.send({
        where: query,
        data: {
            alert: message,
            title: "Dock Monitor"
        }
    }, {
        success: function() {
            console.log("push sent");
        },
        error: function(error) {
            console.log(error);
        }
    });
});
