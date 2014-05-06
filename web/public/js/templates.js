$(document).ready(function() {
    $('tr').on('click', function(event) {
        var id = "img-" + this.id,
            tr = $('#' + id);
        if (tr.hasClass("selected")) tr.removeClass("selected")
        else {
            $('.selected').addClass("hidden").removeClass("selected")
            tr.addClass("selected");
        }
        tr.toggleClass("hidden");
    });
});
