<?php

/**
 * @file
 * Defines McNeese facilities view functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */



/**
 * Create the group path presentation structure.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param int $request_id
 *   The request id.
 * @param string $group
 *   The group name.
 * @param array $presentation_tables
 *   An array of tables defining how the presentation is to be built.
 * @param array $show
 *   An array containing the following keys:
 *   'edit': When TRUE, the edit link will be shown in the group header.
 *   'history': When TRUE, the history link is made visible.
 * @param array $request
 *   The request settings array.
 *
 * @return int|null
 *   The row number or NULL when there is no valid row.
 *   FALSE is returned for invalid rows.
 *
 * @see: mfcs_request_view_0_form()
 */
function mfcs_view_setup_group_path(&$form, $request_id, $group, $presentation_tables, $show, $request) {
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $row = NULL;
  $group_path = &$form['display']['groups'];
  $request_type = $request['information']['type'][0]->value;

  $show_link = TRUE;
  if (isset($presentation_tables[$group]['request_type']['hide_link']) && is_array($presentation_tables[$group]['request_type']['hide_link'])) {
    if (in_array($request_type, $presentation_tables[$group]['request_type']['hide_link'])) {
      $show_link = FALSE;
    }
  }

  if ($show['edit']) {
    if (array_key_exists('editable', $presentation_tables[$group])) {
      $show['edit'] = $presentation_tables[$group]['editable'];
    }
  }

  if ($show['history']) {
    if ($group != 'request') {
      $show['history'] = FALSE;
    }
  }

  if ($show['agreement']) {
    if ($group != 'request') {
      $show['agreement'] = FALSE;
    }
  }

  if ($show['clone']) {
    if ($group != 'request') {
      $show['clone'] = FALSE;
    }
    elseif ($request['top']['status'][0]->value == MFCS_REQUEST_STATUS_DELETED) {
      $show['clone'] = FALSE;
    }
  }

  if ($show['ical']) {
    if ($group != 'dates') {
      $show['ical'] = FALSE;
    }
  }

  // create the group, using the numeric row id if available.
  if (array_key_exists('group', $presentation_tables[$group])) {
    $row = (int) $presentation_tables[$group]['group'];

    if (!array_key_exists($row, $group_path)) {
      $form['display']['groups'][$row] = array(
        '#prefix' => '',
        '#suffix' => '',
        '#weight' => $row,
        '#attributes' => array(
          'id' => 'section-request-' . $request_id . '-group-' . $row,
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

  // create the group if there is no specified row id using default weights.
  if (!array_key_exists($group, $group_path)) {
    $group_path[$group] = array(
      '#prefix' => '',
      '#suffix' => '',
      '#weight' => 0,
      '#attributes' => array(
        'id' => 'section-request-' . $request_id . '-' . $group,
        'class' => array(
          'section-request',
          'section-request-' . $group,
        ),
      ),
      '#extra_prefix' => '',
    );

    if (isset($presentation_tables[$group]['label'])) {
      $group_path[$group]['#extra_prefix'] .= '<div class="section-request-' . $request_id . '-' . $group . '-label-wrapper section-request-label-wrapper">';

      if ($show_link) {
        if ($show['clone']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'requests/create-0/' . $request_id . $url_arguments . '" class="section-request-' . $request_id . '-' . $group . '-history section-request-clone no-print" title="'. t("Copy Request") . '">Copy</a>';
        }

        if ($show['edit']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'requests/edit-0/' . $request_id . '/' . $group . $url_arguments . '" class="section-request-' . $request_id . '-' . $group . '-edit section-request-edit no-print" title="'. t("Edit Request") . '">Edit</a>';
        }

        if ($show['history']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'requests/history-0/' . $request_id . $url_arguments . '" class="section-request-' . $request_id . '-' . $group . '-history section-request-history no-print" title="'. t("View History of Request") . '">History</a>';
        }

        if ($show['agreement']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'requests/agreement-0/' . $request_id . $url_arguments . '" class="section-request-' . $request_id . '-' . $group . '-agreement section-request-agreement no-print" title="'. t("View Request") . '">Agreement</a>';
        }

        if ($show['ical']) {
          $group_path[$group]['#extra_prefix'] .= '<a href="' . base_path() . 'requests/ical-0/' . $request_id . $url_arguments . '" class="section-request-' . $request_id . '-' . $group . '-ical section-request-ical no-print" title="'. t("Download iCal of Request") . '">iCal</a>';
        }
      }

      $group_path[$group]['#extra_prefix'] .= '<h2 id="section-request-' . $request_id . '-' . $group . '-label" class="section-request-label section-request-' . $group . '-label">';
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
 * Generate an array structure for a specific request item.
 *
 * @param int $request_id
 *   The id representing the specific request.
 * @param string $field_group
 *   The name of the group the values are associated with.
 * @param string $field_name
 *   The name of the field the values are associated with.
 * @param array $values
 *   An array of values associated with this field.
 * @param array $presentation
 *   A collection of variables to tweak how this value is presented.
 * @param array $request
 *   The request array containing all of the request values.
 *
 * @return array|bool|null
 *   The built array.
 *   NULL is returned when there should be no array to build.
 *   FALSE is returned on failure.
 */
function mfcs_view_build_request_item_structure($request_id, $field_group, $field_name, $values, $presentation, $request) {
  if (!cf_is_integer($request_id)) {
    cf_error::invalid_integer('request_id');
    return FALSE;
  }

  if (!is_string($field_group)) {
    cf_error::invalid_string('field_group');
    return FALSE;
  }

  if (!is_string($field_name)) {
    cf_error::invalid_string('field_name');
    return FALSE;
  }

  if (!is_array($values)) {
    cf_error::invalid_array('values');
    return FALSE;
  }

  if (!is_array($presentation)) {
    cf_error::invalid_array('presentation');
    return FALSE;
  }

  $request = mfcs_load_request_by_id($request_id);
  if (empty($request)) {
    return FALSE;
  }

  // define if the label should be displayed or not.
  $label_display = TRUE;
  if (!empty($presentation['label_display'])) {
    $label_display = (bool) $presentation['label_display'];
  }

  // define when label gets placed (prepend or postpend).
  $label_prepend = TRUE;
  if (!empty($presentation['label_prepend'])) {
    $label_prepend = (bool) $presentation['label_prepend'];
  }

  // define when label gets placed (prepend or postpend).
  $size = 'auto';
  if (!empty($presentation['size'])) {
    $size = (string) $presentation['size'];
  }

  // define the label text presented to the user.
  $label = '';
  if (isset($presentation['label'])) {
    $label = $presentation['label'];
  }

  $label_length = strlen($label);

  $output = array();
  $no_values = TRUE;
  foreach ($values as $delta => &$value) {
    $generated = mfcs_view_build_request_value_structure($request_id, $field_group, $field_name, $delta, $value, $presentation, $request);

    if ($generated !== FALSE && !isset($generated['no_values'])) {
      $id = 'field-request-' . $request_id . '-item-' . $field_group . '-' . $field_name . '-' . $delta;

      if ($size == 'auto') {
        $size = 'small';

        $value_length = 0;
        if (!empty($generated)) {
          foreach ($generated as $key => $generated_value) {
            if (isset($generated_value['markup']['#length'])) {
              $value_length += $generated_value['markup']['#length'];
            }
          }
        }

        if ($label_length + $value_length > 50) {
          $size = 'large';
        }
      }

      $output[$delta] = array(
        '#id' => $id,
        '#type' => 'container',
        '#attributes' => array(
          'class' => array(
            'item-delta-' . $delta,
            'item-request-' . $request_id,
            'item-group-' . $field_group,
            'item-field-' . $field_name,
            'item-size-' . $size,
            'item-request',
          ),
        ),
      );

      if ($label_display) {
        $output[$delta]['label'] = array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array(
              'item-label',
            ),
          ),
          '#weight' => ($label_prepend) ? -1 : 1,
        );

        $output[$delta]['label']['markup'] = array(
          '#markup' => $label,
        );
      }

      $output[$delta]['value'] = $generated;
      $no_values = FALSE;
    }

    unset($generated);
  }

  if ($no_values) {
    // this is added in case later on it is decided that $output should be returned.
    // the item class needs to communicate that this group has no values.
    $delta = 0;

    $class = $field_group . '-' . $field_name . '-' . $delta;
    $id = 'field-request-' . $request_id . '-item-' . $class;

    $output = array();
    $output[$delta] = array(
      '#id' => $id,
      '#type' => 'container',
      '#attributes' => array(
        'class' => array(
          'field-request-item',
          'field-request-item-' . $class,
          'field-request-item-size-' . $size,
        ),
      ),
    );

    $output['item']['#attributes']['class'][] = 'field-request-item-no_values';

    return NULL;
  }

  return $output;
}

/**
 * Generate an array structure for a specific request value.
 *
 * @param int $request_id
 *   The id representing the specific request.
 * @param string $field_group
 *   The name of the group the values are associated with.
 * @param string $field_name
 *   The name of the field the values are associated with.
 * @param int $delta
 *   This is for multiple values, each value has its own delta.
 * @param object $value
 *   The value object in the format in the database structure or in a sumulated
 *   structure.
 * @param array $presentation
 *   A collection of variables to tweak how this value is presented.
 * @param array $request
 *   The request array containing all of the request values.
 * @param array $options
 *   (optional) An array of options tweaking how this function will run.
 *   - 'no_prefix': If TRUE, don't show prefix.
 *   - 'no_suffix': If TRUE, don't show suffix.
 *   - 'print_delta_for_multiple': If TRUE, prepend the delta to the value for
 *      fields that have #multiple set to TRUE.
 *
 * @return array|bool
 *   The built array.
 *   FALSE is returend on failure.
 */
function mfcs_view_build_request_value_structure($request_id, $field_group, $field_name, $delta, $value, $presentation, $request, $options = array()) {
  if (!is_string($field_group)) {
    cf_error::invalid_string('field_group');
    return FALSE;
  }

  if (!is_string($field_name)) {
    cf_error::invalid_string('field_name');
    return FALSE;
  }

  if (!cf_is_integer($delta)) {
    cf_error::invalid_integer('delta');
    return FALSE;
  }

  if (!is_object($value)) {
    cf_error::invalid_object('value');
    return FALSE;
  }

  if (!is_array($presentation)) {
    cf_error::invalid_array('presentation');
    return FALSE;
  }

  $request = mfcs_load_request_by_id($request_id);
  if (empty($request)) {
    return FALSE;
  }

  if (!is_array($options)) {
    cf_error::invalid_array('options');
    return FALSE;
  }

  // define the combined option.
  $combined = FALSE;
  if (!empty($presentation['combined'])) {
    $combined = (bool) $presentation['combined'];
  }

  // define the presentation type.
  $type = array('value' => 'string');
  if (!empty($presentation['type'])) {
    $type = (array) $presentation['type'];
  }

  // define the presentation display.
  $display = array('value' => 'raw');
  if (!empty($presentation['display'])) {
    $display = (array) $presentation['display'];
  }

  // define the presentation value names.
  $value_names = array('value');
  if (!empty($presentation['value_names'])) {
    $value_names = (array) $presentation['value_names'];
  }

  // define a prefix to append before the value.
  $prefix_span_class = 'item-value-instance ';
  $prefix_span_class .= 'item-value-instance-' . $field_group . ' ';
  $prefix_span_class .= 'item-value-instance-' . $field_group . '-' . $field_name . ' ';
  $prefix_span_class .= 'item-value-instance-' . $field_group . '-' . $field_name . '-' . $delta . ' ';
  $prefix = '<span class="' . $prefix_span_class . '">';
  if (!empty($presentation['prefix'])) {
    $prefix .= $presentation['prefix'];
  }

  if (isset($options['no_prefix']) && $options['no_prefix']) {
    $prefix = '';
  }

  // define a suffix to prepend after the value.
  $suffix = '';
  if (!empty($presentation['suffix'])) {
    $suffix = $presentation['suffix'];
  }
  $suffix .= '</span>';

  if (isset($options['no_suffix']) && $options['no_suffix']) {
    $suffix = '';
  }

  $delta_prefix = '';
  if (isset($options['print_delta_for_multiple']) && $options['print_delta_for_multiple']) {
    if (isset($presentation['multiple']) && $presentation['multiple']) {
      $delta_prefix = '<span class="item-delta">[' . $delta . ']</span> ';
    }
  }

  $venue_coordinator = NULL;
  if (!empty($request['venue_coordinator']['user_id'][0]->value)) {
    $venue_coordinator = user_load($request['venue_coordinator']['user_id'][0]->value);
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $id = 'field-request-' . $request_id . '-item-' . $field_group . '-' . $field_name . '-' . $delta . '-value';

  $output = array();
  $empty_value = TRUE;
  $no_values = TRUE;

  foreach ($value_names as $value_name) {
    if ($combined) {
      if (!isset($output['value']['markup']['#markup'])) {
        $output['value'] = array(
          '#id' => $id . '-' . $value_name,
          '#type' => 'container',
          '#attributes' => array(
            'class' => array(
              'item-value',
              'item-value-name-' . $value_name,
            ),
          ),
        );

        $output['value']['markup'] = array(
          '#markup' => NULL,
        );

        if (!array_key_exists('value', $type)) {
          $type['value'] = 'string';
        }

        $output['value']['#attributes']['class'][] = 'item-value-type-value';

        $output['value']['markup']['#length'] = 0;
      }

      $markup = &$output['value']['markup']['#markup'];
      $output_value = &$output['value'];
      $length = &$output['value']['markup']['#length'];
    }
    else {
      $output[$value_name] = array(
        '#id' => $id . '-' . $value_name,
        '#type' => 'container',
        '#attributes' => array(
          'class' => array(
            'item-value',
            'item-value-name-' . $value_name,
          ),
        ),
      );

      $output[$value_name]['markup'] = array(
        '#markup' => NULL,
      );

      if (!array_key_exists($value_name, $type)) {
        $type[$value_name] = 'string';
      }

      $output[$value_name]['#attributes']['class'][] = 'item-value-type-' . $type[$value_name];
      $output['value']['markup']['#length'] = 0;

      $markup = &$output[$value_name]['markup']['#markup'];
      $output_value = &$output[$value_name];
      $length = &$output['value']['markup']['#length'];
    }

    if (property_exists($value, $value_name) && (is_numeric($value->$value_name) || !empty($value->$value_name))) {
      if ($type[$value_name] == 'string' || $type[$value_name] == 'text') {
        $markup .= check_plain($value->$value_name);

        $length += strlen($value->$value_name);
      }
      elseif ($type[$value_name] == 'text_lined') {
        $markup .= preg_replace("/\n/", "<br>\n", check_plain($value->$value_name));

        $length += strlen($value->$value_name);
      }
      elseif ($type[$value_name] == 'timestamp') {
        if (!array_key_exists($value_name, $display)) {
          $display[$value_name] = 'Y/m/d h:ia';
        }

        if ($display[$value_name] == 'stamp') {
          $markup .= check_plain($value->$value_name);
        }
        else {
          $markup .= check_plain(date($display[$value_name], $value->$value_name));
        }

        $length += strlen($value->$value_name);
      }
      elseif ($type[$value_name] == 'timestamp_linked_month') {
        if (!array_key_exists($value_name, $display)) {
          $display[$value_name] = 'Y/m/d';
        }

        if ($display[$value_name] == 'stamp') {
          $markup .= check_plain($value->$value_name);
        }
        else {
          $s_year = date('Y', $value->$value_name);
          $s_month = date('F', $value->$value_name);
          $s_title = $s_month . " " . $s_year;

          $markup .= '<a href="' . $base_path . 'requests/calendar-0/month/' . $s_year . '/' . $s_month . $url_arguments .'" title="' . $s_title . '">' . check_plain(date($display[$value_name], $value->$value_name)) . '</a>';

          unset($s_year);
          unset($s_month);
          unset($s_title);
        }

        $length += strlen($value->$value_name);
      }
      elseif ($type[$value_name] == 'timestamp_linked_day') {
        if (!array_key_exists($value_name, $display)) {
          $display[$value_name] = 'Y/m/d';
        }

        if ($display[$value_name] == 'stamp') {
          $markup .= check_plain($value->$value_name);
        }
        else {
          $s_year = date('Y', $value->$value_name);
          $s_month = date('F', $value->$value_name);
          $s_day = date('d', $value->$value_name);
          $s_title = $s_month . ", " . $s_day . ", " . $s_year;

          $markup .= '<a href="' . $base_path . 'requests/calendar-0/day/' . $s_year . '/' . $s_month . '/' . $s_day . $url_arguments .'" title="' . $s_title . '">' . check_plain(date($display[$value_name], $value->$value_name)) . '</a>';

          unset($s_year);
          unset($s_month);
          unset($s_day);
          unset($s_title);
        }

        $length += strlen($value->$value_name);
      }
      elseif ($type[$value_name] == 'timestamp_linked_hour') {
        if (!array_key_exists($value_name, $display)) {
          $display[$value_name] = 'h:ia';
        }

        if ($display[$value_name] == 'stamp') {
          $markup .= check_plain($value->$value_name);
        }
        else {
          $s_year = date('Y', $value->$value_name);
          $s_month = date('F', $value->$value_name);
          $s_day = date('d', $value->$value_name);
          $s_hour = date('h', $value->$value_name);
          $s_ampm = date('a', $value->$value_name);
          $s_title = $s_month . ", " . $s_day . ", " . $s_year . " at " . $s_hour . ':00' . $s_ampm;
          $s_id = 'time_slot-' . $s_hour . '_00' . $s_ampm;

          $markup .= '<a href="' . $base_path . 'requests/calendar-0/day/' . $s_year . '/' . $s_month . '/' . $s_day . $url_arguments . '#' . $s_id .'" title="' . $s_title . '">' . check_plain(date($display[$value_name], $value->$value_name)) . '</a>';

          unset($s_year);
          unset($s_month);
          unset($s_day);
          unset($s_hour);
          unset($s_ampm);
          unset($s_title);
          unset($s_id);
        }

        $length += strlen($value->$value_name);
      }
      elseif ($type[$value_name] == 'taxonomy') {
        $term = taxonomy_term_load($value->$value_name);

        if (is_object($term)) {
          $markup .= check_plain($term->name);
          $length += strlen($term->name);
        }

        unset($term);
      }
      elseif ($type[$value_name] == 'user_id') {
        $user = user_load($value->$value_name);

        if (is_object($user)){
          if (!array_key_exists($value_name, $display)) {
            $display[$value_name] = 'name';
          }

          $user_name = check_plain($user->name);

          if (!empty($user->field_user_last_name['und'][0]['value'])) {
            $user_name = check_plain($user->field_user_last_name['und'][0]['value']) . ', ';
          }

          if (!empty($user->field_user_first_name['und'][0]['value'])) {
            $user_name .= check_plain($user->field_user_first_name['und'][0]['value']);
          }

          if ($display[$value_name] == 'account_name') {
            $markup .= '<a href="' . $base_path . 'user/' . $user->uid . $url_arguments . '">' . $user_name . '</a>';
            $length += strlen($user_name);
          }
          elseif ($display[$value_name] != 'pass' && property_exists($user, $display)) {
            $markup .= check_plain($user->$display[$value_name]);
            $length += strlen($user->$display[$value_name]);
          }
        }

        unset($user);
      }
      elseif ($type[$value_name] == 'status') {
        mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

        $status_options = mfcs_get_request_status_list_options();

        if (isset($status_options[$value->$value_name])) {
          $markup .= $status_options[$value->$value_name];
          $length += strlen($value->$value_name);
        }
      }
      elseif ($type[$value_name] == 'step') {
        mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

        $step_options = mfcs_get_request_steps_list_options();

        if (isset($step_options[$value->$value_name])) {
          $markup .= $step_options[$value->$value_name];
          $length += strlen($step_options[$value->$value_name]);
        }
      }
      elseif ($type[$value_name] == 'location') {
        mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

        $location_options = mfcs_get_request_location_list_options(NULL, FALSE, TRUE);

        if (isset($location_options[$value->$value_name])) {
          $markup .= $location_options[$value->$value_name];
          $length += strlen($location_options[$value->$value_name]);
        }
      }
      elseif ($type[$value_name] == 'building') {
        mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

        $building_options = mfcs_get_request_building_list_options(NULL, NULL, TRUE);

        if (isset($building_options[$value->$value_name])) {
          $markup .= $building_options[$value->$value_name];
          $length += strlen($building_options[$value->$value_name]);
        }
      }
      elseif ($type[$value_name] == 'room') {
        mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

        $room_options = mfcs_get_request_room_list_options(NULL, NULL, TRUE);

        if (isset($room_options[$value->$value_name])) {
          $markup .= $room_options[$value->$value_name];
          $length += strlen($room_options[$value->$value_name]);
        }
      }
      elseif ($type[$value_name] == 'currency') {
        $currency = mfcs_convert_value_from_database_format($value->$value_name, 'currency');

        if ($currency !== FALSE) {
          $processed_markup = '$' . printf('%01.2f', $currency);
          $markup .= $processed_markup;
          $length += strlen($processed_markup);
        }
      }
      elseif ($type[$value_name] == 'boolean') {
        $boolean = (bool) $value->$value_name;

        if ($boolean) {
          $markup .= 'yes';
          $length += 3;
        }
        else {
          $markup .= 'no';
          $length += 2;
        }

        if ($display[$value_name] == 'when_true') {
          if (!$boolean) {
            continue;
          }
        }
        elseif ($display[$value_name] == 'when_false') {
          if ($boolean) {
            continue;
          }
        }
      }
      elseif ($type[$value_name] == 'requirement_personnel') {
        $markup .= ' ' . $value->$value_name . ' personnel';
        $length += strlen($value->$value_name . ' personnel');
      }
      elseif ($type[$value_name] == 'requirement_hours') {
        $hours = mfcs_convert_value_from_database_format($value->$value_name, 'currency');

        if (intval($hours) == $hours) {
          $markup .= ' ' . $hours . ' hours';
        }
        else {
          $markup .= ' ' . sprintf('%01.2f', $hours) . ' hours';
        }

        $length += strlen($hours . ' hours');
      }
      elseif ($type[$value_name] == 'requirement_days') {
        $days = mfcs_convert_value_from_database_format($value->$value_name, 'currency');

        if (intval($days) == $days) {
          $markup .= ' ' . $days . ' days';
        }
        else {
          $markup .= ' ' . sprintf('%01.2f', $days) . ' days';
        }

        $length += strlen($days . 'days');
      }
      elseif ($type[$value_name] == 'requirement_amount') {
        $amount = mfcs_convert_value_from_database_format($value->$value_name, 'currency');

        if ($amount !== FALSE) {
          $processed_markup = ' at $' . sprintf('%01.2f', $amount);

          // calculate total and append from other fields that should exist.
          $total = $amount;

          if (in_array('quantity', $value_names) && isset($value->quantity) && is_numeric($value->quantity)) {
            $total = $total * $value->quantity;
          }

          if (in_array('hours', $value_names) && isset($value->hours) && is_numeric($value->hours)) {
            $hours = mfcs_convert_value_from_database_format($value->hours, 'currency');

            if ($hours !== FALSE) {
              $total = $total * $hours;
            }
          }

          if (in_array('days', $value_names) && isset($value->days) && is_numeric($value->days)) {
            $days = mfcs_convert_value_from_database_format($value->days, 'currency');

            if ($days !== FALSE) {
              $total = $total * $days;
            }
          }

          $processed_markup .= ' = $' . sprintf('%01.2f', $total);

          $markup .= $processed_markup;
          $length += strlen($processed_markup);
        }
      }
      elseif ($type[$value_name] == 'venue_coordinator') {
        if ($display[$value_name] == 'name') {
          $processed_markup = '';

          $first_name = FALSE;
          if (!empty($venue_coordinator->field_user_last_name['und'][0]['safe_value'])) {
            $first_name = TRUE;
            $processed_markup .= $venue_coordinator->field_user_last_name['und'][0]['safe_value'];
          }

          if (!empty($venue_coordinator->field_user_first_name['und'][0]['safe_value'])) {
            if ($first_name) {
              $processed_markup .=  ', ';
            }

            $processed_markup .= $venue_coordinator->field_user_first_name['und'][0]['safe_value'];
          }

          $markup .= $processed_markup;
          $length += strlen($processed_markup);
        }
        elseif ($display[$value_name] == 'email') {
          $markup .= $venue_coordinator->mail;
          $length += strlen($venue_coordinator->mail);
        }
        elseif ($display[$value_name] == 'phone') {
          if (!empty($venue_coordinator->field_user_phone_number['und'][0]['safe_value'])) {
            $markup .= $venue_coordinator->field_user_phone_number['und'][0]['safe_value'];
            $length += strlen($venue_coordinator->field_user_phone_number['und'][0]['safe_value']);
          }
        }
      }
      elseif ($type[$value_name] == 'review_decision') {
        if ($value->$value_name == MFCS_REVIEW_DECISION_APPROVE) {
          $markup .= "Approved";
          $length += strlen("Approved");
        }
        elseif ($value->$value_name == MFCS_REVIEW_DECISION_DENY) {
          $markup .= "Denied";
          $length += strlen("Denied");
        }
      }

      if (!is_numeric($markup) && empty($markup)) {
        $output_value['#attributes']['class'][] = 'empty_value';
      }
      else {
        $empty_value = FALSE;

        if (!$combined) {
          $markup = $delta_prefix . $prefix . $markup . $suffix;
        }
      }

      $no_values = FALSE;
    }
  }

  if ($no_values) {
    $output_value['#attributes']['class'][] = 'no_value';
    $output['no_values'] = TRUE;
  }
  elseif ($combined && !$empty_value) {
    $output['value']['markup']['#markup'] = $delta_prefix . $prefix . $output['value']['markup']['#markup'] . $suffix;
  }

  return $output;
}



/**
 * Build the review log display.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 * @param int $request_id
 *   The numeric request id.
 * @param null|int $revision_id
 *   (optional) When not NULL, load the request based on the revision number
 *   instead of the latest revision.
 *
 * @return bool
 *   TRUE on success, FALSE otherwise.
 */
function mfcs_view_build_reviews_log(&$form, &$form_state, $request_id, $revision_id = NULL) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return FALSE;
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');
    return FALSE;
  }

  if (!is_null($revision_id) && !cf_is_integer($revision_id)) {
    cf_error::invalid_integer('revision_id');
    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($revision_id) && !cf_is_integer($revision_id)) {
    cf_error::invalid_integer('revision_id');
    drupal_not_found();
    drupal_exit();
  }

  $request = mfcs_load_request_by_id($request_id, NULL, $revision_id);

  if (empty($request)) {
    return FALSE;
  }

  mfcs_include(MFCS_INCLUDE_TABLE);

  $presentation = mfcs_table_presentation();

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $decision_options = mfcs_get_reviewer_decision_list_options();

  try {
    $query = db_select('mfcs_field_review_review', 'mfrr');

    $query->fields('mfrr');

    $query->condition('mfrr.request_id', $request_id);

    if (!is_null($revision_id) && isset($request['mer']['updated'][0]->value)) {
      $query->condition('mfrr.date', $request['mer']['updated'][0]->value, '<=');
    }

    $query->orderBy('mfrr.date');

    $reviews = $query->execute()->fetchAll();

    if (is_array($reviews)) {
      foreach ($reviews as $review) {
        $query = db_select('mfcs_review_classifications', 'mrc');

        $query->fields('mrc');
        $query->condition('mrc.review_id', $review->id);

        $review_classifications = $query->execute()->fetchAll();

        $review->reviewer_classifications = array();
        if (is_array($review_classifications)) {
          foreach ($review_classifications as $review_classification) {
            $review->reviewer_classifications[$review_classification->classification] = $review_classification->classification;
          }
        }
      }
    }
  }
  catch (Exception $ex) {
    cf_error::on_query_execution($ex);

    return FALSE;
  }


  $changelog_fields = array();
  $changelog_fields['requirements'] = array(
    'facilities',
    'equipment',
    'custodial',
    'maintenance',
    'grounds',
    'security',
    'other',
    'waived',
    'university',
  );
  $changelog_fields['insurance'] = array(
    'insurance',
    'unaffiliated',
    'provided',
  );

  $users = array();
  $reviewers = array();
  $has_log = FALSE;
  if (is_array($reviews)) {
    $review_step_options = mfcs_get_review_step_list_options();
    $reviewer_classification_options = mfcs_get_reviewer_classification_list_options();
    $type = $request['information']['type'][0]->value;

    foreach ($reviews as $row => $review) {
      $user_id = $review->user_id;
      $review_step = $review->step;
      $decision = $review->decision;
      $message = $review->message;
      $date = $review->date;

      $user_can_administer = mfcs_user_can_administer($user_id);
      $user_can_manage = mfcs_user_can_manage($user_id);

      $changelog = mfcs_load_request_by_timestamp($request_id, $date, $user_id, $changelog_fields);

      if (!array_key_exists($user_id, $users)) {
        $users[$user_id] = user_load($user_id);

        if (!is_object($users[$user_id])) {
          $users[$user_id] = NULL;
        }
      }

      $markup = '<div class="review-item review-item-' . $row. '">';

      $user_name = '';
      if (is_null($users[$user_id])) {
        watchdog(MFCS_WATCHDOG_ID, "Failed to load the user id of %user_id for request %request_id review log dated %date.", array('%user_id' => $user_id, '%request_id' => $request_id, '%date' => date("Y/m/d h:i:s a", $date)), WATCHDOG_ERROR);
        continue;
      }

      $reviewer_classification = NULL;
      foreach ($review->reviewer_classifications as $classification) {
        if (is_null($reviewer_classification)) {
          $reviewer_classification = '';
        }
        else {
          $reviewer_classification .= ', ';
        }

        if ($classification == MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR) {
          $reviewer_classification .= "Venue Coordinator";
        }
        elseif ($classification == MFCS_REVIEWER_CLASSIFICATION_VENUE_COORDINATOR_PROXY) {
          $reviewer_classification .= "Venue Coordinator (Proxy)";
        }
        elseif ($classification == MFCS_REVIEWER_CLASSIFICATION_MANAGER) {
          $reviewer_classification .= "Manager";
        }
        elseif ($classification == MFCS_REVIEWER_CLASSIFICATION_SYSTEM_ADMINISTRATOR) {
          $reviewer_classification .= "System Administrator";
        }
        else {
          $reviewer_classification .= $reviewer_classification_options[$classification];
        }
      }

      if (is_null($reviewer_classification)) {
        if ($user_can_administer) {
          $reviewer_classification = "Administrator";
        }
        elseif ($user_can_manage) {
          $reviewer_classification = "Manager";
        }
      }

      if (!is_null($reviewer_classification)) {
        $reviewer_classification = ' as <strong class="review-item-label-text-row-reviewer_classification">'. $reviewer_classification . '</strong>';
      }

      if ($user_id == 1) {
        $user_name .= "System";
        $reviewer_classification = "";
      }
      else {
        if (isset($users[$user_id]->field_user_first_name['und'][0]['value'])) {
          $user_name .= $users[$user_id]->field_user_first_name['und'][0]['value'];

          if (isset($users[$user_id]->field_user_last_name['und'][0]['value'])) {
            $user_name .= ' ' . $users[$user_id]->field_user_last_name['und'][0]['value'];
          }
        }
        elseif (isset($users[$user_id]->field_user_last_name['und'][0]['value'])) {
          $user_name .= ' ' . $users[$user_id]->field_user_last_name['und'][0]['value'];
        }
        else {
          $user_name .= $users[$user_id]->name;
        }
      }

      $user_name_markup = '<strong class="review-item-label-text-row-user">';
      $user_name_markup .= $user_name;
      $user_name_markup .= '</strong>';

      $user_id_markup = '<span class="updater_id">[id: ' . $user_id . ']</span>';

      $row_id_block = '<div class="review-item-label-text-row">[' . $row . ']</div> ';

      $markup .= '<div class="review-item-label">';
      $markup .= '<h3 class="review-item-label-text">' . $row_id_block . '<div class="review-item-label-text-title">';

      if ($decision == MFCS_REVIEW_DECISION_AMENDMENT) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has <strong class="review-item-label-text-row-decision">' . "Amended" . '</strong> the request' . $reviewer_classification . '.';
      }
      elseif ($review_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && $decision == MFCS_REVIEW_DECISION_APPROVE) {
        if ($type == MFCS_REQUEST_TYPE_QUICK_MEETING) {
          $markup .= $user_name_markup . ' ' . $user_id_markup . ' has designated that the <strong class="review-item-label-text-row-decision">Venue is Available</strong>, <strong class="review-item-label-text-row-decision">Confirmed</strong>, and <strong class="review-item-label-text-row-decision">Reserved</strong>' . $reviewer_classification . '.';
        }
        else {
          $markup .= $user_name_markup . ' ' . $user_id_markup . ' has designated that the <strong class="review-item-label-text-row-decision">Venue is Available</strong>' . $reviewer_classification . '.';
        }
      }
      elseif ($review_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && $decision == MFCS_REVIEW_DECISION_DENY) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has designated that the <strong class="review-item-label-text-row-decision">Venue is Unavailable</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MFCS_REVIEW_STEP_MAKE_DECISIONS && $decision == MFCS_REVIEW_DECISION_APPROVE) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has <strong class="review-item-label-text-row-decision">Confirmed and Reserved</strong> the request' . $reviewer_classification . "";
      }
      elseif ($review_step == MFCS_REVIEW_STEP_MAKE_DECISIONS && $decision == MFCS_REVIEW_DECISION_DENY) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has <strong class="review-item-label-text-row-decision">Denied</strong> the request' . $reviewer_classification . '.';
      }
      elseif ($decision == MFCS_REVIEW_DECISION_COMMENT) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has made a <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong>' . $reviewer_classification . '.';
      }
      elseif (($review_step == MFCS_REVIEW_STEP_REVIEW || $review_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) && $decision == MFCS_REVIEW_DECISION_REQUIREMENT) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has designated the <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong>' . $reviewer_classification . '.';
      }
      elseif (($review_step == MFCS_REVIEW_STEP_REVIEW || $review_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE) && $decision == MFCS_REVIEW_DECISION_WAVE) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has waived the <strong class="review-item-label-text-row-decision">' . $decision_options[MFCS_REVIEW_DECISION_REQUIREMENT] . '</strong> ' . $reviewer_classification . '.';
      }
      elseif ($review_step == MFCS_REVIEW_STEP_REQUIREMENTS && $decision == MFCS_REVIEW_DECISION_APPROVE) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has accepted the <strong class="review-item-label-text-row-decision">' . $review_step_options[$review_step] . '</strong>' . $reviewer_classification . '.';
      }
      elseif ($review_step == MFCS_REVIEW_STEP_REQUIREMENTS && $decision == MFCS_REVIEW_DECISION_DENY) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has denied the <strong class="review-item-label-text-row-decision">' . $review_step_options[$review_step] . '</strong>' . $reviewer_classification . '.';
      }
      elseif ($decision == MFCS_REVIEW_DECISION_REQUIREMENT) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' assigned the <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong> for the request' . $reviewer_classification . '.';
      }
      elseif ($decision == MFCS_REVIEW_DECISION_APPROVE && $type == MFCS_REQUEST_TYPE_QUICK_MEETING) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has designated that the <strong class="review-item-label-text-row-decision">Venue is Available</strong>, <strong class="review-item-label-text-row-decision">Confirmed</strong>, and <strong class="review-item-label-text-row-decision">Reserved</strong>' . $reviewer_classification . '.';
      }
      elseif ($decision == MFCS_REVIEW_DECISION_CLOSED) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has <strong class="review-item-label-text-row-decision">Closed</strong> the request' . $reviewer_classification . '.';
      }
      elseif ($decision == MFCS_REVIEW_DECISION_CANCELLED) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has <strong class="review-item-label-text-row-decision">Cancelled</strong> the request' . $reviewer_classification . '.';
      }
      elseif ($decision == MFCS_REVIEW_DECISION_UNCANCELLED) {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' has <strong class="review-item-label-text-row-decision">Uncancelled</strong> the request' . $reviewer_classification . '.';
      }
      else {
        $markup .= $user_name_markup . ' ' . $user_id_markup . ' chose to <strong class="review-item-label-text-row-decision">' . $decision_options[$decision] . '</strong> the request' . $reviewer_classification . '.';
      }
      $markup .= '</div></h3>';
      $markup .= '<div class="review-item-label-date">' . date("Y/m/d h:i:s a", $date) . "</div>";
      $markup .= '</div>';


      // provide additional changelog information.
      if ($decision == MFCS_REVIEW_DECISION_AMENDMENT) {
        $message .= '<div class="review-item-message-decision-amendment review-item-message-decision">';
        $message .= 'The review step has been reset and all prior reviews no longer apply.';
        $message .= '</div>';
      }

      if ($review_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && ($decision == MFCS_REVIEW_DECISION_REQUIREMENT || $decision == MFCS_REVIEW_DECISION_WAVE) && $type != MFCS_REQUEST_TYPE_QUICK_MEETING ) {
        if (!empty($changelog['requirements']['facilities'])) {
          $message .= '<div class="review-item-message-requirements-facilities review-item-message-requirements review-item-message-requirements">';
          $message .= '<div class="item-label">Facilities Fees:</div> ';

          $value = reset($changelog['requirements']['facilities']);
          $value_markup = mfcs_view_build_request_value_structure($request_id, 'requirements', 'facilities', 0, $value, $presentation['requirements']['facilities'], $changelog);

          if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
            $message .= drupal_render($value_markup);
          }
          $message .= '</div>';
        }

        if (!empty($changelog['requirements']['equipment'])) {
          $message .= '<div class="review-item-message-requirements-equipment review-item-message-requirements review-item-message-requirements">';
          $message .= '<div class="item-label">Additional Equipment Fees:</div> ';

          $value = reset($changelog['requirements']['equipment']);
          $value_markup = mfcs_view_build_request_value_structure($request_id, 'requirements', 'equipment', 0, $value, $presentation['requirements']['equipment'], $changelog);

          if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
            $message .= drupal_render($value_markup);
          }
          $message .= '</div>';
        }
      }

      if (!empty($changelog['requirements']['security'])) {
        $message .= '<div class="review-item-message-requirements-security review-item-message-requirements review-item-message-requirements">';
        $message .= '<div class="item-label">Security Fees:</div> ';

        $value = reset($changelog['requirements']['security']);
        $value_markup = mfcs_view_build_request_value_structure($request_id, 'requirements', 'security', 0, $value, $presentation['requirements']['security'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if (!empty($changelog['requirements']['custodial'])) {
        $message .= '<div class="review-item-message-requirements-custodial review-item-message-requirements review-item-message-requirements">';
        $message .= '<div class="item-label">Custodial Fees:</div> ';

        $value = reset($changelog['requirements']['custodial']);
        $value_markup = mfcs_view_build_request_value_structure($request_id, 'requirements', 'custodial', 0, $value, $presentation['requirements']['custodial'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if (!empty($changelog['requirements']['maintenance'])) {
        $message .= '<div class="review-item-message-requirements-maintenance review-item-message-requirements review-item-message-requirements">';
        $message .= '<div class="item-label">Maintenance Fees:</div> ';

        $value = reset($changelog['requirements']['maintenance']);
        $value_markup = mfcs_view_build_request_value_structure($request_id, 'requirements', 'maintenance', 0, $value, $presentation['requirements']['maintenance'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if (!empty($changelog['requirements']['grounds'])) {
        $message .= '<div class="review-item-message-requirements-grounds review-item-message-requirements review-item-message-requirements">';
        $message .= '<div class="item-label">Grounds Fees:</div> ';

        $value = reset($changelog['requirements']['grounds']);
        $value_markup = mfcs_view_build_request_value_structure($request_id, 'requirements', 'grounds', 0, $value, $presentation['requirements']['grounds'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      if ($review_step == MFCS_REVIEW_STEP_VENUE_AVAILABLE && ($decision == MFCS_REVIEW_DECISION_REQUIREMENT || $decision == MFCS_REVIEW_DECISION_WAVE) || ($type == MFCS_REQUEST_TYPE_QUICK_MEETING && $decision == MFCS_REVIEW_DECISION_REQUIREMENT)) {
        if (!empty($changelog['requirements']['other'])) {
          $message .= '<div class="review-item-message-requirements-other review-item-message-requirements review-item-message-requirements">';
          $message .= '<div class="item-label">Other Fees:</div> ';

          // always display the boolean when displayed in the review log.
          foreach ($presentation['requirements']['other']['display'] as &$value) {
            if ($value == 'when_true' || $value == 'when_false') {
              $value = 'raw';
            }
          }

          $value = reset($changelog['requirements']['other']);
          $value_markup = mfcs_view_build_request_value_structure($request_id, 'requirements', 'other', 0, $value, $presentation['requirements']['other'], $changelog);

          if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
            $message .= drupal_render($value_markup);
          }
          $message .= '</div>';
        }

        if (!empty($changelog['requirements']['waived'])) {
          $message .= '<div class="review-item-message-requirements-waived review-item-message-requirements review-item-message-requirements">';
          $message .= '<div class="item-label">Waived with Presidents Approval:</div> ';

          // always display the boolean when displayed in the review log.
          foreach ($presentation['requirements']['waived']['display'] as &$value) {
            if ($value == 'when_true' || $value == 'when_false') {
              $value = 'raw';
            }
          }

          $value = reset($changelog['requirements']['waived']);
          $value_markup = mfcs_view_build_request_value_structure($request_id, 'requirements', 'waived', 0, $value, $presentation['requirements']['waived'], $changelog);

          if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
            $message .= drupal_render($value_markup);
          }
          $message .= '</div>';
        }

        if (!empty($changelog['requirements']['university'])) {
          $message .= '<div class="review-item-message-requirements-university review-item-message-university review-item-message-requirements">';
          $message .= '<div class="item-label">Strictly University Business:</div> ';

          // always display the boolean when displayed in the review log.
          foreach ($presentation['requirements']['university']['display'] as &$value) {
            if ($value == 'when_true' || $value == 'when_false') {
              $value = 'raw';
            }
          }

          $value = reset($changelog['requirements']['university']);
          $value_markup = mfcs_view_build_request_value_structure($request_id, 'requirements', 'university', 0, $value, $presentation['requirements']['university'], $changelog);

          if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
            $message .= drupal_render($value_markup);
          }
          $message .= '</div>';
        }
      }

      // contractor insurance
      if (!empty($changelog['insurance']['contractor'])) {
        $message .= '<div class="review-item-message-insurance-contractor review-item-message-insurance review-item-message-requirements">';
        $message .= '<div class="item-label">Contractor Insurance:</div> ';

        // always display the boolean when displayed in the review log.
        if (!empty($presentation['insurance']['contractor']['display'])) {
          foreach ($presentation['insurance']['contractor']['display'] as &$value) {
            if ($value == 'when_true' || $value == 'when_false') {
              $value = 'raw';
            }
          }
        }

        $value = reset($changelog['insurance']['contractor']);
        $value_markup = mfcs_view_build_request_value_structure($request_id, 'insurance', 'contractor', 0, $value, $presentation['insurance']['contractor'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      // unaffiliated insurance
      if (!empty($changelog['insurance']['unaffiliated'])) {
        $message .= '<div class="review-item-message-insurance-unaffiliated review-item-message-insurance review-item-message-requirements">';
        $message .= '<div class="item-label">Unaffiliated Insurance:</div> ';

        // always display the boolean when displayed in the review log.
        if (!empty($presentation['insurance']['unaffiliated']['display'])) {
          foreach ($presentation['insurance']['unaffiliated']['display'] as &$value) {
            if ($value == 'when_true' || $value == 'when_false') {
              $value = 'raw';
            }
          }
        }

        $value = reset($changelog['insurance']['unaffiliated']);
        $value_markup = mfcs_view_build_request_value_structure($request_id, 'insurance', 'unaffiliated', 0, $value, $presentation['insurance']['unaffiliated'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      // provided insurance
      if (!empty($changelog['insurance']['provided'])) {
        $message .= '<div class="review-item-message-insurance-provided review-item-message-insurance review-item-message-requirements">';
        $message .= '<div class="item-label">Provided Insurance:</div> ';

        // always display the boolean when displayed in the review log.
        if (!empty($presentation['insurance']['provided']['display'])) {
          foreach ($presentation['insurance']['provided']['display'] as &$value) {
            if ($value == 'when_true' || $value == 'when_false') {
              $value = 'raw';
            }
          }
        }

        $value = reset($changelog['insurance']['provided']);
        $value_markup = mfcs_view_build_request_value_structure($request_id, 'insurance', 'provided', 0, $value, $presentation['insurance']['provided'], $changelog);

        if ($value_markup !== FALSE && !isset($value_markup['no_values'])) {
          $message .= drupal_render($value_markup);
        }
        $message .= '</div>';
      }

      $markup .= '<div class="review-item-message">' . $message . '</div>';

      $markup .= '</div>';

      $form['review']['log']['history'][$row] = array(
        '#markup' => $markup,
      );

      $has_log = TRUE;
      unset($changelog);
    }
  }

  if (!$has_log) {
    $form['review']['log']['history']['nothing'] = array(
      '#markup' => 'The review log is empty.',
    );
  }

  return TRUE;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
