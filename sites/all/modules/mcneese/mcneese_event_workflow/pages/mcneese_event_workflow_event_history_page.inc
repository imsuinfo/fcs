<?php

/**
 * @file
 * Defines McNeese event workflow event history page functions.
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
 * @param int $event_id
 *   The unique identifier for an event request.
 * @param int $page_number
 *   (optional) Page number to view results.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mcneese_event_workflow_event_history_0_form($form, &$form_state, $event_id, $page_number = 0, $group = NULL, $field = NULL) {
  $event = event_workflow_load_event_by_id($event_id);

  if (empty($event)) {
    watchdog('mew', "Unable to find event with id of @event_id.", array('@event_id' => $event_id), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
    return array();
  }

  global $base_path;

  $user = cf_current_user();

  $manager = user_access('mcneese event workflow manage');
  $reviewer = user_access('mcneese event workflow review');
  $requester = user_access('mcneese event workflow request');

  $form = array();
  $form['navigator_top'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'event_request-history-page-navigator',
        'event_request-history-page-navigator-top',
      ),
    ),
  );

  $form['history'] = array(
    '#tree' => TRUE,
  );

  $form['navigator_bottom'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'event_request-history-page-navigator',
        'event_request-history-page-navigator-bottom',
      ),
    ),
  );

  $rows_per_page = 10;
  $start_range = $page_number * $rows_per_page;
  $rows_count = 0;

  mcneese_event_workflow_include(0);

  $structure = mcneese_event_workflow_table_structure();
  $presentation = mcneese_event_workflow_table_presentation();
  $presentation_tables = mcneese_event_workflow_table_presentation(1);

  try {
    $query = db_select('mew_event_revisions', 'merv');

    $query->fields('merv');
    $query->condition('merv.event_id', $event_id);
    $query->orderby('merv.date', 'DESC');
    $query->range($start_range, $rows_per_page + 1);

    $revisions = $query->execute()->fetchall();

    $total_results = count($revisions);

    if (is_array($revisions)) {
      foreach ($revisions as $revision) {
        $rows_count++;

        // $rows_per_page + 1 is loaded so that it can be detected whether or not a 'next' link needs to be provided.
        if ($rows_count > $rows_per_page) {
          break;
        }

        $history = event_workflow_load_event_by_timestamp($event_id, $revision->date);
        $date = date("Y/m/d h:ia", $revision->date);

        $fieldset_id = 'event-' . $event_id . '-history_items-' . $revision->date;
        $fieldset_name = 'history_items-' . $revision->date;
        $form['history'][$fieldset_name] = array(
          '#type' => 'fieldset',
          '#id' => $fieldset_id,
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
          '#tree' => TRUE,
          '#title' => '<h3 class="history_items-title">' . "Changes on " . $date . '</h3>',
          '#attributes' => array(
            'class' => array(
              'history_items',
            ),
          ),
        );

        if ($revision->revision == 0) {
          $processed_values = array();
          $user_id = NULL;
          if (!empty($history['mer']['user_id'])) {
            $user_id = reset($history['mer']['user_id']);

            if (is_object($user_id) && !empty($user_id->value) && cf_is_integer($user_id->value)) {
              $user_id = $user_id->value;
            }
          }

          if (!is_null($user_id)) {
            $updater_name = mcneese_event_workflow_event_history_0_form_build_updater_name($user_id, $processed_values);

            $values_name = 'history_item-created-label';
            $form['history'][$fieldset_name][$values_name] = array(
              '#id' => 'event-' . $event_id . '-history_item-created-label',
              '#type' => 'container',
              '#tree' => TRUE,
              '#attributes' => array(
                'class' => array(
                  'history_item-label',
                ),
              ),
            );

            $form['history'][$fieldset_name][$values_name] = array(
              '#id' => $fieldset_id . '-changes_by-' . $user_id,
              '#type' => 'container',
              '#tree' => TRUE,
              '#attributes' => array(
                'class' => array(
                  'history_item-values',
                ),
              ),
              '#prefix' => '<div class="history_item-label"><strong class="updater_name">' . $updater_name . '</strong> <span class="updater_id">[id: ' . $user_id . ']</span> has created the event.</div> ',
            );
          }
        }
        else {
          $processed_values = array();
          foreach ($history as $group_name => $group_values) {
            if ($group_name == 'current') {
              continue;
            }

            if ($group_name == 'mer') {
              continue;
            }

            if (empty($group_values)) {
              continue;
            }

            foreach ($group_values as $field_name => $field_values) {
              if (empty($field_values)) {
                continue;
              }

              $manual_processing = FALSE;
              if (isset($structure[$group_name][$field_name]) && array_key_exists('manual_processing', $structure[$group_name][$field_name])) {
                $manual_processing = $structure[$group_name][$field_name]['manual_processing'];
              }

              if ($manual_processing) {
                foreach ($field_values as $key => $value) {
                  if (!is_object($value)) {
                    continue;
                  }

                  $updater_name = mcneese_event_workflow_event_history_0_form_build_updater_name($value->user_id, $processed_values);

                  if (!isset($processed_values[$value->user_id]['fields'][$group_name])) {
                    $processed_values[$value->user_id]['fields'][$group_name] = array(
                      $field_name => array(),
                    );
                  }

                  if (!isset($processed_values[$value->user_id]['fields'][$group_name][$field_name])) {
                    $processed_values[$value->user_id]['fields'][$group_name][$field_name] = array();
                  }

                  if ($group_name == 'review') {
                    if ($field_name == 'review') {
                      $step_options = event_workflow_get_event_steps_list_options();
                      $decision_options = event_workflow_get_reviewer_decision_list_options();


                      if ($value->decision == MEW_REVIEW_DECISION_APPROVE || $value->decision == MEW_REVIEW_DECISION_DENY) {
                        $custom_message = 'During the <strong class="review-step">' . $step_options[$value->step] . ' Step</strong> ';
                        $custom_message .= 'made a decision of <strong class="review-decision">' . $decision_options[$value->decision] . '</strong>. ';
                      }
                      elseif ($value->decision == MEW_REVIEW_DECISION_COMMENT) {
                        $custom_message = 'During the <strong class="review-step">' . $step_options[$value->step] . ' Step</strong> ';
                        $custom_message .= 'added a <strong class="review-decision">' . $decision_options[$value->decision] . '</strong>. ';
                      }
                      elseif ($value->decision == MEW_REVIEW_DECISION_WAVE) {
                        $custom_message = 'During the <strong class="review-step">' . $step_options[$value->step] . ' Step</strong> ';
                        $custom_message .= 'decided to <strong class="review-decision">' . $decision_options[$value->decision] . '</strong> the usage fees. ';
                      }
                      elseif ($value->decision == MEW_REVIEW_DECISION_USAGE_FEE) {
                        $custom_message = 'During the <strong class="review-step">' . $step_options[$value->step] . ' Step</strong> ';
                        $custom_message .= 'assigned a <strong class="review-decision">' . $decision_options[$value->decision] . '</strong>. ';
                      }

                      $processed_values[$value->user_id]['fields'][$group_name][$field_name][$key] = array(
                        '#markup' => '<div class="item-value">' . $custom_message . '</div>',
                      );
                    }
                  }
                  elseif ($group_name == 'use_fees') {
                    if (isset($presentation[$group_name][$field_name])) {
                      // always display the boolean when displayed in the history log.
                      if (!empty($presentation[$group_name][$field_name]['type'])) {
                        foreach ($presentation[$group_name][$field_name]['type'] as $type_key => $type_value) {
                          if ($type_value == 'boolean') {
                            if (!empty($presentation[$group_name][$field_name]['display'][$type_key])) {
                              $display_value = &$presentation[$group_name][$field_name]['display'][$type_key];

                              if ($display_value == 'when_true' || $display_value == 'when_false') {
                                $display_value = 'raw';
                              }
                            }
                          }
                        }
                      }

                      $options = array(
                        'no_prefix' => TRUE,
                        'no_suffix' => TRUE,
                        'print_delta_for_multiple' => TRUE,
                      );

                      $value_array = mcneese_event_workflow_build_event_request_value_structure($event_id, $group_name, $field_name, $key, $value, $presentation[$group_name][$field_name], $history, $options);

                      if (is_array($value_array) && !isset($value_array['no_values'])) {
                        $processed_values[$value->user_id]['fields'][$group_name][$field_name][$key] = $value_array;
                      }
                    }
                  }

                  if (empty($processed_values[$value->user_id]['fields'][$group_name][$field_name])) {
                    unset($processed_values[$value->user_id]['fields'][$group_name][$field_name]);
                  }

                  if (empty($processed_values[$value->user_id]['fields'][$group_name])) {
                    unset($processed_values[$value->user_id]['fields'][$group_name]);
                  }
                }
              }
              else {
                foreach ($field_values as $key => $value) {
                  if (!is_object($value)) {
                    continue;
                  }

                  $updater_name = mcneese_event_workflow_event_history_0_form_build_updater_name($value->user_id, $processed_values);

                  if (!isset($processed_values[$value->user_id]['fields'][$group_name])) {
                    $processed_values[$value->user_id]['fields'][$group_name] = array(
                      $field_name => array(),
                    );
                  }

                  if (!isset($processed_values[$value->user_id]['fields'][$group_name][$field_name])) {
                    $processed_values[$value->user_id]['fields'][$group_name][$field_name] = array();
                  }

                  if (isset($presentation[$group_name][$field_name])) {
                    // always display the boolean when displayed in the history log.
                    if (!empty($presentation[$group_name][$field_name]['type'])) {
                      foreach ($presentation[$group_name][$field_name]['type'] as $type_key => $type_value) {
                        if ($type_value == 'boolean') {
                          if (!empty($presentation[$group_name][$field_name]['display'][$type_key])) {
                            $display_value = &$presentation[$group_name][$field_name]['display'][$type_key];

                            if ($display_value == 'when_true' || $display_value == 'when_false') {
                              $display_value = 'raw';
                            }
                          }
                        }
                      }
                    }

                    $options = array(
                      'no_prefix' => TRUE,
                      'no_suffix' => TRUE,
                      'print_delta_for_multiple' => TRUE,
                    );

                    $value_array = mcneese_event_workflow_build_event_request_value_structure($event_id, $group_name, $field_name, $key, $value, $presentation[$group_name][$field_name], $history, $options);

                    if (is_array($value_array) && !isset($value_array['no_values'])) {
                      $processed_values[$value->user_id]['fields'][$group_name][$field_name][$key] = $value_array;
                    }
                  }
                }
              }
            }
          }

          foreach ($processed_values as $user_id => $processed_value) {
            $values_name = 'history_items-' . $revision->date . '-changes_by-' . $user_id;
            $form['history'][$fieldset_name][$values_name] = array(
              '#id' => $fieldset_id . '-changes_by-' . $user_id,
              '#type' => 'container',
              '#tree' => TRUE,
              '#attributes' => array(
                'class' => array(
                  'history_item-values',
                ),
              ),
              '#prefix' => '<div class="history_item-label"><strong class="updater_name">' . $updater_name . '</strong> <span class="updater_id">[id: ' . $user_id . ']</span> has made the following changes:</div> ',
            );

            $history_items = array();
            foreach ($processed_value['fields'] as $group_name => $group_values) {
              foreach ($group_values as $field_name => $field_values) {
                $presentation_group = $group_name;
                if (!empty($presentation[$group_name][$field_name]['group'])) {
                  $presentation_group = $presentation[$group_name][$field_name]['group'];
                }

                $group_label = "";
                if (array_key_exists($presentation_group, $presentation_tables)) {
                  $group_label = $presentation_tables[$presentation_group]['label'];
                }

                $group_field_name = $presentation_group . '-' . $field_name;
                $form['history'][$fieldset_name][$values_name][$group_field_name] = array(
                  '#id' => $fieldset_id . '-changes_by-' . $user_id . '-' . $presentation_group . '-' . $field_name . '-wrapper',
                  '#type' => 'container',
                  '#tree' => TRUE,
                  '#attributes' => array(
                    'class' => array(
                      'history_item-value',
                      'history_item-value-' . $group_field_name,
                    ),
                  ),
                );

                $item_label = '<div class="item-label">';
                $item_label .= '<strong class="field_group">' . $group_label . '</strong>  ';
                $item_label .= '<strong class="field_name">' . $field_name . '</strong> to: ';
                $item_label .= '</div>';

                $form['history'][$fieldset_name][$values_name][$group_field_name]['label'] = array(
                  '#markup' => $item_label,
                );

                foreach ($field_values as $key => $value) {
                  $form['history'][$fieldset_name][$values_name][$group_field_name][$key] = $value;
                }
              }
            }
          }
        }
      }
    }

    if ($total_results == 0) {
      drupal_not_found();
      drupal_exit();
      return array();
    }

    $form['navigator_top']['markup'] = array(
      '#markup' => '',
    );

    $output = &$form['navigator_top']['markup']['#markup'];

    if ($page_number > 0) {
      if ($page_number == 1) {
        $output .= '<a href="' . $base_path . 'events/history-0/' . $event_id . '" class="event_request-history-page-choice event_request-history-page-previous" title="Previous Page">Prev</a>';
      }
      else {
        $previous_page = $page_number - 1;
        $output .= '<a href="' . $base_path . 'events/history-0/' . $event_id . '/' . $previous_page . '" class="event_request-history-page-choice event_request-history-page-previous" title="Previous Page">Prev</a>';
      }
    }

    if ($total_results > $rows_per_page) {
      $next_page = $page_number + 1;
      $output .= '<a href="' . $base_path . 'events/history-0/' . $event_id . '/' . $next_page . '" class="event_request-history-page-choice event_request-history-page-next" title="Next Page">Next</a>';
    }

    $form['navigator_bottom']['markup'] = $form['navigator_top']['markup'];
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return array();
  }

  return $form;
}

/**
 * Builds the name of the user who performed some update.
 *
 * @param int $user_id
 *   Drupal system user ID.
 * @param array $processed_values
 *   An array container already processed users for improved performance.
 *
 * @return string|bool|NULL
 *   The built name of the user.
 *   NULL for no user name generated.
 *   FALSE on error.
 */
function mcneese_event_workflow_event_history_0_form_build_updater_name($user_id, &$processed_values) {
  if (!cf_is_integer($user_id)) {
    cf_error::invalid_integer('user_id');
    return FALSE;
  }

  if (!is_array($processed_values)) {
    cf_error::invalid_array('processed_values');
    return FALSE;
  }

  $updater_name = NULL;

  if (!array_key_exists($user_id, $processed_values)) {
    $updater = user_load($user_id);

    $updater_name = NULL;
    if (is_object($updater)) {
      if (!empty($updater->field_user_first_name['und'][0]['safe_value'])) {
        $updater_name = $updater->field_user_first_name['und'][0]['safe_value'];

        if (!empty($updater->field_user_last_name['und'][0]['safe_value'])) {
          $updater_name .= ' ' . $updater->field_user_last_name['und'][0]['safe_value'];
        }
      }

      if (is_null($updater_name)) {
        $updater_name = $updater->name;
      }
    }

    if (is_null($updater_name)) {
      $updater_name = "Unknown User (id: " . check_plain($user_data->value) . ")";
    }

    $processed_values[$user_id] = array(
      'updater_name' => $updater_name,
      'fields' => array(),
    );
  }
  else {
    $updater_name = $processed_values[$user_id]['updater_name'];
  }

  return $updater_name;
}


/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
