(function ($) {
  Drupal.behaviors.mfcs_small_width_detection = {
    attach:function(context) {
      $(window).resize(function() {
        var current_width = $(window).width();
        var current_height = $(window).height();

        if (current_width <= 640) {
          $('#mcneese-body', context).each(function(context) {
            if ($(this).hasClass('page_width-large')) {
              $(this).removeClass('page_width-large');
              $(this).addClass('page_width-small');
            }
            else if (!$(this).hasClass('page_width-small')) {
              $(this).addClass('page_width-small');
            }
          });
        }
        else {
          $('#mcneese-body', context).each(function(context) {
            if ($(this).hasClass('page_width-small')) {
              $(this).removeClass('page_width-small');
              $(this).addClass('page_width-large');
            }
            else if (!$(this).hasClass('page_width-large')) {
              $(this).addClass('page_width-large');
            }
          });
        }
      });
    }
  }
})(jQuery);
