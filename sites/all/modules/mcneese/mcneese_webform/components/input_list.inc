<?php

/**
 * @file
 * A List for webform.
 */


/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_input_list() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'required' => FALSE,
    'extra' => array(
      'title_display' => 0,
      'collapsible' => 0,
      'collapsibed' => 0,
      'description' => '',
      'collapsible' => 0,
      'private' => FALSE,
      'unordered_list_options' => 'disc',
      'ordered_list_options' => 'decimal',
      'reversed_list' => FALSE,
      'list_start' => NULL,
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_input_list($component) {
  $form = array();

  $options = array(
    'ordered' => t("Ordered List"),
    'unordered' => t("Unordered List"),
  );

  $unordered_list_options = array(
    'disc' => t("Disc"),
    'square' => t("Square"),
    'circle' => t("Circle"),
    'initial' => t("Initial"),
    'inherit' => t("Inherit"),
    'none' => t("None"),
  );

  $ordered_list_options = array(
    'armenian' => t("Armenian"),
    'cjk-ideographic' => t("CJK Ideographic"),
    'decimal' => t("Decimal"),
    'decimal-leading-zero' => t("Decimal (Leading Zero)"),
    'georgian' => t("Georgian"),
    'hebrew' => t("Hebrew"),
    'hiragana' => t("Hiragana"),
    'hiragana-iroha' => t("Hiragana Iroha"),
    'katakana' => t("Katakana"),
    'katakana-iroha' => t("Katakana Iroha"),
    'upper-alpha' => t("Alphabetic - Uppercase"),
    'lower-alpha' => t("Alphabetic - Lowercase"),
    'upper-roman' => t("Roman Numerals - Uppercase"),
    'lower-roman' => t("Roman Numerals - Lowercase"),
    'upper-greek' => t("Greek - Uppercase"),
    'lower-greek' => t("Greek - Lowercase"),
    'upper-latin' => t("Latin - Uppercase"),
    'lower-latin' => t("Latin - Lowercase"),
    'initial' => t("Initial"),
    'inherit' => t("Inherit"),
    'none' => t("None"),
  );

  $form['value'] = array(
    '#type' => 'select',
    '#title' => t("List Type"),
    '#description' => t("Select the type of list to present fields under."),
    '#default_value' => $component['value'],
    '#options' => $options,
    '#weight' => 0,
  );

  $form['extra']['unordered_list_options'] = array(
    '#type' => 'select',
    '#title' => t("Unordered List Type"),
    '#description' => t("Select the symbol type to use for the unordered list."),
    '#default_value' => $component['extra']['unordered_list_options'],
    '#options' => $unordered_list_options,
    '#weight' => 1,
    '#parents' => array('extra', 'unordered_list_options'),
    '#dependency' => array('edit-value' => array('unordered')),
  );

  $form['extra']['ordered_list_options'] = array(
    '#type' => 'select',
    '#title' => t("Ordered List Type"),
    '#description' => t("Select the symbol type to use for the ordered list."),
    '#default_value' => $component['extra']['ordered_list_options'],
    '#options' => $ordered_list_options,
    '#weight' => 2,
    '#parents' => array('extra', 'ordered_list_options'),
    '#dependency' => array('edit-value' => array('ordered')),
  );

  $form['extra']['reversed_list'] = array(
    '#type' => 'checkbox',
    '#title' => t("Reversed List"),
    '#description' => t("Display the ordering in reverse."),
    '#default_value' => (bool) $component['extra']['reversed_list'],
    '#weight' => 3,
    '#parents' => array('extra', 'reversed_list'),
    '#dependency' => array('edit-value' => array('ordered')),
    '#required' => FALSE,
  );

  $form['extra']['list_start'] = array(
    '#type' => 'textfield',
    '#title' => t("List Start Value"),
    '#description' => t("Assign a starting value instead of using the default of 1."),
    '#default_value' => $component['extra']['list_start'],
    '#weight' => 4,
    '#parents' => array('extra', 'list_start'),
    '#dependency' => array('edit-value' => array('ordered')),
    '#required' => FALSE,
  );

  if (function_exists('html5_tools_field_widget_info')) {
    $form['extra']['list_start']['#type'] = 'numberfield';
  }

  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_input_list($component, $value = NULL, $filter = TRUE) {
  $element = array(
    '#weight' => 0,
    '#pre_render' => array('mcneese_webform_pre_render_input_list'),
    '#webform_component' => &$component,
  );

  if (array_key_exists('weight', $component)) {
    $element['#weight'] = $component['weight'];
  }

  return $element;
}

/**
 * Pre-render function for the input list webform component.
 */
function mcneese_webform_pre_render_input_list($element) {
  $component = &$element['#webform_component'];

  $list_type = 'ul';
  if (array_key_exists('value', $component)) {
    if ($component['value'] == 'ordered') {
      $list_type = 'ol';
    }
  }

  $list_reversed = FALSE;
  if (isset($component['extra']) && array_key_exists('reversed_list', $component['extra'])) {
    $list_reversed = (bool) $component['extra']['reversed_list'];
  }

  $list_start = NULL;
  if (isset($component['extra']) && array_key_exists('list_start', $component['extra'])) {
    $list_start = $component['extra']['list_start'];
  }

  if ($component['value'] == 'ordered') {
    $list_option = 'decimal';
    if (isset($component['extra']) && array_key_exists('ordered_list_options', $component['extra'])) {
      $list_option = $component['extra']['ordered_list_options'];
    }
  }
  else {
    $list_option = 'disc';
    if (isset($component['extra']) && array_key_exists('unordered_list_options', $component['extra'])) {
      $list_option = $component['extra']['unordered_list_options'];
    }
  }

  // Convert the parents array into a string, excluding the "submitted" wrapper.
  $nested_level = $element['#parents'][0] == 'submitted' ? 1 : 0;
  $parents = str_replace('_', '-', implode('--', array_slice($element['#parents'], $nested_level)));

  $children = &$element['#children'];
  $attributes = array();
  $attributes['class'] = array(
    'form-item',
    'webform-component',
    'webform-component-input-list',
    'webform-component--' . $parents,
  );

  if ($list_reversed) {
    $attributes['reversed'] = 'reversed';
  }
  else {
    unset($attributes['reversed']);
  }

  // @todo: validate list option and enforce default.
  $options = array();

  if ($list_type == 'ul') {
    $options['disc'] = 'disc';
    $options['square'] = 'square';
    $options['circle'] = 'circle';
  }
  else {
    $options['armenian'] = '1';
    $options['cjk-ideographic'] = '1';
    $options['decimal'] = '1';
    $options['decimal-leading-zero'] = '1';
    $options['georgian'] = '1';
    $options['hebrew'] = 'A';
    $options['hiragana'] = '1';
    $options['hiragana-iroha'] = '1';
    $options['katakana'] = '1';
    $options['katakana-iroha'] = '1';
    $options['lower-alpha'] = 'a';
    $options['upper-alpha'] = 'A';
    $options['lower-roman'] = 'i';
    $options['upper-roman'] = 'I';
    $options['lower-greek'] = 'a';
    $options['upper-greek'] = 'A';
    $options['lower-latin'] = 'a';
    $options['upper-latin'] = 'A';
  }

  $options['initial'] = NULL;
  $options['inherit'] = NULL;
  $options['none'] = NULL;
  
  if (array_key_exists($list_option, $options)) {
    if (is_null($options[$list_option])) {
      unset($attributes['type']);
    }
    else {
      $attributes['type'] = $options[$list_option];
    }
  }
  else {
    if ($list_type == 'ul') {
      $attributes['type'] = 'disc';
      $list_option = 'disc';
    }
    else {
      $attributes['type'] = '1';
      $list_option = 'decimal';
    }
  }

  if ($list_type == 'ol' && !is_null($list_start)) {
    if (is_numeric($list_start)) {
      $attributes['start'] = (int) $list_start;
    }
  }

  $class_name = cf_theme_safe_css('list-style-type-' . $list_option);

  if ($class_name !== FALSE) {
    if (!array_key_exists('class', $attributes) || !is_array($attributes['class'])) {
      $attributes['class'] = array();
    }

    if (!in_array($class_name, $attributes['class'])) {
      $attributes['class'][] = $class_name;
    }
  }

  if (!empty($component['extra']['wrapper_classes'])) {
    $wrapper_classes = explode(' ', $component['extra']['wrapper_classes']);

    foreach ($wrapper_classes as &$wrapper_class) {
      $attributes['class'][] = $wrapper_class;
    }
  }

  $css_classes = array();
  if (!empty($component['extra']['css_classes'])) {
    $css_classes = explode(' ', $component['extra']['css_classes']);
  }

  $list = '<' . $list_type;
  foreach ($attributes as $key => &$value) {
    if (is_array($value)) {
      $list .= ' ' . $key . '="' . implode(' ', $value) . '"';
    }
    else {
      $list .= ' ' . $key . '="' . $value . '"';
    }
  }
  $list .= '>';

  $count = 0;
  $list_items = array();
  foreach ($element as $key => $value) {
    if (!is_array($value) || preg_match('/^#/i', $key)) continue;

    $list_item = array();
    $list_item['#type'] = 'markup';
    $list_item['#weight'] = 0;
    $list_item['#tree'] = TRUE;
    $list_item['#markup'] = '';
    $list_item['#attributes'] = array();
    $list_item[$key] = $value;

    if (array_key_exists('#weight', $value)) {
      $list_item['#weight'] = $value['#weight'];
    }

    if (array_key_exists('#pre_render', $value)) {
      $list_item[$key]['#pre_render'] = $value['#pre_render'];
    }
    else {
      $list_item[$key]['#pre_render'] = array();
    }

    $list_item[$key]['#pre_render'][] = 'mcneese_webform_pre_render_input_list_item';

    $count++;

    if (!array_key_exists('#prefix', $value)) {
      $list_item['#prefix'] = '';
    }

    if (!array_key_exists('#suffix', $value)) {
      $list_item['#suffix'] = '';
    }

    $classes = array();
    if ($count % 2 == 0) {
      $classes[] = 'even';
    }
    else {
      $classes[] = 'odd';
    }

    $classes[] = 'list-item-' . $count;

    $classes[] = 'webform-component-input-list-item';
    $classes[] = 'webform-component-input-list-item-' . $count;

    foreach ($css_classes as &$css_class) {
      $classes[] = $css_class;
    }

    $list_item[$key]['#input_list_item_prefix'] = '<li class="' . implode(' ', $classes) . '">';
    $list_item[$key]['#input_list_item_suffix'] = '</li>';

    unset($element[$key]);
    $list_items['list_item-' . $key] = $list_item;
  }

  foreach ($list_items as $item_key => $item_value) {
    $element[$item_key] = $item_value;

    if (!isset($element[$item_key]['#pre_render'])) {
      $element[$item_key]['#pre_render'] = array();
    }
  }

  $element['#prefix'] = $list;
  $element['#suffix'] = '</' . $list_type . '>';

  return $element;
}

/**
 * Pre-render function for the input list item webform component.
 */
function mcneese_webform_pre_render_input_list_item($element) {
  if (!array_key_exists('#input_list_item_prefix', $element)) {
    return $element;
  }

  if (empty($element['#prefix'])) {
    $element['#prefix'] = $element['#input_list_item_prefix'];
  }
  else {
    $element['#prefix'] = $element['#input_list_item_prefix'] . $element['#prefix'];
  }

  if (empty($element['#suffix'])) {
    $element['#suffix'] = $element['#input_list_item_suffix'];
  }
  else {
    $element['#suffix'] .= $element['#input_list_item_suffix'];
  }

  return $element;
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_input_list($component, $value, $format = 'html') {
  if ($format == 'text') {
    $element = array(
      '#title' => $component['name'],
      '#weight' => $component['weight'],
      '#theme_wrappers' => array('webform_element_text'),
      '#translatable' => array('title'),
    );

    return $element;
  }

  $element = _webform_render_input_list($component, $value);
  $element['#format'] = $format;
  $element['#webform_component'] = &$component;

  return $element;
}
