(function ($) {
  Drupal.behaviors.mcneese_front_page_cycle_script_fix = {
    attach:function(context) {
      $('#mcneese-front_page-featured-image', context).removeClass('noscript').each(function() {
        $('#mcneese-front_page-featured-image-previous', context).removeClass('noscript').each(function() {
          $(this).attr('style', '');
        });

        $('#mcneese-front_page-featured-image-next', context).removeClass('noscript').each(function() {
          $(this).attr('style', '');
        });
      });
    }
  }
})(jQuery);
