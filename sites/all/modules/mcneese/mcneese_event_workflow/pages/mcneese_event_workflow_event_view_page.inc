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
 * @param bool $pdf
 *   (optional) Enable special handling for PDF generation.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mcneese_event_workflow_event_view_0_page($form, &$form_state, $id, $mode = 0, $pdf = FALSE) {
  if (!cf_is_integer($id)) {
    drupal_not_found();
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

  if (!is_bool($pdf)) {
    $pdf = FALSE;
  }

  // if cancelling the event, return only the cancellation confirmation form.
  if (isset($form_state['cancel_was_clicked'])) {
    mcneese_event_workflow_include(5);

    $cancel_form = array();
    $cancel_form_state = array();
    return mcneese_event_workflow_event_cancel_0_form($cancel_form, $cancel_form_state, $id);
  }
  // if uncancelling the event, return only the uncancellation confirmation form.
  elseif (isset($form_state['uncancel_was_clicked'])) {
    mcneese_event_workflow_include(5);

    $cancel_form = array();
    $cancel_form_state = array();
    return mcneese_event_workflow_event_uncancel_0_form($cancel_form, $cancel_form_state, $id);
  }

  $show_display = TRUE;
  if ($mode == 1) {
    // @todo: disable other items once they exist.
  }

  if ($pdf) {
    $can_edit = FALSE;
  }
  else {
    $can_edit = (bool) mcneese_event_workflow_management_page_event_access('edit', $id);
  }

  if ($can_edit) {
    // Include the CTools tools that we need.
    ctools_include('ajax');
    ctools_include('modal');

    // Add CTools' javascript to the page.
    ctools_modal_add_js();

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

  if (!isset($event['information']['type'][0]->value) || !cf_is_integer($event['information']['type'][0]->value)) {
    cf_error::invalid_integer('event[information][type][0]->value');
    $form['message'] = array(
      '#markup' => "Error: Unable to load event (ID = " . check_plain($id) . ") due to missing event type value.",
    );

    return $form;
  }

  $event_type = $event['information']['type'][0]->value;

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

    if ($pdf) {
      unset($form['display']['#type']);
      unset($form['display']['#collapsible']);
      unset($form['display']['#collapsed']);
    }

    $form['display']['groups'] = array(
      '#prefix' => '<div id="event-' . $id . '-field_groups" class="event-field_groups">',
      '#suffix' => '</div>',
    );

    $form['groups'] = array();
    $form['groups'] = array(
      '#tree' => TRUE,
    );

    $fields_count = array('top' => 0, 'group' => array());

    mcneese_event_workflow_include(0);

    $presentation = mcneese_event_workflow_secondary_table_presentation();
    $presentation_tables = mcneese_event_workflow_secondary_table_presentation(1);

    foreach ($event as $field_group => $event_array) {
      if (isset($presentation_tables[$field_group]['event_type']) && is_array($presentation_tables[$field_group]['event_type']) && array_key_exists('hide', $presentation_tables[$field_group]['event_type'])) {
        if ($presentation_tables[$field_group]['event_type']['hide'] === TRUE) {
          if (array_key_exists('show', $presentation_tables[$field_group]['event_type'])) {
            if ($presentation_tables[$field_group]['event_type']['show'] === TRUE) {
              // do nothing
            }
            elseif ($presentation_tables[$field_group]['event_type']['show'] === FALSE) {
              continue;
            }
            elseif (is_array($presentation_tables[$field_group]['event_type']['show']) && !in_array($event_type, $presentation_tables[$field_group]['event_type']['show'])) {
              continue;
            }
          }
        }
        elseif ($presentation_tables[$field_group]['event_type']['hide'] === FALSE) {
          // do nothing
        }
        elseif (is_array($presentation_tables[$field_group]['event_type']['hide']) && in_array($event_type, $presentation_tables[$field_group]['event_type']['hide'])) {
          continue;
        }
      }

      $row = NULL;
      if (isset($presentation_tables[$field_group])) {
        $row = mcneese_event_workflow_event_view_0_page_setup_group_path($form, $id, $field_group, $presentation_tables, $can_edit, $event);
      }

      if (!array_key_exists($field_group, $form['groups'])) {
        $form['groups'][$field_group] = array(
          '#tree' => TRUE,
        );
      }

      foreach ($event_array as $field_name => $values) {
        if (!isset($presentation[$field_group][$field_name])) {
          continue;
        }

        $field_presentation = $presentation[$field_group][$field_name];

        if (!isset($field_presentation['label'])) {
          continue;
        }

        if (empty($values)) {
          continue;
        }

        if (isset($field_presentation['event_type']) && is_array($field_presentation['event_type']) && array_key_exists('hide', $field_presentation['event_type'])) {
          if ($field_presentation['event_type']['hide'] === TRUE) {
            if (array_key_exists('show', $field_presentation['event_type'])) {
              if ($field_presentation['event_type']['show'] === TRUE) {
                // do nothing
              }
              elseif ($field_presentation['event_type']['show'] === FALSE) {
                continue;
              }
              elseif (is_array($field_presentation['event_type']['show']) && !in_array($event_type, $field_presentation['event_type']['show'])) {
                continue;
              }
            }
          }
          elseif ($field_presentation['event_type']['hide'] === FALSE) {
            // do nothing
          }
          elseif (is_array($field_presentation['event_type']['hide']) && in_array($event_type, $field_presentation['event_type']['hide'])) {
            continue;
          }
        }

        if (isset($field_presentation['group'])) {
          $group = $field_presentation['group'];

          $custom_row = NULL;
          if (isset($presentation_tables[$group])) {
            $custom_row = mcneese_event_workflow_event_view_0_page_setup_group_path($form, $id, $group, $presentation_tables, $can_edit, $event);
          }

          if (is_null($custom_row)) {
            $group_path = &$form['display']['groups'];

            $group_fields_count = &$fields_count['top'];
          }
          else {
            $group_path = &$form['display']['groups'][$custom_row];

            if (!isset($fields_count['group'][$custom_row] )) {
              $fields_count['group'][$custom_row] = 0;
            }

            $group_fields_count = &$fields_count['group'][$custom_row];
          }

          unset($custom_row);
        }
        else {
          $group = $field_group;

          if (is_null($row)) {
            $group_path = &$form['display']['groups'];

            $group_fields_count = &$fields_count['top'];
          }
          else {
            $group_path = &$form['display']['groups'][$row];

            if (!isset($fields_count['group'][$row])) {
              $fields_count['group'][$row] = 0;
            }

            $group_fields_count = &$fields_count['group'][$row];
          }
        }

        if (!array_key_exists('#group_fields_count', $group_path)) {
          $group_path['#group_fields_count'] = 0;
        }

        $weight = 0;
        if (isset($field_presentation['weight'])) {
          $weight = $field_presentation['weight'];
        }

        if (!array_key_exists($weight, $group_path[$group])) {
          $group_path[$group][$weight] = array();
        }

        $group_path[$group][$weight][$field_name] = mcneese_event_workflow_build_event_request_item_structure($id, $group, $field_name, $values, $field_presentation);

        // This can be FALSE on error or NULL when the ROW should not be displayed, so remove the field entirely.
        if ($group_path[$group][$weight][$field_name] === FALSE || is_null($group_path[$group][$weight][$field_name])) {
          unset($group_path[$group][$weight][$field_name]);
        }
        else {
          $group_fields_count++;
        }
      }

      // make sure the weights are in the correct order.
      ksort($group_path[$group]);
    }

    // post-process the structure for operations that may only be performed once the structure is completed.
    foreach ($form['display']['groups'] as $display_key => &$display_value) {
      if (!empty($display_value['#attributes'])) {
        $display_value['#prefix'] = '<div';
        $display_value['#suffix'] = '</div>';

        foreach ($display_value['#attributes'] as $attribute => $attribute_values) {
          if (is_array($attribute_values)) {
            $display_value['#prefix'] .= ' ' . $attribute . '="' . implode(' ', $attribute_values) . '"';
          }
          else {
            $display_value['#prefix'] .= ' ' . $attribute . '="' . $attribute_values . '"';
          }
        }

        $display_value['#prefix'] .= '>';

        if (isset($display_value['#extra_prefix'])) {
          $display_value['#prefix'] .= $display_value['#extra_prefix'];
        }
      }

      // create the group prefix and suffix and build the row stripes.
      if (is_numeric($display_key)) {
        foreach (array_keys($display_value) as $group_name) {
          if (preg_match('/^#/', $group_name)) continue;

          if (!empty($display_value[$group_name]['#attributes'])) {
            $display_value[$group_name]['#prefix'] = '<div';
            $display_value[$group_name]['#suffix'] = '</div>';

            foreach ($display_value[$group_name]['#attributes'] as $attribute => $attribute_values) {
              if (is_array($attribute_values)) {
                $display_value[$group_name]['#prefix'] .= ' ' . $attribute . '="' . implode(' ', $attribute_values) . '"';
              }
              else {
                $display_value[$group_name]['#prefix'] .= ' ' . $attribute . '="' . $attribute_values . '"';
              }
            }

            $display_value[$group_name]['#prefix'] .= '>';

            if (isset($display_value[$group_name]['#extra_prefix'])) {
              $display_value[$group_name]['#prefix'] .= $display_value[$group_name]['#extra_prefix'];
            }
          }

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

    if (!$pdf) {
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

      if (mcneese_event_workflow_management_page_event_access('cancel', $id)) {
        $form['display']['submit']['cancel'] = array(
          '#id' => 'event-display-submit-cancel',
          '#type' => 'submit',
          '#value' => t("Cancel"),
          '#attributes' => array(
            'class' => array(
              'no-print',
              'event-display-submit-cancel',
            ),
          ),
        );
      }
      elseif (mcneese_event_workflow_management_page_event_access('uncancel', $id)) {
        $form['display']['submit']['uncancel'] = array(
          '#id' => 'event-display-submit-uncancel',
          '#type' => 'submit',
          '#value' => t("Uncancel"),
          '#attributes' => array(
            'class' => array(
              'no-print',
              'event-display-submit-uncancel',
            ),
          ),
        );
      }

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
 * The appropraite ctools_include() and ctools_modal_add_js() must be called
 * before using this function when $show_edit is TRUE.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param int $id
 *   The event request id.
 * @param string $group
 *   The group name.
 * @param array $presentation_tables
 *   An array of tables defining how the presentation is to be built.
 * @param bool $show_edit
 *   When TRUE, the edit link will be shown in the group header.
 * @param array $event
 *   The event settings array.
 *
 * @return int|null
 *   The row number or NULL when there is no valid row.
 *   FALSE is returned for in
 *
 * @see: mcneese_event_workflow_event_view_0_page()
 * @see: ctools_include()
 * @see: ctools_modal_add_js()
 */
function mcneese_event_workflow_event_view_0_page_setup_group_path(&$form, &$id, &$group, &$presentation_tables, $show_edit, $event) {
  $row = NULL;
  $group_path = &$form['display']['groups'];
  $event_type = $event['information']['type'][0]->value;

  $show_link = TRUE;
  if (isset($presentation_tables[$group]['event_type']['hide_link']) && is_array($presentation_tables[$group]['event_type']['hide_link'])) {
    if (in_array($event_type, $presentation_tables[$group]['event_type']['hide_link'])) {
      $show_link = FALSE;
    }
  }

  if (array_key_exists('group', $presentation_tables[$group])) {
    $row = (int) $presentation_tables[$group]['group'];

    if (!array_key_exists($row, $group_path)) {
      $form['display']['groups'][$row] = array(
        '#prefix' => '',
        '#suffix' => '',
        '#weight' => $row,
        '#attributes' => array(
          'id' => 'section-event-' . $id . '-group-' . $row,
          'class' => array(
            'group',
            'group-' . $row,
          ),
        ),
        '#extra_prefix' => '',
      );
    }

    $group_path = &$form['display']['groups'][$row];
  }

  if (!array_key_exists($group, $group_path)) {
    $group_path[$group] = array(
      '#prefix' => '',
      '#suffix' => '',
      '#weight' => 0,
      '#attributes' => array(
        'id' => 'section-event-' . $id . '-' . $group,
        'class' => array(
          'section-event',
          'section-event-' . $group,
        ),
      ),
      '#extra_prefix' => '',
    );

    if (isset($presentation_tables[$group]['label'])) {
      $group_path[$group]['#extra_prefix'] .= '<div class="section-event-' . $id . '-' . $group . '-label-wrapper section-event-label-wrapper">';

      if ($show_link) {
        #if ($show_edit && function_exists('ctools_modal_text_button')) {
        #  $group_path[$group]['#extra_prefix'] .= ctools_modal_text_button(t("Edit"), 'events/edit-0/ctools/' . $id . '/' . $group, t("Edit " . $presentation_tables[$group]['label']), 'section-event-' . $id . '-' . $group . '-edit section-event-edit no-print');
        #}

        if ($show_edit) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'events/edit-0/' . $id . '/' . $group . '" class="section-event-' . $id . '-' . $group . '-edit section-event-edit no-print" title="'. t("Edit " . $presentation_tables[$group]['label']) . '">Edit</a>';
        }
      }

      $group_path[$group]['#extra_prefix'] .= '<h2 id="section-event-' . $id . '-' . $group . '-label" class="section-event-label section-event-' . $group . '-label">';
      $group_path[$group]['#extra_prefix'] .= $presentation_tables[$group]['label'];
      $group_path[$group]['#extra_prefix'] .= '</h2>';

      $group_path[$group]['#extra_prefix'] .= '</div>';
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
    $clicked_id = $form_state['clicked_button']['#id'];
  }

  if ($clicked_id == 'event-display-submit-print') {
    form_set_error('', "The print button uses javascript to help force a print. You can always use your browsers print button (or equivalent) directly.");
  }
  elseif ($clicked_id == 'event-display-submit-download') {
    // nothing to be done here.
  }
  elseif ($clicked_id == 'event-display-submit-cancel') {
    $form_state['cancel_was_clicked'] = TRUE;
    $form_state['rebuild'] = TRUE;
  }
  elseif ($clicked_id == 'event-display-submit-uncancel') {
    $form_state['uncancel_was_clicked'] = TRUE;
    $form_state['rebuild'] = TRUE;
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
    $clicked_id = $form_state['clicked_button']['#id'];
  }

  if ($clicked_id == 'event-cancel-submit-yes') {
    $deleted = mcneese_event_workflow_include(5);

    mcneese_event_workflow_event_cancel_0_form_submit($form, $form_state);
  }
  elseif ($clicked_id == 'event-uncancel-submit-yes') {
    $deleted = mcneese_event_workflow_include(5);

    mcneese_event_workflow_event_uncancel_0_form_submit($form, $form_state);
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
    $clicked_id = $form_state['clicked_button']['#id'];
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
    cf_error::invalid_integer('id');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $event = event_workflow_load_event_by_id($id);

  if (empty($event)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  // @todo: check to see if the user has access to the specified event.
  //        this is a check agains the status field.

  if (!empty($event['information']['title'][0]->value)) {
    $page_title = "Event Request: " . $event['information']['title'][0]->value;
    drupal_set_title($page_title);
  }

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
  $page['html']['content'] = array(mcneese_event_workflow_event_view_0_page($form, $form_state, $id, $mode, TRUE));
  $renderred = drupal_render($page);
  print(drupal_render_page($renderred));
  $html = ob_get_contents();
  ob_end_clean();

  mcneese_event_workflow_event_pdf_0_page_using_wkhtmltopdf($id, $mode, $event, $html);
}

/**
 * Renders a PDF of a given event request using the wkhtmltopdf library.
 *
 * @param int $id
 *   The unique identifier for an event request.
 * @param int|null $mode
 *   Specifies the mode that alters how the form is rendered.
 *   1: Render only the display part.
 */
function mcneese_event_workflow_event_pdf_0_page_using_wkhtmltopdf($id, $mode, $event, $html) {
  if (!cf_is_integer($id)) {
    cf_error::invalid_integer('id');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!cf_is_integer($mode)) {
    cf_error::invalid_integer('mode');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_array($event)) {
    cf_error::invalid_array('event');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_string($html)) {
    cf_error::invalid_string('html');
    drupal_not_found();
    drupal_exit();
    return array();
  }

  $library = libraries_load('phpwkhtmltopdf');
  if ($library === FALSE || empty($library['loaded'])) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  // Reconstruct the HTML output.
  $cf_dom = new cf_dom(TRUE, TRUE, TRUE, $html);

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
          // skip past css uneeded css
          if ($media == 'all') {
            $parts = explode('/', $m);
            if (!isset($parts[4])) {
              continue;
            }
            if ($parts[3] == 'system') {
            }

            if (isset($parts[5])) {
              if ($parts[5] == 'mcneese') {
              }
              elseif ($parts[5] == 'mcneese_event_workflow') {
              }
              else {
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
      $head_markup .= $cf_dom->get_dom()->saveHTML($tag) . "\n";
    }
  }

  if (!empty($css_markup)) {
    $head_markup .= '<style>' . $css_markup . '</style>';
  }

  // link tags
  foreach ($link_tags as $tag) {
    $head_markup .= $cf_dom->get_dom()->saveHTML($tag) . "\n";
  }

  // title tags
  $page_title = "Event Request: " . $event['information']['title'][0]->value;
  $head_markup .= '<title>Event Request #' . $id . ': ' . $event['information']['title'][0]->value . '</title>';

  $body_markup .= $cf_dom->get_dom()->saveHTML($main_tag) . "\n";

  $body_class = 'mcneese mcneese-pdf';
  $cf = &drupal_static('cf_theme_get_variables', array());
  if (isset($cf['markup_css']['body']['class'])) {
    $body_class .= $cf['markup_css']['body']['class'];
  }

  $html = '';
  $html = '<!DOCTYPE html>' . "\n";
  $html .= '<html lang="en" dir="ltr">' . "\n";

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


  $wkpdf = new WkHtmlToPdf();

  // setup the default options
  $wkpdf->setOptions(array(
    'ignoreWarnings' => TRUE, // when not ignored, the PDF does not get generated.
  ));

  $wkpdf->addPage($html);

  $fixed_title = preg_replace('/\W/i', '_', $event['information']['title'][0]->value);
  $filename = sprintf("%'010s", $id) . '-event_request-' . $fixed_title . '.pdf';

  // save the file, locally.
  #$wkpdf->saveAs('/tmp/' . $filename);

  // send to the browser
  #$result = $wkpdf->send($filename, TRUE);

  // send as a download
  $result = $wkpdf->send($filename, FALSE);

  if (!$result) {
    watchdog('mew', "Error occured while generating the PDF of event @id: @error", array('@id' => $id, '@error' => $wkpdf->getError()), WATCHDOG_ERROR);
  }

  exit();
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
