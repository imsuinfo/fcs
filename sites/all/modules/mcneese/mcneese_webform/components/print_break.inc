<?php

/**
 * @file
 * A page break markup for webform.
 */


/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_print_break() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'required' => FALSE,
    'extra' => array(
      'private' => FALSE,
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_print_break($component) {
  $form = array();

  $options = array(
    'before' => t("Page Break Before"),
    'after' => t("Page Break After"),
  );

  $form['value'] = array(
    '#type' => 'select',
    '#title' => t("How to Perform Page Break"),
    '#description' => t("This option determines the method used when performing a page-break. Normally this can be left alone."),
    '#default_value' => $component['value'],
    '#options' => $options,
    '#weight' => 0,
  );

  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_print_break($component, $value = NULL, $filter = TRUE) {
  $break_when = isset($component['value']) ? node_load($component['value']) : 'after';

  $markup = '';
  if ($break_when == 'before') {
    $markup = '<div class="no-screen no-braille page-break-before-always"></div>';
  }
  else {
    $markup = '<div class="no-screen no-braille page-break-after-always"></div>';
  }

  $element = array(
    '#type' => 'markup',
    '#title' => NULL,
    '#weight' => $component['weight'],
    '#markup' => $markup,
    '#theme_wrappers' => array('webform_element'),
    '#translatable' => array('title', 'markup'),
  );

  // TODO: Remove when #markup becomes available in D7.
  $element['#value'] = $element['#markup'];

  return $element;
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_print_break($component, $value, $format = 'html') {
  return '';
}

/**
 * Implements _webform_theme_component().
 */
function _webform_theme_print_break() {
  return array(
    'webform_display_print_break' => array(
      'render element' => 'element',
      'file' => 'components/print_break.inc',
      'path' => drupal_get_path('module', 'mcneese_webform'),
    ),
  );
}
