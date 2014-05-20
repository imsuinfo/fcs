<?php

/**
 * @file
 * Defines McNeese event workflow event view page functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Provides the event workflow event view page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $id
 *   The unique identifier for an event request.
 * @param int $mode
 *   Specifies the mode that alters how the form is rendered.
 *   0: No restrictions.
 *   1: Render only the display part.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mcneese_event_workflow_event_view_0_page($form, &$form_state, $id, $mode = 0) {
  if (!cf_is_integer($id)) {
    drupal_access_denied();
    drupal_exit();
    return array();
  }

  $event = event_workflow_load_event_by_id($id);

  if (empty($event)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_numeric($mode) || $mode < 0) {
    $mode = 0;
  }
  else {
    $mode = (int) $mode;
  }

  $show_display = TRUE;
  if ($mode == 1) {
    // @todo: disable other items once they exist.
  }


  $page_title = "Event Request";
  if (!empty($event['information']['title'][0]->value)) {
    $page_title = "Event Request: " . $event['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  // provide margin information in the printer-friendly version of the page.
  $print_css = '@page { ' . "\n";
  $print_css .= '  size: A4 portrait;' . "\n";
  $print_css .= '  margin: 30px 30px 30px 30px;' . "\n";

  // note: @top-left, and @top-right are currently not supported by most major browsers.
  $print_css .= '  @top-left { content: "' . $page_title . '"; }' . "\n";
  $print_css .= '  @top-right { content: "Page " counter(page); }' . "\n";
  $print_css .= '}' . "\n";
  drupal_add_css($print_css, array('type' => 'inline', 'group' => CSS_THEME, 'weight' => 10, 'media' => 'print', 'preprocess' => FALSE));


  $form = array();

  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );

  $form['event'] = array(
    '#type' => 'value',
    '#value' => &$event,
  );

  if ($show_display) {
    $form['display'] = array(
      '#id' => 'mcneese-event-workflow-event-' . $id . '-section-display',
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Request Details",
      '#attributes' => array(
        'class' => array(
          'event-section-display',
        ),
      ),
    );

    $form['display']['groups'] = array(
      '#prefix' => '<div id="event-' . $id . '-field_groups" class="event-field_groups">',
      '#suffix' => '</div>',
    );

    $form['groups'] = array();
    $form['groups'] = array(
      '#tree' => TRUE,
    );

    $presentation = mcneese_event_workflow_secondary_table_presentation();
    $presentation_tables = mcneese_event_workflow_secondary_table_presentation(1);

    foreach ($event as $field_group => &$event_array) {
      $row = NULL;
      if (isset($presentation_tables[$field_group])) {
        $row = mcneese_event_workflow_event_view_0_page_setup_group_path($form, $id, $field_group, $presentation_tables);
      }

      if (!array_key_exists($field_group, $form['groups'])) {
        $form['groups'][$field_group] = array(
          '#tree' => TRUE,
        );
      }

      foreach ($event_array as $field_name => &$field_array) {
        $field_presentation = &$presentation[$field_group][$field_name];

        if (!isset($field_presentation['label'])) {
          continue;
        }

        if (isset($field_presentation['group'])) {
          $group = $field_presentation['group'];

          $custom_row = NULL;
          if (isset($presentation_tables[$group])) {
            $custom_row = mcneese_event_workflow_event_view_0_page_setup_group_path($form, $id, $group, $presentation_tables);
          }

          if (is_null($custom_row)) {
            $group_path = &$form['display']['groups'];
          }
          else {
            $group_path = &$form['display']['groups'][$custom_row];
          }

          unset($custom_row);
        }
        else {
          $group = $field_group;

          if (is_null($row)) {
            $group_path = &$form['display']['groups'];
          }
          else {
            $group_path = &$form['display']['groups'][$row];
          }
        }

        $weight = 0;
        if (isset($field_presentation['weight'])) {
          $weight = $field_presentation['weight'];
        }

        if (!array_key_exists($weight, $group_path[$group])) {
          $group_path[$group][$weight] = array();
        }

        $group_path[$group][$weight][$field_name] = mcneese_event_workflow_build_event_request_item_structure($id, $group, $field_name, $field_array, $field_presentation);

        // This can be FALSE on error, so remove the field entirely.
        if ($group_path[$group][$weight][$field_name] === FALSE) {
          unset($group_path[$group][$weight][$field_name]);
        }

        foreach ($field_array as $delta => &$value) {
          $form['groups'][$field_group][$delta] = array(
            '#type' => 'value',
            '#value' => $value,
          );
        }
      }

      // make sure the weights are in the correct order.
      ksort($group_path[$group]);
    }

    // post-process the structure for operations that may only be performed once the structure is completed.
    foreach ($form['display']['groups'] as $display_key => &$display_value) {

      // build the row stripes.
      if (is_numeric($display_key)) {
        foreach (array_keys($display_value) as $group_name) {
          if (preg_match('/^#/', $group_name)) continue;

          $by_weight = array();
          foreach ($display_value[$group_name] as $weight => &$group_values) {
            if (!is_numeric($weight)) continue;

            foreach ($group_values as $field_name => &$field_values) {
              if (preg_match('/^#/', $field_name)) continue;

              if (!array_key_exists($weight, $by_weight)) {
                $by_weight[$weight] = array();
                $field_weight = 0;
              }
              else {
                $field_weight = count($by_weight[$weight]);
              }

              // prevent alphabetical positioning from changing the order and therefore breaking the stripe pattern.
              $field_values['item']['#weight'] = $field_weight;

              $by_weight[$weight][$field_weight] = &$field_values['item'];
            }
          }

          // make sure the weights are in the correct order.
          ksort($by_weight);

          $count = 0;
          foreach ($by_weight as $weight => &$item_row) {
            foreach ($item_row as $item_weight => &$item) {
              if ($count % 2 == 0) {
                $item['#attributes']['class'][] = 'even';
              }
              else {
                $item['#attributes']['class'][] = 'odd';
              }

              $count++;
            }
          }
        }
      }
    }

    $form['display']['submit'] = array(
      '#id' => 'event-display-submit',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'no-print',
          'event-display-submit',
        ),
      ),
      '#weight' => 1000,
    );

    $form['display']['submit']['print'] = array(
      '#id' => 'event-display-submit-print',
      '#type' => 'submit',
      '#value' => t("Print"),
      '#attributes' => array(
        'class' => array(
          'no-print',
          'event-display-submit-print',
        ),
        'onclick' => 'window.print(); return false;',
      ),
    );

    if (function_exists('pdf_using_mpdf_library_exist') && pdf_using_mpdf_library_exist()) {
      $form['display']['submit']['download'] = array(
        '#id' => 'event-display-submit-download',
        '#type' => 'submit',
        '#value' => t("Download"),
        '#attributes' => array(
          'class' => array(
            'no-print',
            'event-display-submit-download',
          ),
        ),
        '#submit' => array(
          'mcneese_event_workflow_event_view_0_page_submit_pdf',
        ),
      );
    }
  }

  return $form;
}

/**
 * Create the group path presentation structure.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param int $id
 *   The event request id.
 * @param string $group
 *   The group name.
 * @param array $$presentation_tables
 *   An array of tables defining how the presentation is to be built.
 *
 * @return int|null
 *   The row number or NULL when there is no valid row.
 *   FALSE is returned for in
 *
 * @see: mcneese_event_workflow_event_view_0_page()
 */
function mcneese_event_workflow_event_view_0_page_setup_group_path(&$form, &$id, &$group, &$presentation_tables) {
  $row = NULL;
  $group_path = &$form['display']['groups'];

  if (array_key_exists('group', $presentation_tables[$group])) {
    $row = (int) $presentation_tables[$group]['group'];

    if (!array_key_exists($row, $group_path)) {
      $form['display']['groups'][$row] = array(
        '#prefix' => '<div id="section-event-' . $id . '-group-' . $row . '" class="group group-' . $row . '">',
        '#suffix' => '</div>',
        '#weight' => $row,
      );
    }

    $group_path = &$form['display']['groups'][$row];
  }

  if (!array_key_exists($group, $group_path)) {
    $group_path[$group] = array(
      '#prefix' => '<div id="section-event-' . $id . '-' . $group . '" class="section-event section-event-' . $group . '">',
      '#suffix' => '</div>',
      '#weight' => 0,
    );

    if (isset($presentation_tables[$group]['label'])) {
      $group_path[$group]['#prefix'] .= '<h2 id="section-event-' . $id . '-' . $group . '-label" class="section-event-label section-event-' . $group . '-label">';
      $group_path[$group]['#prefix'] .= $presentation_tables[$group]['label'];
      $group_path[$group]['#prefix'] .= '</h2>';
    }

    if (isset($presentation_tables[$group]['weight'])) {
      $group_path[$group]['#weight'] = $presentation_tables[$group]['weight'];
    }
  }

  return $row;
}

/**
 * Validation for mcneese_event_workflow_event_view_0_page().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_view_0_page()
 */
function mcneese_event_workflow_event_view_0_page_validate($form, &$form_state) {
  $no_problems = TRUE;

  $clicked_id = '';
  if (isset($form_state['clicked_button']['#id'])) {
    $clicked_id = &$form_state['clicked_button']['#id'];
  }

  if ($clicked_id == 'event-display-submit-print') {
    form_set_error('', "The print button uses javascript to help force a print. You can always use your browsers print button (or equivalent) directly.");
  }
  elseif ($clicked_id == 'event-display-submit-download') {
    // nothing to be done here.
  }
}

/**
 * Submit process for mcneese_event_workflow_event_view_0_page().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_view_0_page()
 */
function mcneese_event_workflow_event_view_0_page_submit($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['clicked_button']['#id'])) {
    $clicked_id = &$form_state['clicked_button']['#id'];
  }
}

/**
 * Custom submit process for mcneese_event_workflow_event_view_0_page().
 *
 * Used for generating a print PDF.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_view_0_page()
 */
function mcneese_event_workflow_event_view_0_page_submit_pdf($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['clicked_button']['#id'])) {
    $clicked_id = &$form_state['clicked_button']['#id'];
  }

  $event_id = (int) $form['id']['#value'];

  if ($clicked_id != 'event-display-submit-download' || empty($event_id)) {
    if (empty($event_id)) {
      $form_state['redirect'] = 'events';
    }
    else {
      $form_state['redirect'] = 'events/view-0/' . $event_id;
    }

    return;
  }

  global $base_path;

  // @todo: add support for multiple modes.
  $mode = 1;
  $form_state['redirect'] = '/events/pdf-0/' . $event_id . '/' . $mode;
}

/**
 * Renders a PDF of a given event request.
 *
 * @param int $id
 *   The unique identifier for an event request.
 * @param int|null $mode
 *   Specifies the mode that alters how the form is rendered.
 *   1: Render only the display part.
 */
function mcneese_event_workflow_event_pdf_0_page($id, $mode) {
  $page_title = "Event Request";
  drupal_set_title($page_title);

  if (!cf_is_integer($id)) {
    drupal_access_denied();
    drupal_exit();
    return array();
  }

  if (!cf_is_integer($mode)) {
    drupal_access_denied();
    drupal_exit();
    return array();
  }

  if (!function_exists('pdf_using_mpdf_library_exist')) {
    drupal_access_denied();
    drupal_exit();
    return array();
  }

  $event = event_workflow_load_event_by_id($id);

  if (empty($event)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!empty($event['information']['title'][0]->value)) {
    $page_title = "Event Request: " . $event['information']['title'][0]->value;
    drupal_set_title($page_title);
  }

  $author = '';
  if (isset($event['mer']['user_id'][0]->value)) {
    $user = user_load($event['mer']['user_id'][0]->value);

    if (isset($user->field_user_first_name['und'][0]['safe_value'])) {
      $author = $user->field_user_first_name['und'][0]['safe_value'];

      if (isset($user->field_user_last_name['und'][0]['safe_value'])) {
        $author .= ' ' . $user->field_user_last_name['und'][0]['safe_value'];
      }
    }
    else {
      $author = $user->name;
    }
  }

  global $base_root;
  global $base_path;

  $form = array();
  $form_state = array();

  $page = array();
  $page['html'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => 'mcneese-event-workflow-event-view-0-page',
      'class' => array(
        'mcneese-event-workflow-event-pdf-0',
      ),
    ),
  );

  // turn off devel shutdown functionality.
  $GLOBALS['devel_shutdown'] = FALSE;

  ob_start();
  $page['html']['content'] = array(mcneese_event_workflow_event_view_0_page($form, $form_state, $id, $mode));
  $renderred = drupal_render($page);
  print(drupal_render_page($renderred));
  $html = ob_get_contents();
  ob_end_clean();

  // The html output is not in the proper format and the PDF generator runs into an infinite loop (until out of memory).
  // The drupal @import styles are also not supported by the PDF generator.
  // Therefore, the page must be manually reconstructed with a minimal set of information.
  $cf_dom = new cf_dom(TRUE, TRUE, TRUE, $html);

  // remove all tags with no-print
  // this is a workaround for mPDF.
  $all_tags = $cf_dom->get_dom()->getElementsByTagName('*');
  $remove_tags = array();
  foreach ($all_tags as $tag) {
    if (!$tag->hasAttribute('class')) continue;

    $class = $tag->getAttribute('class');

    if (preg_match('/no-print/i', $class)) {
      $remove_tags[] = $tag;
    }
  }

  while (!empty($remove_tags)) {
    $remove_tag = array_pop($remove_tags);
    $parent_tag = $remove_tag->parentNode;
    $parent_tag->removeChild($remove_tag);
    unset($remove_tag);
  }

  $meta_tags = $cf_dom->get_body()->getElementsByTagName('meta');
  $style_tags = $cf_dom->get_body()->getElementsByTagName('style');
  $link_tags = $cf_dom->get_body()->getElementsByTagName('link');
  $title_tags = $cf_dom->get_body()->getElementsByTagName('title');
  $main_tag = $cf_dom->get_dom()->getElementById('mcneese-content-main');

  $head_markup = '<meta http-equiv="Content-Type" content="text/html; charset=utf-8">';
  $body_markup = '';
  $css_markup = '';

  // meta tags
  foreach ($meta_tags as $tag) {
    $head_markup .= $cf_dom->get_dom()->saveHTML($tag) . "\n";
  }

  // style tags
  $head_markup = '';
  foreach ($style_tags as $tag) {
    // drupal generates css styles using @import, remove all of these.
    $matched = array();
    $matches = preg_match_all('/@import url\("([^"]*)/i', $tag->nodeValue, $matched);

    $media = NULL;
    if ($tag->hasAttribute('media')) {
      $media = $tag->getAttribute('media');
    }
    if (empty($media)) {
      $media = 'all';
    }

    if ($matches > 0) {
      if ($media == 'all' || $media == 'print,embossed,tv' || $media == 'print') {
        $match = array_pop($matched);

        foreach ($match as $m) {
          // skip past css includes that are not needed and may cause problems and will increase renderring time
          if ($media == 'all') {
            $parts = explode('/', $m);
            if (isset($parts[5])) {
              if ($parts[5] == 'jquery_update') {
                continue;
              }
              if (isset($parts[5]) && $parts[5] == 'ldap') {
                continue;
              }
            }
          }

          $fixed_path = preg_replace('/\?.*$/i', '', $m);
          $parts = explode('/', $fixed_path);
          array_shift($parts);
          array_shift($parts);
          $fixed_path = implode('/', $parts);
          $content = drupal_load_stylesheet(DRUPAL_ROOT . '/' . $fixed_path);


          if (!empty($content)) {
            // mPDF does not properly support the > selector.
            $css_markup .= preg_replace('@ > @i', ' ', $content);
          }

          unset($content);
          unset($parts);
          unset($fixed_path);
        }
      }
    }
    else {
      # the custom css crashes the PDF generator! (infinite loop, out of memory..)
      #$head_markup .= $cf_dom->get_dom()->saveHTML($tag) . "\n";
    }
  }

  // load the pdf workaround css.
  // The mPDF library does not properly support CSS.
  // The workaround includes alternate forms of CSS that are, while problematic, are sufficient for printing the PDF.
  // This require the mcneese_event_workflow theme.
  $mcneese_event_workflow_theme_path = drupal_get_path('theme', 'mcneese_event_workflow');
  $workaround_css = DRUPAL_ROOT . '/' . $mcneese_event_workflow_theme_path . '/css/workaround/workaround-pdf-event_form.css';
  $content = drupal_load_stylesheet($workaround_css);
  if (!empty($content)) {
    $css_markup .= $content;
  }

  // link tags
  foreach ($link_tags as $tag) {
    $head_markup .= $cf_dom->get_dom()->saveHTML($tag) . "\n";
  }

  // title tags
  foreach ($title_tags as $tag) {
    $head_markup .= $cf_dom->get_dom()->saveHTML($tag) . "\n";
  }


  $body_markup .= $cf_dom->get_dom()->saveHTML($main_tag) . "\n";

  $body_class = 'mcneese mcneese-pdf';
  $cf = &drupal_static('cf_theme_get_variables', array());
  if (isset($cf['markup_css']['body']['class'])) {
    $body_class .= $cf['markup_css']['body']['class'];
  }

  # for debugging styles
  #$head_markup .= '<style>' . $css_markup . '</style>';

  $html = '';
  $html = '<!DOCTYPE html>' . "\n";
  $html .= '<html lang="en" dir="ltr" class="js">' . "\n";

  $html .= '<head>' . $head_markup . '</head>';
  $html .= '<body class="' . $body_class . '">';
  $html .= '  <div id="mcneese-page" class="mcneese-page">';
  $html .= '    <div id="mcneese-page-content" class="mcneese-content full" role="main">';
  $html .= '    <header class="page-title html_tag-header">';
  $html .= '      <hgroup class="html_tag-hgroup">';
  $html .= '        <h1 class="page-title html_tag-heading" role="main">' . $page_title . '</h1>';
  $html .= '      </hgroup>';
  $html .= '    </header>';
  $html .= '    ' . $body_markup;
  $html .= '    </div>';
  $html .= '  </div>';
  $html .= '</body>';
  $html .= '</html>' . "\n";


  #print($html);
  #exit();

  // Creating Instance of mPDF Class Library.
  $mpdf = new mPDF('utf-8', 'A4');

  // enable PDF/A1-b compliance
  $mpdf->PDFA = TRUE;
  $mpdf->PDFAauto = TRUE;

  // Enabling header option if available.
  #$mpdf->SetHTMLHeader($header);

  // Enabling Footer option if available.
  #$mpdf->SetHTMLFooter($footer);

  // Setting Title to PDF.
  $mpdf->SetTitle($page_title);

  // Setting Author to PDF.
  $mpdf->SetAuthor($author);

  // Setting Subject to PDF.
  $mpdf->SetSubject("Facilities Use and Special Event Request Form");

  // Setting creator to PDF.
  $mpdf->SetCreator("McNeese State University");

  // Setting Password to PDF.
  #$mpdf->SetProtection(array('print', 'copy'), $password, $password);

  // show a progress bar
  #$mpdf->StartProgressBarOutput(2);

  // write the CSS stylesheets
  $mpdf->WriteHTML($css_markup, 1);

  // Writing html content for pdf buffer.
  $mpdf->WriteHTML($html, 2);

  // Generating PDF File.
  // Open in same browser.
  #$mpdf->Output($filename . '.pdf', 'I');
  // Dialog box for Download as PDF.
  $mpdf->Output($page_title . '.pdf', 'D');

  drupal_exit();
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
