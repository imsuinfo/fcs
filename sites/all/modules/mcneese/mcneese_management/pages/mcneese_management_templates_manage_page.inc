<?php

/**
 * @file
 * Defines templates management page functions.
 */

/**
 * @addtogroup mcneese_management
 * @{
 */

/**
 * Provides the templates manage page.
 */
function mcneese_management_templates_manage_page() {
  $output = '';
  $rows = array();

  $template_url = 'admin/content/management/template';

  // database sorting.
  $order = NULL;
  if (isset($_GET['order'])) {
    $order = strtolower($_GET['order']);
  }

  $sort = NULL;
  if (isset($_GET['sort'])) {
    $sort = strtoupper($_GET['sort']);

    if ($sort != 'ASC' && $sort != 'DESC') {
      $sort = NULL;
    }
  }

  try {
    $query = db_select('mcneese_management_template', 'mmt');
    $query->fields('mmt');

    if ($order == 'human name') {
      $query->orderBy('mmt.human_name', $sort);
    }
    elseif ($order == 'machine name') {
      $query->orderBy('mmt.machine_name', $sort);
    }
    elseif ($order == 'id') {
      $query->orderBy('mmt.id', $sort);
    }
    else {
      $query->orderBy('mmt.human_name', 'ASC');
    }

    $executed = $query->execute();

    foreach ($executed as $result) {
      $row = array();
      $id = check_plain($result->id);
      $row[] = $id;
      $row[] = check_plain($result->machine_name);
      $row[] = check_plain($result->human_name);

      $links = '';
      $links .= '<a href="' . url($template_url . '/manage/user/' . $id) . '">' . t("Users") . '</a> ';
      $links .= '<a href="' . url($template_url . '/manage/edit/' . $id) . '">' . t("Edit") . '</a> ';
      $links .= '<a href="' . url($template_url . '/manage/delete/' . $id) . '">' . t("Delete") . '</a> ';
      $row[] = $links;

      $rows[] = $row;
    }
  }
  catch (Exception $e) {
    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }
  }

  $header = array();
  $header[] = array(
    'data' => t("ID"),
    'field' => 'id',
    'align' => 'center',
  );
  $header[] = array(
    'data' => t("Machine Name"),
    'field' => 'machine_name',
    'align' => 'center',
  );
  $header[] = array(
    'data' => t("Human Name"),
    'field' => 'human_name',
    'align' => 'center',
    'sort' => 'asc',
  );
  $header[] = t("Operations");

  $attributes = array();
  $attributes['id'] = 'mcneese_management_template_manage';
  $attributes['summary'] = t("A listing of templates available for management.");
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'caption' => t("Templates"), 'attributes' => $attributes, 'empty' => t("There are no templates available.")));

  $output .= '<br>';
  $output .= '<a href="' . url($template_url . '/manage/add') . '">Add New Template</a>';

  $output .= '<br><br>';
  $output .= '<a id="edit-cancel" href="' . url($template_url) . '">Cancel</a>';

  return $output;
}

/**
 * Add/Edit form for managing templates.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 * @param int $template_id
 *   (optional) The numeric id of the template to edit.
 */
function mcneese_management_templates_manage_edit_form($form, &$form_state, $template_id = NULL) {
  $form = array();

  $destination = isset($_GET['destination']) ? $_GET['destination'] : 'admin/content/management/template/manage';

  $default_human_name = '';
  $default_machine_name = '';
  $default_data = array('inputs' => '', 'targets' => '', 'content_types' => array());
  $default_inputs = '';
  $default_targets = '';
  $default_content_types = '';

  // load existing settings
  if (!empty($template_id)) {
    if (!cf_is_integer($template_id)) {
      if (class_exists('cf_error')) {
        cf_error::invalid_numeric('template_id');
      }

      drupal_not_found();
      drupal_exit();
    }

    $query = db_select('mcneese_management_template', 'mmt');
    $query->fields('mmt');
    $query->condition('id', $template_id);

    $result = $query->execute()->fetchObject();

    if (empty($result)) {
      drupal_not_found();
      drupal_exit();
    }
    else {
      $default_human_name = $result->human_name;
      $default_machine_name = $result->machine_name;
      $default_data = unserialize($result->data);
      $default_content_types = $default_data['content_types'];
      $default_inputs = $default_data['inputs'];
      $default_targets = $default_data['targets'];
    }
  }

  $content_types = array();
  $names = node_type_get_names();
  foreach ($names as $type => $name) {
    $content_types[$type] = $name;
  }

  $form['settings'] = array(
    '#tree' => TRUE,
  );

  $form['settings']['human_name'] = array(
    '#type' => 'textfield',
    '#title' => t("Human Name"),
    '#size' => 64,
    '#default_value' => $default_human_name,
    '#description' => t("A human-friendly name that identifies this template. Does not have to be unique, but it is highly recommended to be unique."),
    '#required' => TRUE,
  );

  $form['settings']['machine_name'] = array(
    '#type' => 'textfield',
    '#title' => t("Machine Name"),
    '#size' => 64,
    '#maxlength' => 64,
    '#default_value' => $default_machine_name,
    '#description' => t("A machine-friendly name that uniquely identifies this template. (Lower case character and underscores only)"),
    '#required' => TRUE,
  );

  $form['settings']['content_types'] = array(
    '#type' => 'select',
    '#title' => t("Content Types"),
    '#default_value' => $default_content_types,
    '#description' => t("Select the content types that this template will support."),
    '#required' => TRUE,
    '#multiple' => TRUE,
    '#options' => $content_types,
  );

  $form['settings']['inputs'] = array(
    '#type' => 'textarea',
    '#title' => t("Template Inputs"),
    '#default_value' => $default_inputs,
    '#description' => t("Template input markup that defines the form inputs for a template."),
    '#required' => TRUE,
  );

  $form['settings']['targets'] = array(
    '#type' => 'textarea',
    '#title' => t("Template Targets"),
    '#default_value' => $default_targets,
    '#description' => t("Template target markup that defines how the form input data is translated into node values."),
    '#required' => TRUE,
  );

  $help_input_markup = '';
  $help_input_markup .= "The template works by using Template Input fields or " . '<strong>' . check_plain('<ti name="field_name"></ti>') . '</strong>, for example:<br>';
  $help_input_markup .= '<ul>';
  $help_input_markup .=  '  <li><strong>' . check_plain('<ti name="field_title" title="Field Title" type="textfield" maxlength="64" required="1">');
  $help_input_markup .=  "The field is described here with optional text and markup." . check_plain('</ti>') . '</strong>.</li>';
  $help_input_markup .= '</ul>';
  $help_input_markup .= '<br>';
  $help_input_markup .= "The following custom attributes are also supported:" . '<br>';
  $help_input_markup .= '<ul>';
  $help_input_markup .= '<li><strong>name</strong>: Overrides HTML name attribute. Required unique identity for the template input field for use by the template targets.</li>';
  $help_input_markup .= '<li><strong>type</strong>: Overrides HTML name attribute. Designates the input field to be used on the template form.</li>';
  $help_input_markup .= '<li><strong>wysiwyg</strong>: When set to "1", provides the user with a WYSIWYG such as ckeditor (may only work with textarea and text_format fields).</li>';
  $help_input_markup .= '<li><strong>markup</strong>: When set to "1", adds the values of this field as HTML markup instead of plain text.</li>';
  $help_input_markup .= '<li><strong>schemes</strong>: A comma separated list of file schemes to use for file based input types such as media (might not yet be supported).</li>';
  $help_input_markup .= '<li><strong>file_types</strong>: A comma separated list of file types to use for file based input types such as media (might not yet be supported).</li>';
  $help_input_markup .= '<li><strong>serialized</strong>: When set to "1", loads and processes the default_value as a serialized string.</li>';
  $help_input_markup .= '<li><strong>has_alt</strong>: Provide a custom alt textfield for targets that support special alt text handling.</li>';
  $help_input_markup .= '<li><strong>has_title</strong>: Provide a custom title textfield for targets that support special title text handling.</li>';
  $help_input_markup .= '<li><strong>default_value</strong>: Provide a default value, for now forms. This is made more dynamic by default_value_replace.</li>';
  $help_input_markup .= '<li><strong>default_value_replace</strong>: Provide custom processing for the default value, currently only current_date is supported (default_value is now treated as an argument to PHP\'s date() function).</li>';
  $help_input_markup .= '</ul>';
  $help_input_markup .= '<br>';
  $help_input_markup .= "The following Template Input types are supported:" . '<br>';
  $help_input_markup .= '<ul>';
  $help_input_markup .= '<li><strong>checkbox</strong>: A standard drupal checkbox.</li>';
  $help_input_markup .= '<li><strong>select</strong>: A standard drupal select list input field.</li>';
  $help_input_markup .= '<li><strong>textarea</strong>: A standard drupal textarea, but does not auto-add support for WYSIWYG editing and handling.</li>';
  $help_input_markup .= '<li><strong>textfield</strong>: A standard drupal textfield.</li>';
  $help_input_markup .= '<li><strong>text_fromat</strong>: A <em>textarea</em> that adds support for WYSIWYG editing and handling.</li>';
  $help_input_markup .= '<li><strong>field_group</strong>: The McNeese www group field select list.</li>';
  $help_input_markup .= '<li><strong>media</strong>: A drupal file field using the media module. This supports the following attributes: <em>has_alt</em>, <em>has_title</em></li>';
  $help_input_markup .= '</ul>';
  $help_input_markup .= '<br>';
  $help_input_markup .= 'Example Template Input fields:<br>';
  $help_input_markup .= '<ul>';
  $help_input_markup .= '<li><strong>' . check_plain('<ti name="field_title" title="Title" type="textfield"></ti>') . '</strong></li>';
  $help_input_markup .= '<li><strong>' . check_plain('<ti name="field_body" title="Body" type="textarea"></ti>') . '</strong></li>';
  $help_input_markup .= '<li><strong>' . check_plain('<ti name="field_markup" title="Markup" type="text_format" markup="1" format="full_html"></ti>') . '</strong></li>';
  $help_input_markup .= '<li><strong>' . check_plain('<ti name="field_featured_image" title="Featured Image" type="media" required="1" has_alt="field_featured_image"></ti>') . '</strong></li>';
  $help_input_markup .= '</ul>';
  $help_input_markup .= '<br>';

  $help_target_markup = '';
  $help_target_markup .= "The form fields are populated using Template Input Targets,  " . '<strong>' . check_plain('<tit field="field_path"></tit>') . '</strong>, such as:<br>';
  $help_target_markup .= '<ul>';
  $help_target_markup .= '<li><strong>';
  $help_target_markup .= check_plain('<tit field="title" cardinality="0">');
  $help_target_markup .= check_plain('<ti name="field_title" key="0"></ti>');
  $help_target_markup .= check_plain('</tit>') . '<br>';
  $help_target_markup .= '</strong></li>';
  $help_target_markup .= '<li><strong>';
  $help_target_markup .= check_plain('<tit field="body][und" key="value" type="textarea" markup="1" format="full_html">');
  $help_target_markup .= check_plain('<ti name="field_body"></ti>');
  $help_target_markup .= check_plain('</tit>') . '<br>';
  $help_target_markup .= '</strong></li>';
  $help_target_markup .= '<li><strong>';
  $help_target_markup .= check_plain('<tit field="field_markup][und" key="value" type="textarea" markup="1" format="full_html">');
  $help_target_markup .= check_plain('<h1>My Markup Region</h1><div><ti name="field_body"></ti></div><h2>A Sub-Region</h2><div>Some text goes <strong>here</strong>.</div>');
  $help_target_markup .= check_plain('</tit>') . '<br>';
  $help_target_markup .= '</strong></li>';
  $help_target_markup .= '<li><strong>';
  $help_target_markup .= check_plain('<tit field="field_group][und" type="taxonomy_term">');
  $help_target_markup .= check_plain('123');
  $help_target_markup .= check_plain('</tit>') . '<br>';
  $help_target_markup .= '</strong></li>';
  $help_target_markup .= '</ul>';
  $help_target_markup .= '<br>';
  $help_target_markup .= "Template Input Targets also support different types (and each type accepts additional arguments):, such as:" . '<br>';
  $help_target_markup .= '<ul>';
  $help_target_markup .= '<li><strong>string</strong>: (default) A simple input field.</li>';
  $help_target_markup .= '<li><strong>integer</strong>: A value that must be cast to an integer before processing.</li>';
  $help_target_markup .= '<li><strong>textfield</strong>: A value field that populates additional properties specific to a Drupal textfield.</li>';
  $help_target_markup .= '<li><strong>textarea</strong>: A value field that populates additional properties specific to a Drupal textarea. This supports the following attributes: <em>format</em>, <em>markup</em>.</li>';
  $help_target_markup .= '<li><strong>taxonomy_term</strong>: A value field that is stored as an array of a taxonomy term object. This accepts only the numeric term id.</li>';
  $help_target_markup .= '<li><strong>media</strong>: A value field that is stored as a file id and presented as a file object. This accepts only the numeric file id.</li>';
  $help_target_markup .= '<li><strong>date</strong>: (Not yet implemented) A value field for managing date-specific fields.</li>';
  $help_target_markup .= '</ul>';
  $help_target_markup .= '<br>';
  $help_target_markup .= 'The Template Input tags inside of a Template Input Target may specify their own <em>key</em> attribute. This <em>key</em> attribute is used for processing each position after the cardinality position. When not specified, cardinality is assumed to be 1.<br>';
  $help_target_markup .= '<br>';
  $help_target_markup .= 'The <em>cardinality</em> for a given field is used to process and generate multiple value fields. Drupal stores multiple values like this: <strong>body][language][cardinality][value</strong>.<br>';
  $help_target_markup .= '<br>';

  $help_if_markup = '';
  $help_if_markup .= "If conditions are also supported using Template If, " . '<strong>' . check_plain("<tif></tif>") . '</strong>, such as:<br>';
  $help_if_markup .= '<ul>';
  $help_if_markup .= '<li><strong>' . check_plain('<tif target="field_group][und" key="0" type="not_empty"><ti name="group" key="tid">/ti></tif>') . '</strong></li>';
  $help_if_markup .= '<li><strong>' . check_plain('<tif target="title" type="!=" value="My Title"><ti name="title" key="0"></ti></tif>') . '</strong></li>';
  $help_if_markup .= '</ul>';
  $help_if_markup .= '<br>';
  $help_if_markup .= "The following Template If types are supported:" . '<br>';
  $help_if_markup .= '<ul>';
  $help_if_markup .= '<li><strong>empty</strong>: Check if value specified by target (and the key) is empty.</li>';
  $help_if_markup .= '<li><strong>not_empty</strong>: Check if value specified by target (and the key) is not empty.</li>';
  $help_if_markup .= '<li><strong>&gt;</strong>: Check if the target (and the key) is greater than the contents of the value attribute.</li>';
  $help_if_markup .= '<li><strong>&lt;</strong>: Check if the target (and the key) is less than the contents of the value attribute.</li>';
  $help_if_markup .= '<li><strong>&gt;=</strong>: Check if the target (and the key) is greater than or equal to the contents of the value attribute.</li>';
  $help_if_markup .= '<li><strong>&lt;=</strong>: Check if the target (and the key) is less than or equal to the contents of the value attribute.</li>';
  $help_if_markup .= '<li><strong>==</strong>: Check if the target (and the key) is equal to the contents of the value attribute.</li>';
  $help_if_markup .= '<li><strong>!=</strong>: Check if the target (and the key) is not equal to the contents of the value attribute.</li>';
  $help_if_markup .= '<li><strong>true</strong>: Check if the target (and the key) evaluates to TRUE (as a boolean value, or as a 1).</li>';
  $help_if_markup .= '<li><strong>false</strong>: Check if the target (and the key) evaluates to FALSE (as a boolean value, or as a 0).</li>';
  $help_if_markup .= '</ul>';
  $help_if_markup .= '<br>';
  $help_if_markup .= 'Template If conditions do not support cardinality and are not currently input type aware at this time. You must specify the appropriate data location using the key attribute.';
  $help_if_markup .= 'Template If conditions currently only support processing of Template Input fields. Template If\'s do not support Template Targets or node object values.<br>';
  $help_if_markup .= '<br>';

  $form['settings']['template_input_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t("Template Input Help"),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['settings']['template_target_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t("Template Target Help"),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['settings']['template_if_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t("Template If Help"),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['settings']['template_input_fieldset']['help'] = array(
    '#type' => 'markup',
    '#markup' => $help_input_markup,
  );

  $form['settings']['template_target_fieldset']['help'] = array(
    '#type' => 'markup',
    '#markup' => $help_target_markup,
  );

  $form['settings']['template_if_fieldset']['help'] = array(
    '#type' => 'markup',
    '#markup' => $help_if_markup,
  );

  $form['template_id'] = array(
    '#type' => 'value',
    '#value' => $template_id,
  );

  $form['redirect_to'] = array(
    '#type' => 'value',
    '#value' => $destination,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t("Save Settings"),
  );

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t("Cancel"),
    '#href' =>  $destination,
    '#weight' => 100,
  );

  return $form;
}

/**
 * Validation for mcneese_management_templates_manage_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_templates_manage_edit_form_validate($form, &$form_state) {
  if (empty($form_state['values']['settings']['human_name'])) {
    form_set_error('settings][human_name', t("An invalid human name has been specified."));
  }

  $machine_name = NULL;
  if (empty($form_state['values']['settings']['machine_name'])) {
    form_set_error('settings][machine_name', t("An invalid machine name has been specified."));
  }
  else {
    $machine_name = strtolower($form_state['values']['settings']['machine_name']);
  }

  if (empty($form_state['values']['settings']['inputs'])) {
    form_set_error('settings][inputs', t("An invalid template inputs has been specified."));
  }

  if (empty($form_state['values']['settings']['targets'])) {
    form_set_error('settings][targets', t("An invalid template targets has been specified."));
  }

  $check_machine_name = FALSE;
  if (empty($form['template_id']['#value'])) {
    if (!empty($machine_name)) {
      $check_machine_name = TRUE;
    }
  }
  else {
    if (cf_is_integer($form['template_id']['#value'])) {
      try {
        $query = db_select('mcneese_management_template', 'mmt');
        $query->fields('mmt', array('machine_name'));
        $query->condition('id', $form['template_id']['#value']);

        $result = $query->execute()->fetchField();

        if (empty($result)) {
          form_set_error('template_id', t("An invalid template id has been specified."));
        }
        elseif ($result != $machine_name) {
          $check_machine_name = TRUE;
        }
      }
      catch (Exception $e) {
        if (class_exists('cf_error')) {
          cf_error::on_query_execution($e);
        }
      }
    }
    else {
      form_set_error('template_id', t("An invalid template id has been specified."));
    }
  }

  if ($check_machine_name) {
    try {
      $query = db_select('mcneese_management_template', 'mmt');
      $query->fields('mmt', array('id'));
      $query->condition('machine_name', $machine_name);
      $query->countQuery();

      $result = $query->execute()->fetchField();

      if ($result > 0) {
        form_set_error('machine_name', t("The specified machine_name '%machine_name' already exists and therefore cannot be used.", array('%machine_name' => $machine_name)));
      }
    }
    catch (Exception $e) {
      if (class_exists('cf_error')) {
        cf_error::on_query_execution($e);
      }
    }
  }
}

/**
 * Submit process for mcneese_management_templates_manage_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_templates_manage_edit_form_submit($form, &$form_state) {
  $template = array();
  $template['human_name'] = $form_state['values']['settings']['human_name'];
  $template['machine_name'] = $form_state['values']['settings']['machine_name'];

  $data = array();
  $data['inputs'] = $form_state['values']['settings']['inputs'];
  $data['targets'] = $form_state['values']['settings']['targets'];
  $data['content_types'] = $form_state['values']['settings']['content_types'];
  $template['data'] = serialize($data);

  $transaction = db_transaction();

  try {
    if (empty($form['template_id']['#value'])) {
      $query = db_insert('mcneese_management_template');
      $query->fields($template);
      $result = $query->execute();
    }
    else {
      $query = db_update('mcneese_management_template');
      $query->condition('id', $form['template_id']['#value']);
      $query->fields($template);
      $result = $query->execute();
    }

    // enforce transaction execution
    unset($transaction);
  }
  catch (Exception $e) {
    $transaction->rollback();

    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }
  }

  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = 'admin/content/management/template/manage';
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Deletes a specified template.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 * @param int $template_id
 *   The numeric id of the template to delete.
 */
function mcneese_management_templates_manage_delete_form($form, &$form_state, $template_id) {
  $form = array();

  if (!cf_is_integer($template_id)) {
    if (class_exists('cf_error')) {
      cf_error::invalid_numeric('template_id');
    }

    drupal_not_found();
    drupal_exit();
  }

  $destination = isset($_GET['destination']) ? $_GET['destination'] : 'admin/content/management/template/manage';

  $form['settings'] = array(
    '#tree' => TRUE,
  );

  $form['template_id'] = array(
    '#type' => 'value',
    '#value' => $template_id,
  );

  $form['redirect_to'] = array(
    '#type' => 'value',
    '#value' => $destination,
  );

  return confirm_form($form,
    t("Are you sure you want to delete this template?", array()),
    $destination,
    t("This action cannot be undone."),
    t("Delete"),
    t("Cancel")
  );
}

/**
 * Validation for mcneese_management_templates_manage_delete_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_templates_manage_delete_form_validate($form, &$form_state) {
  if (!isset($form['template_id']['#value']) || !cf_is_integer($form['template_id']['#value'])) {
    form_set_error('template_id', t("An invalid template id has been specified."));
  }
}

/**
 * Submit process for mcneese_management_templates_manage_delete_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_templates_manage_delete_form_submit($form, &$form_state) {
  $transaction = db_transaction();

  try {
    $query = db_delete('mcneese_management_template');
    $query->condition('id', $form['template_id']['#value']);
    $query->execute();

    // enforce transaction execution
    unset($transaction);
  }
  catch (Exception $e) {
    $transaction->rollback();

    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }
  }

  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = 'admin/content/management/template/manage';
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Provides the templates user manage page.
 *
 * @param int $template_id
 *   The numeric id of the template to display users who have access.
 */
function mcneese_management_templates_manage_user_page($template_id) {
  $output = '';
  $template_url = 'admin/content/management/template';

  if (!cf_is_integer($template_id)) {
    if (class_exists('cf_error')) {
      cf_error::invalid_numeric('template_id');
    }

    drupal_not_found();
    drupal_exit();
  }

  // database sorting.
  $order = NULL;
  if (isset($_GET['order'])) {
    $order = strtolower($_GET['order']);
  }

  $sort = NULL;
  if (isset($_GET['sort'])) {
    $sort = strtoupper($_GET['sort']);

    if ($sort != 'ASC' && $sort != 'DESC') {
      $sort = NULL;
    }
  }

  try {
    $query = db_select('mcneese_management_template_user', 'mmtu');
    $query->join('users', 'u', 'mmtu.user_id = u.uid');
    $query->fields('mmtu');
    $query->fields('u', array('name'));
    $query->condition('template_id', $template_id);

    if ($order == 'user name') {
      $query->orderBy('u.name', $sort);
    }
    elseif ($order == 'id') {
      $query->orderBy('mmtu.user_id', $sort);
    }
    else {
      $query->orderBy('u.name', 'ASC');
    }

    $executed = $query->execute();

    $rows = array();
    foreach ($executed as $result) {
      $row = array();

      $user_id = check_plain($result->user_id);
      $row[] = $user_id;
      $row[] = check_plain($result->name);

      $links = '';
      $links .= '<a href="' . url($template_url . '/manage/user/delete/' . check_plain($template_id) . '/' . $user_id) . '">' . t("Delete") . '</a> ';
      $row[] = $links;

      $rows[] = $row;
    }
  }
  catch (Exception $e) {
    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }
  }

  $header = array();
  $header[] = array(
    'data' => t("ID"),
    'field' => 'id',
    'align' => 'center',
  );
  $header[] = array(
    'data' => t("User Name"),
    'field' => 'name',
    'align' => 'center',
    'sort' => 'asc',
  );
  $header[] = t("Operations");

  $attributes = array();
  $attributes['id'] = 'mcneese_management_template_manage_user';
  $attributes['summary'] = t("A listing of users who have been granted access to the specified template.");
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'caption' => t("Users"), 'attributes' => $attributes, 'empty' => t("No users have been granted access to this template.")));

  $output .= '<br>';
  $output .= '<a href="' . url($template_url . '/manage/user/add/' . $template_id) . '">Grant User Access to Template</a>';

  $output .= '<br><br>';
  $output .= '<a id="edit-cancel" href="' . url($template_url . '/manage') . '">Cancel</a>';

  return $output;
}

/**
 * Add form for managing template users.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 * @param string $template_id
 *   The numeric ID of the template to delete.
 */
function mcneese_management_templates_manage_user_add_form($form, &$form_state, $template_id) {
  $form = array();

  if (!cf_is_integer($template_id)) {
    if (class_exists('cf_error')) {
      cf_error::invalid_numeric('template_id');
    }

    drupal_not_found();
    drupal_exit();
  }

  $destination = isset($_GET['destination']) ? $_GET['destination'] : 'admin/content/management/template/manage/user/' . $template_id;

  $form['settings'] = array(
    '#tree' => TRUE,
  );

  $form['settings']['user'] = array(
    '#type' => 'textfield',
    '#title' => t("User"),
    '#description' => t("Select a user to grant access to use this theme."),
    '#default_value' => '',
    '#autocomplete_path' => 'admin/content/management/template/ajax/autocomplete/user',
    '#required' => TRUE,
  );

  $form['template_id'] = array(
    '#type' => 'value',
    '#value' => $template_id,
  );

  $form['redirect_to'] = array(
    '#type' => 'value',
    '#value' => $destination,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t("Save Settings"),
  );

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t("Cancel"),
    '#href' =>  $destination,
    '#weight' => 100,
  );

  return $form;
}

/**
 * Validation for mcneese_management_templates_manage_user_add_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_templates_manage_user_add_form_validate($form, &$form_state) {
  $valid_template = FALSE;

  $user_id = mcneese_management_validate_user($form_state['values']['settings']['user']);
  if ($user_id === FALSE) {
    form_set_error('settings][user', t("The user '%user' does not exist."), array('%user' => $form_state['values']['settings']['user']));
  }

  if (!isset($form['template_id']['#value']) || !cf_is_integer($form['template_id']['#value'])) {
    form_set_error('template_id', t("An invalid template id has been specified."));
  }
  else {
    $valid_template = TRUE;
  }

  if ($user_id !== FALSE && $valid_template) {
    try {
      $query = db_select('mcneese_management_template_user', 'mmtu');
      $query->fields('mmtu', array('user_id'));
      $query->condition('template_id', $form['template_id']['#value']);
      $query->condition('user_id', $user_id);
      $query->countQuery();

      $result = $query->execute()->fetchField();

      if ($result > 0) {
        form_set_error('machine_name', t("The specified user '%user' is already added and therefore cannot be added again.", array('%user' => $form_state['values']['settings']['user'])));
      }
    }
    catch (Exception $e) {
      if (class_exists('cf_error')) {
        cf_error::on_query_execution($e);
      }
    }
  }
}

/**
 * Submit process for mcneese_management_templates_manage_user_add_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_templates_manage_user_add_form_submit($form, &$form_state) {
  $user = $form_state['values']['settings']['user'];
  $user_id = NULL;

  if (cf_is_integer($user)) {
    $user_id = $user;
  }
  else {
    $matches = array();
    $matched = preg_match('/ \[uid:\s*(\d+)]\s*$/i', $user, $matches);

    if ($matched) {
      $user_id = $matches[1];
    }
    else {
      return;
    }
  }

  $user = array();
  $user['template_id'] = $form['template_id']['#value'];
  $user['user_id'] = $user_id;

  $transaction = db_transaction();

  try {
    $query = db_insert('mcneese_management_template_user');
    $query->fields($user);
    $result = $query->execute();

    // enforce transaction execution
    unset($transaction);
  }
  catch (Exception $e) {
    $transaction->rollback();

    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }
  }

  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = 'admin/content/management/template/manage';
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Deletes access that was granted to a user for a specific template.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 * @param int $template_id
 *   The numeric id of the template to delete.
 * @param int $user_id
 *   The numeric id of the user to remove access from.
 */
function mcneese_management_templates_manage_user_delete_form($form, &$form_state, $template_id, $user_id) {
  $form = array();

  if (!cf_is_integer($template_id)) {
    if (class_exists('cf_error')) {
      cf_error::invalid_numeric('template_id');
    }

    drupal_not_found();
    drupal_exit();
  }

  if (!cf_is_integer($user_id)) {
    if (class_exists('cf_error')) {
      cf_error::invalid_numeric('user_id');
    }

    drupal_not_found();
    drupal_exit();
  }

  $destination = isset($_GET['destination']) ? $_GET['destination'] : 'admin/content/management/template/manage/user/' . $template_id;

  $form['settings'] = array(
    '#tree' => TRUE,
  );

  $form['template_id'] = array(
    '#type' => 'value',
    '#value' => $template_id,
  );

  $form['user_id'] = array(
    '#type' => 'value',
    '#value' => $user_id,
  );

  $form['redirect_to'] = array(
    '#type' => 'value',
    '#value' => $destination,
  );

  return confirm_form($form,
    t("Are you sure you want to delete access for this user for this template?", array()),
    $destination,
    t("This action cannot be undone."),
    t("Delete"),
    t("Cancel")
  );
}

/**
 * Validation for mcneese_management_templates_manage_user_delete_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_templates_manage_user_delete_form_validate($form, &$form_state) {
  if (!isset($form['template_id']['#value']) || !cf_is_integer($form['template_id']['#value'])) {
    form_set_error('template_id', t("An invalid template id has been specified."));
  }

  if (!isset($form['user_id']['#value']) || !cf_is_integer($form['user_id']['#value'])) {
    form_set_error('user_id', t("An invalid user id has been specified."));
  }
}

/**
 * Submit process for mcneese_management_templates_manage_user_delete_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_templates_manage_user_delete_form_submit($form, &$form_state) {
  $transaction = db_transaction();

  try {
    $query = db_delete('mcneese_management_template_user');
    $query->condition('template_id', $form['template_id']['#value']);
    $query->condition('user_id', $form['user_id']['#value']);
    $query->execute();

    // enforce transaction execution
    unset($transaction);
  }
  catch (Exception $e) {
    $transaction->rollback();

    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }
  }

  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = 'admin/content/management/template/manage';
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * @} End of '@addtogroup mcneese_management'.
 */
