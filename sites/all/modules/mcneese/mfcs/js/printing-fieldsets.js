(function ($) {
  Drupal.behaviors.mfcs_alter_fieldsets_when_printing = {
    attach:function(context) {

      var mfcs_alter_fieldsets_before_printing = function() {
        var fieldsets_print_as_div = document.getElementsByClassName("fieldset_print_as_div");

        if (fieldsets_print_as_div != null) {
          for (var i = 0; i < fieldsets_print_as_div.length; i++) {
            var replacement = document.createElement("div");

            for (var j = 0; j < fieldsets_print_as_div[i].attributes.length; j++) {
              replacement.setAttribute(fieldsets_print_as_div[i].attributes[j].name, fieldsets_print_as_div[i].attributes[j].value);
            }

            while (fieldsets_print_as_div[i].firstChild) {
              replacement.appendChild(fieldsets_print_as_div[i].firstChild);
            }

            fieldsets_print_as_div[i].parentNode.replaceChild(replacement, fieldsets_print_as_div[i]);
          }
        }
      }

      var mfcs_alter_fieldsets_after_printing = function() {
        var fieldsets_print_as_div = document.getElementsByClassName("fieldset_print_as_div");

        if (fieldsets_print_as_div != null) {

          for (var i = 0; i < fieldsets_print_as_div.length; i++) {
            var replacement = document.createElement("fieldset");

            for (var j = 0; j < fieldsets_print_as_div[i].attributes.length; j++) {
              replacement.setAttribute(fieldsets_print_as_div[i].attributes[j].name, fieldsets_print_as_div[i].attributes[j].value);
            }

            while (fieldsets_print_as_div[i].firstChild) {
              replacement.appendChild(fieldsets_print_as_div[i].firstChild);
            }

            fieldsets_print_as_div[i].parentNode.replaceChild(replacement, fieldsets_print_as_div[i]);
          }
        }
      }

      if (window.matchMedia) {
        var print_matches = window.matchMedia('print');

        print_matches.addListener(function(match) {
          if (match.matches) {
            mfcs_alter_fieldsets_before_printing();
          }
          else {
            mfcs_alter_fieldsets_after_printing();
          }
        });
      }

      window.onbeforeprint = mfcs_alter_fieldsets_before_printing;
      window.onafterprint = mfcs_alter_fieldsets_after_printing;
    }
  }
})(jQuery);