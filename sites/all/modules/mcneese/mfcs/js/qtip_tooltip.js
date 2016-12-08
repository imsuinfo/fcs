(function ($) {
  Drupal.behaviors.mfcs_data_qtip_tooltip = {
    attach:function(context) {
      $('.mfcs-qtip-tooltip').each(function(context) {
        var qtip_element = $(this);

        $(qtip_element).qtip({
          id: $(qtip_element).attr('qtip_tooltip-id'),
          content: {
            title: $(qtip_element).attr('qtip_tooltip-title'),
            text: $(qtip_element).attr('qtip_tooltip-text')
          },
          style: {
            classes: $(qtip_element).attr('qtip_tooltip-class')
          },
          position: {
            my: 'top center',
            at: 'bottom center'
          }
        });
      });
    }
  }
})(jQuery);
