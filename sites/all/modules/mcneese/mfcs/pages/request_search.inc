<?php

/**
 * @file
 * Defines McNeese facilities use request search page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request search page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 *
 * @return array
 *   An array containing the generated form.
 */
function mfcs_request_search_0_form($form, &$form_state) {
  mfcs_add_canonical_headers('requests/search-0');

  $user = cf_current_user();

  $manager = user_access('mfcs manage', $user);
  $reviewer = user_access('mfcs review', $user);
  $requester = user_access('mfcs request', $user);

  $form = array();
  $form['form'] = array(
    '#tree' => TRUE,
  );

  $form['form']['simple_search-page_number'] = array(
    '#type' => 'value',
    '#value' => 0,
  );

  if (isset($form_state['simple_search-page_number']) && cf_is_integer($form_state['simple_search-page_number'])) {
    $form['form']['simple_search-page_number']['#value'] = $form_state['simple_search-page_number'];
  }

  $form['form']['advanced_search-page_number'] = array(
    '#type' => 'value',
    '#value' => 0,
  );

  if (isset($form_state['advanced_search-page_number']) && cf_is_integer($form_state['advanced_search-page_number'])) {
    $form['form']['advanced_search-page_number']['#value'] = $form_state['advanced_search-page_number'];
  }

  $form['form']['per_page'] = array(
    '#type' => 'value',
    '#value' => 25,
  );

  $form['form']['conditionally_required'] = array(
    '#type' => 'value',
    '#value' => array(),
  );

  $form['form']['existing_search_targets'] = array(
    '#type' => 'value',
    '#value' => array(),
  );

  if (empty($form['form']['existing_search_targets']['#value'])) {
    $existing_search_targets = mfcs_search_target_grouping_load();
    if (is_array($existing_search_targets)) {
      $form['form']['existing_search_targets']['#value'] = $existing_search_targets;
    }
  }

  mfcs_include(MFCS_INCLUDE_TABLE);
  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $structure = mfcs_table_structure();
  $presentation = mfcs_table_presentation();

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

  $form['form']['structure'] = array(
    '#type' => 'value',
    '#value' => &$structure,
  );

  $form['form']['presentation'] = array(
    '#type' => 'value',
    '#value' => &$presentation,
  );

  $form['form']['advanced_search-possible_targets'] = array(
    '#type' => 'value',
    '#value' => $possible_targets,
  );

  $form['form']['advanced_search-row_count'] = array(
    '#type' => 'value',
    '#value' => 0,
  );

  $row_count = &$form['form']['advanced_search-row_count']['#value'];
  if (isset($form_state['advanced_search-row_count']) && $form_state['advanced_search-row_count'] > 0) {
    $row_count = $form_state['advanced_search-row_count'];
  }

  $form['simple'] = array(
    '#id' => 'search-simple-fieldset',
    '#type' => 'fieldset',
    '#title' => "Simple Search",
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'fieldset_print_as_div',
      ),
    ),
    '#legend_attributes' => array(
      'class' => array(
        'no_print_links_as_link',
      ),
    ),
  );

  $form['simple']['text'] = array(
    '#id' => 'search-simple-text',
    '#type' => 'textfield',
    '#default_value' => isset($form_state['values']['simple']['text']) ? $form_state['values']['simple']['text'] : FALSE,
    '#defaults_loaded' => FALSE,
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
    '#default_value' => isset($form_state['values']['simple']['sensitive']) ? $form_state['values']['simple']['sensitive'] : FALSE,
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
        'page-' . $form['form']['simple_search-page_number']['#value'],
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
        'page-' . $form['form']['simple_search-page_number']['#value'],
      ),
    ),
  );

  $form['simple']['submit']['clear'] = array(
    '#id' => 'search-simple-submit-clear',
    '#name' => 'search-simple-submit-clear',
    '#type' => 'submit',
    '#default_value' => t("Clear"),
    '#attributes' => array(
      'class' => array(
        'no-print',
        'search-simple-submit-clear',
        'page-' . $form['form']['simple_search-page_number']['#value'],
      ),
    ),
  );

  $form['simple']['results'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
  );

  // populate the search results.
  if (isset($form_state['simple_search-results']) && is_array($form_state['simple_search-results'])) {
    $request_table_extra = array(
      'table_sort' => FALSE,
    );
    $simple_table = mfcs_build_request_table($form_state['simple_search-results'], $form['form']['per_page']['#value'], NULL, NULL, $request_table_extra);

    $simple_table['caption'] = t("Simple Search Results");
    $simple_table['header']['submitted_on']['sort'] = 'desc';

    $simple_table['attributes']['id'] = 'mfcs-management_page-simple_table';
    $simple_table['attributes']['class'] = array ('mfcs-table-requests', 'mfcs-management_page-simple_table');
    $simple_table['attributes']['summary'] = 'A table containing the search results.';

    $simple_table['empty'] = t("No requests were found matching your simple search query.");

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

    if ($form['form']['simple_search-page_number']['#value'] < 1) {
      unset($form['simple']['results']['navigation']['previous']);
    }

    if ($total_results <= $form['form']['per_page']['#value']) {
      unset($form['simple']['results']['navigation']['next']);
    }
  }

  $form['advanced'] = array(
    '#id' => 'search-advanced-fieldset',
    '#type' => 'fieldset',
    '#title' => "Advanced Search",
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'fieldset_print_as_div',
      ),
    ),
    '#legend_attributes' => array(
      'class' => array(
        'no_print_links_as_link',
      ),
    ),
  );

  $form['advanced']['sensitive'] = array(
    '#id' => 'search-advanced-sensitive',
    '#type' => 'checkbox',
    '#title' => "Case Sensitive",
    '#default_value' => isset($form_state['values']['advanced']['sensitive']) ? $form_state['values']['advanced']['sensitive'] : FALSE,
    '#element_attributes' => array(
      'class' => array(
        'no-print',
        'form-item',
        'form-type-checkbox',
        'search-advanced-field',
        'search-advanced-field-sensitive',
      ),
    ),
    '#weight' => -2,
  );

  $form['advanced']['targets'] = array(
    '#id' => 'search-advanced-targets',
    '#type' => 'fieldset',
    '#title' => "Search Targets",
    '#description' => "All targets within this fieldset are matched using the " . '<em class="uppercase">And</em>' . " operation.",
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#attributes' => array(
      'class' => array(
        'no-print',
        'form-item',
        'search-advanced-targets',
      ),
    ),
    '#tree' => TRUE,
  );

  $chosen_targets = array();
  if (isset($form_state['advanced_search-chosen_targets'])) {
    $chosen_targets = $form_state['advanced_search-chosen_targets'];
  }

  $chosen_options = array();
  $logic_options = array();
  $state_location = NULL;
  if (isset($form_state['values']['advanced']['targets'])) {
    $state_location = $form_state['values']['advanced']['targets'];
  }

  mfcs_request_search_0_form_build_targets_and_groups($form, $form_state, $form['advanced'], $state_location, $chosen_targets, 0, '', $chosen_options, $logic_options);
  mfcs_request_search_0_form_build_grouping($form, $form_state, $form['advanced'], 0, '', $chosen_options, $logic_options);

  $form['advanced']['submit'] = array(
    '#id' => 'search-advanced-submit',
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'no-print',
        'search-advanced-submit',
        'page-' . $form['form']['advanced_search-page_number']['#value'],
      ),
    ),
    '#weight' => 2,
  );

  $form['advanced']['submit']['search'] = array(
    '#id' => 'search-advanced-submit-search',
    '#name' => 'search-advanced-submit-search',
    '#type' => 'submit',
    '#default_value' => t("Search"),
    '#attributes' => array(
      'class' => array(
        'no-print',
        'search-advanced-submit-search',
        'page-' . $form['form']['advanced_search-page_number']['#value'],
      ),
    ),
  );

  $form['advanced']['submit']['clear'] = array(
    '#id' => 'search-advanced-submit-clear',
    '#name' => 'search-advanced-submit-clear',
    '#type' => 'submit',
    '#default_value' => t("Clear"),
    '#attributes' => array(
      'class' => array(
        'no-print',
        'search-advanced-submit-clear',
        'page-' . $form['form']['advanced_search-page_number']['#value'],
      ),
    ),
  );

  $form['advanced']['submit']['reset'] = array(
    '#id' => 'search-advanced-submit-reset',
    '#name' => 'search-advanced-submit-reset',
    '#type' => 'submit',
    '#default_value' => t("Reset"),
    '#attributes' => array(
      'class' => array(
        'no-print',
        'search-advanced-submit-reset',
        'page-' . $form['form']['advanced_search-page_number']['#value'],
      ),
    ),
  );

  $form['advanced']['results'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#weight' => 3,
  );

  // populate the search results.
  if (isset($form_state['advanced_search-results']) && is_array($form_state['advanced_search-results'])) {
    $request_table_extra = array(
      'table_sort' => FALSE,
    );
    $advanced_table = mfcs_build_request_table($form_state['advanced_search-results'], $form['form']['per_page']['#value'], NULL, NULL, $request_table_extra);

    $advanced_table['caption'] = t("Advanced Search Results");
    $advanced_table['header']['submitted_on']['sort'] = 'desc';

    $advanced_table['attributes']['id'] = 'mfcs-management_page-advanced_table';
    $advanced_table['attributes']['class'] = array('mfcs-table-requests', 'mfcs-management_page-advanced_table');
    $advanced_table['attributes']['summary'] = 'A table containing the search results.';

    $advanced_table['empty'] = t("No requests were found matching your advanced search query.");

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

    if ($form['form']['advanced_search-page_number']['#value'] < 1) {
      unset($form['advanced']['results']['navigation']['previous']);
    }

    if ($total_results <= $form['form']['per_page']['#value']) {
      unset($form['advanced']['results']['navigation']['next']);
    }
  }

  // tweak printing display based on results
  $is_empty_simple_search = empty($form_state['simple_search-results']);
  $is_empty_advanced_search = empty($form_state['advanced_search-results']);
  if ($is_empty_simple_search || $is_empty_advanced_search) {
    if ($is_empty_simple_search && !$is_empty_advanced_search) {
      $form['simple']['#legend_attributes']['class'][] = 'no-print';
      $form['advanced']['#legend_attributes']['class'][] = 'no-print';
    }
    elseif (!$is_empty_simple_search && $is_empty_advanced_search) {
      $form['simple']['#legend_attributes']['class'][] = 'no-print';
      $form['advanced']['#legend_attributes']['class'][] = 'no-print';
    }
  }

  return $form;
}

/**
 * Pre-Validation for mfcs_request_search_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 *
 * @see: mfcs_request_search_0_form()
 */
function mfcs_request_search_0_form_pre_validate(&$form, &$form_state) {
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

  if ($clicked_id == 'search-advanced-search-controls-manage-new-submit-save') {
    $form['advanced']['targets']['search']['controls']['manage']['new']['human_name']['#required'] = TRUE;
  }
}

/**
 * Validation for mfcs_request_search_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 *
 * @see: mfcs_request_search_0_form()
 */
function mfcs_request_search_0_form_validate($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'search-simple-submit-search') {
    if (empty($form_state['values']['simple']['text']) && !is_numeric($form_state['values']['simple']['text'])) {
      form_set_error('simple][text', "You must supply some text to search for.");
    }
  }
  elseif ($clicked_id == 'search-simple-submit-clear') {
  }
  elseif ($clicked_id == 'search-advanced-submit-search') {
    $state_location = NULL;
    if (isset($form_state['values']['advanced']['targets'])) {
      $state_location = $form_state['values']['advanced']['targets'];
    }

    if (empty($form_state['advanced_search-chosen_targets'])) {
      form_set_error('search-advanced-submit-search', "You did not specify any targets to search for in the advanced search section.");
    }
    else {
      mfcs_request_search_0_form_validate_targets($form, $form_state, $state_location, $form_state['advanced_search-chosen_targets'], array('advanced', 'targets'), 0);
    }
  }
  elseif ($clicked_id == 'search-advanced-submit-clear') {
  }
  elseif ($clicked_id == 'search-advanced-submit-reset') {
  }
  elseif ($clicked_id == 'search-simple-results-navigation-previous') {
  }
  elseif ($clicked_id == 'search-simple-results-navigation-next') {
  }
  elseif ($clicked_id == 'search-advanced-results-navigation-previous') {
  }
  elseif ($clicked_id == 'search-advanced-results-navigation-next') {
  }
  elseif ($clicked_id == 'search-advanced-search-controls-manage-new-submit-save') {
    if (empty($form_state['advanced_search-chosen_targets'])) {
      form_set_error('advanced[search][controls][manage][existing][save]', "There are no search targets available for performing the requested action.");
    }
  }
  elseif ($clicked_id == 'search-advanced-search-controls-manage-existing-submit-load') {
    if (empty($form_state['values']['advanced']['search']['controls']['manage']['existing']['select'])) {
      form_set_error('advanced[search][controls][manage][existing][select]', "There are no search target groupings available for performing the requested action.");
    }
  }
  elseif ($clicked_id == 'search-advanced-search-controls-manage-existing-submit-update') {
    if (empty($form_state['values']['advanced']['search']['controls']['manage']['existing']['select'])) {
      form_set_error('advanced[search][controls][manage][existing][update]', "There are no search target groupings available for performing the requested action.");
    }
    elseif (empty($form_state['advanced_search-chosen_targets'])) {
      form_set_error('advanced[search][controls][manage][existing][update]', "There are no search targets available for performing the requested action.");
    }
  }
  elseif ($clicked_id == 'search-advanced-search-controls-manage-existing-submit-delete') {
    if (empty($form_state['values']['advanced']['search']['controls']['manage']['existing']['select'])) {
      form_set_error('advanced[search][controls][manage][existing][select]', "There are no search target groupings available for performing the requested action.");
    }
  }
  else {
    $invalid = TRUE;
    $clicked_id_parts = explode('-', $clicked_id);

    if (count($clicked_id_parts) > 2) {
      $clicked_id_parts = explode('-', $clicked_id);

      $row_names = array();
      if (is_array($clicked_id_parts) && !empty($clicked_id_parts)) {
        while (count($clicked_id_parts) > 6) {
          $row_names[] = array_pop($clicked_id_parts);
        }
      }

      $clicked_sub_id = implode('-', $clicked_id_parts);

      if ($clicked_sub_id == 'search-advanced-search-controls-possible_targets-add') {
        $invalid = FALSE;
      }
      elseif ($clicked_sub_id == 'search-advanced-search-controls-chosen_targets-remove') {
        $invalid = FALSE;
      }
      elseif ($clicked_sub_id == 'search-advanced-search-controls-grouping-add_and') {
        $invalid = FALSE;
      }
      elseif ($clicked_sub_id == 'search-advanced-search-controls-grouping-add_or') {
        $invalid = FALSE;
      }
    }

    if ($invalid) {
      form_set_error('', "Internal Error. An unknown submit process has been detected. Please contact the appropriate support staff.");
    }
  }
}

/**
 * Recursively walk the values so that they can be individually validated.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 * @param array|null $state_location
 *   Location in the form state array where the target might store its default
 *   values. If NULL, then there are no current default values.
 *
 * @param array $path_array
 *   An array containing the form_state path array structure.
 *   This will be converted to a path string and sent to form_set_error() as
 *   necessary.
 * @param int $recurse_count
 *   An integer used to detect and limit the recursion depth.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_request_search_0_form_validate_targets($form, &$form_state, $state_location, $targets, $path_array, $recurse_count) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return FALSE;
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');
    return FALSE;
  }

  if (!is_array($state_location)) {
    cf_error::invalid_array('state_location');
    return FALSE;
  }

  if (!is_array($targets)) {
    cf_error::invalid_array('targets');
    return FALSE;
  }

  if (!is_array($path_array)) {
    cf_error::invalid_array('path_array');
    return FALSE;
  }

  if (!cf_is_integer($recurse_count)) {
    cf_error::invalid_integer('recurse_count');
    return FALSE;
  }

  if ($recurse_count >= MFCS_MAX_SEARCH_RECURSION) {
    return FALSE;
  }

  foreach ($targets as $target_name => $target_values) {
    if (!is_array($target_values)) continue;

    $target_name_parts = explode('_', $target_name, 2);

    if (count($target_name_parts) == 2 && ($target_name_parts[0] == 'and' || $target_name_parts[0] == 'or')) {
      $state_location_next = NULL;
      if (isset($state_location[$target_name]['targets'])) {
        $state_location_next = $state_location[$target_name]['targets'];
      }

      $path_array_next = $path_array;
      $path_array_next[] = $target_name;
      $path_array_next[] = 'targets';

      mfcs_request_search_0_form_validate_targets($form, $form_state, $state_location_next, $target_values, $path_array_next, $recurse_count + 1);

      unset($state_location_next);
      unset($path_array_next);
    }
    else {
      foreach ($target_values as $target_delta => $target_human_name) {
        mfcs_request_search_0_form_validate_target($form, $form_state, $state_location, $target_name, $target_delta, $path_array);
      }
    }
  }
}

/**
 * Validate the submitted target values.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 * @param array|null $state_location
 *   Location in the form state array where the target might store its default
 *   values. If NULL, then there are no current default values.
 * @param string $target_name
 *   Name of the field to process and convert into a search field.
 * @param int $target_delta
 *   Delta of the field to process and convert into a search field.
 * @param array $path_array
 *   An array containing the form_state path array structure.
 *   This will be converted to a path string and sent to form_set_error() as
 *   necessary.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 *
 * @see: form_set_error()
 */
function mfcs_request_search_0_form_validate_target($form, &$form_state, &$state_location, $target_name, $target_delta, $path_array) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return FALSE;
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');
    return FALSE;
  }

  if (!is_array($state_location)) {
    cf_error::invalid_array('state_location');
    return FALSE;
  }

  if (!is_string($target_name)) {
    cf_error::invalid_string('target_name');
    return FALSE;
  }

  if (!cf_is_integer($target_delta)) {
    cf_error::invalid_integer('target_delta');
    return FALSE;
  }

  if (!is_array($path_array)) {
    cf_error::invalid_array('path_array');
    return FALSE;
  }

  $target_name_parts = explode('-', $target_name);
  if (count($target_name_parts) != 2) {
    return FALSE;
  }

  if (empty($target_name_parts[0]) || empty($target_name_parts[1])) {
    return FALSE;
  }

  $path_array[] = $target_name;
  $path_array[] = $target_delta;
  $path_array[] = 'search';

  $table_name = $target_name_parts[0];
  $field_name = $target_name_parts[1];

  $structure = array();
  if (isset($form['form']['structure']['#value']) && is_array($form['form']['structure']['#value'])) {
    $structure = &$form['form']['structure']['#value'];
  }

  $presentation = array();
  if (isset($form['form']['presentation']['#value']) && is_array($form['form']['presentation']['#value'])) {
    $presentation = &$form['form']['presentation']['#value'];
  }

  if (!isset($structure[$table_name][$field_name])) {
    return FALSE;
  }

  $field = $structure[$table_name][$field_name];

  $field_presentation = array();
  if (isset($presentation[$table_name][$field_name])) {
    $field_presentation = $presentation[$table_name][$field_name];
  }

  $state = $state_location[$target_name][$target_delta];

  if ($field['type'] == 'number') {
    if (!empty($state['search']) && !cf_is_integer($state['search'])) {
      form_set_error(implode('][', $path_array), t('@value is not a valid integer.', array('@value' => $state['search'])));
    }
  }
  elseif ($field['type'] == 'date') {
    if ($state['date_mode'] == 'd') {
      if (!cf_is_integer($state['search'])) {
        $search_date = strtolower($state['search']);
        $search_day = FALSE;

        if ($search_date == 'today' || $search_date == 'yesterday' || $search_date == 'tomorrow') {
          $search_day = strtotime($search_date);
        }

        if ($search_day === FALSE) {
          form_set_error(implode('][', $path_array), t('@value is not a valid day.', array('@value' => $state['search'])));
        }
        else {
          $state['search'] = date('d', $search_day);
        }

        unset($search_date);
        unset($search_day);
      }
      elseif ($state['search'] < 1 || $state['search'] > 31) {
        form_set_error(implode('][', $path_array), t('@value is not a valid day.', array('@value' => $state['search'])));
      }
    }
    elseif ($state['date_mode'] == 'm') {
      if (cf_is_integer($state['search'])) {
        if ($state['search'] < 1 || $state['search'] > 12) {
          form_set_error(implode('][', $path_array), t('@value is not a valid month.', array('@value' => $state['search'])));
        }
      }
      else {
        $stamp = strtotime($state['search']);

        if ($stamp === FALSE) {
          form_set_error(implode('][', $path_array), t('@value is not a valid month.', array('@value' => $state['search'])));
        }
        else {
          $month_full = strtolower(date('F', $stamp));
          $month_short = strtolower(date('M', $stamp));
          $month_value = strtolower($state['search']);

          if ($month_value != $month_full && $month_value != $month_short) {
            form_set_error(implode('][', $path_array), t('@value is not a valid month.', array('@value' => $state['search'])));
          }
        }
      }
    }
    elseif ($state['date_mode'] == 'y') {
      if (!cf_is_integer($state['search'])) {
        form_set_error(implode('][', $path_array), t('@value is not a valid year.', array('@value' => $state['search'])));
      }
    }
    elseif ($state['date_mode'] == 'y/m/d') {
      $matches = array();
      $matched = preg_match('@^\s*(\d\d\d\d)\s*/\s*(\d+)\s*/\s*(\d+)\s*$@i', $state['search'], $matches);

      if ($matched) {
        $date_value_stamp = strtotime($matches[1] . '/' . $matches[2] . '/' . $matches[3]);
        if ($date_value_stamp === FALSE) {
          form_set_error(implode('][', $path_array), t('@value is not a valid YYYY/MM/DD date.', array('@value' => $state['search'])));
        }
      }
      else {
        form_set_error(implode('][', $path_array), t('@value is not a valid YYYY/MM/DD date.', array('@value' => $state['search'])));
      }
    }
    elseif ($state['date_mode'] == 'y/m') {
      $matches = array();
      $matched = preg_match('@^\s*(\d\d\d\d)\s*/\s*(\d+)\s*(/\s*\d+)*\s*$@i', $state['search'], $matches);

      if ($matched) {
        $date_value_stamp = strtotime($matches[1] . '/' . $matches[2] . '/1');
        if ($date_value_stamp === FALSE) {
          form_set_error(implode('][', $path_array), t('@value is not a valid YYYY/MM date.', array('@value' => $state['search'])));
        }
      }
      else {
        form_set_error(implode('][', $path_array), t('@value is not a valid YYYY/MM date.', array('@value' => $state['search'])));
      }
    }
  }

  return TRUE;
}

/**
 * Submit for mfcs_request_search_0_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 *
 * @see: mfcs_request_search_0_form()
 */
function mfcs_request_search_0_form_submit($form, &$form_state) {
  $user = cf_current_user();

  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  if ($clicked_id == 'search-simple-submit-search') {
    $search = $form_state['values']['simple']['text'];
    $sensitive = (bool) $form_state['values']['simple']['sensitive'];

    $per_page = $form['form']['per_page']['#value'];
    $start = $form['form']['simple_search-page_number']['#value'] * $per_page;

    $form_state['simple_search-results'] = mfcs_request_load_listing($search, $sensitive, $start, $per_page);
    $form_state['simple_search-search'] = $search;
    $form_state['simple_search-sensitive'] = $sensitive;
  }
  elseif ($clicked_id == 'search-simple-submit-clear') {
    unset($form_state['simple_search-results']);
    unset($form_state['simple_search-search']);
    unset($form_state['values']['simple']['text']);
    unset($form_state['input']['simple']['text']);
  }
  elseif ($clicked_id == 'search-simple-results-navigation-previous') {
    $previous_page = $form['form']['simple_search-page_number']['#value'] - 1;
    $search = $form_state['simple_search-search'];
    $sensitive = (bool) $form_state['simple_search-sensitive'];

    $per_page = $form['form']['per_page']['#value'];
    $start = $previous_page * $per_page;

    $form_state['simple_search-results'] = mfcs_request_load_listing($search, $sensitive, $start, $per_page);
    $form_state['simple_search-page_number'] = $previous_page;
    $form_state['simple_search-search'] = $search;
    $form_state['simple_search-sensitive'] = $sensitive;
  }
  elseif ($clicked_id == 'search-simple-results-navigation-next') {
    $next_page = $form['form']['simple_search-page_number']['#value'] + 1;
    $search = $form_state['simple_search-search'];
    $sensitive = (bool) $form_state['simple_search-sensitive'];

    $per_page = $form['form']['per_page']['#value'];
    $start = $next_page * $per_page;

    $form_state['simple_search-results'] = mfcs_request_load_listing($search, $sensitive, $start, $per_page);
    $form_state['simple_search-page_number'] = $next_page;
    $form_state['simple_search-search'] = $search;
    $form_state['simple_search-sensitive'] = $sensitive;
  }
  elseif ($clicked_id == 'search-advanced-results-navigation-previous') {
    $previous_page = $form['form']['advanced_search-page_number']['#value'] - 1;
    $search = $form_state['advanced_search-search'];
    $sensitive = (bool) $form_state['advanced_search-sensitive'];

    $per_page = $form['form']['per_page']['#value'];
    $start = $previous_page * $per_page;

    $form_state['advanced_search-results'] = mfcs_request_load_listing($search, $sensitive, $start, $per_page);
    $form_state['advanced_search-page_number'] = $previous_page;
    $form_state['advanced_search-search'] = $search;
    $form_state['advanced_search-sensitive'] = $sensitive;
  }
  elseif ($clicked_id == 'search-advanced-results-navigation-next') {
    $next_page = $form['form']['advanced_search-page_number']['#value'] + 1;
    $search = $form_state['advanced_search-search'];
    $sensitive = (bool) $form_state['advanced_search-sensitive'];

    $per_page = $form['form']['per_page']['#value'];
    $start = $next_page * $per_page;

    $form_state['advanced_search-results'] = mfcs_request_load_listing($search, $sensitive, $start, $per_page);
    $form_state['advanced_search-page_number'] = $next_page;
    $form_state['advanced_search-search'] = $search;
    $form_state['advanced_search-sensitive'] = $sensitive;
  }
  elseif ($clicked_id == 'search-advanced-submit-search') {
    $sensitive = (bool) $form_state['values']['advanced']['sensitive'];

    $per_page = $form['form']['per_page']['#value'];
    $start = $form['form']['advanced_search-page_number']['#value'] * $per_page;

    mfcs_include(MFCS_INCLUDE_TABLE);

    $structure = mfcs_table_structure();

    if (empty($form_state['values']['advanced']['targets'])) {
      $form_state['advanced_search-results'] = array();
      $form_state['advanced_search-search'] = array();
      $form_state['advanced_search-sensitive'] = $sensitive;
    }
    else {
      $search = mfcs_request_search_0_form_build_search_targets($structure, $form_state['values']['advanced']['targets'], 0);

      if (is_array($search)) {
        $form_state['advanced_search-results'] = mfcs_request_load_listing($search, $sensitive, $start, $per_page);
        $form_state['advanced_search-search'] = $search;
        $form_state['advanced_search-sensitive'] = $sensitive;
      }
    }
  }
  elseif ($clicked_id == 'search-advanced-submit-clear') {
    unset($form_state['advanced_search-results']);
    unset($form_state['advanced_search-search']);
    unset($form_state['values']['advanced']);
    unset($form_state['input']['advanced']);
  }
  elseif ($clicked_id == 'search-advanced-submit-reset') {
    unset($form_state['advanced_search-results']);
    unset($form_state['advanced_search-search']);
    $form_state['advanced_search-clear'] = TRUE;

    unset($form_state['values']['advanced']['targets']);
    unset($form_state['advanced_search-chosen_targets']);
  }
  elseif ($clicked_id == 'search-advanced-search-controls-manage-new-submit-save') {
    if (isset($form_state['values']['advanced']['search']['controls']['manage']['new']['human_name']) && isset($form_state['advanced_search-chosen_targets'])) {
      $human_name = $form_state['values']['advanced']['search']['controls']['manage']['new']['human_name'];

      $targets_data = array(
        'targets_values' => $form_state['values']['advanced']['targets'],
        'chosen_targets' => $form_state['advanced_search-chosen_targets'],
      );

      $transaction = db_transaction();
      try {
        $mfcs_search_settings = array(
          'user_id' => $user->uid,
          'human_name' => $human_name,
          'data' => serialize($targets_data),
        );
        unset($human_name);

        $query = db_insert('mfcs_search_settings');
        $query->fields($mfcs_search_settings);
        $query->execute();

        // enforce transaction execution
        unset($transaction);
      }
      catch (Exception $ex) {
        $transaction->rollback();
        cf_error::on_exception($ex);

        return FALSE;
      }

      // force reloading of the search target settings.
      $form['form']['existing_search_targets']['#value'] = array();
    }
  }
  elseif ($clicked_id == 'search-advanced-search-controls-manage-existing-submit-load') {
    if (isset($form_state['values']['advanced']['search']['controls']['manage']['existing']['select'])) {
      $id = $form_state['values']['advanced']['search']['controls']['manage']['existing']['select'];
      if (array_key_exists($id, $form['form']['existing_search_targets']['#value'])) {
        $targets_data = unserialize($form['form']['existing_search_targets']['#value'][$id]->data);

        $form_state['values']['advanced']['targets'] = $targets_data['targets_values'];
        $form_state['advanced_search-chosen_targets'] = $targets_data['chosen_targets'];
      }
    }
  }
  elseif ($clicked_id == 'search-advanced-search-controls-manage-existing-submit-update') {
    if (isset($form_state['values']['advanced']['search']['controls']['manage']['existing']['select']) && isset($form_state['advanced_search-chosen_targets'])) {
      $id = $form_state['values']['advanced']['search']['controls']['manage']['existing']['select'];

      $targets_data = array(
        'targets_values' => $form_state['values']['advanced']['targets'],
        'chosen_targets' => $form_state['advanced_search-chosen_targets'],
      );

      $transaction = db_transaction();
      try {
        $query = db_update('mfcs_search_settings');
        $query->fields(array('data' => serialize($targets_data)));
        $query->condition('id', $id);

        // for security reasons require that the current user be the only user allowed to update their own settings.
        $query->condition('user_id', $user->uid);

        $query->execute();

        // enforce transaction execution
        unset($transaction);
      }
      catch (Exception $ex) {
        $transaction->rollback();
        cf_error::on_exception($ex);

        return FALSE;
      }
    }
  }
  elseif ($clicked_id == 'search-advanced-search-controls-manage-existing-submit-delete') {
    if (isset($form_state['values']['advanced']['search']['controls']['manage']['existing']['select'])) {
      $id = $form_state['values']['advanced']['search']['controls']['manage']['existing']['select'];

      $transaction = db_transaction();
      try {
        $query = db_delete('mfcs_search_settings');
        $query->condition('id', $id);

        // for security reasons require that the current user be the only user allowed to delete their own settings.
        $query->condition('user_id', $user->uid);

        $query->execute();

        // enforce transaction execution
        unset($transaction);
      }
      catch (Exception $ex) {
        $transaction->rollback();
        cf_error::on_exception($ex);

        return FALSE;
      }

      // force reloading of the search target settings.
      $form['form']['existing_search_targets']['#value'] = array();
    }
  }
  else {
    $clicked_id_parts = explode('-', $clicked_id);

    $row_names = array();
    if (is_array($clicked_id_parts) && !empty($clicked_id_parts)) {
      while (count($clicked_id_parts) > 6) {
        $row_names[] = array_pop($clicked_id_parts);
      }
    }

    $clicked_sub_id = implode('-', $clicked_id_parts);

    if ($clicked_sub_id == 'search-advanced-search-controls-possible_targets-add') {
      if (!isset($form_state['advanced_search-chosen_targets']) || !is_array($form_state['advanced_search-chosen_targets'])) {
        $form_state['advanced_search-chosen_targets'] = array();
      }

      $row_location = &$form_state['advanced_search-chosen_targets'];
      $state_location = &$form_state['values']['advanced'];

      if (!empty($row_names)) {
        $recurse_count = 0;
        while (count($row_names) > 0 && $recurse_count < MFCS_MAX_SEARCH_RECURSION) {
          $row_name = array_pop($row_names);
          $recurse_count++;

          if (isset($row_location[$row_name]) && isset($state_location['targets'][$row_name])) {
            $row_location = &$row_location[$row_name];
            $state_location = &$state_location['targets'][$row_name];
          }
          else {
            unset($row_location);
            break;
          }
        }
      }

      if (isset($row_location) && isset($state_location['search']['controls']['possible_targets']['select'])) {
        $target = $state_location['search']['controls']['possible_targets']['select'];

        $target_count = 0;
        if (isset($row_location[$target])) {
          $target_count = count($row_location[$target]);
        }
        else {
          $row_location[$target] = array();
        }

        $row_location[$target][$target_count] = $form['form']['advanced_search-possible_targets']['#value'][$target];
      }
    }
    elseif ($clicked_sub_id == 'search-advanced-search-controls-chosen_targets-remove') {
      if (!isset($form_state['advanced_search-chosen_targets']) || !is_array($form_state['advanced_search-chosen_targets'])) {
        $form_state['advanced_search-chosen_targets'] = array();
      }

      $row_location = &$form_state['advanced_search-chosen_targets'];
      $state_location = &$form_state['values']['advanced'];

      if (!empty($row_names)) {
        $recurse_count = 0;
        while (count($row_names) > 0 && $recurse_count < MFCS_MAX_SEARCH_RECURSION) {
          $row_name = array_pop($row_names);
          $recurse_count++;

          if (isset($row_location[$row_name]) && isset($state_location['targets'][$row_name])) {
            $row_location = &$row_location[$row_name];
            $state_location = &$state_location['targets'][$row_name];
          }
          else {
            unset($row_location);
            break;
          }
        }
      }

      if (isset($row_location) && isset($state_location['search']['controls']['chosen_targets']['select'])) {
        $target = $state_location['search']['controls']['chosen_targets']['select'];
        $target_parts = explode('-', $target);

        if (count($target_parts) == 3 && !empty($target_parts[0]) && !empty($target_parts[1]) && is_numeric($target_parts[2])) {
          $chosen_target_field_name = $target_parts[0] . '-' . $target_parts[1];

          if (isset($form['form']['advanced_search-possible_targets']['#value'][$chosen_target_field_name][$target_parts[2]])) {
            unset($row_location[$chosen_target_field_name][$target_parts[2]]);
          }

          if (empty($row_location[$chosen_target_field_name])) {
            unset($row_location[$chosen_target_field_name]);
          }
        }
        elseif (count($target_parts) == 1) {
          $target_parts = explode('_', $target);

          if (count($target_parts) == 2 && ($target_parts[0] == 'and' || $target_parts[0] == 'or')) {
            unset($row_location[$target]);
          }
        }
      }
    }
    elseif ($clicked_sub_id == 'search-advanced-search-controls-grouping-add_and') {
      if (!isset($form_state['advanced_search-chosen_targets']) || !is_array($form_state['advanced_search-chosen_targets'])) {
        $form_state['advanced_search-chosen_targets'] = array();
      }

      if (isset($form['form']['advanced_search-row_count']['#value'])) {
        $row_count = $form['form']['advanced_search-row_count']['#value'];
        $target = 'and_' . $row_count;

        $row_location = &$form_state['advanced_search-chosen_targets'];

        if (!empty($row_names)) {
          $recurse_count = 0;
          while (count($row_names) > 0 && $recurse_count < MFCS_MAX_SEARCH_RECURSION) {
            $row_name = array_pop($row_names);
            $recurse_count++;

            if (isset($row_location[$row_name])) {
              $row_location = &$row_location[$row_name];
            }
            else {
              unset($row_location);
              break;
            }
          }
        }

        if (isset($row_location)) {
          $row_location[$target] = array();
          $form_state['advanced_search-row_count'] = $row_count + 1;
        }
      }
    }
    elseif ($clicked_sub_id == 'search-advanced-search-controls-grouping-add_or') {
      if (!isset($form_state['advanced_search-chosen_targets']) || !is_array($form_state['advanced_search-chosen_targets'])) {
        $form_state['advanced_search-chosen_targets'] = array();
      }

      if (isset($form['form']['advanced_search-row_count']['#value'])) {
        $row_count = $form['form']['advanced_search-row_count']['#value'];
        $target = 'or_' . $row_count;

        $row_location = &$form_state['advanced_search-chosen_targets'];

        if (!empty($row_names)) {
          $recurse_count = 0;
          while (count($row_names) > 0 && $recurse_count < MFCS_MAX_SEARCH_RECURSION) {
            $row_name = array_pop($row_names);
            $recurse_count++;

            if (isset($row_location[$row_name])) {
              $row_location = &$row_location[$row_name];
            }
            else {
              unset($row_location);
              break;
            }
          }
        }

        if (isset($row_location)) {
          $row_location[$target] = array();
          $form_state['advanced_search-row_count'] = $row_count + 1;
        }
      }
    }
  }

  // return to the form with the search results.
  $form_state['rebuild'] = TRUE;
  $form_state['redirect'] = FALSE;
  $form_state['submitted'] = TRUE;
}

/**
 * Recursively construct the targets and the group options.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 * @param array $location
 *   Location in the form array where to build the target.
 * @param array|null $state_location
 *   Location in the form state array where the target might store its default
 *   values. If NULL, then there are no current default values.
 * @param array $chosen_targets
 *   An array of targets already chosen prior to this function call.
 * @param int $recurse_count
 *   An integer used to detect and limit the recursion depth.
 * @param string $id_extra
 *   A string to insert to the id tag of fields creating by this function.
 * @param array $chosen_options
 *   An array of chosen targets used for presenting a select list.
 * @param array $logic_options
 *   An array of logic operations (AND, OR) used for presenting a select list.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_request_search_0_form_build_targets_and_groups($form, &$form_state, &$location, $state_location, $chosen_targets, $recurse_count, $id_extra, &$chosen_options, &$logic_options) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return FALSE;
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');
    return FALSE;
  }

  if (!is_array($location)) {
    cf_error::invalid_array('location');
    return FALSE;
  }

  if (!is_array($state_location) && !is_null($state_location)) {
    cf_error::invalid_array('state_location');
    return FALSE;
  }

  if (!is_array($chosen_targets)) {
    cf_error::invalid_array('chosen_targets');
    return FALSE;
  }

  if (!cf_is_integer($recurse_count)) {
    cf_error::invalid_integer('recurse_count');
    return FALSE;
  }

  if (!is_string($id_extra)) {
    cf_error::invalid_string('id_extra');
    return FALSE;
  }

  if (!is_array($chosen_options)) {
    cf_error::invalid_array('chosen_options');
    return FALSE;
  }

  if (!is_array($logic_options)) {
    cf_error::invalid_array('logic_options');
    return FALSE;
  }

  if ($recurse_count > MFCS_MAX_SEARCH_RECURSION) {
    return FALSE;
  }

  if (empty($chosen_targets)) {
    return TRUE;
  }

  $chosen_target_options = array();
  foreach ($chosen_targets as $chosen_name => $chosen_values) {
    if (!is_array($chosen_values)) continue;

    $chosen_name_parts = explode('_', $chosen_name, 2);
    if (count($chosen_name_parts) == 2 && ($chosen_name_parts[0] == 'and' || $chosen_name_parts[0] == 'or')) {
      if ($recurse_count == MFCS_MAX_SEARCH_RECURSION) {
        continue;
      }

      if (!isset($location['targets'][$chosen_name])) {
        $title = "And #" . $chosen_name_parts[1];
        $description = "All targets within this fieldset are matched using the " . '<em class="uppercase">And</em>' . " operation.";
        if ($chosen_name_parts[0] == 'or') {
          $title = 'Or #' . $chosen_name_parts[1];
          $description = "All targets within this fieldset are matched using the " . '<em class="uppercase">Or</em>' . " operation.";
        }

        $location['targets'][$chosen_name] = array(
          '#id' => 'search-advanced-' . $chosen_name . $id_extra,
          '#type' => 'fieldset',
          '#title' => $title,
          '#description' => $description,
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
          '#attributes' => array(
            'class' => array(
              'no-print',
              'form-item',
              'search-advanced-targets',
              'search-advanced-' . $chosen_name_parts[0],
            ),
          ),
          '#weight' => 1,
          '#tree' => TRUE,
        );
      }

      if (empty($id_extra)) {
        $new_id_extra = '-' . $chosen_name;
      }
      else {
        $new_id_extra = $id_extra . '-' . $chosen_name;
      }

      if (!isset($chosen_options[$chosen_name])) {
        $chosen_options[$chosen_name] = array();
      }

      if (!isset($logic_options[$chosen_name])) {
        $logic_options[$chosen_name] = array();
      }

      $new_recurse_count = $recurse_count + 1;

      $state_location_next = NULL;
      if (isset($state_location[$chosen_name]['targets'])) {
        $state_location_next = $state_location[$chosen_name]['targets'];
      }

      mfcs_request_search_0_form_build_targets_and_groups($form, $form_state, $location['targets'][$chosen_name], $state_location_next, $chosen_values, $new_recurse_count, $new_id_extra, $chosen_options[$chosen_name], $logic_options[$chosen_name]);
      mfcs_request_search_0_form_build_grouping($form, $form_state, $location['targets'][$chosen_name], $new_recurse_count, $new_id_extra, $chosen_options[$chosen_name], $logic_options[$chosen_name]);
    }
    else {
      if (empty($chosen_values)) continue;

      foreach ($chosen_values as $delta => $chosen_human_name) {
        if (!isset($chosen_options['options'][$chosen_name])) {
          $chosen_options['options'][$chosen_name] = array();
        }

        $chosen_options['options'][$chosen_name][$chosen_name . '-' . $delta] = $chosen_human_name . ' #' . $delta;

        mfcs_request_search_0_form_build_target($form, $form_state, $location, $state_location, $chosen_name, $delta, $id_extra);
      }
    }
  }

  return TRUE;
}

/**
 * Builds the search target form field.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 * @param array $location
 *   Location in the form array where to build the target.
 * @param array|null $state_location
 *   Location in the form state array where the target might store its default
 *   values. If NULL, then there are no current default values.
 * @param string $target_name
 *   Name of the field to process and convert into a search field.
 * @param int $target_delta
 *   Delta of the field to process and convert into a search field.
 * @param string $id_extra
 *   A string to insert to the id tag of fields creating by this function.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 *
 * @see mfcs_table_structure()
 */
function mfcs_request_search_0_form_build_target(&$form, &$form_state, &$location, $state_location, $target_name, $target_delta, $id_extra) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return FALSE;
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');
    return FALSE;
  }

  if (!is_array($location)) {
    cf_error::invalid_array('location');
    return FALSE;
  }

  if (!is_array($state_location) && !is_null($state_location)) {
    cf_error::invalid_array('state_location');
    return FALSE;
  }

  if (!is_string($target_name)) {
    cf_error::invalid_string('target_name');
    return FALSE;
  }

  if (!cf_is_integer($target_delta)) {
    cf_error::invalid_integer('target_delta');
    return FALSE;
  }

  if (!is_string($id_extra)) {
    cf_error::invalid_string('id_extra');
    return FALSE;
  }

  $target_name_parts = explode('-', $target_name);
  if (count($target_name_parts) != 2) {
    return FALSE;
  }

  if (empty($target_name_parts[0]) || empty($target_name_parts[1])) {
    return FALSE;
  }

  $table_name = $target_name_parts[0];
  $field_name = $target_name_parts[1];

  $structure = array();
  if (isset($form['form']['structure']['#value']) && is_array($form['form']['structure']['#value'])) {
    $structure = &$form['form']['structure']['#value'];
  }

  $presentation = array();
  if (isset($form['form']['presentation']['#value']) && is_array($form['form']['presentation']['#value'])) {
    $presentation = &$form['form']['presentation']['#value'];
  }

  if (!isset($structure[$table_name][$field_name])) {
    return FALSE;
  }

  $field = $structure[$table_name][$field_name];


  $field_presentation = array();
  if (isset($presentation[$table_name][$field_name])) {
    $field_presentation = $presentation[$table_name][$field_name];
  }

  if (!isset($location['targets'][$target_name])) {
    $location['targets'][$target_name] = array(
      '#id' => 'search-advanced-targets' . $id_extra . '-target-' . $target_name,
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item',
          'search-advanced-target',
          'search-advanced-target-' . $target_name,
        ),
      ),
    );
  }

  $location['targets'][$target_name][$target_delta] = array(
    '#id' => 'search-advanced-targets' . $id_extra . '-targets-' . $target_name . '-' . $target_delta . '-wrapper',
    '#type' => 'fieldset',
    '#title' => $field['name'] . ' #' . $target_delta,
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'search-advanced-target-wrapper',
        'search-advanced-target-' . $target_name . '-wrapper',
        'search-advanced-target-' . $target_name . '-' . $target_delta . '-wrapper',
      ),
    ),
  );

  if ($field['type'] == 'number' || $field['type'] == 'date') {
    $operators = array(
      MFCS_OPERATOR_LESS_THAN => "Less Than",
      MFCS_OPERATOR_LESS_THAN_EQUAL => "Less Than or Equal To",
      MFCS_OPERATOR_EQUAL => "Equal To",
      MFCS_OPERATOR_GREATER_THAN_EQUAL => "Greater Than or Equal To",
      MFCS_OPERATOR_GREATER_THAN => "Greater Than",
      MFCS_OPERATOR_NOT_EQUAL => "Not Equal To",
    );

    $default_value = MFCS_OPERATOR_EQUAL;
  }
  elseif ($field['type'] == 'text') {
    $operators = array(
      MFCS_OPERATOR_LIKE => "Similar To",
      MFCS_OPERATOR_NOT_LIKE => "Not Similar To",
      MFCS_OPERATOR_EQUAL => "Equal To",
    );

    $default_value = MFCS_OPERATOR_LIKE;
  }
  else {
    $operators = array(
      MFCS_OPERATOR_EQUAL => "Equal To",
      MFCS_OPERATOR_NOT_EQUAL => "Not Equal To",
    );

    $default_value = MFCS_OPERATOR_EQUAL;
  }

  if (isset($field['allows_null']) && $field['allows_null']) {
    $operators[MFCS_OPERATOR_IS_NULL] = "Not Defined / Not Used";
    $operators[MFCS_OPERATOR_IS_NOT_NULL] = "Defined / Used";
  }

  if (isset($state_location[$target_name][$target_delta]['operator'])) {
    $default_value = $state_location[$target_name][$target_delta]['operator'];
  }
  $location['targets'][$target_name][$target_delta]['operator'] = array(
    '#id' => 'search-advanced-targets' . $id_extra . '-targets-' . $target_name . '-' . $target_delta . '-operator',
    '#title' => "Operator",
    '#type' => 'select',
    '#default_value' => $default_value,
    '#options' => $operators,
    '#required' => TRUE,
    '#attributes' => array(
      'class' => array(
        'search-advanced-target-operator',
        'search-advanced-target-' . $target_name . '-operator',
        'search-advanced-target-' . $target_name . '-' . $target_delta . '-operator',
      ),
    ),
  );

  $default_value = NULL;

  if (isset($state_location[$target_name][$target_delta]['search'])) {
    if (!isset($form_state['advanced_search-clear']) || !$form_state['advanced_search-clear']) {
      $default_value = $state_location[$target_name][$target_delta]['search'];
    }
  }
  $location['targets'][$target_name][$target_delta]['search'] = array(
    '#id' => 'search-advanced-targets' . $id_extra . '-targets-' . $target_name . '-' . $target_delta . '-search',
    '#title' => "Value",
    '#default_value' => $default_value,
    '#required' => FALSE,
    '#attributes' => array(
      'class' => array(
        'search-advanced-target-search',
        'search-advanced-target-' . $target_name . '-search',
        'search-advanced-target-' . $target_name . '-' . $target_delta . '-search',
      ),
    ),
  );

  $location['targets'][$target_name][$target_delta]['search']['#states'] = array(
    'visible' => array(
      'or' => array(
      ),
    ),
  );

  $location['targets'][$target_name][$target_delta]['search']['#states'] = array(
    'invisible' => array(
      ':input[name="advanced[targets][' . $target_name . '][' . $target_delta . '][operator]"]' => array(
        array('value' => MFCS_OPERATOR_IS_NULL),
        array('value' => MFCS_OPERATOR_IS_NOT_NULL),
      ),
    ),
  );

  if (!isset($form['form']['conditionally_required']['#value'][0])) {
    $form['form']['conditionally_required']['#value'][0] = array();
  }

  if (!isset($form['form']['conditionally_required']['#value'][0][$target_name])) {
    $form['form']['conditionally_required']['#value'][0][$target_name] = array();
  }

  $form['form']['conditionally_required']['#value'][0][$target_name][$target_delta] = &$location['targets'][$target_name][$target_delta]['search'];

  if (isset($form_state['values']['advanced']['targets'][$target_name][$target_delta]['search'])) {
    $location['targets'][$target_name][$target_delta]['search']['#default_value'] = $form_state['values']['advanced']['targets'][$target_name][$target_delta]['search'];
  }

  if ($field['type'] == 'text') {
    $location['targets'][$target_name][$target_delta]['search']['#type'] = 'textfield';
    $location['targets'][$target_name][$target_delta]['search']['#size'] = 32;
  }
  elseif ($field['type'] == 'bool') {
    $location['targets'][$target_name][$target_delta]['search']['#type'] = 'select';
    $location['targets'][$target_name][$target_delta]['search']['#options'] = array(1 => "Yes / True", 0 => "No / False");
  }
  elseif ($field['type'] == 'options') {
    $location['targets'][$target_name][$target_delta]['search']['#type'] = 'select';

    if ($table_name == 'top') {
      if ($field_name == 'status') {
        $location['targets'][$target_name][$target_delta]['search']['#options'] = mfcs_get_request_status_list_options('search');
      }
      elseif ($field_name == 'step') {
        $location['targets'][$target_name][$target_delta]['search']['#options'] = mfcs_get_request_steps_list_options('search');
      }
      elseif ($field_name == 'location') {
        $location['targets'][$target_name][$target_delta]['search']['#options'] = mfcs_get_request_location_list_options('search');
      }
      elseif ($field_name == 'building') {
        $location['targets'][$target_name][$target_delta]['search']['#options'] = mfcs_get_request_building_list_options(NULL, 'search');
      }
      elseif ($field_name == 'room') {
        $location['targets'][$target_name][$target_delta]['search']['#options'] = mfcs_get_request_room_list_options(NULL, 'search');
      }
    }
    elseif ($table_name == 'information') {
      if ($field_name == 'type') {
        $location['targets'][$target_name][$target_delta]['search']['#options'] = mfcs_get_request_type_list_options('search');
      }
    }
    elseif ($table_name == 'dates') {
      if ($field_name == 'category') {
        $location['targets'][$target_name][$target_delta]['search']['#options'] = mfcs_get_dates_category_list_options('search');
      }
    }
    elseif ($table_name == 'plans') {
      if ($field_name == 'audience') {
        $location['targets'][$target_name][$target_delta]['search']['#options'] = mfcs_get_audience_list_options('search');
      }
    }
    elseif ($table_name == 'request_coordinator') {
      if ($field_name == 'classification') {
        $location['targets'][$target_name][$target_delta]['search']['#options'] = mfcs_get_request_classification_list_options('search');
      }
    }
  }
  elseif ($field['type'] == 'checkboxes') {
    $location['targets'][$target_name][$target_delta]['option'] = array(
      '#id' => 'search-advanced-targets' . $id_extra . '-targets-' . $target_name . '-' . $target_delta . '-field',
      '#title' => "Option",
      '#type' => "select",
      '#default_value' => NULL,
      '#required' => TRUE,
      '#attributes' => array(
        'class' => array(
          'search-advanced-target-field',
          'search-advanced-target-' . $target_name . '-field',
          'search-advanced-target-' . $target_name . '-' . $target_delta . '-field',
        ),
      ),
    );

    $location['targets'][$target_name][$target_delta]['option']['#states'] = array(
      'invisible' => array(
        ':input[name="advanced[targets][' . $target_name . '][' . $target_delta . '][operator]"]' => array(
          array('value' => MFCS_OPERATOR_IS_NULL),
          array('value' => MFCS_OPERATOR_IS_NOT_NULL),
        ),
      ),
    );

    if (isset($form_state['values']['advanced']['targets'][$target_name][$target_delta]['option'])) {
      if (!isset($form_state['advanced_search-clear']) || !$form_state['advanced_search-clear']) {
        $location['targets'][$target_name][$target_delta]['option']['#default_value'] = $form_state['values']['advanced']['targets'][$target_name][$target_delta]['option'];
      }
    }

    $location['targets'][$target_name][$target_delta]['search']['#type'] = 'select';
    $location['targets'][$target_name][$target_delta]['search']['#options'] = array(0 => "Unchecked / Disabled", 1 => "Checked / Enabled");

    if (isset($form_state['values']['advanced']['targets'][$target_name][$target_delta]['search'])) {
      if (!isset($form_state['advanced_search-clear']) || !$form_state['advanced_search-clear']) {
        $location['targets'][$target_name][$target_delta]['search']['#default_value'] = $form_state['values']['advanced']['targets'][$target_name][$target_delta]['search'];
      }
    }

    if ($table_name == 'registration') {
      if ($field_name == 'revenue_generated') {
        $location['targets'][$target_name][$target_delta]['option']['#options'] = mfcs_get_revenue_generated_list_options('search');
      }
    }
    elseif ($table_name == 'presentation') {
      if ($field_name == 'technical_equipment_details') {
        $location['targets'][$target_name][$target_delta]['option']['#options'] = mfcs_get_technical_equipment_details_list_options('search');
      }
      elseif ($field_name == 'publicity_details') {
        $location['targets'][$target_name][$target_delta]['option']['#options'] = mfcs_get_publicity_details_list_options('search');
      }
      elseif ($field_name == 'designing_material') {
        $location['targets'][$target_name][$target_delta]['option']['#options'] = mfcs_get_designing_material_list_options('search');
      }
    }
  }
  elseif ($field['type'] == 'requirements') {
    unset($location['targets'][$target_name][$target_delta]['search']);
    unset($form['form']['conditionally_required']['#value'][0][$target_name][$target_delta]);

    if (isset($field['columns']) && in_array('quantity', $field['columns'])) {
      $location['targets'][$target_name][$target_delta]['quantity'] = array(
        '#id' => 'search-advanced-targets' . $id_extra . '-targets-' . $target_name . '-quantity',
        '#type' => 'textfield',
        '#size' => 9,
        '#title' => "Quantity",
        '#default_value' => NULL,
        '#required' => FALSE,
        '#attributes' => array(
          'class' => array(
            'search-advanced-target-search',
            'search-advanced-target-' . $target_name . '-quantity',
          ),
        ),
      );

      if (isset($field_presentation['type']) && is_array($field_presentation['type']) && array_key_exists('quantity', $field_presentation['type']) && $field_presentation['type']['quantity'] == 'requirement_personnel') {
        $location['targets'][$target_name][$target_delta]['quantity']['#title'] = "Personnel";
      }

      $location['targets'][$target_name][$target_delta]['quantity']['#states'] = array(
        'invisible' => array(
          ':input[name="advanced[targets][' . $target_name . '][' . $target_delta . '][operator]"]' => array(
            array('value' => MFCS_OPERATOR_IS_NULL),
            array('value' => MFCS_OPERATOR_IS_NOT_NULL),
          ),
        ),
      );

      $form['form']['conditionally_required']['#value'][0][$target_name][] = &$location['targets'][$target_name][$target_delta]['quantity'];
    }

    if (isset($field['columns']) && in_array('hours', $field['columns'])) {
      $location['targets'][$target_name][$target_delta]['hours'] = array(
        '#id' => 'search-advanced-targets' . $id_extra . '-targets-' . $target_name . '-hours',
        '#type' => 'textfield',
        '#size' => 9,
        '#title' => "Hours",
        '#default_value' => NULL,
        '#required' => FALSE,
        '#attributes' => array(
          'class' => array(
            'search-advanced-target-search',
            'search-advanced-target-' . $target_name . '-hours',
          ),
        ),
      );

      $location['targets'][$target_name][$target_delta]['hours']['#states'] = array(
        'invisible' => array(
          ':input[name="advanced[targets][' . $target_name . '][' . $target_delta . '][operator]"]' => array(
            array('value' => MFCS_OPERATOR_IS_NULL),
            array('value' => MFCS_OPERATOR_IS_NOT_NULL),
          ),
        ),
      );

      $form['form']['conditionally_required']['#value'][0][$target_name][] = &$location['targets'][$target_name][$target_delta]['hours'];
    }

    if (isset($field['columns']) && in_array('days', $field['columns'])) {
    $location['targets'][$target_name][$target_delta]['days'] = array(
        '#id' => 'search-advanced-targets' . $id_extra . '-targets-' . $target_name . '-days',
        '#type' => 'textfield',
        '#size' => 9,
        '#title' => "Days",
        '#default_value' => NULL,
        '#required' => FALSE,
        '#attributes' => array(
          'class' => array(
            'search-advanced-target-search',
            'search-advanced-target-' . $target_name . '-days',
          ),
        ),
      );

      $location['targets'][$target_name][$target_delta]['days']['#states'] = array(
        'invisible' => array(
          ':input[name="advanced[targets][' . $target_name . '][' . $target_delta . '][operator]"]' => array(
            array('value' => MFCS_OPERATOR_IS_NULL),
            array('value' => MFCS_OPERATOR_IS_NOT_NULL),
          ),
        ),
      );

      $form['form']['conditionally_required']['#value'][0][$target_name][] = &$location['targets'][$target_name][$target_delta]['days'];
    }

    if (isset($field['columns']) && in_array('amount', $field['columns'])) {
      $location['targets'][$target_name][$target_delta]['amount'] = array(
        '#id' => 'search-advanced-targets' . $id_extra . '-targets-' . $target_name . '-amount',
        '#type' => 'textfield',
        '#size' => 12,
        '#title' => "Amount",
        '#default_value' => NULL,
        '#required' => FALSE,
        '#attributes' => array(
          'class' => array(
            'search-advanced-target-search',
            'search-advanced-target-' . $target_name . '-amount',
          ),
        ),
      );

      $location['targets'][$target_name][$target_delta]['amount']['#states'] = array(
        'invisible' => array(
          ':input[name="advanced[targets][' . $target_name . '][' . $target_delta . '][operator]"]' => array(
            array('value' => MFCS_OPERATOR_IS_NULL),
            array('value' => MFCS_OPERATOR_IS_NOT_NULL),
          ),
        ),
      );

      $form['form']['conditionally_required']['#value'][0][$target_name][] = &$location['targets'][$target_name][$target_delta]['amount'];
    }
  }
  elseif ($field['type'] == 'venue_coordinator') {
    $venue_coordinator_options = mfcs_request_get_available_venue_coordinators();
    if (!is_array($venue_coordinator_options)) {
      $venue_coordinator_options = array();
    }

    $location['targets'][$target_name][$target_delta]['search']['#type'] = 'select';
    $location['targets'][$target_name][$target_delta]['search']['#options'] = $venue_coordinator_options;
  }
  elseif ($field['type'] == 'date') {
    $location['targets'][$target_name][$target_delta]['search']['#type'] = 'textfield';
    $location['targets'][$target_name][$target_delta]['search']['#size'] = 20;

    $default_value = 'y/m/d';
    if (isset($state_location[$target_name][$target_delta]['date_mode'])) {
      if (isset($form_state['advanced_search-clear']) && $form_state['advanced_search-clear']) {
        $default_value = $state_location[$target_name][$target_delta]['date_mode'];
      }
    }
    $location['targets'][$target_name][$target_delta]['date_mode'] = array(
      '#id' => 'search-advanced-targets' . $id_extra . '-targets-' . $target_name . '-' . $target_delta . '-date_mode',
      '#title' => "Date Mode",
      '#type' => 'select',
      '#default_value' => $default_value,
      '#options' => array('y' => "Year", 'y/m' => "Year / Month", 'y/m/d' => "Year / Month / Day", 'm' => "Month", 'd' => "Day"),
      '#required' => TRUE,
      '#attributes' => array(
        'class' => array(
          'search-advanced-target-date_mode',
          'search-advanced-target-' . $target_name . '-date_mode',
          'search-advanced-target-' . $target_name . '-' . $target_delta . '-date_mode',
        ),
      ),
    );

    $form['form']['conditionally_required']['#value'][0][$target_name][] = &$location['targets'][$target_name][$target_delta]['date_mode'];
  }
  elseif ($field['type'] == 'time') {
    $location['targets'][$target_name][$target_delta]['search']['#type'] = 'textfield';
    $location['targets'][$target_name][$target_delta]['search']['#size'] = 8;
    $location['targets'][$target_name][$target_delta]['search']['#maxlength'] = 8;
    $location['targets'][$target_name][$target_delta]['search']['#expected_format'] = array('date' => "h:ia");
    $location['targets'][$target_name][$target_delta]['search']['#description'] = "E.g, 12:31pm";
  }
  elseif ($field['type'] == 'number') {
    $location['targets'][$target_name][$target_delta]['search']['#type'] = 'numberfield';
  }

  return TRUE;
}

/**
 * Recursively builds the list search array for the search targets.
 *
 * @param array $structure
 *   Structure array as returned by mfcs_table_structure().
 * @param string $targets
 *   An array of all target field values to convert into a search list array.
 * @param int $recurse_count
 *   An integer used to detect and limit the recursion depth.
 *
 * @return array|bool
 *   Array of built search targets or FALSE on error.
 *
 * @see mfcs_table_structure()
 */
function mfcs_request_search_0_form_build_search_targets($structure, $targets, $recurse_count) {
  if (!is_array($structure)) {
    cf_error:invalid_array('structure');
    return FALSE;
  }

  if (!is_array($targets)) {
    cf_error::invalid_array('targets');
    return FALSE;
  }

  if (!cf_is_integer($recurse_count)) {
    cf_error::invalid_integer('recurse_count');
    return FALSE;
  }

  if ($recurse_count >= MFCS_MAX_SEARCH_RECURSION) {
    return FALSE;
  }

  $search = array();

  $name_count = 0;
  foreach ($targets as $target_name => $target_values) {
    if (!is_array($target_values)) {
      continue;
    }

    $target_name_parts = explode('_', $target_name, 2);
    if (count($target_name_parts) && ($target_name_parts[0] == 'and' || $target_name_parts[0] == 'or')) {
      if (!isset($target_values['targets']) || !is_array($target_values['targets'])) {
        continue;
      }

      $search_items = mfcs_request_search_0_form_build_search_targets($structure, $target_values['targets'], $recurse_count + 1);

      if (is_array($search_items)) {
        $search[$target_name] = $search_items;
      }
    }
    else {
      foreach ($target_values as $target_delta => $target_value) {
        $search_item = mfcs_request_search_0_form_build_search_target($structure, $target_name, $target_delta, $target_value);

        if (is_array($search_item)) {
          foreach ($search_item as $search_item_name => $search_item_values) {
            $search[$search_item_name] = $search_item_values;
          }
        }
      }
    }
  }

  return $search;
}

/**
 * Build a list search array item for a single search target.
 *
 * @param array $structure
 *   Structure array as returned by mfcs_table_structure().
 * @param string $target_name
 *   Name of the field to process and convert into a search field.
 * @param int $target_delta
 *   The fields row number.
 * @param array $target_values
 *   An array of values associated with a given field.
 *
 * @return array|bool
 *   Array of search options or FALSE on error.
 *
 * @see mfcs_table_structure()
 */
function mfcs_request_search_0_form_build_search_target($structure, $target_name, $target_delta, $target_values) {
  if (!is_array($structure)) {
    cf_error::invalid_array('structure');
    return FALSE;
  }

  if (!is_string($target_name)) {
    cf_error::invalid_string('target_name');
    return FALSE;
  }

  if (!cf_is_integer($target_delta)) {
    cf_error::invalid_integer('target_delta');
    return FALSE;
  }

  if (!is_array($target_values)) {
    cf_error::invalid_array('target_values');
    return FALSE;
  }

  $target_name_parts = explode('-', $target_name);
  if (count($target_name_parts) != 2) {
    return FALSE;
  }

  if (empty($target_name_parts[0]) || empty($target_name_parts[1])) {
    return FALSE;
  }

  $table_name = $target_name_parts[0];
  $field_name = $target_name_parts[1];

  if (!isset($structure[$table_name][$field_name])) {
    return FALSE;
  }

  // target values may be empty, that just means there is nothing to search for, so return array() instead of FALSE.
  if (empty($target_values)) {
    return array();
  }

  $field = $structure[$table_name][$field_name];

  $search_value = NULL;
  if (is_array($target_values) && array_key_exists('search', $target_values)) {
    $search_value = &$target_values['search'];
  }

  $operator_value = MFCS_OPERATOR_EQUAL;
  if (is_array($target_values) && array_key_exists('operator', $target_values)) {
    $operator_value = &$target_values['operator'];
  }

  $search = array();
  if ($field['type'] == 'requirements') {
    $and = array();

    if (isset($field['columns']) && in_array('quantity', $field['columns'])) {
      if ($operator_value == MFCS_OPERATOR_IS_NULL || $operator_value == MFCS_OPERATOR_IS_NOT_NULL || $target_values['quantity'] == '0' || !empty($target_values['quantity'])) {
        $target_field_name = 'field_' . $target_name . '-quantity-' . $target_delta;
        $and[$target_field_name]['group_name'] = $table_name;
        $and[$target_field_name]['field_name'] = $field_name;
        $and[$target_field_name]['search'] = $target_values['quantity'];
        $and[$target_field_name]['column'] = 'quantity';
        $and[$target_field_name]['type'] = $field['type'];
        $and[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
        $and[$target_field_name]['operator'] = $operator_value;

        if ($operator_value == MFCS_OPERATOR_IS_NULL || $operator_value == MFCS_OPERATOR_IS_NOT_NULL) {
          $and[$target_field_name]['search'] = NULL;
          $and[$target_field_name]['join'] = MFCS_JOIN_LEFT;
        }
      }
    }

    if (isset($field['columns']) && in_array('hours', $field['columns'])) {
      if ($operator_value == MFCS_OPERATOR_IS_NULL || $operator_value == MFCS_OPERATOR_IS_NOT_NULL || $target_values['hours'] == '0' || !empty($target_values['hours'])) {
        $target_field_name = 'field_' . $target_name . '-hours-' . $target_delta;
        $and[$target_field_name]['group_name'] = $table_name;
        $and[$target_field_name]['field_name'] = $field_name;
        $and[$target_field_name]['search'] = NULL;
        $and[$target_field_name]['column'] = 'hours';
        $and[$target_field_name]['type'] = $field['type'];
        $and[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
        $and[$target_field_name]['operator'] = $operator_value;

        if ($operator_value == MFCS_OPERATOR_IS_NULL || $operator_value == MFCS_OPERATOR_IS_NOT_NULL) {
          $and[$target_field_name]['search'] = NULL;
          $and[$target_field_name]['join'] = MFCS_JOIN_LEFT;
        }
        elseif ($target_values['hours'] == '0' || !empty($target_values['hours'])) {
          $and[$target_field_name]['search'] = mfcs_convert_value_to_database_format($target_values['hours'], 'currency');

          if ($and[$target_field_name]['search'] === FALSE) {
            $and[$target_field_name]['search'] = NULL;
          }
        }
      }
    }

    if (isset($field['columns']) && in_array('days', $field['columns'])) {
      if ($operator_value == MFCS_OPERATOR_IS_NULL || $operator_value == MFCS_OPERATOR_IS_NOT_NULL || $target_values['days'] == '0' || !empty($target_values['days'])) {
        $target_field_name = 'field_' . $target_name . '-days-' . $target_delta;
        $and[$target_field_name]['group_name'] = $table_name;
        $and[$target_field_name]['field_name'] = $field_name;
        $and[$target_field_name]['search'] = NULL;
        $and[$target_field_name]['column'] = 'days';
        $and[$target_field_name]['type'] = $field['type'];
        $and[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
        $and[$target_field_name]['operator'] = $operator_value;

        if ($operator_value == MFCS_OPERATOR_IS_NULL || $operator_value == MFCS_OPERATOR_IS_NOT_NULL) {
          $and[$target_field_name]['search'] = NULL;
          $and[$target_field_name]['join'] = MFCS_JOIN_LEFT;
        }
        elseif ($target_values['days'] == '0' || !empty($target_values['days'])) {
          $and[$target_field_name]['search'] = mfcs_convert_value_to_database_format($target_values['days'], 'currency');

          if ($and[$target_field_name]['search'] === FALSE) {
            $and[$target_field_name]['search'] = NULL;
          }
        }
      }
    }

    if (isset($field['columns']) && in_array('amount', $field['columns'])) {
      if ($operator_value == MFCS_OPERATOR_IS_NULL || $operator_value == MFCS_OPERATOR_IS_NOT_NULL || $target_values['amount'] == '0' || !empty($target_values['amount'])) {
        $target_field_name = 'field_' . $target_name . '-amount-' . $target_delta;
        $and[$target_field_name]['group_name'] = $table_name;
        $and[$target_field_name]['field_name'] = $field_name;
        $and[$target_field_name]['search'] = NULL;
        $and[$target_field_name]['column'] = 'amount';
        $and[$target_field_name]['type'] = $field['type'];
        $and[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
        $and[$target_field_name]['operator'] = $operator_value;

        if ($operator_value == MFCS_OPERATOR_IS_NULL || $operator_value == MFCS_OPERATOR_IS_NOT_NULL) {
          $and[$target_field_name]['search'] = NULL;
          $and[$target_field_name]['join'] = MFCS_JOIN_LEFT;
        }
        elseif ($target_values['amount'] == '0' || !empty($target_values['amount'])) {
          $and[$target_field_name]['search'] = mfcs_convert_value_to_database_format($target_values['amount'], 'currency');

          if ($and[$target_field_name]['search'] === FALSE) {
            $and[$target_field_name]['search'] = NULL;
          }
        }
      }
    }

    if (!empty($and)) {
      $search['and_' . $target_delta . '_field'] = $and;
    }
  }
  elseif ($field['type'] == 'date') {
    $column_name = 'value';

    $date_mode = 'y/m/d';
    if (isset($target_values['date_mode'])) {
      $date_mode = $target_values['date_mode'];
    }

    if (is_numeric($search_value)) {
      $date_value = (int) $search_value;
    }
    else {
      $date_value_stamp = strtotime($search_value);

      if ($date_mode == 'y/m/d') {
        $date_value = $date_value_stamp;
      }
      elseif ($date_mode == 'y/m') {
        $matches = array();
        $matched = preg_match('@(\d\d\d\d)\s*/\s*(\d+)\s*(/\s*\d+)*@i', $search_value, $matches);

        if ($matched) {
          $date_value_stamp = strtotime($matches[1] . '/' . $matches[2] . '/1');
          $date_value = $date_value_stamp;
        }
        else {
          // silently ignore invalid date values.
          return array();
        }
      }
      elseif ($date_mode == 'y') {
        $date_value = date("Y", $date_value_stamp);
      }
      elseif ($date_mode == 'm') {
        $date_value = date("m", $date_value_stamp);
      }
      elseif ($date_mode == 'd') {
        $date_value = date("d", $date_value_stamp);
      }
      else {
        // silently ignore invalid date values.
        return array();
      }

      if ($date_value === FALSE) {
        // silently ignore invalid date values.
        return array();
      }
      else {
        $date_value = (int) $date_value;
      }
    }

    if ($date_mode == 'y') {
      $date_value_pad = '';
      if ($date_value < 1000) {
        $date_value_pad .= '0';
      }
      if ($date_value < 100) {
        $date_value_pad .= '0';
      }
      if ($date_value < 10) {
        $date_value_pad .= '0';
      }

      $timestamp_start = strtotime('midnight first day of january 1 ' . $date_value_pad . $date_value);
      $timestamp_stop = strtotime('midnight last day of december 1 ' . $date_value_pad . $date_value);
      $timestamp_stop = strtotime('midnight tomorrow', $timestamp_stop);

      if ($operator_value == MFCS_OPERATOR_EQUAL || $operator_value == MFCS_OPERATOR_NOT_EQUAL) {
        $start_op = MFCS_OPERATOR_GREATER_THAN_EQUAL;
        $stop_op = MFCS_OPERATOR_LESS_THAN;

        $conjunction = array();
        $conjunction_type = 'and';
        if ($operator_value == MFCS_OPERATOR_NOT_EQUAL) {
          $conjunction_type = 'nand';
        }

        $target_field_name = 'field_' . $target_name . '-start-' . $target_delta;
        $conjunction[$target_field_name]['group_name'] = $table_name;
        $conjunction[$target_field_name]['field_name'] = $field_name;
        $conjunction[$target_field_name]['column'] = $column_name;
        $conjunction[$target_field_name]['search'] = $timestamp_start;
        $conjunction[$target_field_name]['type'] = $field['type'];
        $conjunction[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
        $conjunction[$target_field_name]['operator'] = $start_op;

        $target_field_name = 'field_' . $target_name . '-stop-' . $target_delta;
        $conjunction[$target_field_name]['group_name'] = $table_name;
        $conjunction[$target_field_name]['field_name'] = $field_name;
        $conjunction[$target_field_name]['column'] = $column_name;
        $conjunction[$target_field_name]['search'] = $timestamp_stop;
        $conjunction[$target_field_name]['type'] = $field['type'];
        $conjunction[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
        $conjunction[$target_field_name]['operator'] = $stop_op;

        if (!empty($conjunction)) {
          $search[$conjunction_type . '_' . $target_delta . '_field'] = $conjunction;
        }
      }
      else {
        if ($operator_value == MFCS_OPERATOR_LESS_THAN) {
          $date_value = $timestamp_start;
        }
        elseif ($operator_value == MFCS_OPERATOR_LESS_THAN_EQUAL) {
          $date_value = $timestamp_stop;
          $operator_value = MFCS_OPERATOR_LESS_THAN;
        }
        elseif ($operator_value == MFCS_OPERATOR_GREATER_THAN) {
          $date_value = $timestamp_stop;
          $operator_value = MFCS_OPERATOR_GREATER_THAN_EQUAL;
        }
        elseif ($operator_value == MFCS_OPERATOR_GREATER_THAN_EQUAL) {
          $date_value = $timestamp_start;
        }

        $target_field_name = 'field_' . $target_name . '-' . $target_delta;
        $search[$target_field_name] = array();
        $search[$target_field_name]['group_name'] = $table_name;
        $search[$target_field_name]['field_name'] = $field_name;
        $search[$target_field_name]['column'] = $column_name;
        $search[$target_field_name]['type'] = $field['type'];
        $search[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
        $search[$target_field_name]['operator'] = $operator_value;
        $search[$target_field_name]['search'] = $date_value;
      }
    }
    elseif ($date_mode == 'm') {
      $target_field_name = 'field_' . $target_name . '-' . $target_delta;
      $search[$target_field_name] = array();
      $search[$target_field_name]['group_name'] = $table_name;
      $search[$target_field_name]['field_name'] = $field_name;
      $search[$target_field_name]['column'] = $column_name . '_month';
      $search[$target_field_name]['type'] = $field['type'];
      $search[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
      $search[$target_field_name]['operator'] = $operator_value;
      $search[$target_field_name]['search'] = $date_value;
    }
    elseif ($date_mode == 'd') {
      $target_field_name = 'field_' . $target_name . '-' . $target_delta;
      $search[$target_field_name] = array();
      $search[$target_field_name]['group_name'] = $table_name;
      $search[$target_field_name]['field_name'] = $field_name;
      $search[$target_field_name]['column'] = $column_name . '_day';
      $search[$target_field_name]['type'] = $field['type'];
      $search[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
      $search[$target_field_name]['operator'] = $operator_value;
      $search[$target_field_name]['search'] = $date_value;
    }
    elseif ($operator_value == MFCS_OPERATOR_EQUAL || $operator_value == MFCS_OPERATOR_NOT_EQUAL) {
      if ($date_mode == 'y/m/d') {
        $timestamp = strtotime($date_value);
        $target_field_name = 'field_' . $target_name . '-' . $target_delta;
        $search[$target_field_name] = array();
        $search[$target_field_name]['group_name'] = $table_name;
        $search[$target_field_name]['field_name'] = $field_name;
        $search[$target_field_name]['column'] = $column_name;
        $search[$target_field_name]['type'] = $field['type'];
        $search[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
        $search[$target_field_name]['operator'] = $operator_value;
        $search[$target_field_name]['search'] = $timestamp;
      }
      else {
        $conjunction = array();
        $conjunction_type = NULL;

        if ($date_mode == 'y/m') {
          $timestamp_start = strtotime('midnight first day of ', $date_value);
          $timestamp_stop = strtotime('midnight last day of ', $date_value);
          $timestamp_stop = strtotime('midnight tomorrow', $timestamp_stop);

          $start_op = MFCS_OPERATOR_GREATER_THAN_EQUAL;
          $stop_op = MFCS_OPERATOR_LESS_THAN;

          $conjunction_type = 'and';
          if ($operator_value == MFCS_OPERATOR_NOT_EQUAL) {
            $conjunction_type = 'nand';
          }

          $target_field_name = 'field_' . $target_name . '-start-' . $target_delta;
          $conjunction[$target_field_name]['group_name'] = $table_name;
          $conjunction[$target_field_name]['field_name'] = $field_name;
          $conjunction[$target_field_name]['column'] = $column_name;
          $conjunction[$target_field_name]['search'] = $timestamp_start;
          $conjunction[$target_field_name]['type'] = $field['type'];
          $conjunction[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
          $conjunction[$target_field_name]['operator'] = $start_op;

          $target_field_name = 'field_' . $target_name . '-stop-' . $target_delta;
          $conjunction[$target_field_name]['group_name'] = $table_name;
          $conjunction[$target_field_name]['field_name'] = $field_name;
          $conjunction[$target_field_name]['column'] = $column_name;
          $conjunction[$target_field_name]['search'] = $timestamp_stop;
          $conjunction[$target_field_name]['type'] = $field['type'];
          $conjunction[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
          $conjunction[$target_field_name]['operator'] = $stop_op;
        }

        if (!empty($conjunction)) {
          $search[$conjunction_type . '_' . $target_delta . '_field'] = $conjunction;
        }
      }
    }
    else {
      $target_field_name = 'field_' . $target_name . '-' . $target_delta;
      $search[$target_field_name] = array();
      $search[$target_field_name]['group_name'] = $table_name;
      $search[$target_field_name]['field_name'] = $field_name;
      $search[$target_field_name]['column'] = $column_name;
      $search[$target_field_name]['type'] = $field['type'];
      $search[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
      $search[$target_field_name]['operator'] = $operator_value;
      $search[$target_field_name]['search'] = $date_value;
    }
  }
  elseif (isset($field['columns']) && is_array($field['columns'])) {
    if (is_array($target_values) && array_key_exists('search', $target_values)) {
      if (isset($target_values['option']) && !empty($target_values['option'])) {
        if (array_key_exists($target_values['option'], $field['columns']) && !is_array($search_value)) {
          // checkboxes have their values stored with the key and not a boolean, so convert the boolean to the appropriate key.
          if ($field['type'] == 'checkboxes') {
            if ($search_value == 1) {
              $search_value = $target_values['option'];
            }
          }

          $column_name = $field['columns'][$target_values['option']];
          $target_field_name = 'field_' . $target_name . '-' . $column_name . '-' . $target_delta;
          $search[$target_field_name] = array();
          $search[$target_field_name]['group_name'] = $table_name;
          $search[$target_field_name]['field_name'] = $field_name;
          $search[$target_field_name]['column'] = $column_name;
          $search[$target_field_name]['type'] = $field['type'];
          $search[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
          $search[$target_field_name]['search'] = $search_value;
          $search[$target_field_name]['operator'] = $operator_value;

          if ($operator_value == MFCS_OPERATOR_IS_NULL || $operator_value == MFCS_OPERATOR_IS_NOT_NULL) {
            $search[$target_field_name]['search'] = NULL;
            $search[$target_field_name]['join'] = MFCS_JOIN_LEFT;
          }
        }
      }
      elseif (is_array($search_value)) {
        $and = array();

        foreach ($field['columns'] as $term_id => $column_name) {
          $target_field_name = 'field_' . $target_name . '-' . $column_name . '-' . $target_delta;
          $and[$target_field_name] = array();
          $and[$target_field_name]['group_name'] = $table_name;
          $and[$target_field_name]['field_name'] = $field_name;
          $and[$target_field_name]['column'] = $column_name;
          $and[$target_field_name]['type'] = $field['type'];
          $and[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
          $and[$target_field_name]['search'] = $search_value[$term_id];
          $and[$target_field_name]['operator'] = $operator_value;

          if ($operator_value == MFCS_OPERATOR_IS_NULL || $operator_value == MFCS_OPERATOR_IS_NOT_NULL) {
            $and[$target_field_name]['search'] = NULL;
            $and[$target_field_name]['join'] = MFCS_JOIN_LEFT;
          }
        }

        if (!empty($and)) {
          $search['and_' . $target_delta . '_field'] = $and;
        }
      }
    }
  }
  else {
    if (is_array($target_values) && array_key_exists('search', $target_values)) {
      $column_name = 'value';

      // force empty values for numeric types to be 0.
      if ($field['type'] == 'number' && empty($search_value)) {
        $search_value = 0;
      }

      // Handle types that are stored in their own special formats.
      if (isset($field['storage'])) {
        if ($field['storage'] == 'timestamp') {
          if ($field['type'] == 'time') {
            $search_value = mfcs_convert_value_to_database_format($search_value, 'timestamp_relative');
            $column_name .= '_relative';
          }
          else {
            $search_value = mfcs_convert_value_to_database_format($search_value, 'timestamp');
          }
        }
        elseif (!empty($field['storage'])) {
          $search_value = mfcs_convert_value_to_database_format($search_value, $field['storage']);
        }
      }

      $target_field_name = 'field_' . $target_name . '-' . $target_delta;
      $search[$target_field_name] = array();
      $search[$target_field_name]['group_name'] = $table_name;
      $search[$target_field_name]['field_name'] = $field_name;
      $search[$target_field_name]['column'] = $column_name;
      $search[$target_field_name]['type'] = $field['type'];
      $search[$target_field_name]['multiple'] = isset($field['multiple']) ? $field['multiple'] : FALSE;
      $search[$target_field_name]['operator'] = $operator_value;
      $search[$target_field_name]['search'] = $search_value;

      if ($operator_value == MFCS_OPERATOR_IS_NULL || $operator_value == MFCS_OPERATOR_IS_NOT_NULL) {
        $search[$target_field_name]['search'] = NULL;
        $search[$target_field_name]['join'] = MFCS_JOIN_LEFT;
      }
    }
  }

  return $search;
}

/**
 * Builds the grouping form based on recurion depth and row count.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 * @param array $location
 *   The specific location in the form array to add the grouping interface.
 * @param int $recurse_count
 *   An integer used to detect and limit the recursion depth.
 * @param string $id_extra
 *   A string to insert to the id tag of fields creating by this function.
 * @param array $chosen_options
 *   An array containing the chosen search targets.
 * @param array $logic_options
 *   An array containing the chosen operations.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_request_search_0_form_build_grouping($form, &$form_state, &$location, $recurse_count, $id_extra, $chosen_options, $logic_options) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return FALSE;
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');
    return FALSE;
  }

  if (!is_array($location)) {
    cf_error::invalid_array('location');
    return FALSE;
  }

  if (!is_array($chosen_options)) {
    cf_error::invalid_array('chosen_options');
    return FALSE;
  }

  if (!cf_is_integer($recurse_count)) {
    cf_error::invalid_integer('recurse_count');
    return FALSE;
  }

  if (!is_string($id_extra)) {
    cf_error::invalid_string('id_extra');
    return FALSE;
  }

  // When a search query is made, auto-collapse controls by default.
  $collapse_controls = FALSE;
  if (isset($form_state['triggering_element']['#id']) && $form_state['triggering_element']['#id'] == 'search-advanced-submit-search') {
    $collapse_controls = TRUE;
  }
  // When the depth > 0, auto-collapse controls by default.
  elseif (!empty($id_extra)) {
    $collapse_controls = TRUE;
  }

  $possible_targets = $form['form']['advanced_search-possible_targets']['#value'];

  $location['search'] = array(
    '#id' => 'search-advanced-search' . $id_extra,
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'no-print',
        'form-item',
        'search-advanced-search',
      ),
    ),
    '#tree' => TRUE,
    '#weight' => -1,
  );

  $location['search']['controls'] = array(
    '#id' => 'search-advanced-search-controls' . $id_extra,
    '#type' => 'fieldset',
    '#title' => "Search Target Controls",
    '#collapsible' => TRUE,
    '#collapsed' => $collapse_controls,
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'no-print',
        'form-item',
        'search-advanced-search-controls',
      ),
    ),
  );

  $location['search']['controls']['possible_targets'] = array(
    '#id' => 'search-advanced-search-controls-possible_targets' . $id_extra,
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'form-item',
        'form-type-select',
        'search-advanced-search-controls-possible_targets',
      ),
    ),
    '#tree' => TRUE,
  );

  $location['search']['controls']['possible_targets']['select'] = array(
    '#id' => 'search-advanced-search-controls-possible_targets-select' . $id_extra,
    '#type' => 'select',
    '#title' => t("Available Search Targets"),
    '#description' => t("Choose a particular target to search by."),
    '#options' => $possible_targets,
    '#element_attributes' => array(
      'class' => array(
        'form-item',
        'form-type-select',
        'search-advanced-search-controls-possible_targets-select',
      ),
    ),
  );

  $location['search']['controls']['possible_targets']['add'] = array(
    '#id' => 'search-advanced-search-controls-possible_targets-add' . $id_extra,
    '#name' => 'search-advanced-search-controls-possible_targets-add' . $id_extra,
    '#type' => 'submit',
    '#default_value' => "Add Target",
    '#attributes' => array(
      'class' => array(
        'form-item',
        'form-type-submit',
        'search-advanced-search-controls-possible_targets-add',
      ),
    ),
  );

  $location['search']['controls']['chosen_targets'] = array(
    '#id' => 'search-advanced-search-controls-chosen_targets' . $id_extra,
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'form-item',
        'search-advanced-search-controls-chosen_targets',
      ),
    ),
    '#tree' => TRUE,
  );

  $combined_targets = array();

  if (isset($chosen_options['options'])) {
    $combined_targets = $chosen_options['options'];
  }

  foreach ($logic_options as $logic_name => $logic_values) {
    $logic_name_parts = explode('_', $logic_name, 2);

    if (count($logic_name_parts) == 2 && ($logic_name_parts[0] == 'and' || $logic_name_parts[0] == 'or')) {
      if ($logic_name_parts[0] == 'and') {
        $combined_targets['and'][$logic_name] = "And #" . $logic_name_parts[1];
      }
      else {
        $combined_targets['or'][$logic_name] = "Or #" . $logic_name_parts[1];
      }
    }
  }

  $location['search']['controls']['chosen_targets']['select'] = array(
    '#id' => 'search-advanced-search-controls-chosen_targets-select' . $id_extra,
    '#type' => 'select',
    '#title' => t("Chosen Search Targets"),
    '#description' => t("Remove a particular target from the search by choices."),
    '#options' => $combined_targets,
    '#element_attributes' => array(
      'class' => array(
        'form-item',
        'form-type-select',
        'search-advanced-search-controls-chosen_targets-select',
      ),
    ),
  );

  $location['search']['controls']['chosen_targets']['remove'] = array(
    '#id' => 'search-advanced-search-controls-chosen_targets-remove' . $id_extra,
    '#name' => 'search-advanced-search-controls-chosen_targets-remove' . $id_extra,
    '#type' => 'submit',
    '#default_value' => "Remove Target",
    '#element_attributes' => array(
      'class' => array(
        'form-item',
        'form-type-submit',
        'search-advanced-search-controls-chosen_targets-remove',
      ),
    ),
  );

  if ($recurse_count < MFCS_MAX_SEARCH_RECURSION) {
    $location['search']['controls']['grouping'] = array(
      '#id' => 'search-advanced-search-controls-grouping' . $id_extra,
      '#type' => 'container',
      '#attributes' => array(
        'class' => array(
          'form-item',
          'form-type-select',
          'search-advanced-search-controls-grouping',
        ),
      ),
      '#tree' => TRUE,
    );

    $location['search']['controls']['grouping']['explanation'] = array(
      '#id' => 'search-advanced-search-controls-grouping-explanation' . $id_extra,
      '#type' => 'container',
      '#description' => "Perform logical search grouping of (field_1 OR field_2).",
      '#attributes' => array(
        'class' => array(
          'form-item',
          'search-advanced-search-controls-grouping-explanation',
        ),
      ),
    );

    $location['search']['controls']['grouping']['explanation']['markup'] = array(
      '#markup' => '',
    );

    $location['search']['controls']['grouping']['explanation']['markup']['#markup'] = '<strong>' . "Target Grouping" . '</strong><br>';
    $location['search']['controls']['grouping']['explanation']['markup']['#markup'] .= '<em class="uppercase">And</em>' . " = match all targets.<br>\n";
    $location['search']['controls']['grouping']['explanation']['markup']['#markup'] .= '<em class="uppercase">OR</em>' . " = match any target.";

    $location['search']['controls']['grouping']['add_and'] = array(
      '#id' => 'search-advanced-search-controls-grouping-add_and' . $id_extra,
      '#name' => 'search-advanced-search-controls-grouping-add_and' . $id_extra,
      '#type' => 'submit',
      '#default_value' => "Add AND",
      '#element_attributes' => array(
        'class' => array(
          'form-item',
          'form-type-submit',
          'search-advanced-search-controls-grouping-add_and',
        ),
      ),
    );

    $location['search']['controls']['grouping']['add_or'] = array(
      '#id' => 'search-advanced-search-controls-grouping-add_or' . $id_extra,
      '#name' => 'search-advanced-search-controls-grouping-add_or' . $id_extra,
      '#type' => 'submit',
      '#default_value' => "Add OR",
      '#element_attributes' => array(
        'class' => array(
          'form-item',
          'form-type-submit',
          'search-advanced-search-controls-grouping-add_or',
        ),
      ),
    );
  }

  if ($recurse_count == 0) {
    $saved_searches = array();
    foreach ($form['form']['existing_search_targets']['#value'] as $existing_search_target) {
      $saved_searches[$existing_search_target->id] = $existing_search_target->human_name;
    }

    $location['search']['controls']['manage'] = array(
      '#id' => 'search-advanced-search-controls-manage',
      '#type' => 'fieldset',
      '#title' => "Search Target Groupings",
      '#description' => "Load, update, save, or delete existing search target structures.",
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#attributes' => array(
        'class' => array(
          'no-print',
          'search-advanced-search-controls-manage',
        ),
      ),
      '#tree' => TRUE,
    );

    $location['search']['controls']['manage']['existing'] = array(
      '#id' => 'search-advanced-search-controls-manage-existing',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'search-advanced-search-controls-manage-existing',
        ),
      ),
    );

    $location['search']['controls']['manage']['existing']['select'] = array(
      '#id' => 'search-advanced-search-controls-manage-existing-select',
      '#type' => 'select',
      '#options' => $saved_searches,
      '#default_value' => '',
      '#title' => t("Available Search Target Groupings"),
      '#description' => t("Load existing saved search target groupings."),
      '#element_attributes' => array(
        'class' => array(
          'search-advanced-search-controls-manage-existing-select',
        ),
      ),
    );

    $location['search']['controls']['manage']['existing']['submit'] = array(
      '#id' => 'search-advanced-search-controls-manage-existing-submit',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'search-advanced-search-controls-manage-existing-submit',
        ),
      ),
    );

    $location['search']['controls']['manage']['existing']['submit']['load'] = array(
      '#id' => 'search-advanced-search-controls-manage-existing-submit-load',
      '#name' => 'search-advanced-search-controls-manage-existing-submit-load',
      '#type' => 'submit',
      '#default_value' => t("Load"),
      '#attributes' => array(
        'class' => array(
          'search-advanced-search-controls-manage-existing-submit-load',
        ),
      ),
    );

    $location['search']['controls']['manage']['existing']['submit']['update'] = array(
      '#id' => 'search-advanced-search-controls-manage-existing-submit-update',
      '#name' => 'search-advanced-search-controls-manage-existing-submit-update',
      '#type' => 'submit',
      '#default_value' => t("Update"),
      '#attributes' => array(
        'class' => array(
          'search-advanced-search-controls-manage-existing-submit-update',
        ),
      ),
    );

    $location['search']['controls']['manage']['existing']['submit']['delete'] = array(
      '#id' => 'search-advanced-search-controls-manage-existing-submit-delete',
      '#name' => 'search-advanced-search-controls-manage-existing-submit-delete',
      '#type' => 'submit',
      '#default_value' => t("Delete"),
      '#attributes' => array(
        'class' => array(
          'search-advanced-search-controls-manage-existing-submit-delete',
        ),
      ),
    );

    $location['search']['controls']['manage']['new'] = array(
      '#id' => 'search-advanced-search-controls-manage-new',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'search-advanced-search-controls-manage-new',
        ),
      ),
    );

    $location['search']['controls']['manage']['new']['human_name'] = array(
      '#id' => 'search-advanced-search-controls-manage-new-human_name',
      '#type' => 'textfield',
      '#size' => 32,
      '#maxlength' => 63,
      '#required' => FALSE,
      '#default_value' => '',
      '#title' => t("Name"),
      '#description' => t("Supply a name for this new search target grouping."),
      '#element_attributes' => array(
        'class' => array(
          'search-advanced-search-controls-manage-new-human_name',
        ),
      ),
    );

    $location['search']['controls']['manage']['new']['submit'] = array(
      '#id' => 'search-advanced-search-controls-manage-new-submit',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'search-advanced-search-controls-manage-new-submit',
        ),
      ),
    );

    $location['search']['controls']['manage']['new']['submit']['save'] = array(
      '#id' => 'search-advanced-search-controls-manage-new-submit-save',
      '#name' => 'search-advanced-search-controls-manage-new-submit-save',
      '#type' => 'submit',
      '#default_value' => t("Save"),
      '#attributes' => array(
        'class' => array(
          'search-advanced-search-controls-manage-new-submit-save',
        ),
      ),
    );
  }

  return TRUE;
}

/**
 * Loads all saved search target groupings for a given user.
 *
 * @param object|null $user
 *   The user object of the user to load or NULL for current user.
 *
 * @return array|bool
 *   The saved target search groupings or FALSE on error.
 */
function mfcs_search_target_grouping_load($user = NULL) {
  if (is_null($user)) {
    $user = cf_current_user();
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  $grouping = array();
  try {
    $query = db_select('mfcs_search_settings', 'mss');
    $query->fields('mss');

    $query->condition('mss.user_id', $user->uid);
    $query->orderBy('mss.human_name');

    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
      $grouping[$result->id] = $result;
    }
  }
  catch (Exception $ex) {
    cf_error::on_exception($ex);

    return FALSE;
  }

  return $grouping;
}

/**
 * @} End of '@addtogroup mfcs'.
 */