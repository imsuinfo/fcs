(function ($) {
  Drupal.behaviors.mfcs_calendar_day_zindex = {
    attach:function(context) {
      $('#mfcs-calendar-0-day > .calendar-day > .calendar-body-wrapper.calendar-range > .calendar-body > .calendar-time_slot-wrapper > .calendar-time_slot > .calendar-time_slot-request-wrapper > .calendar-time_slot-request > .calendar-item-wrapper', context).each(function(context) {
        var item_wrapper = $(this);

        $(item_wrapper).click(function(context) {
          if ($(item_wrapper).hasClass('item-zindex-normal')) {
            $(item_wrapper).removeClass('item-zindex-normal');
            $(item_wrapper).addClass('item-zindex-top');
          }
          else if ($(this).hasClass('item-zindex-top')) {
            $(item_wrapper).removeClass('item-zindex-top');
            $(item_wrapper).addClass('item-zindex-bottom');
          }
          else if ($(this).hasClass('item-zindex-bottom')) {
            $(item_wrapper).removeClass('item-zindex-bottom');
            $(item_wrapper).addClass('item-zindex-normal');
          }
        });
      });
    }
  }
})(jQuery);
