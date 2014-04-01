(function ($) {
  Drupal.behaviors.jquery_ui_prepper = {
    attach:function(context) {



      // ineractions
      $('.mcneese-draggable', context).each(function() {
        var item = $(this).draggable();
        var options = $(this).attr('draggable_options');

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).draggable('option', first, second);
            }
          }
        }
      });

      $('.mcneese-droppable', context).each(function() {
        var item = $(this).droppable();
        var options = $(this).attr('droppable_options');

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).droppable('option', first, second);
            }
          }
        }
      });

      $('.mcneese-resizable', context).each(function() {
        var item = $(this).resizable();
        var options = $(this).attr('resizable_options');

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).resizable('option', first, second);
            }
          }
        }
      });

      $('.mcneese-selectable', context).each(function() {
        var item = $(this).selectable();
        var options = $(this).attr('selectable_options');

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).selectable('option', first, second);
            }
          }
        }
      });

      $('.mcneese-sortable', context).each(function() {
        var item = $(this).sortable();
        var options = $(this).attr('sortable_options');

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).sortable('option', first, second);
            }
          }
        }
      });



      // widgets
      $('.mcneese-accordion', context).each(function() {
        var item = null;
        var options = $(this).attr('accordion_options');

        // heightStyle only works during creation time it seems.
        var heightStyle = $(this).attr('accordion_option-heightStyle');

        if (heightStyle == '' || heightStyle == 'auto') {
          item = $(this).accordion({heightStyle: 'auto'});
        }
        else if (heightStyle == 'fill') {
          item = $(this).accordion({heightStyle: 'fill'});
        }
        else if (heightStyle == 'content') {
          item = $(this).accordion({heightStyle: 'content', autoHeight: false});
        }

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).accordion('option', first, second);
            }
          }
        }
      });

      $('.mcneese-autocomplete', context).each(function() {
        var item = $(this).autocomplete();
        var options = $(this).attr('autocomplete_options');

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).autocomplete('option', first, second);
            }
          }
        }
      });

      $('.mcneese-button', context).each(function() {
        var item = $(this).button();
        var options = $(this).attr('button_options');

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).button('option', first, second);
            }
          }
        }
      });

      $('.mcneese-datepicker', context).each(function() {
        var item = $(this).datepicker();
        var options = $(this).attr('daterpicker_options');

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).datepicker('option', first, second);
            }
          }
        }
      });

      $('.mcneese-dialog', context).each(function() {
        var item = $(this).dialog();
        var options = $(this).attr('dialog_options');

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).dialog('option', first, second);
            }
          }
        }
      });

      $('.mcneese-menu', context).each(function() {
        var item = $(this).menu();
        var options = $(this).attr('menu_options');

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).menu('option', first, second);
            }
          }
        }
      });

      $('.mcneese-progressbar', context).each(function() {
        var item = $(this).progressbar();
        var options = $(this).attr('progressbar_options');

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).progressbar('option', first, second);
            }
          }
        }
      });

      $('.mcneese-slider', context).each(function() {
        var item = $(this).slider();
        var options = $(this).attr('slider_options');

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).slider('option', first, second);
            }
          }
        }
      });

      $('.mcneese-spinner', context).each(function() {
        var item = $(this).spinner();
        var options = $(this).attr('spinner_options');

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).spinner('option', first, second);
            }
          }
        }
      });

      $('.mcneese-tabs', context).each(function() {
        var item = $(this).tabs();
        var options = $(this).attr('tabs_options');

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).tabs('option', first, second);
            }
          }
        }
      });

      $('.mcneese-tooltip', context).each(function() {
        var item = $(this).tooltip();
        var options = $(this).attr('tooltip_options');

        if (options != "") {
          var options_array = options.split(';');

          for (i=0; i < options_array.length; i++) {
            var parts = options_array[i].split(':');

            if (parts.length == 2) {
              var first = parts[0].replace(/^\s*/ig, '');
              var second = parts[1].replace(/(^\s*|\s*$)/ig, '');
              $(item).tooltip('option', first, second);
            }
          }
        }
      });
    }
  }
})(jQuery);
