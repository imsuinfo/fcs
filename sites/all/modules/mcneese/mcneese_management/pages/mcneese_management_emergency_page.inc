<?php

/**
 * @file
 * Defines emergency settings page functions.
 */

/**
 * @addtogroup mcneese_management
 * @{
 */

/**
 * Provides the emergency settings form page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_emergency_settings_page($form, &$form_state) {
  $emergency_modes = mcneese_management_get_emergency_mode(NULL);
  $emergency_nodes = mcneese_management_get_emergency_node(NULL);

  $stage_3_mode = FALSE;
  $stage_2_mode = FALSE;
  $stage_1_mode = FALSE;

  $stage_3_node = NULL;
  $stage_2_node = NULL;
  $stage_1_node = NULL;

  $stage_3_node_value = '';
  $stage_2_node_value = '';
  $stage_1_node_value = '';

  if (is_array($emergency_modes)) {
    if (array_key_exists('stage_3', $emergency_modes)) {
      $stage_3_mode = (bool) $emergency_modes['stage_3'];
    }

    if (array_key_exists('stage_2', $emergency_modes)) {
      $stage_2_mode = (bool) $emergency_modes['stage_2'];
    }

    if (array_key_exists('stage_1', $emergency_modes)) {
      $stage_1_mode = (bool) $emergency_modes['stage_1'];
    }
  }

  if (is_array($emergency_nodes)) {
    if (!empty($emergency_nodes['stage_3'])) {
      if ($emergency_nodes['stage_3'] > 0) {
        $stage_3_node = node_load($emergency_nodes['stage_3']);

        if (is_object($stage_3_node)) {
          $value = $stage_3_node->title . ' [nid: ' . $stage_3_node->nid . ']';
          $stage_3_node_value = check_plain($value);
          unset($value);
        }
      }
    }

    if (!empty($emergency_nodes['stage_2'])) {
      if ($emergency_nodes['stage_2'] > 0) {
        $stage_2_node = node_load($emergency_nodes['stage_2']);

        if (is_object($stage_2_node)) {
          $value = $stage_2_node->title . ' [nid: ' . $stage_2_node->nid . ']';
          $stage_2_node_value = check_plain($value);
          unset($value);
        }
      }
    }

    if (!empty($emergency_nodes['stage_1'])) {
      if ($emergency_nodes['stage_1'] > 0) {
        $stage_1_node = node_load($emergency_nodes['stage_1']);

        if (is_object($stage_1_node)) {
          $value = $stage_1_node->title . ' [nid: ' . $stage_1_node->nid . ']';
          $stage_1_node_value = check_plain($value);
          unset($value);
        }
      }
    }
  }

  $form = array();

  $form['settings'] = array(
    '#tree' => TRUE,
  );

  $form['settings']['stage_3'] = array(
    '#type' => 'fieldset',
    '#title' => t("Stage 3 Emergency Settings (Major Emergencies)"),
    '#description' => t("This will place the entire site in an alert mode and not just the front page."),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  $form['settings']['stage_3']['mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Emergency Mode'),
    '#default_value' => $stage_3_mode,
    '#description' => t("Enable/Disable emergency mode for a major emergency."),
  );

  $form['settings']['stage_3']['node'] = array(
    '#type' => 'textfield',
    '#title' => t("Emergency Node"),
    '#default_value' => $stage_3_node_value,
    '#description' =>
      t("Specify the numeric node id of the page that is to be used as the emergency node.") . '<br>' .
      t("You can search for a specific node id by typing in the title in this field."),
    '#size' => 64,
    '#autocomplete_path' => 'admin/content/management/emergency/autocomplete_nid',
  );

  $form['settings']['stage_2'] = array(
    '#type' => 'fieldset',
    '#title' => t("Stage 2 Emergency Settings (Moderate Emergencies)"),
    '#description' => t("This only affects the front page."),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );

  $form['settings']['stage_2']['mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Emergency Mode'),
    '#default_value' => $stage_2_mode,
    '#description' => t("Enable/Disable emergency mode for a moderate emergency."),
  );

  $form['settings']['stage_2']['node'] = array(
    '#type' => 'textfield',
    '#title' => t("Emergency Node"),
    '#default_value' => $stage_2_node_value,
    '#description' =>
      t("Specify the numeric node id of the page that is to be used as the emergency node.") . '<br>' .
      t("You can search for a specific node id by typing in the title in this field."),
    '#size' => 64,
    '#autocomplete_path' => 'admin/content/management/emergency/autocomplete_nid',
  );

  $form['settings']['stage_1'] = array(
    '#type' => 'fieldset',
    '#title' => t("Stage 1 Emergency Settings (Minor Emergencies)"),
    '#description' => t("This only affects the front page."),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );

  $form['settings']['stage_1']['mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Emergency Mode'),
    '#default_value' => $stage_1_mode,
    '#description' => t("Enable/Disable emergency mode for a minor emergency."),
  );

  $form['settings']['stage_1']['node'] = array(
    '#type' => 'textfield',
    '#title' => t("Emergency Node"),
    '#default_value' => $stage_1_node_value,
    '#description' =>
      t("Specify the numeric node id of the page that is to be used as the emergency node.") . '<br>' .
      t("You can search for a specific node id by typing in the title in this field."),
    '#size' => 64,
    '#autocomplete_path' => 'admin/content/management/emergency/autocomplete_nid',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t("Save Settings"),
  );

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t("Cancel"),
    '#href' =>  'admin/content/management',
    '#weight' => 100,
  );

  return $form;
}

/**
 * Validation for mcneese_management_emergency_settings_page().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_emergency_settings_page_validate($form, &$form_state) {
  $node_values = array();
  $node_values[1] = &$form_state['values']['settings']['stage_1']['node'];
  $node_values[2] = &$form_state['values']['settings']['stage_2']['node'];
  $node_values[3] = &$form_state['values']['settings']['stage_3']['node'];

  $stage_1_mode = &$form_state['values']['settings']['stage_1']['mode'];
  $stage_2_mode = &$form_state['values']['settings']['stage_2']['mode'];
  $stage_3_mode = &$form_state['values']['settings']['stage_3']['mode'];

  if ($stage_3_mode && ($stage_2_mode || $stage_1_mode)) {
    form_set_error('settings[stage_3][mode]', t("You may only enable one emergency mode at a time."));
  }

  if ($stage_2_mode && $stage_1_mode) {
    form_set_error('settings[stage_2][mode]', t("You may only enable one emergency mode at a time."));
  }

  if ($stage_3_mode && empty($node_values[3])) {
    form_set_error('settings[stage_3][mode]', t("Cannot enable emergency mode when no emergency node is defined."));
  }

  if ($stage_2_mode && empty($node_values[2])) {
    form_set_error('settings[stage_2][mode]', t("Cannot enable emergency mode when no emergency node is defined."));
  }

  if ($stage_1_mode && empty($node_values[1])) {
    form_set_error('settings[stage_1][mode]', t("Cannot enable emergency mode when no emergency node is defined."));
  }

  foreach ($node_values as $key => &$value) {
    if (empty($value)) continue;

    $nid = NULL;
    if (cf_is_integer($value)) {
      $nid = (int) trim($value);
    }
    else {
      $matches = array();
      $matched = preg_match('@\[nid:\s*(\d+)\s*\].*@', $value, $matches);

      if ($matched && isset($matches[1])) {
        $nid = (int) trim($matches[1]);
      }
    }

    if (is_null($nid)) {
      form_set_error('settings[stage_' . $key . '][node', t("An invalid value has been specified for the stage :key emergency node.", array(':key' => $key)));
    }
    else {
      $node = node_load($nid);

      if (is_object($node)) {
        $value = $nid;
      }
      else {
        form_set_error('settings[stage_' . $key . '][node', t("Cannot use %nid for the stage :key emergency node because no node by that id exists.", array(':key' => $key, '%nid' => $nid)));
      }
    }
  }
}

/**
 * Submit process for mcneese_management_emergency_settings_page().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_emergency_settings_page_submit($form, &$form_state) {
  $nodes = array();
  $nodes['stage_1'] = (int) $form_state['values']['settings']['stage_1']['node'];
  $nodes['stage_2'] = (int) $form_state['values']['settings']['stage_2']['node'];
  $nodes['stage_3'] = (int) $form_state['values']['settings']['stage_3']['node'];

  $modes = array();
  $modes['stage_1'] = (bool) $form_state['values']['settings']['stage_1']['mode'];
  $modes['stage_2'] = (bool) $form_state['values']['settings']['stage_2']['mode'];
  $modes['stage_3'] = (bool) $form_state['values']['settings']['stage_3']['mode'];

  $emergency = $modes['stage_3'] || $modes['stage_2'] || $modes['stage_1'];
  $existing_mode = mcneese_management_get_emergency_mode();
  $existing_node = mcneese_management_get_emergency_node();

  variable_set('emergency_nodes', $nodes);
  variable_set('emergency_modes', $modes);

  $clear_cache = FALSE;

  if ($emergency) {
    if ($existing_mode === FALSE) {
      $clear_cache = TRUE;
    }
    elseif ($modes['stage_3']) {
      if ($existing_node != $nodes['stage_3']) {
        $clear_cache = TRUE;
      }
    }
    elseif ($modes['stage_2']) {
      if ($existing_node != $nodes['stage_2']) {
        $clear_cache = TRUE;
      }
    }
    elseif ($modes['stage_1']) {
      if ($existing_node != $nodes['stage_1']) {
        $clear_cache = TRUE;
      }
    }
  }
  elseif ($existing_mode > 0) {
    $clear_cache = TRUE;
  }

  if ($modes['stage_1']) {
    $node = node_load($nodes['stage_1']);

    if (is_object($node) && $node->status != NODE_PUBLISHED) {
      $node->status = NODE_PUBLISHED;
      $node->revision = TRUE;
      $node->log = t("Auto published when emergency mode level 1 was enabled.");

      if (function_exists('workbench_moderation_node_type_moderated') && workbench_moderation_node_type_moderated($node->type)) {
        $node->workbench_moderation_state_new = workbench_moderation_state_published();
      }

      node_save($node);
    }
  }
  elseif ($form['settings']['stage_1']['mode']['#default_value']) {
    $nid = NULL;
    if (cf_is_integer($form['settings']['stage_1']['mode']['#default_value'])) {
      $nid = (int) trim($form['settings']['stage_1']['mode']['#default_value']);
    }
    else {
      $matches = array();
      $matched = preg_match('@\[nid:\s*(\d+)\s*\].*@', $form['settings']['stage_1']['mode']['#default_value'], $matches);

      if ($matched && isset($matches[1])) {
        $nid = (int) trim($matches[1]);
      }
    }

    $node = NULL;
    if (!is_null($nid)) {
      $node = node_load($nid);
    }

    if (is_object($node) && $node->status > 0) {
      $node->status = 0;
      $node->revision = TRUE;
      $node->log = t("Auto unpublished when emergency mode level 1 was disabled.");

      if (function_exists('workbench_moderation_node_type_moderated') && workbench_moderation_node_type_moderated($node->type)) {
        try {
          // Remove the moderation record's "published" flag.
          $query = db_update('workbench_moderation_node_history')
            ->condition('hid', $node->workbench_moderation['published']->hid)
            ->fields(array('published' => 0))
            ->execute();
        }
        catch (Exception $e) {
          if (class_exists('cf_error')) {
            cf_error::on_query_execution($e);
          }
        }

        // Moderate the revision.
        workbench_moderation_moderate($node, workbench_moderation_state_none());

        // Make sure the 'current' revision is the 'live' revision -- ie, the revision in {node}.
        $live_revision = workbench_moderation_node_current_load($node);
        $live_revision->status = 0;
        $live_revision->revision = 0;
        $live_revision->workbench_moderation['updating_live_revision'] = TRUE;
        $live_revision->log = t("Auto unpublished when emergency mode level 1 was disabled.");

        node_save($live_revision);
      }
      else {
        node_save($node);
      }
    }
  }

  // avoid making changes if possible because some changes requires clearing the entire site cache.
  if ($clear_cache) {
    _drupal_flush_css_js();
    drupal_clear_css_cache();
    drupal_clear_js_cache();
    cache_clear_all('*', 'cache', TRUE);
    cache_clear_all('*', 'cache_block', TRUE);
    cache_clear_all('*', 'cache_bootstrap', TRUE);
    cache_clear_all('*', 'cache_field', TRUE);
    cache_clear_all('*', 'cache_filter', TRUE);
    cache_clear_all('*', 'cache_form', TRUE);
    cache_clear_all('*', 'cache_menu', TRUE);
    cache_clear_all('*', 'cache_page', TRUE);
    cache_clear_all('*', 'cache_path', TRUE);
  }

  $maintenance_mode = variable_get('maintenance_mode', FALSE);

  if ($emergency) {
    if ($modes['stage_3']) {
      variable_set('emergency_mode', 3);
      variable_set('maintenance_mode', TRUE);
    }
    elseif ($modes['stage_2']) {
      variable_set('emergency_mode', 2);
    }
    elseif ($modes['stage_1']) {
      variable_set('emergency_mode', 1);
    }
  }
  else {
    if ($existing_mode == 3) {
      variable_set('maintenance_mode', FALSE);
    }

    variable_set('emergency_mode', FALSE);
  }
}

/**
 * @} End of '@addtogroup mcneese_management'.
 */
