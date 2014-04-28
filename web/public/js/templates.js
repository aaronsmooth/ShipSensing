$(document).ready(function() {
    $('tr').on('click', function(event) {
        var id = "img-" + this.id;
        $('#' + id).toggleClass("hidden");
    });
});
