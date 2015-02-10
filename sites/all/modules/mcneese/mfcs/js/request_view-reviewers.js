(function ($) {
  Drupal.behaviors.mfcs_request_view_reviewers_details = {
    attach:function(context) {
      $('#mfcs-request-view-0-form .request-section-review-log-yet_to_review > .yet_to_review-list > .yet_to_review-list_item > .details_list', context).each(function(context) {
        var details_list = $(this);

        $(details_list).children('.details_list-label').children('.details_list-link').click(function(context) {
          $(details_list).children('.details_list-item').each(function() {
            if ($(this).hasClass('script-hidden')) {
              $(this).removeClass('script-hidden');
            }
            else {
              $(this).addClass('script-hidden');
            }
          });
        });
      });
    }
  }
})(jQuery);
