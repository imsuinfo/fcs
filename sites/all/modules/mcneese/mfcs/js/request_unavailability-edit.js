(function ($) {
  Drupal.behaviors.mfcs_generate_unavailability_timepicker = {
    attach:function(context) {
      $('#field-unavailable-date_start-time').timepicker({
        'timeFormat': 'g:ia',
        'step': 15
      });

      $('#field-unavailable-date_stop-time').timepicker({
        'timeFormat': 'g:ia',
        'step': 15
      });
    }
  }
})(jQuery);
