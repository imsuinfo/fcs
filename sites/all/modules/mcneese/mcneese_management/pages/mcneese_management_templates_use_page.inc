<?php

/**
 * @file
 * Defines templates use page functions.
 */

/**
 * @addtogroup mcneese_management
 * @{
 */

/**
 * Provides the templates use page.
 */
function mcneese_management_templates_use_page() {
  $output = '';
  $rows = array();
  $user = cf_current_user();

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
    $query = db_select('mcneese_management_template_user', 'mmtu');
    $query->innerjoin('mcneese_management_template', 'mmt', 'mmt.id = mmtu.template_id');
    $query->fields('mmt', array('id', 'machine_name', 'human_name'));
    $query->condition('user_id', $user->uid);

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
      $links .= '<a href="' . url($template_url . '/use/list/' . $id) . '">' . t("List") . '</a> ';
      $links .= '<a href="' . url($template_url . '/use/add/' . $id) . '">' . t("Create") . '</a> ';
      $row[] = $links;

      $rows[] = $row;
    }
  }
  catch (Error $e) {
    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
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
  $attributes['id'] = 'mcneese_management_template_use';
  $attributes['summary'] = t("A listing of templates available for use.");
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'caption' => t("Templates"), 'attributes' => $attributes, 'empty' => t("There are no templates available for use.")));

  $output .= '<br><br>';
  $output .= '<a id="edit-cancel" href="' . url($template_url) . '">Cancel</a>';

  return $output;
}

/**
 * Provides the a table listing of nodes using a given template.
 *
 * @param int $template_id
 *   The numeric template ID.
 */
function mcneese_management_templates_use_list_page($template_id) {
  $template = mcneese_management_template_load_template($template_id);
  if (!is_object($template)) {
    drupal_not_found();
    drupal_exit();
  }

  $output = '';
  $rows = array();
  $user = cf_current_user();

  $template_url = 'admin/content/management/template/use';
  $options = array('query' => drupal_get_destination());

  drupal_set_title("Nodes Using Template: " . $template->human_name);

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
    $query = db_select('mcneese_management_template_node', 'mmtn');
    $query->join('node', 'n', 'mmtn.node_id = n.nid');
    $query->join('field_data_field_group', 'fdfg', 'mmtn.node_id = fdfg.entity_id and fdfg.entity_type = \'node\'');
    $query->join('taxonomy_term_data', 'ttd', 'fdfg.field_group_tid = ttd.tid');
    $query->fields('mmtn', array('node_id'));
    $query->fields('n', array('title'));
    $query->fields('fdfg', array('field_group_tid'));
    $query->fields('ttd', array('tid', 'name'));
    $query->condition('template_id', $template->id);

    if ($order == 'node id') {
      $query->orderBy('mmtn.node_id', $sort);
    }
    elseif ($order == 'title') {
      $query->orderBy('n.title', $sort);
    }
    elseif ($order == 'group') {
      $query->orderBy('ttd.name', $sort);
      $query->orderBy('ttd.tid', $sort);
    }
    else {
      $query->orderBy('n.title', 'ASC');
    }

    $executed = $query->execute();

    $rows = array();
    foreach ($executed as $result) {
      $row = array();
      $row[] = check_plain($result->node_id);
      $row[] = check_plain($result->title);
      $row[] = check_plain($result->name . ' [tid: ' . $result->field_group_tid . ']');

      $links = '';
      $links .= '<a href="' . url('node/' . $result->node_id) . '">' . t("View") . '</a> ';
      $links .= '<a href="' . url('node/' . $result->node_id . '/template', $options) . '">' . t("Edit") . '</a> ';
      $links .= '<a href="' . url('node/' . $result->node_id . '/delete', $options) . '">' . t("Delete") . '</a> ';
      $row[] = $links;

      $rows[] = $row;
    }
  }
  catch (Error $e) {
    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }
  }
  catch (Exception $e) {
    if (class_exists('cf_error')) {
      cf_error::on_query_execution($e);
    }
  }

  $header = array();
  $header[] = array(
    'data' => t("Node ID"),
    'field' => 'node_id',
    'align' => 'center',
  );
  $header[] = array(
    'data' => t("Title"),
    'field' => 'title',
    'align' => 'center',
    'sort' => 'asc',
  );
  $header[] = array(
    'data' => t("Group"),
    'field' => 'field_group_tid',
    'align' => 'center',
  );
  $header[] = t("Operations");

  $attributes = array();
  $attributes['id'] = 'mcneese_management_template_use_list';
  $attributes['summary'] = t("A listing of nodes associated with the specified template.");
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'caption' => t("Nodes"), 'attributes' => $attributes, 'empty' => t("There are no nodes associated with this template.")));

  $output .= '<br>';
  $output .= '<a href="' . url($template_url . '/add/'. $template->id) . '">' . t("Create new node using this template.") . '</a><br>';

  $output .= '<br><br>';
  $output .= '<a id="edit-cancel" href="' . url($template_url) . '">Cancel</a>';

  return $output;
}

/**
 * Create content from a given template.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 * @param int $template_id
 *   The numeric id of the template.
 * @param string $content_type
 *   (optional) The content type to create this template as.
 */
function mcneese_management_templates_use_add_form($form, &$form_state, $template_id, $content_type = NULL) {
  $template = mcneese_management_template_load_template($template_id);
  if (!is_object($template)) {
    drupal_not_found();
    drupal_exit();
  }

  $type = NULL;
  $types = array();
  if (!is_null($content_type)) {
    $type = $content_type;
  }
  elseif (!empty($form_state['values']['settings']['content_type'])) {
    $type = $form_state['values']['settings']['content_type'];
  }
  else {
    $names = node_type_get_names();
    $content_types = node_type_get_types();
    foreach ($names as $t => $name) {
      if (node_access('create', $t)) {
        if (in_array($t, $template->data['content_types'])) {
          $types[$t] = $name;
        }
      }
    }
    unset($t);
    unset($name);
  }

  if (count($types) == 1) {
    $types = array_keys($types);
    $type = array_pop($types);
  }

  if (!is_null($type)) {
    if (!node_access('create', $type)) {
      drupal_access_denied();
      drupal_exit();
    }
  }

  $destination = isset($_GET['destination']) ? $_GET['destination'] : 'admin/content/management/template/use';

  $form['settings'] = array(
    '#tree' => TRUE,
  );

  $form['template'] = array(
    '#type' => 'value',
    '#value' => $template,
  );

  // fields can be targetted via <tit name="field_body"></tit> (add this to the documentation).
  // make sure to populate any body fields with <template_management>
  if (is_null($type)) {
    $form['settings']['content_type'] = array(
      '#type' => 'select',
      '#title' => t("Select Content Type"),
      '#default_value' => NULL,
      '#options' => $types,
      '#description' => t("Select a content type to save the template onto. Once selected, you will be able to create a new node using the specified content type."),
    );
  }
  else {
    // set the title
    drupal_set_title("Template: " . $template->human_name);

    $user = cf_current_user();

    $form['content_type'] = array(
      '#type' => 'value',
      '#value' => $type,
    );

    $form['user'] = array(
      '#type' => 'value',
      '#value' => $user,
    );

    $form['node'] = array(
      '#type' => 'value',
      '#value' => (object) array(
        'uid' => $user->uid,
        'name' => isset($user->name) ? $user->name : '',
        'type' => $type,
        'language' => LANGUAGE_NONE,
        'mcneese_management_template' => array(
          'id' => $template->id,
          'data' => array(),
        ),
      ),
    );

    node_object_prepare($form['node']['#value']);
    form_load_include($form_state, 'inc', 'node', 'node.pages');

    mcneese_management_template_build_form($form);
  }

  $form['redirect_to'] = array(
    '#type' => 'value',
    '#value' => $destination,
  );

  if (is_null($type)) {
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t("Select Template"),
    );
  }
  else {
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t("Populate Node"),
    );
  }

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t("Cancel"),
    '#href' =>  $destination,
    '#weight' => 100,
  );

  return $form;
}

/**
 * Validation for mcneese_management_templates_use_add_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_templates_use_add_form_validate($form, &$form_state) {
  if (empty($form['content_type']['#value'])) {
    if (empty($form_state['values']['settings']['content_type'])) {
      form_set_error('settings][content_type', t("No valid content type was specified."));
      return;
    }
  }
  else {
    // populate the node fields during validate so that on error, an error can be presented without submitting.
    mcneese_management_template_process_form_state($form, $form_state);
  }
}

/**
 * Submit process for mcneese_management_templates_use_add_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_templates_use_add_form_submit($form, &$form_state) {
  if (empty($form['content_type']['#value'])) {
    drupal_goto('node/add/' . check_plain($form_state['values']['settings']['content_type']) . '/template/' . check_plain($form['template']['#value']->id));
    return;
  }

  $node = $form_state['node'];
  $node->mcneese_management_template['data'] = array();
  if (isset($form_state['values']['settings']['template']['fields'])) {
    $node->mcneese_management_template['data'] = $form_state['values']['settings']['template']['fields'];
  }

  if (property_exists($node, 'log')) {
    $node->log .= "\n" . 'Created via template interface.';
  }

  node_save($node);

  // redirect after submitting.
  if (empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = 'admin/content/management/template/use';
  }
  else {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * Edit content using a given template.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_templates_use_edit_form($form, &$form_state, $node) {
  if (!is_object($node)) {
    drupal_not_found();
    drupal_exit();
  }

  $template = mcneese_management_template_load_template($node->mcneese_management_template['id']);
  if (!is_object($template)) {
    drupal_not_found();
    drupal_exit();
  }

  $form = array();

  drupal_set_title("Edit Template: " . $template->human_name);

  $user = cf_current_user();
  $destination = isset($_GET['destination']) ? $_GET['destination'] : '';

  $form['settings'] = array(
    '#tree' => TRUE,
  );

  $form['template'] = array(
    '#type' => 'value',
    '#value' => $template,
  );

  $form['content_type'] = array(
    '#type' => 'value',
    '#value' => $node->type,
  );

  $form['user'] = array(
    '#type' => 'value',
    '#value' => $user,
  );

  // note, maybe this node object should be created on submit instead.
  $form['node'] = array(
    '#type' => 'value',
    '#value' => $node,
  );

  node_object_prepare($form['node']['#value']);

  mcneese_management_template_build_form($form);

  $form['redirect_to'] = array(
    '#type' => 'value',
    '#value' => $destination,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t("Save Changes"),
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
 * Validation for mcneese_management_templates_use_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_templates_use_edit_form_validate($form, &$form_state) {
  // populate the node fields during validate so that on error, an error can be presented without submitting.
  mcneese_management_template_process_form_state($form, $form_state);
}

/**
 * Submit process for mcneese_management_templates_use_edit_form().
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api.
 */
function mcneese_management_templates_use_edit_form_submit($form, &$form_state) {
  $node = $form_state['node'];
  $node->mcneese_management_template['data'] = array();
  if (isset($form_state['values']['settings']['template']['fields'])) {
    $node->mcneese_management_template['data'] = $form_state['values']['settings']['template']['fields'];
  }

  if (property_exists($node, 'log')) {
    $node->log .= "\n" . 'Updated via template interface.';
  }

  node_save($node);

  // redirect after submitting.
  if (!empty($form_state['values']['redirect_to'])) {
    $form_state['redirect'] = $form_state['values']['redirect_to'];
  }
}

/**
 * @} End of '@addtogroup mcneese_management'.
 */
