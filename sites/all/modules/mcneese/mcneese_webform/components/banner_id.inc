<?php

/**
 * @file
 * A boolean field for webform.
 */


// Inherit from textfield.
module_load_include('inc', 'webform', 'components/textfield');


/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_banner_id() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'required' => FALSE,
    'extra' => array(
      'width' => 9,
      'maxlength' => 9,
      'field_prefix' => '',
      'field_suffix' => '',
      'unique' => 0,
      'title_display' => 0,
      'description' => '',
      'attributes' => array(),
      'private' => TRUE,
      'readonly' => FALSE,
      'disabled' => FALSE,
      'autofill' => FALSE,
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_banner_id($component) {
  $form = array();

  $form['value'] = array(
    '#type' => 'textfield',
    '#title' => t("Default value"),
    '#description' => t("The default value of the field.") . theme('webform_token_help'),
    '#default_value' => $component['value'],
    '#size' => 9,
    '#maxlength' => 9,
    '#weight' => 0,
  );

  $form['extra'] = array(
    '#type' => 'fieldset',
    '#title' => t("Field Settings"),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['extra']['autofill'] = array(
    '#type' => 'checkbox',
    '#title' => t("Autofill"),
    '#description' => t("Automatically populate the Banner ID (Requires the user to be logged in from a valid McNeese LDAP account)."),
    '#weight' => 1,
    '#default_value' => $component['extra']['autofill'],
    '#return_value' => TRUE,
    '#parents' => array('extra', 'autofill'),
  );

  $form['display']['width'] = array(
    '#type' => 'textfield',
    '#title' => t("Width"),
    '#description' => t("Width of the textfield.") . ' ' . t("Leaving blank will use the default size."),
    '#size' => 5,
    '#maxlength' => 10,
    '#weight' => 0,
    '#default_value' => $component['extra']['width'],
    '#parents' => array('extra', 'width'),
  );

  $form['display']['maxlength'] = array(
    '#type' => 'textfield',
    '#title' => t("Max Length"),
    '#description' => t("Maximum number of digits/characters to allow. This should be 9."),
    '#size' => 5,
    '#maxlength' => 10,
    '#weight' => 1,
    '#default_value' => $component['extra']['maxlength'],
    '#parents' => array('extra', 'maxlength'),
  );

  $form['display']['readonly'] = array(
    '#type' => 'checkbox',
    '#title' => t("Readonly"),
    '#description' => t("Make this field non-editable. Read-only fields still get submitted whereas disabled should not. Useful for setting an unchangeable default value."),
    '#weight' => 11,
    '#default_value' => $component['extra']['readonly'],
    '#return_value' => TRUE,
    '#parents' => array('extra', 'readonly'),
  );

  $form['display']['disabled'] = array(
    '#type' => 'checkbox',
    '#title' => t("Disabled"),
    '#description' => t("Make this field non-editable. Disabled are readonly but also do not get submitted. Useful for setting an unchangeable default value."),
    '#weight' => 11,
    '#default_value' => $component['extra']['disabled'],
    '#return_value' => TRUE,
    '#parents' => array('extra', 'disabled'),
  );

  $form['display']['field_prefix'] = array(
    '#type' => 'textfield',
    '#title' => t("Label placed to the left of the textfield"),
    '#default_value' => $component['extra']['field_prefix'],
    '#description' => t("Examples: $, #, -."),
    '#size' => 40,
    '#maxlength' => 127,
    '#weight' => 1.1,
    '#parents' => array('extra', 'field_prefix'),
  );

  $form['display']['field_suffix'] = array(
    '#type' => 'textfield',
    '#title' => t("Label placed to the right of the textfield"),
    '#default_value' => $component['extra']['field_suffix'],
    '#description' => t("Examples: lb, kg, %."),
    '#size' => 40,
    '#maxlength' => 127,
    '#weight' => 1.2,
    '#parents' => array('extra', 'field_suffix'),
  );

  $form['validation']['unique'] = array(
    '#type' => 'checkbox',
    '#title' => t("Unique"),
    '#description' => t("Require all entered values for this field to be unique."),
    '#weight' => 1,
    '#default_value' => $component['extra']['unique'],
    '#return_value' => TRUE,
    '#parents' => array('extra', 'unique'),
  );

  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_banner_id($component, $value = NULL, $filter = TRUE) {
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;

  $element = array(
    '#type' => 'textfield',
    '#title' => $filter ? _webform_filter_xss($component['name']) : $component['name'],
    '#title_display' => $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before',
    '#default_value' => $filter ? _webform_filter_values($component['value'], $node, NULL, NULL, FALSE) : $component['value'],
    '#required' => $component['required'],
    '#weight' => $component['weight'],
    '#field_prefix' => empty($component['extra']['field_prefix']) ? NULL : ($filter ? _webform_filter_xss($component['extra']['field_prefix']) : $component['extra']['field_prefix']),
    '#field_suffix' => empty($component['extra']['field_suffix']) ? NULL : ($filter ? _webform_filter_xss($component['extra']['field_suffix']) : $component['extra']['field_suffix']),
    '#description' => $filter ? _webform_filter_descriptions($component['extra']['description'], $node) : $component['extra']['description'],
    '#attributes' => $component['extra']['attributes'],
    '#theme_wrappers' => array('webform_element'),
    '#translatable' => array('title', 'description', 'field_prefix', 'field_suffix'),
    '#element_validate' => array('mcneese_webform_validate_banner_id'),
  );

  if (isset($value)) {
    $element['#default_value'] = $value[0];
  }

  // Enforce uniqueness.
  if ($component['extra']['unique']) {
    $element['#element_validate'][] = 'webform_validate_unique';
  }

  // Change the 'width' option to the correct 'size' option.
  if ($component['extra']['width'] > 0) {
    $element['#size'] = $component['extra']['width'];
  }

  if ($component['extra']['readonly']) {
    $element['#attributes']['readonly'] = 'readonly';
  }

  if ($component['extra']['disabled']) {
    $element['#attributes']['disabled'] = 'disabled';
  }

  if ($component['extra']['autofill']) {
    $user = cf_current_user();
    $element['#attributes']['value'] = $user->field_user_banner_id['und'][0]['value'];
  }

  return $element;
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_banner_id($component, $value, $format = 'html') {
  $element = _webform_display_textfield($component, $value, $format);

  return $element;
}

/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_banner_id($component, $sids = array()) {
  $element = _webform_analysis_textfield($component, $sids);

  return $element;
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_banner_id($component, $value) {
  $element = _webform_table_textfield($component, $value);

  return $element;
}

/**
 * Implements _webform_csv_headers_component().
 */
function _webform_csv_headers_banner_id($component, $export_options) {
  $element = _webform_csv_headers_textfield($component, $export_options);

  return $element;
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_banner_id($component, $export_options, $value) {
  $element = _webform_csv_data_textfield($component, $export_options, $value);

  return $element;
}

/**
 * Implements _webform_form_builder_map_component().
 */
function _webform_form_builder_map_banner_id() {
  return array(
    'form_builder_type' => 'banner_id',
    'properties' => array(
      'size' => array(
        'form_parents' => array('display', 'width'),
        'storage_parents' => array('extra', 'width'),
      ),
      'maxlength' => array(
        'form_parents' => array('display', 'maxlength'),
        'storage_parents' => array('extra', 'maxlength'),
      ),
      'disabled' => array(
        'form_parents' => array('display', 'disabled'),
        'storage_parents' => array('extra', 'disabled'),
      ),
      'readonly' => array(
        'form_parents' => array('display', 'readonly'),
        'storage_parents' => array('extra', 'readonly'),
      ),
      'field_prefix' => array(
        'form_parents' => array('display', 'field_prefix'),
        'storage_parents' => array('extra', 'field_prefix'),
      ),
      'field_suffix' => array(
        'form_parents' => array('display', 'field_suffix'),
        'storage_parents' => array('extra', 'field_suffix'),
      ),
    ),
  );
}
