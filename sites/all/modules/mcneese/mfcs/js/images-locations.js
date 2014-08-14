(function ($) {
  Drupal.behaviors.mfcs_images_location = {
    attach:function(context) {
      $('#field-event-location-0 > .form-item.form-type-radio > input', context).each(function() {
        var item = $(this);

        $(item).click(function() {
          $('#field-event-location-0 > .location-image', context).each(function() {
            if ($(this).hasClass('location-image-visible')) {
              $(this).removeClass('location-image-visible');
            }

            if (!$(this).hasClass('location-image-invisible')) {
              $(this).addClass('location-image-invisible');
            }
          });

          if ($(item).val() == 9) {
            $('#field-event-location-0-image-1', context).each(function() {
              if ($(this).hasClass('location-image-invisible')) {
                $(this).removeClass('location-image-invisible');
              }

              if (!$(this).hasClass('location-image-visible')) {
                $(this).addClass('location-image-visible');
              }
            });
          }
          else {
            $('#field-event-location-0-image-0', context).each(function() {
              if ($(this).hasClass('location-image-invisible')) {
                $(this).removeClass('location-image-invisible');
              }

              if (!$(this).hasClass('location-image-visible')) {
                $(this).addClass('location-image-visible');
              }
            });
          }
        });
      });
    }