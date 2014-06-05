/**
 *  @author - Pasang Sherpa
 *  @author - Aaron Nelson
 *  @author - Jonathan Forbes
 *  @author - Takatoshi Tomoyose
 */

require('cloud/app');

Parse.Cloud.afterSave("Activity", function(request, response) {
    var query = new Parse.Query(Parse.Installation);
    Parse.Push.send({
        where: query,
        data: {
            alert: "There is some activity at EB1.",
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
