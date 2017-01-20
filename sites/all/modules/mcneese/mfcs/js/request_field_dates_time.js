(function ($) {
  Drupal.behaviors.mfcs_generate_request_field_dates_time_timepicker = {
    attach:function(context) {
      $('.field-request-item-dates-time_start > .form-type-textfield > .form-text').each(function() {
        $(this).timepicker({
          'timeFormat': 'g:ia',
          'step': 15
        });
      });

      $('.field-request-item-dates-time_stop > .form-type-textfield > .form-text').each(function() {
        $(this).timepicker({
          'timeFormat': 'g:ia',
          'step': 15
        });
      });
    }
  }
})(jQuery);
