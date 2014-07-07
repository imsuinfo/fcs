<?php

/**
 * @file
 * Defines McNeese event workflow event search page functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Provides the event workflow event search page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 *
 * @return array
 *   An array containing the generated form.
 */
function mcneese_event_workflow_event_search_0_form($form, &$form_state) {
  $manager = user_access('mcneese event workflow manage');
  $reviewer = user_access('mcneese event workflow review');
  $requester = user_access('mcneese event workflow request');

  $form = array();
  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['page_number'] = array(
    '#type' => 'value',
    '#value' => 0,
  );

  if (isset($form_state['simple_search-page_number']) && cf_is_integer($form_state['simple_search-page_number'])) {
    $form['form']['page_number']['#value'] = $form_state['simple_search-page_number'];
  }

  $form['form']['per_page'] = array(
    '#type' => 'value',
    '#value' => 25,
  );

  $form['form']['conditionally_required'] = array(
    '#type' => 'value',
    '#value' => array(),
  );


  mcneese_event_workflow_include(0);
  mcneese_event_workflow_include(4);

  $structure = mcneese_event_workflow_table_structure();

  $possible_targets = array();
  foreach ($structure as $table_name => $table_fields) {
    foreach ($table_fields as $field_name => $field_values) {
      if (empty($field_values['name'])) continue;

      // @todo: adding the advanced search of the review logs will be very complicated, so implement it when more time is available.
      if ($table_name == 'review') continue;

      $possible_targets[$table_name . '-' . $field_name] = $field_values['name'];
    }
  }

  asort($possible_targets);

  $form['form']['possible_targets'] = array(
    '#type' => 'value',
    '#value' => $possible_targets,
  );

  $form['simple'] = array(
    '#id' => 'search-simple-fieldset',
    '#type' => 'fieldset',
    '#title' => "Simple Search",
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  $form['simple']['text'] = array(
    '#id' => 'search-simple-text',
    '#type' => 'textfield',
    '#element_attributes' => array(
      'class' => array(
        'no-print',
        'form-item',
        'form-type-textfield',
        'search-simple-field',
        'search-simple-field-text',
      ),
    ),
  );

  $form['simple']['sensitive'] = array(
    '#id' => 'search-simple-sensitive',
    '#type' => 'checkbox',
    '#title' => "Case Sensitive",
    '#default_value' => FALSE,
    '#element_attributes' => array(
      'class' => array(
        'no-print',
        'form-item',
        'form-type-checkbox',
        'search-simple-field',
        'search-simple-field-sensitive',
      ),
    ),
  );

  $form['simple']['submit'] = array(
    '#id' => 'search-simple-submit',
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'no-print',
        'search-simple-submit',
        'page-' . $form['form']['page_number']['#value'],
      ),
    ),
  );

  $form['simple']['submit']['search'] = array(
    '#id' => 'search-simple-submit-search',
    '#name' => 'search-simple-submit-search',
    '#type' => 'submit',
    '#default_value' => t("Search"),
    '#attributes' => array(
      'class' => array(
        'no-print',
        'search-simple-submit-search',
        'page-' . $form['form']['page_number']['#value'],
      ),
      'name' => 'search-simple-submit-search',
    ),
  );

  $form['simple']['results'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
  );

  // populate the search results.
  if (isset($form_state['simple_search-results']) && is_array($form_state['simple_search-results'])) {
    $simple_table = mcneese_event_workflow_build_request_table($form_state['simple_search-results'], $form['form']['per_page']['#value']);

    $simple_table['caption'] = t("Simple Search Results");
    $simple_table['header']['submitted_on']['sort'] = 'desc';

    $simple_table['attributes'] = array();
    $simple_table['attributes']['id'] = 'mcneese_event_workflow-management_page-simple_table';
    $simple_table['attributes']['class'] = array ('mcneese_event_workflow-management_page-simple_table');
    $simple_table['attributes']['summary'] = 'A table containing the search results.';

    $simple_table['empty'] = t("No event requests were found matching your simple search query.");

    $form['simple']['results']['table'] = array(
      '#markup' => theme('table', $simple_table),
    );

    $form['simple']['results']['navigation'] = array(
      '#id' => 'search-simple-results-navigation',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'no-print',
          'search-simple-results-navigation',
        ),
      ),
    );

    $form['simple']['results']['navigation']['previous'] = array(
      '#id' => 'search-simple-results-navigation-previous',
      '#name' => 'search-simple-results-navigation-previous',
      '#type' => 'submit',
      '#default_value' => t("Prev"),
      '#attributes' => array(
        'class' => array(
          'no-print',
          'search-simple-results-navigation-choice',
          'search-simple-results-navigation-previous',
        ),
        'title' => "Previous Simple Search",
      ),
    );

    $form['simple']['results']['navigation']['next'] = array(
      '#id' => 'search-simple-results-navigation-next',
      '#name' => 'search-simple-results-navigation-next',
      '#type' => 'submit',
      '#default_value' => t("Next"),
      '#attributes' => array(
        'class' => array(
          'no-print',
          'search-simple-results-navigation-choice',
          'search-simple-results-navigation-next',
        ),
        'title' => "Next Simple Search",
      ),
    );

    $total_results = count($form_state['simple_search-results']);

    if ($form['form']['page_number']['#value'] < 1) {
      unset($form['simple']['results']['navigation']['previous']);
    }

    if ($total_results <= $form['form']['per_page']['#value']) {
      unset($form['simple']['results']['navigation']['next']);
    }
  }

  $available_targets = $possible_targets;
  $chosen_targets = array();
  $chosen_targets_options = array();

  if (!empty($form_state['chosen_targets']) && is_array($form_state['chosen_targets'])) {
    $chosen_targets = $form_state['chosen_targets'];

    foreach ($chosen_targets as $chosen_target_field => $chose_target_fields) {
      if (!empty($chose_target_fields)) {
        foreach ($chose_target_fields as $delta => $field_name) {
          if (!isset($chosen_targets_options[$field_name])) {
            $chosen_targets_options[$field_name] = array();
          }
          $chosen_targets_options[$field_name][$chosen_target_field . '-' . $delta] = $field_name . ' #' . $delta;
        }
      }
    }
  }

  $form['advanced'] = array(
    '#id' => 'search-advanced-fieldset',
    '#type' => 'fieldset',
    '#title' => "Search",
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  $form['advanced']['search_targets'] = array(
    '#id' => 'search-advanced-search_targets',
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'no-print',
        'form-item',
        'form-type-select',
        'search-advanced-search_targets',
      ),
    ),
    '#tree' => TRUE,
  );

  $form['advanced']['search_targets']['available_targets'] = array(
    '#id' => 'search-advanced-search_targets-available_targets',
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'form-item',
        'form-type-select',
        'search-advanced-search_targets-available_targets',
      ),
    ),
    '#tree' => TRUE,
  );

  $form['advanced']['search_targets']['available_targets']['select'] = array(
    '#id' => 'search-advanced-search_targets-available_targets-select',
    '#type' => 'select',
    '#title' => t("Available Search Targets"),
    '#description' => t("Choose a particular target to search by."),
    '#options' => $available_targets,
    '#element_attributes' => array(
      'class' => array(
        'form-item',
        'form-type-select',
        'search-advanced-search_targets-available_targets-select',
      ),
    ),
  );

  $form['advanced']['search_targets']['available_targets']['add'] = array(
    '#id' => 'search-advanced-search_targets-available_targets-add',
    '#name' => 'search-advanced-search_targets-available_targets-add',
    '#type' => 'submit',
    '#default_value' => "Add Target",
    '#attributes' => array(
      'class' => array(
        'form-item',
        'form-type-submit',
        'search-advanced-search_targets-available_targets-add',
      ),
    ),
  );

  $form['advanced']['search_targets']['chosen_targets'] = array(
    '#id' => 'search-advanced-search_targets-chosen_targets',
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'form-item',
        'form-type-select',
        'search-advanced-search_targets-chosen_targets',
      ),
    ),
    '#tree' => TRUE,
  );

  $form['advanced']['search_targets']['chosen_targets']['select'] = array(
    '#id' => 'search-advanced-search_targets-chosen_targets-select',
    '#type' => 'select',
    '#title' => t("Chosen Search Targets"),
    '#description' => t("Remove a particular target from the search by choices."),
    '#options' => $chosen_targets_options,
    '#element_attributes' => array(
      'class' => array(
        'form-item',
        'form-type-select',
        'search-advanced-search_targets-chosen_targets-select',
      ),
    ),
  );

  $form['advanced']['search_targets']['chosen_targets']['remove'] = array(
    '#id' => 'search-advanced-search_targets-chosen_targets-remove',
    '#name' => 'search-advanced-search_targets-chosen_targets-remove',
    '#type' => 'submit',
    '#default_value' => "Remove Target",
    '#element_attributes' => array(
      'class' => array(
        'form-item',
        'form-type-submit',
        'search-advanced-search_targets-chosen_targets-remove',
      ),
    ),
  );

  $form['advanced']['sensitive'] = array(
    '#id' => 'search-advanced-sensitive',
    '#type' => 'checkbox',
    '#title' => "Case Sensitive",
    '#default_value' => FALSE,
    '#element_attributes' => array(
      'class' => array(
        'no-print',
        'form-item',
        'form-type-checkbox',
        'search-advanced-field',
        'search-advanced-field-sensitive',
      ),
    ),
  );

  $form['advanced']['targets'] = array(
    '#id' => 'search-advanced-targets',
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'no-print',
        'form-item',
        'search-advanced-targets',
      ),
    ),
    '#tree' => TRUE,
  );

  foreach ($chosen_targets_options as $chosen_target_group => $chosen_target_fields) {
    if (empty($chosen_target_fields)) continue;

    foreach ($chosen_target_fields as $chosen_target_field => $target_field_name) {
      mcneese_event_workflow_event_search_0_form_build_target($form, $form_state, $structure, $chosen_target_field);
    }
  }

  $form['advanced']['submit'] = array(
    '#id' => 'search-advanced-submit',
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'no-print',
        'search-advanced-submit',
        'page-' . $form['form']['page_number']['#value'],
      ),
    ),
  );

  $form['advanced']['submit']['search'] = array(
    '#id' => 'search-advanced-submit-search',
    '#name' => 'search-advanced-submit-search',
    '#type' => 'submit',
    '#default_value' => t("Advanced Search"),
    '#attributes' => array(
      'class' => array(
        'no-print',
        'search-advanced-submit-search',
        'page-' . $form['form']['page_number']['#value'],
      ),
      'name' => 'search-advanced-submit-search',
    ),
  );

  $form['advanced']['results'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
  );

  // populate the search results.
  if (isset($form_state['advanced_search-results']) && is_array($form_state['advanced_search-results'])) {
    $advanced_table = mcneese_event_workflow_build_request_table($form_state['advanced_search-results'], $form['form']['per_page']['#value']);

    $advanced_table['caption'] = t("Advanced Search Results");
    $advanced_table['header']['submitted_on']['sort'] = 'desc';

    $advanced_table['attributes']['id'] = 'mcneese_event_workflow-management_page-advanced_table';
    $advanced_table['attributes']['class'] = array ('mcneese_event_workflow-management_page-advanced_table');
    $advanced_table['attributes']['summary'] = 'A table containing the search results.';

    $advanced_table['empty'] = t("No event requests were found matching your advanced search query.");

    $form['advanced']['results']['table'] = array(
      '#markup' => theme('table', $advanced_table),
    );

    $form['advanced']['results']['navigation'] = array(
      '#id' => 'search-advanced-results-navigation',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'no-print',
          'search-advanced-results-navigation',
        ),
      ),
    );

    $form['advanced']['results']['navigation']['previous'] = array(
      '#id' => 'search-advanced-results-navigation-previous',
      '#name' => 'search-advanced-results-navigation-previous',
      '#type' => 'submit',
      '#default_value' => t("Prev"),
      '#attributes' => array(
        'class' => array(
          'no-print',
          'search-advanced-results-navigation-choice',
          'search-advanced-results-navigation-previous',
        ),
        'title' => "Previous Advanced Search",
      ),
    );

    $form['advanced']['results']['navigation']['next'] = array(
      '#id' => 'search-advanced-results-navigation-next',
      '#name' => 'search-advanced-results-navigation-next',
      '#type' => 'submit',
      '#default_value' => t("Next"),
      '#attributes' => array(
        'class' => array(
          'no-print',
          'search-advanced-results-navigation-choice',
          'search-advanced-results-navigation-next',
        ),
        'title' => "Next Advanced Search",
      ),
    );

    $total_results = count($form_state['advanced_search-results']);

    if ($form['form']['page_number']['#value'] < 1) {
      unset($form['advanced']['results']['navigation']['previous']);
    }

    if ($total_results <= $form['form']['per_page']['#value']) {
      unset($form['advanced']['results']['navigation']['next']);
    }
  }

  return $form;
}

/**
 * Pre-Validation for mcneese_event_workflow_event_search_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_search_0_form()
 */
function mcneese_event_workflow_event_search_0_form_pre_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id != 'search-advanced-submit-search') {
    if (!empty($form['form']['conditionally_required']['#value'][0])) {
      foreach ($form['form']['conditionally_required']['#value'][0] as $field_name => &$fields) {
        if (!empty($fields)) {
          foreach ($fields as $delta => &$field) {
            $field['#required'] = FALSE;
          }
        }
      }
    }
  }
}

/**
 * Validation for mcneese_event_workflow_event_search_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_search_0_form()
 */
function mcneese_event_workflow_event_search_0_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'search-simple-submit-search') {
    if (empty($form_state['values']['simple']['text']) && !is_numeric($form_state['values']['simple']['text'])) {
      form_set_error('simple][text', "You must supply some text to search for.");
    }
  }
  elseif ($clicked_id == 'search-advanced-submit-search') {
  }
  elseif ($clicked_id == 'search-simple-results-navigation-previous') {
  }
  elseif ($clicked_id == 'search-simple-results-navigation-next') {
  }
  elseif ($clicked_id == 'search-advanced-search_targets-available_targets-add') {
  }
  elseif ($clicked_id == 'search-advanced-search_targets-chosen_targets-remove') {
  }
  else {
    form_set_error('', "Internal Error. An unknown submit process has been detected. Please contact the appropriate support staff.");
  }
}

/**
 * Submit for mcneese_event_workflow_event_search_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mcneese_event_workflow_event_search_0_form()
 */
function mcneese_event_workflow_event_search_0_form_submit($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'search-simple-submit-search') {
    $search = $form_state['values']['simple']['text'];
    $sensitive = (bool) $form_state['values']['simple']['sensitive'];

    $per_page = $form['form']['per_page']['#value'];
    $start = $form['form']['page_number']['#value'] * $per_page;

    $form_state['simple_search-results'] = mcneese_event_workflow_event_load_listing($search, $sensitive, $start, $per_page);
    $form_state['simple_search-search'] = $search;
    $form_state['simple_search-sensitive'] = $sensitive;
  }
  elseif ($clicked_id == 'search-simple-results-navigation-previous') {
    $previous_page = $form['form']['page_number']['#value'] - 1;
    $search = $form_state['simple_search-search'];
    $sensitive = (bool) $form_state['search-sensitive'];

    $per_page = $form['form']['per_page']['#value'];
    $start = $previous_page * $per_page;

    $form_state['simple_search-results'] = mcneese_event_workflow_event_load_listing($search, $sensitive, $start, $per_page);
    $form_state['simple_search-page_number'] = $previous_page;
    $form_state['simple_search-search'] = $search;
    $form_state['simple_search-sensitive'] = $sensitive;
  }
  elseif ($clicked_id == 'search-simple-results-navigation-next') {
    $next_page = $form['form']['page_number']['#value'] + 1;
    $search = $form_state['simple_search-search'];
    $sensitive = (bool) $form_state['simple_search-sensitive'];

    $per_page = $form['form']['per_page']['#value'];
    $start = $next_page * $per_page;

    $form_state['simple_search-results'] = mcneese_event_workflow_event_load_listing($search, $sensitive, $start, $per_page);
    $form_state['simple_search-page_number'] = $next_page;
    $form_state['simple_search-search'] = $search;
    $form_state['simple_search-sensitive'] = $sensitive;
  }
  elseif ($clicked_id == 'search-advanced-search_targets-available_targets-add') {
    if (!isset($form_state['chosen_targets']) || !is_array($form_state['chosen_targets'])) {
      $form_state['chosen_targets'] = array();
    }

    if (isset($form_state['values']['advanced']['search_targets']['available_targets']['select'])) {
      $target = $form_state['values']['advanced']['search_targets']['available_targets']['select'];

      $target_count = 0;

      if (isset($form_state['chosen_targets'][$target])) {
        $target_count = count($form_state['chosen_targets'][$target]);
      }
      else {
        $form_state['chosen_targets'][$target] = array();
      }

      if (isset($form['form']['possible_targets']['#value'][$target])) {
        $form_state['chosen_targets'][$target][$target_count] = $form['form']['possible_targets']['#value'][$target];
      }
    }
  }
  elseif ($clicked_id == 'search-advanced-search_targets-chosen_targets-remove') {
    if (!isset($form_state['chosen_targets']) || !is_array($form_state['chosen_targets'])) {
      $form_state['chosen_targets'] = array();
    }

    if (isset($form_state['values']['advanced']['search_targets']['chosen_targets']['select'])) {
      $target = $form_state['values']['advanced']['search_targets']['chosen_targets']['select'];

      $target_parts = explode('-', $target);

      if (count($target_parts) == 3 && !empty($target_parts[0]) && !empty($target_parts[1]) && is_numeric($target_parts[2])) {
        $chosen_target_field_name = $target_parts[0] . '-' . $target_parts[1];

        if (isset($form['form']['possible_targets']['#value'][$chosen_target_field_name][$target_parts[2]])) {
          unset($form_state['chosen_targets'][$chosen_target_field_name][$target_parts[2]]);
        }
      }
    }
  }
  elseif ($clicked_id == 'search-advanced-submit-search') {
    $search = array();
    $sensitive = (bool) $form_state['values']['simple']['sensitive'];

    $per_page = $form['form']['per_page']['#value'];
    $start = $form['form']['page_number']['#value'] * $per_page;

    mcneese_event_workflow_include(0);

    $structure = mcneese_event_workflow_table_structure();

    $or = array();
    if (empty($form_state['values']['advanced']['targets'])) {
      $form_state['advanced_search-results'] = array();
      $form_state['advanced_search-search'] = $search;
      $form_state['advanced_search-sensitive'] = $sensitive;
    }
    else {
      foreach ($form_state['values']['advanced']['targets'] as $target_name => $target_values) {
        foreach ($target_values as $target_delta => $target_value) {
          $built = mcneese_event_workflow_event_search_0_form_build_search_target($structure, $target_name, $target_value);

          if (!empty($built)) {
            foreach ($built as $key => $value) {
              $or[$key] = $value;
            }

            unset($built);
          }
        }
      }

      $search = array('or' => $or);

      $form_state['advanced_search-results'] = mcneese_event_workflow_event_load_listing($search, $sensitive, $start, $per_page);
      $form_state['advanced_search-search'] = $search;
      $form_state['advanced_search-sensitive'] = $sensitive;
    }
  }

  // return to the form with the search results.
  $form_state['rebuild'] = TRUE;
  $form_state['redirect'] = FALSE;
  $form_state['submitted'] = TRUE;
}

/**
 * Provides the event workflow event search page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 * @param array $structure
 *   Structure array as returned by mcneese_event_workflow_table_structure().
 * @param string $target_field
 *   Name of the field to process and convert into a search field.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 *
 * @see mcneese_event_workflow_table_structure()
 */
function mcneese_event_workflow_event_search_0_form_build_target(&$form, &$form_state, $structure, $target_field) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return FALSE;
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');
    return FALSE;
  }

  if (!is_array($structure)) {
    cf_error::invalid_array('structure');
    return FALSE;
  }

  if (!is_string($target_field)) {
    cf_error::invalid_string('target_field');
    return FALSE;
  }

  $target_field_parts = explode('-', $target_field);
  if (count($target_field_parts) != 3) {
    return FALSE;
  }

  if (empty($target_field_parts[0]) || empty($target_field_parts[1]) || !cf_is_integer($target_field_parts[2])) {
    return FALSE;
  }

  $table_name = $target_field_parts[0];
  $field_name = $target_field_parts[1];
  $field_delta = $target_field_parts[2];

  if (!isset($structure[$table_name][$field_name])) {
    return FALSE;
  }

  $field = $structure[$table_name][$field_name];
  $targets = &$form['advanced']['targets'];

  if (!isset($targets[$target_field])) {
    $targets[$target_field] = array(
      '#id' => 'search-advanced-targets-' . $target_field,
      '#type' => 'container',
      '#attributes' => array(
        'class' => array(
          'search-advanced-target',
          'search-advanced-targets-' . $target_field,
        ),
      ),
      '#tree' => TRUE,
    );
  }

  $targets[$target_field][$field_delta] = array(
    '#tree' => TRUE,
  );

  $targets[$target_field][$field_delta]['search'] = array(
    '#id' => 'search-advanced-targets-' . $target_field . '-' . $field_delta . '-search',
    '#title' => $field['name'] . ' #' . $field_delta,
    '#default_value' => NULL,
    '#required' => FALSE,
    '#attributes' => array(
      'class' => array(
        'search-advanced-target-search',
        'search-advanced-targets-' . $target_field . '-search',
        'search-advanced-targets-' . $target_field . '-' . $field_delta . '-search',
      ),
    ),
  );

  if (!isset($form['form']['conditionally_required']['#value'][0])) {
    $form['form']['conditionally_required']['#value'][0] = array();
  }

  if (!isset($form['form']['conditionally_required']['#value'][0][$target_field])) {
    $form['form']['conditionally_required']['#value'][0][$target_field] = array();
  }

  $form['form']['conditionally_required']['#value'][0][$target_field][$field_delta] = &$targets[$target_field][$field_delta]['search'];

  if (isset($form_state['values']['advanced']['targets'][$target_field][$field_delta]['search'])) {
    $targets[$target_field][$field_delta]['search']['#default_value'] = $form_state['values']['advanced']['targets'][$target_field][$field_delta]['search'];
  }

  if ($field['type'] == 'text') {
    $targets[$target_field][$field_delta]['search']['#type'] = 'textfield';
    $targets[$target_field][$field_delta]['search']['#size'] = 32;
  }
  elseif ($field['type'] == 'bool') {
    $targets[$target_field][$field_delta]['search']['#type'] = 'select';
    $targets[$target_field][$field_delta]['search']['#options'] = array(1 => "Yes / True", 0 => "No / False");
  }
  elseif ($field['type'] == 'event_status') {
    $targets[$target_field][$field_delta]['search']['#type'] = 'select';
    $targets[$target_field][$field_delta]['search']['#options'] = event_workflow_get_event_status_list_options('search');
  }
  elseif ($field['type'] == 'event_step') {
    $targets[$target_field][$field_delta]['search']['#type'] = 'select';
    $targets[$target_field][$field_delta]['search']['#options'] = event_workflow_get_event_steps_list_options('search');
  }
  elseif ($field['type'] == 'event_location') {
    $targets[$target_field][$field_delta]['search']['#type'] = 'select';
    $targets[$target_field][$field_delta]['search']['#options'] = event_workflow_get_event_location_list_options('search');
  }
  elseif ($field['type'] == 'event_room') {
    $targets[$target_field][$field_delta]['search']['#type'] = 'select';
    $targets[$target_field][$field_delta]['search']['#options'] = event_workflow_get_event_room_list_options(NULL, 'search');
  }
  elseif ($field['type'] == 'use_fees') {
    unset($targets[$target_field][$field_delta]['search']);
    unset($form['form']['conditionally_required']['#value'][0][$target_field][$field_delta]);

    $targets[$target_field][$field_delta]['hours'] = array(
      '#id' => 'search-advanced-targets-' . $target_field . '-hours',
      '#type' => 'textfield',
      '#size' => 9,
      '#title' => $field['name'] . " Hours" . ' #' . $field_delta,
      '#default_value' => NULL,
      '#required' => FALSE,
      '#attributes' => array(
        'class' => array(
          'search-advanced-target-search',
          'search-advanced-targets-' . $target_field . '-hours',
        ),
      ),
    );

    $targets[$target_field][$field_delta]['days'] = array(
      '#id' => 'search-advanced-targets-' . $target_field . '-days',
      '#type' => 'textfield',
      '#size' => 9,
      '#title' => $field['name'] . " Days" . ' #' . $field_delta,
      '#default_value' => NULL,
      '#required' => FALSE,
      '#attributes' => array(
        'class' => array(
          'search-advanced-target-search',
          'search-advanced-targets-' . $target_field . '-days',
        ),
      ),
    );

    $targets[$target_field][$field_delta]['amount'] = array(
      '#id' => 'search-advanced-targets-' . $target_field . '-amount',
      '#type' => 'textfield',
      '#size' => 12,
      '#title' => $field['name'] . " Amount" . ' #' . $field_delta,
      '#default_value' => NULL,
      '#required' => FALSE,
      '#attributes' => array(
        'class' => array(
          'search-advanced-target-search',
          'search-advanced-targets-' . $target_field . '-amount',
        ),
      ),
    );

    $form['form']['conditionally_required']['#value'][0][$target_field][$field_delta . '-hours'] = &$targets[$target_field][$field_delta]['hours'];
    $form['form']['conditionally_required']['#value'][0][$target_field][$field_delta . '-days'] = &$targets[$target_field][$field_delta]['days'];
    $form['form']['conditionally_required']['#value'][0][$target_field][$field_delta . '-amount'] = &$targets[$target_field][$field_delta]['amount'];
  }
  elseif ($field['type'] == 'venue_coordinator') {
    $targets[$target_field][$field_delta]['search']['#type'] = 'textfield';
    $targets[$target_field][$field_delta]['search']['#size'] = 32;
  }
  elseif ($field['type'] == 'date') {
    $targets[$target_field][$field_delta]['search']['#type'] = 'date_popup';
    $targets[$target_field][$field_delta]['search']['#size'] = 32;
    $targets[$target_field][$field_delta]['search']['#date_format'] = "Y/m/d";
    $targets[$target_field][$field_delta]['search']['#date_label_position'] = 'invisible';
    $targets[$target_field][$field_delta]['search']['#expected_format'] = array('date' => "Y/m/d");
  }
  elseif ($field['type'] == 'time') {
    $targets[$target_field][$field_delta]['search']['#type'] = 'textfield';
    $targets[$target_field][$field_delta]['search']['#size'] = 8;
    $targets[$target_field][$field_delta]['search']['#maxlength'] = 8;
    $targets[$target_field][$field_delta]['search']['#expected_format'] = array('date' => "h:ia");
    $targets[$target_field][$field_delta]['search']['#description'] = "E.g, 12:31pm";
  }

  return TRUE;
}

/**
 * Build the array structure for.
 *
 * @param array $structure
 *   Structure array as returned by mcneese_event_workflow_table_structure().
 * @param string $target_field
 *   Name of the field to process and convert into a search field.
 * @param array $target_values
 *   An array of values associated with a given field.
 *
 * @return array|bool
 *   Array of search options or FALSE on error.
 *
 * @see mcneese_event_workflow_table_structure()
 */
function mcneese_event_workflow_event_search_0_form_build_search_target($structure, $target_field, $target_values) {
  if (!is_array($structure)) {
    cf_error::invalid_array('structure');
    return FALSE;
  }

  if (!is_string($target_field)) {
    cf_error::invalid_string('target_field');
    return FALSE;
  }

  if (!is_array($target_values)) {
    cf_error::invalid_array('target_values');
    return FALSE;
  }

  $target_field_parts = explode('-', $target_field);
  if (count($target_field_parts) != 3) {
    return FALSE;
  }

  if (empty($target_field_parts[0]) || empty($target_field_parts[1]) || !cf_is_integer($target_field_parts[2])) {
    return FALSE;
  }

  $table_name = $target_field_parts[0];
  $field_name = $target_field_parts[1];
  $field_delta = $target_field_parts[2];

  if (!isset($structure[$table_name][$field_name])) {
    return FALSE;
  }

  // target values may be empty, that just means there is nothing to search for, so return array() instead of FALSE.
  if (empty($target_values)) {
    return array();
  }

  $field = $structure[$table_name][$field_name];

  $search = array();
  if ($field['type'] == 'use_fees') {
    $and = array();

    if (!empty($target_values['hours'])) {
      $target_field_alt = $target_field . '-hours';
      $and[$target_field_alt]['group_name'] = $table_name;
      $and[$target_field_alt]['field_name'] = $field_name;
      $and[$target_field_alt]['search'] = $target_values['hours'];
      $and[$target_field_alt]['column'] = 'hours';
      $and[$target_field_alt]['type'] = $field['type'];
      $and[$target_field_alt]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
    }

    if (!empty($target_values['days'])) {
      $target_field_alt = $target_field . '-days';
      $and[$target_field_alt]['group_name'] = $table_name;
      $and[$target_field_alt]['field_name'] = $field_name;
      $and[$target_field_alt]['search'] = $target_values['days'];
      $and[$target_field_alt]['column'] = 'days';
      $and[$target_field_alt]['type'] = $field['type'];
      $and[$target_field_alt]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
    }

    if (!empty($target_values['amount'])) {
      $target_field_alt = $target_field . '-amount';
      $and[$target_field_alt]['group_name'] = $table_name;
      $and[$target_field_alt]['field_name'] = $field_name;
      $and[$target_field_alt]['search'] = event_workflow_convert_value_to_database_format($target_values['amount'], 'currency');
      $and[$target_field_alt]['column'] = 'amount';
      $and[$target_field_alt]['type'] = $field['type'];
      $and[$target_field_alt]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
    }

    if (!empty($and)) {
      $search['and'] = $and;
    }
  }
  else {
    if (is_array($target_values) && array_key_exists('search', $target_values)) {
      $search[$target_field] = array();
      $search[$target_field]['group_name'] = $table_name;
      $search[$target_field]['field_name'] = $field_name;
      $search[$target_field]['column'] = 'value';
      $search[$target_field]['type'] = $field['type'];
      $search[$target_field]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;

      if (!empty($field['storage'])) {
        $search[$target_field]['search'] = event_workflow_convert_value_to_database_format($target_values['search'], $field['storage']);
      }
      else {
        $search[$target_field]['search'] = $target_values['search'];
      }
    }
  }

  return $search;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
