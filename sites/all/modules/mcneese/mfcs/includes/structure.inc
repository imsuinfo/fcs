<?php

/**
 * @file
 * Defines McNeese facilities use request commonly used form and structure functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Return a list of sections and their relevant information.
 *
 * Manually updated to reflect mfcs_get_request_fields().
 *
 * @return array
 *   An array containing the generated sections.
 *
 * @see: mfcs_get_request_fields()
 */
function mfcs_get_request_section_information() {
  $information = array();

  $information['request'] = array(
    'steps' => array(
      'location',
      'rooms',
      'standard',
      'details',
    ),
  );

  $information['information'] = array(
    'steps' => array(
      'standard',
    ),
  );

  $information['coordinator'] = array(
    'steps' => array(
      'standard',
    ),
  );

  $information['plans'] = array(
    'steps' => array(
      'details',
    ),
  );

  $information['dates'] = array(
    'steps' => array(
      'standard',
    ),
  );

  $information['registration'] = array(
    'steps' => array(
      'details',
    ),
  );

  $information['setup'] = array(
    'steps' => array(
      'details',
    ),
  );

  $information['presentation'] = array(
    'steps' => array(
      'details',
    ),
  );

  $information['services'] = array(
    'steps' => array(
      'details',
    ),
  );

  $information['top'] = array(
    'steps' => array(
      'details',
    ),
  );

  $information['message'] = array(
    'steps' => array(
      'details',
    ),
  );

  $information['review'] = array(
    'steps' => array(
      'review',
    ),
  );

  $information['requirements'] = array(
    'steps' => array(
      'requirements',
    ),
  );

  return $information;
}

/**
 * Return a list of request form fields.
 *
 * This requires the following fields to be defined before calling:
 * - $form['form']['sections']
 * - $form['form']['step']
 * - $form['form']['existing']
 * - $form['form']['user']
 * - $form['form']['parents']
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @return array
 *   An array containing the generated form.
 */
function mfcs_get_request_fields($form, &$form_state) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');
    return array();
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');
    return array();
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  global $base_path;

  $ps = NULL;

  if (!isset($form_state['values']['request'])) {
    if (isset($form['form']['existing']['#value']['request'])) {
      $form_state['values']['request'] = $form['form']['existing']['#value']['request'];
    }
    else {
      $form_state['values']['request'] = array();
    }
  }
  $request_state = &$form_state['values']['request'];

  $fields = array();
  if (!isset($form['form']['sections']['#value']) || !is_array($form['form']['sections']['#value'])) {
    $form['form']['sections'] = array(
      '#type' => 'value',
      '#value' => mfcs_get_request_section_information(),
    );
  }


  // initialize conditionally_required 3, which defines range limits for each given field.
  $form_state['conditionally_required'][3] = array();


  // Fieldset: Status
  mfcs_get_request_field_status($form, $form_state, $fields, $request_state, $ps);


  // Fieldset: Step
  mfcs_get_request_field_step($form, $form_state, $fields, $request_state, $ps);


  // Fieldset: Location
  mfcs_get_request_field_location($form, $form_state, $fields, $request_state, $ps);


  // Fieldset: Room
  $room = mfcs_get_request_field_room($form, $form_state, $fields, $request_state, $ps);

  $form['form']['room_information'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );

  if (isset($room[0])) {
    $form['form']['room_information']['#value'] = mfcs_load_rooms(NULL, NULL, $room[0]);
  }


  // Fieldset: Building
  mfcs_get_request_field_building($form, $form_state, $fields, $request_state, $ps);


  // Fieldset: Review
  if ($form['form']['step']['#value'] == 'review' && array_key_exists('review', $form['form']['sections']['#value'])) {
    if (empty($form['form']['request']['#value']['mer']['id'][0]->value) || !mfcs_page_request_access('review', $form['form']['request']['#value']['mer']['id'][0]->value)) {
      drupal_access_denied();
      drupal_exit();
      return array();
    }

    $ps = 'review';
    if (!is_null($form['form']['parents']['#value']['path'])) {
      $ps .= ']';
    }

    if (!isset($request_state['review'])) {
      $request_state['review'] = array();
    }

    $fields['review'] = array(
      '#id' => 'section-request-review',
      '#tree' => TRUE,
    );
  }

  // Fieldset: Requirements
  if ($form['form']['step']['#value'] == 'requirements' && array_key_exists('requirements', $form['form']['sections']['#value'])) {
    if (empty($form['form']['request']['#value']['mer']['id'][0]->value) || !mfcs_page_request_access('requirements', $form['form']['request']['#value']['mer']['id'][0]->value)) {
      drupal_access_denied();
      drupal_exit();
      return array();
    }

    $ps = 'requirements';
    if (!is_null($form['form']['parents']['#value']['path'])) {
      $ps .= ']';
    }

    if (!isset($request_state['requirements'])) {
      $request_state['requirements'] = array();
    }

    $fields['requirements'] = array(
      '#id' => 'section-request-requirements',
      '#tree' => TRUE,
    );
  }


  // Fieldset: Dates
  $ps = 'dates';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($request_state['dates'])) {
    $request_state['dates'] = array();
  }

  $fields['dates'] = array(
    '#id' => 'section-request-dates',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('dates', $form['form']['sections']['#value'])) {
    $fields['dates'] = array(
      '#id' => 'section-request-dates',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => 'Date Settings',
    );
  }

  // date field contains different fields grouped togethor with it, process them simultanseously.
  mfcs_get_request_group_dates_date($form, $form_state, $fields, $request_state, $ps);

  // add/remove date fields
  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('dates', $form['form']['sections']['#value'])) {
    $dates = array(0 => NULL);
    if (!empty($request_state['dates']['date'])) {
      $dates = $request_state['dates']['date'];
    }

    if (count($dates) > 1) {
      foreach ($dates as $date_key => $date_value) {
        $fields['dates']['group_dates'][$date_key]['dates']['remove'] = array(
          '#id' => 'buttons-request-remove-date-' . $date_key,
          '#type' => 'container',
          '#attributes' => array(
            'class' => array(
              'buttons-request-remove-date',
            ),
          ),
        );

        $fields['dates']['group_dates'][$date_key]['dates']['remove']['button-' . $date_key] = array(
          '#id' => 'submit-request-remove-date-' . $date_key,
          '#name' => 'submit-request-remove-date-' . $date_key,
          '#type' => 'submit',
          '#default_value' => 'Remove Date',
          '#attributes' => array(
            'class' => array(
              'buttons-request-remove-date-button',
            ),
          ),
        );
      }
    }

    $fields['dates']['markup'] = array(
      '#type' => 'markup',
      '#markup' => '',
    );

    if (isset($form['form']['room_information']['#value']->room_id)) {
      $room_id = (int) $form['form']['room_information']['#value']->room_id;
      $fields['dates']['markup']['#markup'] .= '<div class="dates-holiday_markup">You may want to <a href="' . $base_path . 'requests/holiday-0" target="_blank">view the holidays</a> or <a href="' . $base_path . 'requests/unavailable-0/' . $room_id . '" target="_blank">view the room unavailability</a> when selecting your dates.</div>';
      unset($room_id);
    }

    $fields['dates']['add'] = array(
      '#id' => 'buttons-request-add-date',
      '#type' => 'container',
      '#attributes' => array(
        'class' => array(
          'buttons-request-add-date',
        ),
      ),
    );

    $fields['dates']['add']['button'] = array(
      '#id' => 'submit-request-add-date',
      '#name' => 'submit-request-add-date',
      '#type' => 'submit',
      '#default_value' => 'Add Date',
      '#attributes' => array(
        'class' => array(
          'buttons-request-add-date-button',
        ),
      ),
    );
  }


  // Fieldset: Venue Coordinator
  $ps = 'venue_coordinator';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($request_state['venue_coordinator'])) {
    $request_state['venue_coordinator'] = array();
  }

  $fields['venue_coordinator'] = array(
    '#id' => 'section-request-venue_coordinator',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['venue_coordinator'] = array(
      '#id' => 'section-request-venue_coordinator',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => 'Venue Coordinator Settings',
      '#description' => 'The venue coordinator is either the room coordinator or if there is no room coordinator, then the venue coordinator.',
    );
  }

  mfcs_get_request_field_venue_coordinator_user_id($form, $form_state, $fields, $request_state, $ps);


  // Fieldset: Request Coordinator
  $ps = 'request_coordinator';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($request_state['request_coordinator'])) {
    $request_state['request_coordinator'] = array();
  }

  $fields['request_coordinator'] = array(
    '#id' => 'section-request-request_coordinator',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['request_coordinator'] = array(
      '#id' => 'section-request-request_coordinator',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => 'Request Coordinator Settings',
    );
  }

  mfcs_get_request_field_request_coordinator_id($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_request_coordinator_name($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_request_coordinator_phone($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_request_coordinator_email($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_request_coordinator_classification($form, $form_state, $fields, $request_state, $ps);


  // Fieldset: Information
  $ps = 'information';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($request_state['information'])) {
    $request_state['information'] = array();
  }

  $fields['information'] = array(
    '#id' => 'section-request-information',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['information'] = array(
      '#id' => 'section-request-information',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => 'Request Information',
    );
  }

  $information_title = mfcs_get_request_field_information_title($form, $form_state, $fields, $request_state, $ps);
  $information_attendance = mfcs_get_request_field_information_attendance($form, $form_state, $fields, $request_state, $ps);
  $information_type = mfcs_get_request_field_information_type($form, $form_state, $fields, $request_state, $ps);
  $information_organization = mfcs_get_request_field_information_organization($form, $form_state, $fields, $request_state, $ps);
  $information_adviser_approval = mfcs_get_request_field_information_adviser_approval($form, $form_state, $fields, $request_state, $ps);

  $form['form']['information_type'] = array(
    '#type' => 'value',
    '#value' => $information_type,
  );

  $form['form']['information_type_meeting'] = array(
    '#type' => 'value',
    '#value' => FALSE,
  );

  $information_type_value =MFCS_REQUEST_TYPE_NONE;
  if (isset($information_type[0]) && is_numeric($information_type[0])) {
    $information_type_value = (int) $information_type[0];
  }

  if ($information_type_value === MFCS_REQUEST_TYPE_MEETING || $information_type_value === MFCS_REQUEST_TYPE_QUICK_MEETING || $information_type_value === MFCS_REQUEST_TYPE_CLUB_MEETING) {
    $form['form']['information_type_meeting']['#value'] = TRUE;
  }
  unset($information_type_value);


  // Fieldset: Plans
  $ps = 'plans';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($request_state['plans'])) {
    $request_state['plans'] = array();
  }

  $fields['plans'] = array(
    '#id' => 'section-request-plans',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('plans', $form['form']['sections']['#value'])) {
    $fields['plans'] = array(
      '#id' => 'section-request-plans',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => 'Request Plans',
    );
  }

  mfcs_get_request_field_plans_note($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_plans_description($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_plans_activities($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_plans_audience($form, $form_state, $fields, $request_state, $ps);


  // Fieldset: Registration
  $ps = 'registration';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($request_state['registration'])) {
    $request_state['registration'] = array();
  }

  $fields['registration'] = array(
    '#id' => 'section-request-registration',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
      $fields['registration'] = array(
        '#id' => 'section-request-registration',
        '#type' => 'fieldset',
        '#tree' => TRUE,
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#title' => 'Request Registration',
      );
    }
  }

  mfcs_get_request_field_registration_require($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_registration_website($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_registration_phone($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_registration_ticket_price($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_registration_ticket_dates($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_registration_ticket_website($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_registration_ticket_phone($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_registration_generate_revenue($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_registration_revenue_generated($form, $form_state, $fields, $request_state, $ps);


  // Fieldset: Setup
  $ps = 'setup';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($request_state['setup'])) {
    $request_state['setup'] = array();
  }

  $fields['setup'] = array(
    '#id' => 'section-request-setup',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup'] = array(
      '#id' => 'section-request-setup',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => 'Request Setup',
    );
  }

  mfcs_get_request_field_setup_rectangular_tables_8ft($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_setup_round_tables_8ft($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_setup_other_tables($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_setup_standard_blue_chairs($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_setup_podium($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_setup_portable_stage($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_setup_portable_stage_configuration($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_setup_security($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_setup_parking_assistance($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_setup_parking_assistance_area($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_setup_road_closures($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_setup_road_closures_details($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_setup_special_requests($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_setup_special_requests_details($form, $form_state, $fields, $request_state, $ps);


  // Fieldset: Presentation
  $ps = 'presentation';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($request_state['presentation'])) {
    $request_state['presentation'] = array();
  }

  $fields['presentation'] = array(
    '#id' => 'section-request-presentation',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation'] = array(
      '#id' => 'section-request-presentation',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => 'Request Presentation',
    );
  }

  // mising: external_audio_person  // if (!$meeting_request) {
  mfcs_get_request_field_presentation_technical_equipment($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_technical_equipment_note($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_technical_equipment_details($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_technical_equipment_microphone($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_external_audio_person($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_external_audio_person_name($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_external_audio_person_email($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_external_audio_person_phone($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_production($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_production_name($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_production_email($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_production_phone($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_publicity($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_publicity_details($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_printed_material($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_university_logo($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_designing_material($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_designing_material_name($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_designing_material_email($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_presentation_designing_material_phone($form, $form_state, $fields, $request_state, $ps);


  // Fieldset: Services
  $ps = 'services';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($request_state['services'])) {
    $request_state['services'] = array();
  }

  $fields['services'] = array(
    '#id' => 'section-request-services',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('services', $form['form']['sections']['#value'])) {
    $fields['services'] = array(
      '#id' => 'section-request-services',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => 'Request Services',
    );
  }

  mfcs_get_request_field_services_food_served($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_services_food_caterer($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_services_alcohol($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_services_alcohol_note($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_services_open_flames($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_services_open_flames_note($form, $form_state, $fields, $request_state, $ps);


  // Fieldset: Additional
  $ps = 'top';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }
  mfcs_get_request_field_additional($form, $form_state, $fields, $request_state, $ps);


  if ($form['form']['step']['#value'] == 'details' && array_key_exists('message', $form['form']['sections']['#value'])) {
    $fields['message'] = array(
      '#markup' => '',
    );

    $fields['message']['#markup'] .= '<div id="mfcs-markup-request-message" class="markup-request">';
    $fields['message']['#markup'] .= 'Once your request has been approved appropriate campus personnel, you will be emailed with the outcome. ';
    $fields['message']['#markup'] .= 'If the request receives approval, you will also receive an itemized detail of any and all charges that will be assessed to you.' . '<br>';
    $fields['message']['#markup'] .= '<strong>You must agree and accept the terms offered for the venue to be reserved.</strong> ';
    $fields['message']['#markup'] .= '</div>';
  }


  // Fieldset: Hidden Fields
  mfcs_get_request_field_insurance_affiliated($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_insurance_contractor($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_insurance_provided($form, $form_state, $fields, $request_state, $ps);
  mfcs_get_request_field_insurance_unaffiliated($form, $form_state, $fields, $request_state, $ps);

  return $fields;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_status($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => MFCS_REQUEST_STATUS_UNLOCKED);

  if (empty($request_state['status'])) {
    if (isset($form['form']['existing']['#value']['request']['status'])) {
      $form['form']['status']['#value'] = $form['form']['existing']['#value']['request']['status'];
    }
  }
  else {
    $values = $request_state['status'];
  }

  $limits_check = array(
    'title' => 'Status',
    'field' => 'status',
    'group' => 'top',
    'path' => 'status',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($form['form']['step']['#value'] == 'status' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['status'] = array(
      '#id' => 'field-request-status',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-status',
        ),
      ),
    );

    $limits_check['visible'] = TRUE;
  }

  foreach ($values as $key => $value) {
    $fields['status'][$key] = array(
      '#id' => 'field-request-status-' . $key,
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_step($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => MFCS_REVIEW_STEP_VENUE_AVAILABLE);

  if (empty($request_state['step'])) {
    if (isset($form['form']['existing']['#value']['request']['step'])) {
      $form['form']['step']['#value'] = $form['form']['existing']['#value']['request']['step'];
    }
    else {
      $form['form']['step']['#value'] = array(MFCS_REVIEW_STEP_VENUE_AVAILABLE);
    }
  }
  else {
    $values = $request_state['step'];
  }

  $limits_check = array(
    'title' => 'Step',
    'field' => 'step',
    'group' => 'top',
    'path' => 'step',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($form['form']['step']['#value'] == 'step' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['step'] = array(
      '#id' => 'field-request-step',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-step',
        ),
      ),
    );

    $limits_check['visible'] = TRUE;
  }

  foreach ($values as $key => $value) {
    $fields['step'][$key] = array(
      '#id' => 'field-request-step-' . $key,
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_location($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['location'])) {
    $values = $request_state['location'];
  }

  // do not allow certain fields to be editable when a request is completed, even by admins (in this case there is a move operation to change location, building, and room).
  $readonly = FALSE;
  if (isset($form['form']['operation']['#value']) && $form['form']['operation']['#value'] === 'edit' && isset($form['form']['existing']['#value'])) {
    if (isset($form['form']['existing']['#value']['request']['step'][0]) && is_numeric($form['form']['existing']['#value']['request']['step'][0])) {
      if ((int)$form['form']['existing']['#value']['request']['step'][0] === MFCS_REVIEW_STEP_COMPLETED) {
        $readonly = TRUE;
      }
    }
  }

  $form['form']['operation'] = array(
    '#type' => 'value',
    '#value' => 'create',
  );

  $field_title = 'Please indicate the requested facility or request location?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'location',
    'group' => 'top',
    'path' => 'location',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($form['form']['step']['#value'] == 'location' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['location'] = array(
      '#id' => 'field-request-location',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-location',
        ),
      ),
    );

    global $base_path;
    $module_path = drupal_get_path('module', 'mfcs');

    drupal_add_js($module_path . '/js/images-locations.js', array('type' => 'file', 'group' => JS_DEFAULT, 'preprocess' => TRUE));

    foreach ($values as $key => $value) {
      $fields['location'][$key] = array(
        '#id' => 'field-request-location-' . $key,
        '#type' => 'radios',
        '#title' => '<span class="strong underline">' . $field_title . '</span>',
        '#options' => mfcs_get_request_location_list_options('radios'),
        '#default_value' => $value,
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#description' => 'If you are unsure of which location to choose, then please contact the administration or your adviser.',
      );

      if ($readonly) {
        $fields['location'][$key]['#disabled'] = TRUE;
        $fields['location'][$key]['#attributes']['readonly'] = 'readonly';
      }
    }
    krsort($fields['location']);

    $form_state['conditionally_required'][0][] = 'location';

    $fields['location'][0]['location-images'] = array(
      '#tree' => TRUE,
    );

    $fields['location'][0]['location-images'][0] = array();
    $fields['location'][0]['location-images'][1] = array();

    $fields['location'][0]['location-images'][0]['#markup'] = '<a href="//www.mcneese.edu/node/5530" class="location-image-link" title="View Campus Map" target="_blank"><img id="field-request-location-0-image-0" src="' . $base_path . $module_path . '/images/example-map.png" alt="Campus Map" class="location-image location-image-visible noscript"></a>';
    $fields['location'][0]['location-images'][1]['#markup'] = '<a href="//www.mcneese.edu/node/5530" class="location-image-link" title="View Campus Map" target="_blank"><img id="field-request-location-0-image-1" src="' . $base_path . $module_path . '/images/example-seed.png" alt="Campus Map, SEED Center" class="location-image location-image-invisible noscript"></a>';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['location'][$key] = array(
        '#id' => 'field-request-location-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_building($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (isset($form['form']['room_information']['#value']->building_id)) {
    $values[0] = $form['form']['room_information']['#value']->building_id;
  }

  // do not allow certain fields to be editable when a request is completed, even by admins (in this case there is a move operation to change location, building, and room).
  $readonly = FALSE;
  if (isset($form['form']['operation']['#value']) && $form['form']['operation']['#value'] === 'edit' && isset($form['form']['existing']['#value'])) {
    if (isset($form['form']['existing']['#value']['request']['step'][0]) && is_numeric($form['form']['existing']['#value']['request']['step'][0])) {
      if ((int)$form['form']['existing']['#value']['request']['step'][0] === MFCS_REVIEW_STEP_COMPLETED) {
        $readonly = TRUE;
      }
    }
  }

  $limits_check = array(
    'title' => 'Building',
    'field' => 'building',
    'group' => 'top',
    'path' => 'building',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($form['form']['step']['#value'] == 'building' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['building'] = array(
      '#id' => 'field-request-building',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-building',
        ),
      ),
    );

    if ($readonly) {
      $fields['building']['#disabled'] = TRUE;
      $fields['building']['#attributes']['readonly'] = 'readonly';
    }

    $limits_check['visible'] = TRUE;
  }

  // the building is determined by the room, therefore this is more or less a stub/placeholder.
  foreach ($values as $key => $value) {
    $fields['building'][$key] = array(
      '#id' => 'field-request-building-' . $key,
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_room($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['room'])) {
    $values = $request_state['room'];
  }

  // do not allow certain fields to be editable when a request is completed, even by admins (in this case there is a move operation to change location, building, and room).
  $readonly = FALSE;
  if (isset($form['form']['operation']['#value']) && $form['form']['operation']['#value'] === 'edit' && isset($form['form']['existing']['#value'])) {
    if (isset($form['form']['existing']['#value']['request']['step'][0]) && is_numeric($form['form']['existing']['#value']['request']['step'][0])) {
      if ((int)$form['form']['existing']['#value']['request']['step'][0] === MFCS_REVIEW_STEP_COMPLETED) {
        $readonly = TRUE;
      }
    }
  }

  $field_title = 'Select a Room';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'room',
    'group' => 'top',
    'path' => 'room',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'rooms' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['room'] = array(
      '#id' => 'field-request-room',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-room',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $location = '';
      if (!empty($request_state['location'][$key])) {
        $location = $request_state['location'][$key];
      }

      $options = mfcs_get_request_room_list_options($location, 'special');
      if (!is_array($options)) {
        $options = array();
        $value = NULL;
      }
      elseif (!array_key_exists($value, $options)) {
        $value = NULL;
      }

      $fields['room'][$key] = array(
        '#id' => 'field-request-room-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#options' => $options,
        '#default_value' => $value,
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );

      if ($readonly) {
        $fields['room'][$key]['#disabled'] = TRUE;
        $fields['room'][$key]['#attributes']['readonly'] = 'readonly';
      }
    }
    krsort($fields['room']);

    $form_state['conditionally_required'][0][] = 'room';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['room'][$key] = array(
        '#id' => 'field-request-room-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_group_dates_date($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['dates']['date'])) {
    $values = $request_state['dates']['date'];
  }

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('dates', $form['form']['sections']['#value'])) {
    $fields['dates']['group_dates'] = array(
      '#id' => 'group-request-dates-date',
      '#type' => 'container',
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'group-request-item',
          'group-request-item-dates-date',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['dates']['group_dates'][$key] = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array(
            'group-field-container',
            'group-field-container-' . $key,
          ),
        ),
      );

      $new_ps = $ps . '[group_dates][' . $key . ']';

      mfcs_get_request_field_dates_date($form, $form_state, $fields['dates']['group_dates'][$key], $request_state, $new_ps, $key);
      mfcs_get_request_field_dates_time_start($form, $form_state, $fields['dates']['group_dates'][$key], $request_state, $new_ps, $key);
      mfcs_get_request_field_dates_time_stop($form, $form_state, $fields['dates']['group_dates'][$key], $request_state, $new_ps, $key);
      mfcs_get_request_field_dates_category($form, $form_state, $fields['dates']['group_dates'][$key], $request_state, $new_ps, $key);
    }
  }
  else {
    foreach ($values as $key => $value) {
      $new_ps = $ps . '[group_dates][' . $key . ']';

      mfcs_get_request_field_dates_date($form, $form_state, $fields['dates']['group_dates'][$key], $request_state, $new_ps, $key);
      mfcs_get_request_field_dates_time_start($form, $form_state, $fields['dates']['group_dates'][$key], $request_state, $new_ps, $key);
      mfcs_get_request_field_dates_time_stop($form, $form_state, $fields['dates']['group_dates'][$key], $request_state, $new_ps, $key);
      mfcs_get_request_field_dates_category($form, $form_state, $fields['dates']['group_dates'][$key], $request_state, $new_ps, $key);
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_dates_date($form, &$form_state, &$fields, &$request_state, $ps, $delta = NULL) {
  $values = array(0 => NULL);
  if (!empty($request_state['dates']['date'])) {
    $values = $request_state['dates']['date'];
  }

  $delta_id = NULL;
  if (is_numeric($delta)) {
    $values = array($delta => NULL);

    if (!empty($request_state['dates']['date'][$delta])) {
      $values[$delta] = $request_state['dates']['date'][$delta];
    }

    $delta_id = '-delta_' . $delta;
  }

  $field_title = 'Request Date';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'date',
    'group' => 'date',
    'path' => 'dates][date',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('dates', $form['form']['sections']['#value'])) {
    $fields['dates']['date'] = array(
      '#id' => 'field-request-dates-date' . $delta_id,
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-dates-date',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['dates']['date'][$key] = array(
        '#id' => 'field-request-dates-date-' . $key,
        '#type' => 'date_popup',
        '#title' => $field_title,
        '#default_value' => $value,
        '#date_format' => 'Y/m/d',
        '#date_label_position' => 'invisible',
        '#date_year_range' => '0:+4',
        '#expected_format' => array('date' => 'Y/m/d'),
        '#required' => TRUE,
        '#weight' => 0,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#parents' => array('request', 'dates', 'date', $delta),
      );
    }
    krsort($fields['dates']['date']);

    if (is_numeric($delta)) {
      $form_state['conditionally_required'][0][] = $ps . '[dates][date';
    }
    else {
      $form_state['conditionally_required'][0][] = 'dates][date';
    }

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['dates']['date'][$key] = array(
        '#id' => 'field-request-dates-date-' . $key,
        '#type' => 'value',
        '#value' => $value,
        '#parents' => array('request', 'dates', 'date', $delta),
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_dates_time_start($form, &$form_state, &$fields, &$request_state, $ps, $delta = NULL) {
  $values = array(0 => NULL);
  if (!empty($request_state['dates']['time_start'])) {
    $values = $request_state['dates']['time_start'];
  }

  $delta_id = NULL;
  if (is_numeric($delta)) {
    $values = array($delta => NULL);

    if (!empty($request_state['dates']['time_start'][$delta])) {
      $values[$delta] = $request_state['dates']['time_start'][$delta];
    }

    $delta_id = '-delta_' . $delta;
  }

  $field_title = 'Request Start Time';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'time_start',
    'group' => 'date',
    'path' => 'dates][time_start',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('dates', $form['form']['sections']['#value'])) {
    $fields['dates']['time_start'] = array(
      '#id' => 'field-request-dates-time_start' . $delta_id,
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-dates-time_start',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['dates']['time_start'][$key] = array(
        '#id' => 'field-request-dates-time_start-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 8,
        '#maxlength' => 8,
        '#expected_format' => array('date' => MFCS_DATE_FORMAT_TIME),
        '#description' => 'E.g, 12:31pm',
        '#weight' => 1,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#parents' => array('request', 'dates', 'time_start', $delta),
      );
    }
    krsort($fields['dates']['time_start']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['dates']['time_start'][$key] = array(
        '#id' => 'field-request-dates-time_start-' . $key,
        '#type' => 'value',
        '#value' => $value,
        '#parents' => array('request', 'dates', 'time_start', $delta),
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_dates_time_stop($form, &$form_state, &$fields, &$request_state, $ps, $delta = NULL) {
  $values = array(0 => NULL);
  if (!empty($request_state['dates']['time_stop'])) {
    $values = $request_state['dates']['time_stop'];
  }

  $delta_id = NULL;
  if (is_numeric($delta)) {
    $values = array($delta => NULL);

    if (!empty($request_state['dates']['time_stop'][$delta])) {
      $values[$delta] = $request_state['dates']['time_stop'][$delta];
    }

    $delta_id = '-delta_' . $delta;
  }

  $field_title = 'Request Stop Time';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'time_stop',
    'group' => 'date',
    'path' => 'dates][time_stop',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('dates', $form['form']['sections']['#value'])) {
    $fields['dates']['time_stop'] = array(
      '#id' => 'field-request-dates-time_stop' . $delta_id,
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-dates-time_stop',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['dates']['time_stop'][$key] = array(
        '#id' => 'field-request-dates-time_stop-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 8,
        '#maxlength' => 8,
        '#expected_format' => array('date' => MFCS_DATE_FORMAT_TIME),
        '#description' => 'E.g, 12:31pm',
        '#weight' => 2,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#parents' => array('request', 'dates', 'time_stop', $delta),
      );
    }
    krsort($fields['dates']['time_stop']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['dates']['time_stop'][$key] = array(
        '#id' => 'field-request-dates-time_stop-' . $key,
        '#type' => 'value',
        '#value' => $value,
        '#parents' => array('request', 'dates', 'time_stop', $delta),
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_dates_category($form, &$form_state, &$fields, &$request_state, $ps, $delta = NULL) {
  $values = array(0 => NULL);
  if (!empty($request_state['dates']['category'])) {
    $values = $request_state['dates']['category'];
  }

  $delta_id = NULL;
  if (is_numeric($delta)) {
    $values = array($delta => NULL);

    if (!empty($request_state['dates']['category'][$delta])) {
      $values[$delta] = $request_state['dates']['category'][$delta];
    }

    $delta_id = '-delta_' . $delta;
  }

  $field_title = 'Category';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'category',
    'group' => 'date',
    'path' => 'dates][category',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('dates', $form['form']['sections']['#value'])) {
    $fields['dates']['category'] = array(
      '#id' => 'field-request-dates-category' . $delta_id,
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-dates-category',
        ),
      ),
    );

    $dates_category_options = mfcs_get_dates_category_list_options('radios');

    foreach ($values as $key => $value) {
      $fields['dates']['category'][$key] = array(
        '#id' => 'field-request-dates-category-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#required' => TRUE,
        '#default_value' => $value,
        '#options' => $dates_category_options,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#parents' => array('request', 'dates', 'category', $delta),
      );
    }
    krsort($fields['dates']['category']);

    if (is_numeric($delta)) {
      $form_state['conditionally_required'][0][] = $ps . '[dates][category';
    }
    else {
      $form_state['conditionally_required'][0][] = 'dates][category';
    }

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['dates']['category'][$key] = array(
        '#id' => 'field-request-dates-category-' . $key,
        '#type' => 'value',
        '#value' => $value,
        '#parents' => array('request', 'dates', 'category', $delta),
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_venue_coordinator_user_id($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);

  // never assign default here because the default is inforced only after the room is selected, not before.
  if (!empty($request_state['venue_coordinator']['user_id'][0])) {
    if (!empty($request_state['venue_coordinator']['user_id']) && array_key_exists(0, $request_state['venue_coordinator']['user_id'])) {
      $values[0] = $request_state['venue_coordinator']['user_id'][0];
    }
  }

  $fields['venue_coordinator']['user_id'] = array(
    '#tree' => TRUE,
  );

  $fields['venue_coordinator']['user_id'][0] = array(
    '#id' => 'field-request-venue_coordinator-user-0',
    '#type' => 'value',
    '#value' => $values[0],
  );

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_request_coordinator_id($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);

  if (isset($request_state['request_coordinator']) && array_key_exists('user_id', $request_state['request_coordinator'])) {
    $values = $request_state['request_coordinator']['user_id'];
  }
  else {
    if (!empty($form['form']['user']['#value']->uid)) {
      $values[0] = (int) $form['form']['user']['#value']->uid;
    }
  }

  foreach ($values as $key => $value) {
    $fields['request_coordinator']['user_id'][$key] = array(
      '#id' => 'field-request-request_coordinator-user_id-' . $key,
      '#type' => 'value',
      '#value' => empty($value) ? NULL : (int) $value,
    );
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_request_coordinator_name($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);

  if (isset($request_state['request_coordinator']) && array_key_exists('name', $request_state['request_coordinator'])) {
    $values = $request_state['request_coordinator']['name'];
  }
  else {
    if (isset($form['form']['user']['#value']) && is_object($form['form']['user']['#value'])) {
      $values[0] = mfcs_generate_form_user_name($form['form']['user']['#value']);
      if ($values[0] === FALSE) {
        $values[0] = NULL;
      }
    }
  }

  $field_title = 'Request Coordinator Name';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'name',
    'group' => 'request_coordinator',
    'path' => 'request_coordinator][name',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['request_coordinator']['name'] = array(
      '#id' => 'field-request-request_coordinator-name',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-request_coordinator-name',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['request_coordinator']['name'][$key] = array(
        '#id' => 'field-request-request_coordinator-name-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#required' => TRUE,
        '#size' => 48,
        '#maxlength' => 255,
        '#readonly' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#autocomplete_path' => 'requests/create-0/autocomplete/request_coordinator/name',
      );
    }
    krsort($fields['request_coordinator']['name']);

    $form_state['conditionally_required'][0][] = 'request_coordinator][name';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['request_coordinator']['name'][$key] = array(
        '#id' => 'field-request-request_coordinator-name-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_request_coordinator_phone($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);

  if (isset($request_state['request_coordinator']) && array_key_exists('phone', $request_state['request_coordinator'])) {
    $values = $request_state['request_coordinator']['phone'];
  }
  else {
    if (!empty($form['form']['user']['#value']->mfcs['phone_number'])) {
      $values[0] = check_plain($form['form']['user']['#value']->mfcs['phone_number']);
    }
  }

  $field_title = 'Request Coordinator Phone';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'phone',
    'group' => 'request_coordinator',
    'path' => 'request_coordinator][phone',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['request_coordinator']['phone'] = array(
      '#id' => 'field-request-request_coordinator-phone',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-request_coordinator-phone',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['request_coordinator']['phone'][$key] = array(
        '#id' => 'field-request-request_coordinator-phone-' . $key,
        '#type' => 'telfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#autocomplete_path' => 'requests/create-0/autocomplete/request_coordinator/phone',
      );
    }
    krsort($fields['request_coordinator']['phone']);

    if (!empty($request_state['request_coordinator']['phone'])) {
      $values = $request_state['request_coordinator']['phone'];
    }

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['request_coordinator']['phone'][$key] = array(
        '#id' => 'field-request-request_coordinator-phone-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_request_coordinator_email($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);

  if (isset($request_state['request_coordinator']) && array_key_exists('email', $request_state['request_coordinator'])) {
    $values = $request_state['request_coordinator']['email'];
  }
  else {
    if (!empty($form['form']['user']['#value']->mail)) {
      $values[0] = $form['form']['user']['#value']->mail;
    }
  }

  $field_title = 'Request Coordinator E-Mail';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'email',
    'group' => 'request_coordinator',
    'path' => 'request_coordinator][email',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['request_coordinator']['email'] = array(
      '#id' => 'field-request-request_coordinator-email',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-request_coordinator-email',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['request_coordinator']['email'][$key] = array(
        '#id' => 'field-request-request_coordinator-email-' . $key,
        '#type' => 'emailfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 48,
        '#maxlength' => 255,
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#autocomplete_path' => 'requests/create-0/autocomplete/request_coordinator/email',
      );
    }
    krsort($fields['request_coordinator']['email']);

    $form_state['conditionally_required'][0][] = 'request_coordinator][email';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['request_coordinator']['email'][$key] = array(
        '#id' => 'field-request-request_coordinator-email-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_request_coordinator_classification($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['request_coordinator']['classification'])) {
    $values = $request_state['request_coordinator']['classification'];
  }

  $edit_operation = FALSE;
  if (isset($form['form']['operation']['#value']) && $form['form']['operation']['#value'] === 'edit') {
    $edit_operation = TRUE;
  }

  $user = NULL;
  if (isset($form['form']['user']['#value']) && is_object($form['form']['user']['#value'])) {
    $user = $form['form']['user']['#value'];
  }

  $readonly = FALSE;

  // when editing a request, if an existing classification is unavailable to the user, it needs to be preserved even if that user is normally not allowed access to that classification.
  $existing_option = NULL;
  if (isset($form['form']['existing']['#value']['request']['request_coordinator']['classification'][0]) && is_numeric($form['form']['existing']['#value']['request']['request_coordinator']['classification'][0])) {
    $existing_option = (int) $form['form']['existing']['#value']['request']['request_coordinator']['classification'][0];
  }

  // check to see if the user is a venue coordinator or a venue coordinator proxy.
  $proxies = mfcs_get_proxy_venue_coordinator($user->uid);
  $is_venue_coordinator_ids = array();
  if (!empty($proxies)) {
    $is_venue_coordinator_ids = $proxies;
  }
  $is_venue_coordinator_ids[$user->uid] = $user->uid;

  $is_venue_coordinator = FALSE;
  $venue_coordinator_id = NULL;
  if (!empty($request_state['venue_coordinator']['user_id'][0])) {
    if (!empty($request_state['venue_coordinator']['user_id']) && array_key_exists(0, $request_state['venue_coordinator']['user_id'])) {
      $venue_coordinator_id = $request_state['venue_coordinator']['user_id'][0];
    }
  }
  if (!is_null($venue_coordinator_id)) {
    $is_venue_coordinator = (in_array($venue_coordinator_id, $is_venue_coordinator_ids));
  }
  unset($is_venue_coordinator_ids);
  unset($venue_coordinator_id);

  if ($is_venue_coordinator) {
    $classification = mfcs_get_request_classification_list_options('select');
  }
  else {
    $classification = mfcs_get_request_classification_list_options('select', $user);
  }

  $classification_count = count($classification);
  if ($classification_count == 2) {
    unset($classification[NULL]);
    $classification_count = 1;
  }
  elseif ($classification_count < 2) {
    unset($classification[NULL]);
    $classification_count = 0;
  }

  $classification_all = mfcs_get_request_classification_list_options(NULL, NULL, TRUE, TRUE);

  if (!is_null($existing_option) && !array_key_exists($existing_option, $classification)) {
    $classification[$existing_option] = $classification_all[$existing_option];
    $classification_count++;

    // if the existing type is not a type the current user is allowed to assign, deny access to change unless that user is a manager or an administer.
    $is_manager = FALSE;
    $is_administer = FALSE;
    if (is_object($user)) {
      $is_manager = user_access('mfcs manage', $user);
      $is_administer = user_access('mfcs administer', $user);
    }

    if (!($is_manager || $is_administer)) {
      $readonly = TRUE;
    }
  }

  // if the user classification has changed during the request process, attempt to reset the classification.
  if (isset($values[0]) && is_numeric($values[0])) {
    if (!array_key_exists($values[0], $classification)) {
      if ($classification_count == 1) {
        $classification_keys = array_keys($classification);
        $values[0] = reset($classification_keys);
        unset($classification_keys);
      }
      else {
        $values[0] = NULL;
      }
    }
  }

  $field_title = 'Request Coordinator Classification';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'classification',
    'group' => 'request_coordinator',
    'path' => 'request_coordinator][classification',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['request_coordinator']['classification'] = array(
      '#id' => 'field-request-request_coordinator-classification',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-request_coordinator-classification',
        ),
      ),
    );

    // multiple classifications exists, so prompt user for desired classification.
    if ($classification_count > 1) {
      foreach ($values as $key => $value) {
        $fields['request_coordinator']['classification'][$key] = array(
          '#id' => 'field-request-request_coordinator-classification-' . $key,
          '#type' => 'select',
          '#title' => $field_title,
          '#description' => 'Associate this request with a specific classification.',
          '#options' => $classification,
          '#default_value' => $value,
          '#required' => TRUE,
          '#attributes' => array(
            'class' => array(
              'form-item-delta',
              'form-item-delta-' . $key,
            ),
          ),
        );

        if ($readonly) {
          $fields['request_coordinator']['classification'][$key]['#disabled'] = TRUE;
          $fields['request_coordinator']['classification'][$key]['#attributes']['readonly'] = 'readonly';
        }
      }
      krsort($fields['request_coordinator']['classification']);

      $form_state['conditionally_required'][0][] = 'request_coordinator][classification';

      $limits_check['visible'] = TRUE;
    }
    else {
      foreach ($values as $key => $value) {
        // if value is not defined and only one classification is possible, make that one classification the value.
        if (is_null($value)) {
          $classification_keys = array_keys($classification);
          $value = reset($classification_keys);
          unset($classification_keys);
        }

        $fields['request_coordinator']['classification'][$key] = array(
          '#id' => 'field-request-request_coordinator-classification-' . $key,
          '#type' => 'select',
          '#title' => 'Request Coordinator Classification',
          '#options' => $classification,
          '#default_value' => $value,
          '#value' => $value,
          '#readonly' => TRUE,
          '#attributes' => array('readonly' => 'readonly'),
          '#required' => TRUE,
          '#attributes' => array(
            'class' => array(
              'form-item-delta',
              'form-item-delta-' . $key,
            ),
            'readonly' => 'readonly',
          ),
        );

        if ($readonly) {
          $fields['request_coordinator']['classification'][$key]['#disabled'] = TRUE;
        }
      }

      $form_state['conditionally_required'][0][] = 'request_coordinator][classification';
    }
  }
  else {
    foreach ($values as $key => $value) {
      $fields['request_coordinator']['classification'][$key] = array(
        '#id' => 'field-request-request_coordinator-classification-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_information_title($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['information']['title'])) {
    $values = $request_state['information']['title'];
  }

  $field_title = 'Request Name'; // renamed from title to "Request Name" as per fcs-23.
  $limits_check = array(
    'title' => $field_title,
    'field' => 'title',
    'group' => 'information',
    'path' => 'information][title',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['information']['title'] = array(
      '#id' => 'field-request-information-title',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-information-title',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['information']['title'][$key] = array(
        '#id' => 'field-request-information-title-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#required' => TRUE,
        '#size' => 48,
        '#maxlength' => 255,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#description' => 'The Name of the Event or Meeting.',
      );
    }
    krsort($fields['information']['title']);

    $form_state['conditionally_required'][0][] = 'information][title';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['information']['title'][$key] = array(
        '#id' => 'field-request-information-title-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_information_attendance($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['information']['attendance'])) {
    $values = $request_state['information']['attendance'];
  }

  $field_title = 'Expected Number of Attendance';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'attendance',
    'group' => 'information',
    'path' => 'information][attendance',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['information']['attendance'] = array(
      '#id' => 'field-request-information-attendance',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-information-attendance',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['information']['attendance'][$key] = array(
        '#id' => 'field-request-information-attendance-' . $key,
        '#type' => 'numberfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#min' => 0,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#states' => array(
          'invisible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . 'information][type]' . '[' . $key . ']'  . '"]' => array('value' => MFCS_REQUEST_TYPE_QUICK_MEETING),
          ),
        ),
      );

      if (!empty($form['form']['room_information']['#value']->capacity_normal)) {
        $fields['information']['attendance'][$key]['#max'] = (int) $form['form']['room_information']['#value']->capacity_normal;
        $fields['information']['attendance'][$key]['#description'] = 'Current room has a max attendance of ' . $fields['information']['attendance'][$key]['#max'] . '.';
      }
    }
    krsort($fields['information']['attendance']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['information']['attendance'][$key] = array(
        '#id' => 'field-request-information-attendance-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_information_type($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['information']['type'])) {
    $values = $request_state['information']['type'];
  }

  $edit_operation = FALSE;
  if (isset($form['form']['operation']['#value']) && $form['form']['operation']['#value'] === 'edit') {
    $edit_operation = TRUE;
  }

  $readonly = FALSE;

  $field_title = 'Type of Request';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'type',
    'group' => 'information',
    'path' => 'information][type',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['information']['type'] = array(
      '#id' => 'field-request-information-type',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-information-type',
        ),
      ),
    );

    $type_options = mfcs_get_request_type_list_options('select');
    $type_options_all = mfcs_get_request_type_list_options(NULL, TRUE, TRUE);

    // when editing a request, if an existing type is unavailable to the user, it needs to be preserved even if that user is normally not allowed access to that type.
    $existing_option = NULL;
    if (isset($form['form']['existing']['#value']['request']['information']['type'][0]) && is_numeric($form['form']['existing']['#value']['request']['information']['type'][0])) {
      $existing_option = (int) $form['form']['existing']['#value']['request']['information']['type'][0];

      if ($edit_operation && !array_key_exists($existing_option, $type_options)) {
        $type_options[$existing_option] = $type_options_all[$existing_option];

        // if the existing type is not a type the current user is allowed to assign, deny access to change unless that user is a manager or an administer.
        $is_manager = FALSE;
        $is_administer = FALSE;
        if (isset($form['form']['user']['#value']) && is_object($form['form']['user']['#value'])) {
          $is_manager = user_access('mfcs manage', $form['form']['user']['#value']);
          $is_administer = user_access('mfcs administer', $form['form']['user']['#value']);
        }

        if (!($is_manager || $is_administer)) {
          $readonly = TRUE;
        }
      }
    }

    // quick meetings can only be used by venue coordinators and their proxies.
    $venue_coordinator_ids = array();
    if (!empty($form['form']['user']['#value']->uid)) {
      $proxies = mfcs_get_proxy_venue_coordinator($form['form']['user']['#value']->uid);
      if (!empty($proxies)) {
        $venue_coordinator_ids = $proxies;
      }

      $venue_coordinator_ids[$form['form']['user']['#value']->uid] = $form['form']['user']['#value']->uid;
      unset($proxies);
    }

    if (empty($fields['venue_coordinator']['user_id'][0]['#value']) || !in_array($fields['venue_coordinator']['user_id'][0]['#value'], $venue_coordinator_ids)) {
      if ($edit_operation && $existing_option === MFCS_REQUEST_TYPE_QUICK_MEETING) {
        // if the existing type is not a type the current user is allowed to assign, deny access to change unless that user is a manager or an administer.
        $is_manager = FALSE;
        $is_administer = FALSE;
        if (isset($form['form']['user']['#value']) && is_object($form['form']['user']['#value'])) {
          $is_manager = user_access('mfcs manage', $form['form']['user']['#value']);
          $is_administer = user_access('mfcs administer', $form['form']['user']['#value']);
        }

        if (!($is_manager || $is_administer)) {
          $readonly = TRUE;
        }
      }
      else {
        unset($type_options[MFCS_REQUEST_TYPE_QUICK_MEETING]);
      }
    }

    if (!isset($fields['request_coordinator']['classification'][0]['#value']) || $fields['request_coordinator']['classification'][0]['#value'] != MFCS_REQUEST_CLASSIFICATION_STUDENT) {
      if (!$edit_operation || $existing_option !== MFCS_REQUEST_TYPE_CLUB_MEETING) {
        if (!isset($fields['request_coordinator']['classification'][0]['#options']) || !is_array($fields['request_coordinator']['classification'][0]['#options']) || !array_key_exists(MFCS_REQUEST_CLASSIFICATION_STUDENT, $fields['request_coordinator']['classification'][0]['#options'])) {
          unset($type_options[MFCS_REQUEST_TYPE_CLUB_MEETING]);
        }
      }
    }

    foreach ($values as $key => $value) {
      $fields['information']['type'][$key] = array(
        '#id' => 'field-request-information-type-' . $key,
        '#type' => 'select',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => $type_options,
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );

      if ($readonly) {
        $fields['information']['type'][$key]['#disabled'] = TRUE;
        $fields['information']['type'][$key]['#attributes']['readonly'] = 'readonly';
      }
    }
    krsort($fields['information']['type']);

    $form_state['conditionally_required'][0][] = 'information][type';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['information']['type'][$key] = array(
        '#id' => 'field-request-information-type-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_information_organization($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['information']['organization'])) {
    $values = $request_state['information']['organization'];
  }

  $field_title = 'Organization / Affiliation';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'organization',
    'group' => 'information',
    'path' => 'information][organization',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['information']['organization'] = array(
      '#id' => 'field-request-information-organization',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-information-organization',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['information']['organization'][$key] = array(
        '#id' => 'field-request-information-organization-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#required' => FALSE,
        '#size' => 48,
        '#maxlength' => 255,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#states' => array(
          'invisible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . 'information][type]' . '[' . $key . ']'  . '"]' => array('value' => MFCS_REQUEST_TYPE_QUICK_MEETING),
          ),
        ),
      );
    }
    krsort($fields['information']['organization']);

    $form_state['conditionally_required'][0][] = 'information][organization';
    $form_state['conditionally_required'][2][] = 'information][organization'; // only if student organization.

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['information']['organization'][$key] = array(
        '#id' => 'field-request-information-organization-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_information_adviser_approval($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['information']['adviser_approval'])) {
    $values = $request_state['information']['adviser_approval'];
  }

  $render_field = TRUE;
  if (isset($form['form']['target_section']['#value']) && $form['form']['target_section']['#value'] == 'information') {
    // The adviser field is dependent on another that does not appear in the same section..
    // Don't render the field if the the dependency determines that it must not be renderred.
    if (!isset($fields['request_coordinator']['classification'][0]['#value']) || $fields['request_coordinator']['classification'][0]['#value'] != MFCS_REQUEST_CLASSIFICATION_STUDENT) {
      $render_field = FALSE;
    }
  }

  $field_title = 'My adviser has been notified and has approved this request.';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'adviser_approval',
    'group' => 'information',
    'path' => 'information][adviser_approval',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($render_field && $form['form']['step']['#value'] == 'standard' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['information']['adviser_approval'] = array(
      '#id' => 'field-request-information-adviser_approval',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-information-adviser_approval',
        ),
      ),
      // drupal core's 'states' functionality is lacking and fails to provided a way to do combination of and/ors and also setting priority, such as if quick meeting, then don't display ever.
      // therefore, the entire adviser_approval wrapper is hidden when quick meeting is selected.
      '#states' => array(
        'invisible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . 'information][type][0]' . '"]' => array('value' => MFCS_REQUEST_TYPE_QUICK_MEETING),
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['information']['adviser_approval'][$key] = array(
        '#id' => 'field-request-information-adviser_approval-' . $key,
        '#type' => 'checkbox',
        '#title' => $field_title,
        '#default_value' => $value,
        '#required' => FALSE,
        '#fieldset' => TRUE,
        '#fieldset_legend' => 'Adviser Approval',
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . 'request_coordinator][classification]' . '[' . $key . ']'  . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . 'request_coordinator][classification]' . '[' . $key . ']'  . '"]' => array('value' => NULL),
              ':input[name="' . $form['form']['parents']['#value']['path'] . 'information][type]' . '[' . $key . ']'  . '"]' => array('value' => MFCS_REQUEST_TYPE_QUICK_MEETING),
            ),
          ),
        ),
      );
    }
    krsort($fields['information']['adviser_approval']);

    $form_state['conditionally_required'][0][] = 'information][adviser_approval';
    $form_state['conditionally_required'][2][] = 'information][adviser_approval'; // only if student adviser_approval.

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['information']['adviser_approval'][$key] = array(
        '#id' => 'field-request-information-adviser_approval-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_plans_note($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('plans', $form['form']['sections']['#value'])) {
    $fields['plans']['note'] = array(
      '#id' => 'field-request-plans-note',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-plans-note',
        ),
      ),
    );

    $fields['plans']['note'][0] = array(
      '#id' => 'field-request-plans-note-0',
      '#type' => 'container',
      '#attributes' => array(
        'class' => array(
          'form-item-delta',
          'form-item-delta-0',
        ),
      ),
    );

    $fields['plans']['note'][0]['markup'] = array(
      '#markup' => '',
    );

    $fields['plans']['note'][0]['markup']['#markup'] .= '<div id="mfcs-markup-request-plans-note" class="markup-request">';
    $fields['plans']['note'][0]['markup']['#markup'] .= '*Please note a specified drawing of setup requests is required to complete the reservation. ';
    $fields['plans']['note'][0]['markup']['#markup'] .= 'The reserving office will provide a venue layout form for setup submission. ';
    $fields['plans']['note'][0]['markup']['#markup'] .= '</div>';
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_plans_description($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['plans']['description'])) {
    $values = $request_state['plans']['description'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Request Description/Purpose';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'description',
    'group' => 'plans',
    'path' => 'plans][description',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('plans', $form['form']['sections']['#value'])) {
    $fields['plans']['description'] = array(
      '#id' => 'field-request-plans-description',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-plans-description',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['plans']['description'][$key] = array(
        '#id' => 'field-request-plans-description-' . $key,
        '#type' => 'textarea',
        '#title' => $field_title,
        '#default_value' => $value,
        '#rows' => 2,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['plans']['description']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['plans']['description'][$key] = array(
        '#id' => 'field-request-plans-description-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_plans_activities($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['plans']['activities'])) {
    $values = $request_state['plans']['activities'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'List all activities planned within the request';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'activities',
    'group' => 'plans',
    'path' => 'plans][activities',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('plans', $form['form']['sections']['#value'])) {
    $fields['plans']['activities'] = array(
      '#id' => 'field-request-plans-activities',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-plans-activities',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['plans']['activities'][$key] = array(
        '#id' => 'field-request-plans-activities-' . $key,
        '#type' => 'textarea',
        '#title' => $field_title,
        '#default_value' => $value,
        '#rows' => 2,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['plans']['activities']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['plans']['activities'][$key] = array(
        '#id' => 'field-request-plans-activities-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_plans_audience($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['plans']['audience'])) {
    $values = $request_state['plans']['audience'];
  }

  $field_title = 'Intended Audience';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'audience',
    'group' => 'plans',
    'path' => 'plans][audience',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'details' && array_key_exists('plans', $form['form']['sections']['#value'])) {
    $fields['plans']['audience'] = array(
      '#id' => 'field-request-plans-audience',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-plans-audience',
        ),
      ),
    );

    $audience_options = mfcs_get_audience_list_options('select');

    foreach ($values as $key => $value) {
      $fields['plans']['audience'][$key] = array(
        '#id' => 'field-request-plans-audience-' . $key,
        '#type' => 'select',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => $audience_options,
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['plans']['audience']);

    $form_state['conditionally_required'][0][] = 'plans][audience';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['plans']['audience'][$key] = array(
        '#id' => 'field-request-plans-audience-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_registration_require($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['registration']['require'])) {
    $values = $request_state['registration']['require'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Does the request require registration or ticket purchase?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'require',
    'group' => 'registration',
    'path' => 'registration][require',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['require'] = array(
      '#id' => 'field-request-registration-require',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-registration-require',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['require'][$key] = array(
        '#id' => 'field-request-registration-require-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['registration']['require']);

    $form_state['conditionally_required'][0][] = 'registration][require';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['registration']['require'][$key] = array(
        '#id' => 'field-request-registration-require-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_registration_website($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['registration']['website'])) {
    $values = $request_state['registration']['website'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Please indicate a <em>Registration Website</em>';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'website',
    'group' => 'registration',
    'path' => 'registration][website',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['website'] = array(
      '#id' => 'field-request-registration-website',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-registration-website',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['website'][$key] = array(
        '#id' => 'field-request-registration-website-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 48,
        '#maxlength' => 255,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps. '[require]' . '[' . $key . ']'  . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['registration']['website']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['registration']['website'][$key] = array(
        '#id' => 'field-request-registration-website-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_registration_phone($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['registration']['phone'])) {
    $values = $request_state['registration']['phone'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Please indicate a <em>Registration Phone</em>';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'phone',
    'group' => 'registration',
    'path' => 'registration][phone',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['phone'] = array(
      '#id' => 'field-request-registration-phone',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-registration-phone',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['phone'][$key] = array(
        '#id' => 'field-request-registration-phone-' . $key,
        '#type' => 'telfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['registration']['phone']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['registration']['phone'][$key] = array(
        '#id' => 'field-request-registration-phone-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_registration_ticket_price($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['registration']['ticket_price'])) {
    $values = $request_state['registration']['ticket_price'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Please indicate a <em>Ticket Price</em>';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'ticket_price',
    'group' => 'registration',
    'path' => 'registration][ticket_price',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['ticket_price'] = array(
      '#id' => 'field-request-registration-ticket_price',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-registration-ticket_price',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['ticket_price'][$key] = array(
        '#id' => 'field-request-registration-ticket_price-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 48,
        '#maxlength' => 255,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['registration']['ticket_price']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['registration']['ticket_price'][$key] = array(
        '#id' => 'field-request-registration-ticket_price-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_registration_ticket_dates($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['registration']['ticket_dates'])) {
    $values = $request_state['registration']['ticket_dates'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Please indicate <em>Dates</em> the tickets are one sale';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'ticket_dates',
    'group' => 'registration',
    'path' => 'registration][ticket_dates',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['ticket_dates'] = array(
      '#id' => 'field-request-registration-ticket_dates',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-registration-ticket_dates',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['ticket_dates'][$key] = array(
        '#id' => 'field-request-registration-ticket_dates-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 48,
        '#maxlength' => 255,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['registration']['ticket_dates']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['registration']['ticket_dates'][$key] = array(
        '#id' => 'field-request-registration-ticket_dates-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_registration_ticket_website($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['registration']['ticket_website'])) {
    $values = $request_state['registration']['ticket_website'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Please indicate a <em>Ticket Website</em>';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'ticket_website',
    'group' => 'registration',
    'path' => 'registration][ticket_website',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['ticket_website'] = array(
      '#id' => 'field-request-registration-ticket_website',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-registration-ticket_website',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['ticket_website'][$key] = array(
        '#id' => 'field-request-registration-ticket_website-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 48,
        '#maxlength' => 255,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['registration']['ticket_website']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['registration']['ticket_website'][$key] = array(
        '#id' => 'field-request-registration-ticket_website-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_registration_ticket_phone($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['registration']['ticket_phone'])) {
    $values = $request_state['registration']['ticket_phone'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Please indicate a <em>Ticket Phone</em>';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'ticket_phone',
    'group' => 'registration',
    'path' => 'registration][ticket_phone',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['ticket_phone'] = array(
      '#id' => 'field-request-registration-ticket_phone',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-registration-ticket_phone',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['ticket_phone'][$key] = array(
        '#id' => 'field-request-registration-ticket_phone-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 48,
        '#maxlength' => 255,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '[' . $key . ']'  . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['registration']['ticket_phone']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['registration']['ticket_phone'][$key] = array(
        '#id' => 'field-request-registration-ticket_phone-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_registration_generate_revenue($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['registration']['generate_revenue'])) {
    $values = $request_state['registration']['generate_revenue'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Will the Event Generate Revenue?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'generate_revenue',
    'group' => 'registration',
    'path' => 'registration][generate_revenue',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['generate_revenue'] = array(
      '#id' => 'field-request-registration-generate_revenue',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-registration-generate_revenue',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['generate_revenue'][$key] = array(
        '#id' => 'field-request-registration-generate_revenue-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['registration']['generate_revenue']);

    $form_state['conditionally_required'][0][] = 'registration][generate_revenue';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['registration']['generate_revenue'][$key] = array(
        '#id' => 'field-request-registration-generate_revenue-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_registration_revenue_generated($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => array());
  if (!empty($request_state['registration']['revenue_generated'])) {
    $values = $request_state['registration']['revenue_generated'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'How Will Revenue be Generated?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'revenue_generated',
    'group' => 'registration',
    'path' => 'registration][revenue_generated',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['revenue_generated'] = array(
      '#id' => 'field-request-registration-revenue_generated',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-registration-revenue_generated',
        ),
      ),
    );

    $revenue_generated_options = mfcs_get_revenue_generated_list_options('checkboxes');

    foreach ($values as $key => $value) {
      $fields['registration']['revenue_generated'][$key] = array(
        '#id' => 'field-request-registration-revenue_generated-' . $key,
        '#type' => 'checkboxes',
        '#title' => $field_title,
        '#default_value' => $value,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[generate_revenue]' . '[' . $key . ']'  . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[generate_revenue]' . '[' . $key . ']'  . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[generate_revenue]' . '[' . $key . ']'  . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#options' => $revenue_generated_options,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['registration']['revenue_generated']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['registration']['revenue_generated'][$key] = array(
        '#id' => 'field-request-registration-revenue_generated-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_setup_rectangular_tables_8ft($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['setup']['rectangular_tables_8ft'])) {
    $values = $request_state['setup']['rectangular_tables_8ft'];
  }

  $field_title = 'How Many 8 ft Rectangular Tables Will You Need?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'rectangular_tables_8ft',
    'group' => 'setup',
    'path' => 'setup][rectangular_tables_8ft',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['rectangular_tables_8ft'] = array(
      '#id' => 'field-request-setup-rectangular_tables_8ft',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-setup-rectangular_tables_8ft',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['rectangular_tables_8ft'][$key] = array(
        '#id' => 'field-request-setup-rectangular_tables_8ft-' . $key,
        '#type' => 'numberfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#min' => 0,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['setup']['rectangular_tables_8ft']);

    $form_state['conditionally_required'][0][] = 'setup][rectangular_tables_8ft';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['setup']['rectangular_tables_8ft'][$key] = array(
        '#id' => 'field-request-setup-rectangular_tables_8ft-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_setup_round_tables_8ft($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['setup']['round_tables_8ft'])) {
    $values = $request_state['setup']['round_tables_8ft'];
  }

  $field_title = 'How Many 8 ft Round Tables Will You Need?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'round_tables_8ft',
    'group' => 'setup',
    'path' => 'setup][round_tables_8ft',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['round_tables_8ft'] = array(
      '#id' => 'field-request-setup-round_tables_8ft',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-setup-round_tables_8ft',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['round_tables_8ft'][$key] = array(
        '#id' => 'field-request-setup-round_tables_8ft-' . $key,
        '#type' => 'numberfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#min' => 0,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['setup']['round_tables_8ft']);

    $form_state['conditionally_required'][0][] = 'setup][round_tables_8ft';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['setup']['round_tables_8ft'][$key] = array(
        '#id' => 'field-request-setup-round_tables_8ft-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_setup_other_tables($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['setup']['other_tables'])) {
    $values = $request_state['setup']['other_tables'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'How Many Other Tables Will You Need? <span id="mfcs-markup-request-title_display-note-setup_other_tables" class="title_display-note describe_the_table">(Describe the Table)</span>';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'other_tables',
    'group' => 'setup',
    'path' => 'setup][other_tables',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['other_tables'] = array(
      '#id' => 'field-request-setup-other_tables',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-setup-other_tables',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['other_tables'][$key] = array(
        '#id' => 'field-request-setup-other_tables-' . $key,
        '#type' => 'textarea',
        '#title' => $field_title,
        '#default_value' => $value,
        '#rows' => 2,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['setup']['other_tables']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['setup']['other_tables'][$key] = array(
        '#id' => 'field-request-setup-other_tables-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_setup_standard_blue_chairs($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['setup']['standard_blue_chairs'])) {
    $values = $request_state['setup']['standard_blue_chairs'];
  }

  $field_title = 'How Many Standard Chairs Will You Need?'; // renamed "Blue Chairs" to "Chairs" as per fcs-22.
  $limits_check = array(
    'title' => $field_title,
    'field' => 'standard_blue_chairs',
    'group' => 'setup',
    'path' => 'setup][standard_blue_chairs',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['standard_blue_chairs'] = array(
      '#id' => 'field-request-setup-standard_blue_chairs',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-setup-standard_blue_chairs',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['standard_blue_chairs'][$key] = array(
        '#id' => 'field-request-setup-standard_blue_chairs-' . $key,
        '#type' => 'numberfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#min' => 0,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['setup']['standard_blue_chairs']);

    $form_state['conditionally_required'][0][] = 'setup][standard_blue_chairs';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['setup']['standard_blue_chairs'][$key] = array(
        '#id' => 'field-request-setup-standard_blue_chairs-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_setup_podium($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['setup']['podium'])) {
    $values = $request_state['setup']['podium'];
  }

  $field_title = 'Will You Need a Podium?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'podium',
    'group' => 'setup',
    'path' => 'setup][podium',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['podium'] = array(
      '#id' => 'field-request-setup-podium',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-setup-podium',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['podium'][$key] = array(
        '#id' => 'field-request-setup-podium-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['setup']['podium']);

    $form_state['conditionally_required'][0][] = 'setup][podium';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['setup']['podium'][$key] = array(
        '#id' => 'field-request-setup-podium-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_setup_portable_stage($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['setup']['portable_stage'])) {
    $values = $request_state['setup']['portable_stage'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Will You Require a Portable Stage?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'portable_stage',
    'group' => 'setup',
    'path' => 'setup][portable_stage',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['portable_stage'] = array(
      '#id' => 'field-request-setup-portable_stage',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-setup-portable_stage',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['portable_stage'][$key] = array(
        '#id' => 'field-request-setup-portable_stage-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['setup']['portable_stage']);

    $form_state['conditionally_required'][0][] = 'setup][portable_stage';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['setup']['portable_stage'][$key] = array(
        '#id' => 'field-request-setup-portable_stage-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_setup_portable_stage_configuration($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['setup']['portable_stage_configuration'])) {
    $values = $request_state['setup']['portable_stage_configuration'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'What size stage and configuration is needed?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'portable_stage_configuration',
    'group' => 'setup',
    'path' => 'setup][portable_stage_configuration',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['portable_stage_configuration'] = array(
      '#id' => 'field-request-setup-portable_stage_configuration',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-setup-portable_stage_configuration',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['portable_stage_configuration'][$key] = array(
        '#id' => 'field-request-setup-portable_stage_configuration-' . $key,
        '#type' => 'textarea',
        '#title' => $field_title,
        '#default_value' => $value,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[portable_stage]' . '[' . $key . ']'  . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[portable_stage]' . '[' . $key . ']'  . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[portable_stage]' . '[' . $key . ']'  . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#rows' => 2,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['setup']['portable_stage_configuration']);

    $form_state['conditionally_required'][1][] = 'setup][portable_stage_configuration';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['setup']['portable_stage_configuration'][$key] = array(
        '#id' => 'field-request-setup-portable_stage_configuration-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_setup_security($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['setup']['security'])) {
    $values = $request_state['setup']['security'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Will the Event Require Security? <span id="mfcs-markup-request-title_display-note-setup_security" class="title_display-note additional_requirements_may_apply">(Additional Requirements May Apply)</span>';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'security',
    'group' => 'setup',
    'path' => 'setup][security',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['security'] = array(
      '#id' => 'field-request-setup-security',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-setup-security',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['security'][$key] = array(
        '#id' => 'field-request-setup-security-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['setup']['security']);

    $form_state['conditionally_required'][0][] = 'setup][security';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['setup']['security'][$key] = array(
        '#id' => 'field-request-setup-security-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_setup_parking_assistance($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['setup']['parking_assistance'])) {
    $values = $request_state['setup']['parking_assistance'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Will the Event Require Parking Assistance or Restrictions?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'parking_assistance',
    'group' => 'setup',
    'path' => 'setup][parking_assistance',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['parking_assistance'] = array(
      '#id' => 'field-request-setup-parking_assistance',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-setup-parking_assistance',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['parking_assistance'][$key] = array(
        '#id' => 'field-request-setup-parking_assistance-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['setup']['parking_assistance']);

    $form_state['conditionally_required'][0][] = 'setup][parking_assistance';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['setup']['parking_assistance'][$key] = array(
        '#id' => 'field-request-setup-parking_assistance-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_setup_parking_assistance_area($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['setup']['parking_assistance_area'])) {
    $values = $request_state['setup']['parking_assistance_area'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'What Parking Area will be Accessed or Restricted?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'parking_assistance_area',
    'group' => 'setup',
    'path' => 'setup][parking_assistance_area',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['parking_assistance_area'] = array(
      '#id' => 'field-request-setup-parking_assistance_area',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-setup-parking_assistance_area',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['parking_assistance_area'][$key] = array(
        '#id' => 'field-request-setup-parking_assistance_area-' . $key,
        '#type' => 'textarea',
        '#title' => $field_title,
        '#default_value' => $value,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[parking_assistance]' . '[' . $key . ']'  . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[parking_assistance]' . '[' . $key . ']'  . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[parking_assistance]' . '[' . $key . ']'  . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#rows' => 2,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['setup']['parking_assistance_area']);

    $form_state['conditionally_required'][1][] = 'setup][parking_assistance_area';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['setup']['parking_assistance_area'][$key] = array(
        '#id' => 'field-request-setup-parking_assistance_area-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_setup_road_closures($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['setup']['road_closures'])) {
    $values = $request_state['setup']['road_closures'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Will the Event Require Road Closures or Barricades?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'road_closures',
    'group' => 'setup',
    'path' => 'setup][road_closures',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['road_closures'] = array(
      '#id' => 'field-request-setup-road_closures',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-setup-road_closures',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['road_closures'][$key] = array(
        '#id' => 'field-request-setup-road_closures-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['setup']['road_closures']);

    $form_state['conditionally_required'][0][] = 'setup][road_closures';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['setup']['road_closures'][$key] = array(
        '#id' => 'field-request-setup-road_closures-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_setup_road_closures_details($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['setup']['road_closures_details'])) {
    $values = $request_state['setup']['road_closures_details'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Provide an Explanation on the Road Closures or Barricades';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'road_closures_details',
    'group' => 'setup',
    'path' => 'setup][road_closures_details',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['road_closures_details'] = array(
      '#id' => 'field-request-setup-road_closures_details',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-setup-road_closures_details',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['road_closures_details'][$key] = array(
        '#id' => 'field-request-setup-road_closures_details-' . $key,
        '#type' => 'textarea',
        '#title' => $field_title,
        '#default_value' => $value,
        '#rows' => 2,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[road_closures]' . '[' . $key . ']'  . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[road_closures]' . '[' . $key . ']'  . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[road_closures]' . '[' . $key . ']'  . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['setup']['road_closures_details']);

    $form_state['conditionally_required'][1][] = 'setup][road_closures_details';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['setup']['road_closures_details'][$key] = array(
        '#id' => 'field-request-setup-road_closures_details-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_setup_special_requests($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['setup']['special_requests'])) {
    $values = $request_state['setup']['special_requests'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Will the Event Require any Additional Special Requests for Setup?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'special_requests',
    'group' => 'setup',
    'path' => 'setup][special_requests',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['special_requests'] = array(
      '#id' => 'field-request-setup-special_requests',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-setup-special_requests',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['special_requests'][$key] = array(
        '#id' => 'field-request-setup-special_requests-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['setup']['special_requests']);

    $form_state['conditionally_required'][0][] = 'setup][special_requests';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['setup']['special_requests'][$key] = array(
        '#id' => 'field-request-setup-special_requests-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_setup_special_requests_details($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['setup']['special_requests_details'])) {
    $values = $request_state['setup']['special_requests_details'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Explanation on the Special Requests';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'special_requests_details',
    'group' => 'setup',
    'path' => 'setup][special_requests_details',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['special_requests_details'] = array(
      '#id' => 'field-request-setup-special_requests_details',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-setup-special_requests_details',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['special_requests_details'][$key] = array(
        '#id' => 'field-request-setup-special_requests_details-' . $key,
        '#type' => 'textarea',
        '#title' => $field_title,
        '#default_value' => $value,
        '#rows' => 2,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[special_requests]' . '[' . $key . ']'  . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[special_requests]' . '[' . $key . ']'  . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[special_requests]' . '[' . $key . ']'  . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['setup']['special_requests_details']);

    $form_state['conditionally_required'][1][] = 'setup][special_requests_details';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['setup']['special_requests_details'][$key] = array(
        '#id' => 'field-request-setup-special_requests_details-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_technical_equipment($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['presentation']['technical_equipment'])) {
    $values = $request_state['presentation']['technical_equipment'];
  }

  $field_title = 'Will the Event Require any Technical Equipment? <span id="mfcs-markup-request-title_display-note-presentation_technical_equipment" class="title_display-note additional_requirements_may_apply">(Additional Requirements May Apply)</span>';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'technical_equipment',
    'group' => 'presentation',
    'path' => 'presentation][technical_equipment',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['technical_equipment'] = array(
      '#id' => 'field-request-presentation-technical_equipment',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-technical_equipment',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['technical_equipment'][$key] = array(
        '#id' => 'field-request-presentation-technical_equipment-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['technical_equipment']);

    $form_state['conditionally_required'][0][] = 'presentation][technical_equipment';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['technical_equipment'][$key] = array(
        '#id' => 'field-request-presentation-technical_equipment-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_technical_equipment_note($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['technical_equipment_note'] = array(
      '#id' => 'field-request-presentation-technical_equipment_note',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-technical_equipment_note',
        ),
      ),
    );

    $fields['presentation']['technical_equipment_note'][0] = array(
      '#id' => 'field-request-presentation-technical_equipment_note-0',
      '#type' => 'container',
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment][0]' . '"]' => array('value' => 1),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment][0]' . '"]' => array('value' => 0),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment][0]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
      '#attributes' => array(
        'class' => array(
          'form-item-delta',
          'form-item-delta-0',
        ),
      ),
    );

    $fields['presentation']['technical_equipment_note'][0]['markup'] = array(
      '#markup' => '',
    );

    $fields['presentation']['technical_equipment_note'][0]['markup']['#markup'] .= '<div id="mfcs-markup-request-presentation-technical_equipment_note" class="markup-request">';
    $fields['presentation']['technical_equipment_note'][0]['markup']['#markup'] .= 'The technology you requested may or may not be available for this venue. ';
    $fields['presentation']['technical_equipment_note'][0]['markup']['#markup'] .= '<em><a href="//www.mcneese.edu/node/5506" target="_blank" class="no_print_as_link markup-request-link">You may also be required to complete an additional request form to secure the item(s).</a></em> ';
    $fields['presentation']['technical_equipment_note'][0]['markup']['#markup'] .= 'Communicate with the venue coordinator to confirm your technical requests. ';
    $fields['presentation']['technical_equipment_note'][0]['markup']['#markup'] .= '</div>';
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_technical_equipment_details($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => array());
  if (!empty($request_state['presentation']['technical_equipment_details'])) {
    $values = $request_state['presentation']['technical_equipment_details'];
  }

  $field_title = 'Specify the Equipment Needed';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'technical_equipment_details',
    'group' => 'presentation',
    'path' => 'presentation][technical_equipment_details',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['technical_equipment_details'] = array(
      '#id' => 'field-request-presentation-technical_equipment_details',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-technical_equipment_details',
        ),
      ),
    );

    $technical_equipment_details_options = mfcs_get_technical_equipment_details_list_options('checkboxes');

    foreach ($values as $key => $value) {
      $fields['presentation']['technical_equipment_details'][$key] = array(
        '#id' => 'field-request-presentation-technical_equipment_details-' . $key,
        '#type' => 'checkboxes',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => $technical_equipment_details_options,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment]' . '[' . $key . ']'  . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment]' . '[' . $key . ']'  . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment]' . '[' . $key . ']'  . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['technical_equipment_details']);

    $form_state['conditionally_required'][0][] = 'presentation][technical_equipment_details';
    $form_state['conditionally_required'][1][] = 'presentation][technical_equipment_details';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['technical_equipment_details'][$key] = array(
        '#id' => 'field-request-presentation-technical_equipment_details-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_technical_equipment_microphone($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['presentation']['technical_equipment_microphone'])) {
    $values = $request_state['presentation']['technical_equipment_microphone'];
  }

  $field_title = 'Specify the Quantity of Microphones';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'technical_equipment_microphone',
    'group' => 'presentation',
    'path' => 'presentation][technical_equipment_microphone',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['technical_equipment_microphone'] = array(
      '#id' => 'field-request-presentation-technical_equipment_microphone',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-technical_equipment_microphone',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['technical_equipment_microphone'][$key] = array(
        '#id' => 'field-request-presentation-technical_equipment_microphone-' . $key,
        '#type' => 'numberfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#min' => 0,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment]' . '[' . $key . ']'  . '"]' => array('value' => 1),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment_details]' . '[' . $key . ']' . '[76]' . '"]' => array('checked' => TRUE),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment_details]' . '[' . $key . ']' . '[76]' . '"]' => array('checked' => FALSE),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment_details]' . '[' . $key . ']' . '[76]' . '"]' => array('value' => NULL),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment]' . '[' . $key . ']' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['technical_equipment_microphone']);

    $form_state['conditionally_required'][1][] = 'presentation][technical_equipment_microphone';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['technical_equipment_microphone'][$key] = array(
        '#id' => 'field-request-presentation-technical_equipment_microphone-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_external_audio_person($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['presentation']['external_audio_person'])) {
    $values = $request_state['presentation']['external_audio_person'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Will an External Audio/Video Person/Company be Utilized?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'external_audio_person',
    'group' => 'presentation',
    'path' => 'presentation][external_audio_person',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['external_audio_person'] = array(
      '#id' => 'field-request-presentation-external_audio_person',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-external_audio_person',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['external_audio_person'][$key] = array(
        '#id' => 'field-request-presentation-external_audio_person-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['external_audio_person']);

    $form_state['conditionally_required'][0][] = 'presentation][external_audio_person';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['external_audio_person'][$key] = array(
        '#id' => 'field-request-presentation-external_audio_person-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_external_audio_person_name($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['presentation']['external_audio_person_name'])) {
    $values = $request_state['presentation']['external_audio_person_name'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Provide the Name of the Person/Company Providing the External Audio/Video';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'external_audio_person_name',
    'group' => 'presentation',
    'path' => 'presentation][external_audio_person_name',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['external_audio_person_name'] = array(
      '#id' => 'field-request-presentation-external_audio_person_name',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-external_audio_person_name',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['external_audio_person_name'][$key] = array(
        '#id' => 'field-request-presentation-external_audio_person_name-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 48,
        '#maxlength' => 255,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '[' . $key . ']' . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '[' . $key . ']' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['external_audio_person_name']);

    $form_state['conditionally_required'][1][] = 'presentation][external_audio_person_name';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['external_audio_person_name'][$key] = array(
        '#id' => 'field-request-presentation-external_audio_person_name-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_external_audio_person_email($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['presentation']['external_audio_person_email'])) {
    $values = $request_state['presentation']['external_audio_person_email'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Provide the E-mail of the Person/Company Providing the External Audio/Video';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'external_audio_person_email',
    'group' => 'presentation',
    'path' => 'presentation][external_audio_person_email',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['external_audio_person_email'] = array(
      '#id' => 'field-request-presentation-external_audio_person_email',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-external_audio_person_email',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['external_audio_person_email'][$key] = array(
        '#id' => 'field-request-presentation-external_audio_person_email-' . $key,
        '#type' => 'emailfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 48,
        '#maxlength' => 255,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '[' . $key . ']' . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '[' . $key . ']' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['external_audio_person_email']);

    $form_state['conditionally_required'][0][] = 'presentation][external_audio_person_email';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['external_audio_person_email'][$key] = array(
        '#id' => 'field-request-presentation-external_audio_person_email-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_external_audio_person_phone($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['presentation']['external_audio_person_phone'])) {
    $values = $request_state['presentation']['external_audio_person_phone'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Provide the Phone Number of the Person/Company Providing the External Audio/Video';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'external_audio_person_phone',
    'group' => 'presentation',
    'path' => 'presentation][external_audio_person_phone',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['external_audio_person_phone'] = array(
      '#id' => 'field-request-presentation-external_audio_person_phone',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-external_audio_person_phone',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['external_audio_person_phone'][$key] = array(
        '#id' => 'field-request-presentation-external_audio_person_phone-' . $key,
        '#type' => 'telfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '[' . $key . ']' . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '[' . $key . ']' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['external_audio_person_phone']);

    $form_state['conditionally_required'][1][] = 'presentation][external_audio_person_phone';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['external_audio_person_phone'][$key] = array(
        '#id' => 'field-request-presentation-external_audio_person_phone-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_production($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['presentation']['production'])) {
    $values = $request_state['presentation']['production'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Is the Event Using a Production Company or Professional Promoter?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'production',
    'group' => 'presentation',
    'path' => 'presentation][production',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['production'] = array(
      '#id' => 'field-request-presentation-production',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-production',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['production'][$key] = array(
        '#id' => 'field-request-presentation-production-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['production']);

    $form_state['conditionally_required'][0][] = 'presentation][production';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['production'][$key] = array(
        '#id' => 'field-request-presentation-production-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_production_name($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['presentation']['production_name'])) {
    $values = $request_state['presentation']['production_name'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Name of the Company/Individual';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'production_name',
    'group' => 'presentation',
    'path' => 'presentation][production_name',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['production_name'] = array(
      '#id' => 'field-request-presentation-production_name',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-production_name',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['production_name'][$key] = array(
        '#id' => 'field-request-presentation-production_name-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 48,
        '#maxlength' => 255,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '[' . $key . ']' . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '[' . $key . ']' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['production_name']);

    $form_state['conditionally_required'][0][] = 'presentation][production_name';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['production_name'][$key] = array(
        '#id' => 'field-request-presentation-production_name-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_production_email($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['presentation']['production_email'])) {
    $values = $request_state['presentation']['production_email'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Contact E-mail of the Company/Individual';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'production_email',
    'group' => 'presentation',
    'path' => 'presentation][production_email',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['production_email'] = array(
      '#id' => 'field-request-presentation-production_email',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-production_email',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['production_email'][$key] = array(
        '#id' => 'field-request-presentation-production_email-' . $key,
        '#type' => 'emailfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 48,
        '#maxlength' => 255,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '[' . $key . ']' . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '[' . $key . ']' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['production_email']);

    $form_state['conditionally_required'][0][] = 'presentation][production_email';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['production_email'][$key] = array(
        '#id' => 'field-request-presentation-production_email-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_production_phone($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['presentation']['production_phone'])) {
    $values = $request_state['presentation']['production_phone'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Phone Number of the Company/Individual';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'production_phone',
    'group' => 'presentation',
    'path' => 'presentation][production_phone',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['production_phone'] = array(
      '#id' => 'field-request-presentation-production_phone',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-production_phone',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['production_phone'][$key] = array(
        '#id' => 'field-request-presentation-production_phone-' . $key,
        '#type' => 'telfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '[' . $key . ']' . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '[' . $key . ']' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['production_phone']);

    $form_state['conditionally_required'][0][] = 'presentation][production_phone';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['production_phone'][$key] = array(
        '#id' => 'field-request-presentation-production_phone-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_publicity($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['presentation']['publicity'])) {
    $values = $request_state['presentation']['publicity'];
  }

  $field_title = 'Will the Event Need Publicity?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'publicity',
    'group' => 'presentation',
    'path' => 'presentation][publicity',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['publicity'] = array(
      '#id' => 'field-request-presentation-publicity',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-publicity',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['publicity'][$key] = array(
        '#id' => 'field-request-presentation-publicity-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['publicity']);

    $form_state['conditionally_required'][0][] = 'presentation][publicity';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['publicity'][$key] = array(
        '#id' => 'field-request-presentation-publicity-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_publicity_details($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => array());
  if (!empty($request_state['presentation']['publicity_details'])) {
    $values = $request_state['presentation']['publicity_details'];
  }

  $field_title = 'Specify all Publicity Medium that Apply';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'publicity_details',
    'group' => 'presentation',
    'path' => 'presentation][publicity_details',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['publicity_details'] = array(
      '#id' => 'field-request-presentation-publicity_details',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-publicity_details',
        ),
      ),
    );

    $publicity_details_options = mfcs_get_publicity_details_list_options('checkboxes');

    foreach ($values as $key => $value) {
      $fields['presentation']['publicity_details'][$key] = array(
        '#id' => 'field-request-presentation-publicity_details-' . $key,
        '#type' => 'checkboxes',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => $publicity_details_options,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[publicity]' . '[' . $key . ']' . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[publicity]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[publicity]' . '[' . $key . ']' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['publicity_details']);

    $form_state['conditionally_required'][1][] = 'presentation][publicity_details';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['publicity_details'][$key] = array(
        '#id' => 'field-request-presentation-publicity_details-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_printed_material($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['presentation']['printed_material'])) {
    $values = $request_state['presentation']['printed_material'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Will the Event use Printed Material?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'printed_material',
    'group' => 'presentation',
    'path' => 'presentation][printed_material',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['printed_material'] = array(
      '#id' => 'field-request-presentation-printed_material',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-printed_material',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['printed_material'][$key] = array(
        '#id' => 'field-request-presentation-printed_material-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['printed_material']);

    $form_state['conditionally_required'][0][] = 'presentation][printed_material';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['printed_material'][$key] = array(
        '#id' => 'field-request-presentation-printed_material-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_university_logo($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['presentation']['university_logo'])) {
    $values = $request_state['presentation']['university_logo'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Will the Event use the University Logo?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'university_logo',
    'group' => 'presentation',
    'path' => 'presentation][university_logo',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['university_logo'] = array(
      '#id' => 'field-request-presentation-university_logo',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-university_logo',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['university_logo'][$key] = array(
        '#id' => 'field-request-presentation-university_logo-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '[' . $key . ']' . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '[' . $key . ']' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['university_logo']);

    $form_state['conditionally_required'][0][] = 'presentation][university_logo';
    $form_state['conditionally_required'][1][] = 'presentation][university_logo';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['university_logo'][$key] = array(
        '#id' => 'field-request-presentation-university_logo-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_designing_material($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => array());
  if (!empty($request_state['presentation']['designing_material'])) {
    $values = $request_state['presentation']['designing_material'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Who will be Designing the Material?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'designing_material',
    'group' => 'presentation',
    'path' => 'presentation][designing_material',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['designing_material'] = array(
      '#id' => 'field-request-presentation-designing_material',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-designing_material',
        ),
      ),
    );

    $desiging_material_options = mfcs_get_designing_material_list_options('checkboxes');

    foreach ($values as $key => $value) {
      $fields['presentation']['designing_material'][$key] = array(
        '#id' => 'field-request-presentation-designing_material-' . $key,
        '#type' => 'checkboxes',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => $desiging_material_options,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '[' . $key . ']' . '"]' => array('value' => 1),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '[' . $key . ']' . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '[' . $key . ']' . '"]' => array('value' => NULL),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '[' . $key . ']' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['designing_material']);

    $form_state['conditionally_required'][1][] = 'presentation][designing_material';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['designing_material'][$key] = array(
        '#id' => 'field-request-presentation-designing_material-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_designing_material_name($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['presentation']['designing_material_name'])) {
    $values = $request_state['presentation']['designing_material_name'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Name of who will be Designing the Material';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'designing_material_name',
    'group' => 'presentation',
    'path' => 'presentation][designing_material_name',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['designing_material_name'] = array(
      '#id' => 'field-request-presentation-designing_material_name',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-designing_material_name',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['designing_material_name'][$key] = array(
        '#id' => 'field-request-presentation-designing_material_name-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 48,
        '#maxlength' => 255,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '[' . $key . ']' . '"]' => array('value' => 1),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '[' . $key . ']' . '"]' => array('value' => 1),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[designing_material]' . '[' . $key . ']' . '[97]' . '"]' => array('checked' => TRUE),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[designing_material]' . '[' . $key . ']' . '[97]' . '"]' => array('checked' => FALSE),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '[' . $key . ']' . '"]' => array('value' => NULL),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '[' . $key . ']' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['designing_material_name']);

    $form_state['conditionally_required'][1][] = 'presentation][designing_material_name';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['designing_material_name'][$key] = array(
        '#id' => 'field-request-presentation-designing_material_name-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_designing_material_email($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['presentation']['designing_material_email'])) {
    $values = $request_state['presentation']['designing_material_email'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'E-mail of who will be Designing the Material';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'designing_material_email',
    'group' => 'presentation',
    'path' => 'presentation][designing_material_email',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['designing_material_email'] = array(
      '#id' => 'field-request-presentation-designing_material_email',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-designing_material_email',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['designing_material_email'][$key] = array(
        '#id' => 'field-request-presentation-designing_material_email-' . $key,
        '#type' => 'emailfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 48,
        '#maxlength' => 255,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '[' . $key . ']' . '"]' => array('value' => 1),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '[' . $key . ']' . '"]' => array('value' => 1),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[designing_material]' . '[' . $key . ']' . '[97]' . '"]' => array('checked' => TRUE),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[designing_material]' . '[' . $key . ']' . '[97]' . '"]' => array('checked' => FALSE),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '[' . $key . ']' . '"]' => array('value' => NULL),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '[' . $key . ']' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['designing_material_email']);

    $form_state['conditionally_required'][1][] = 'presentation][designing_material_email';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['designing_material_email'][$key] = array(
        '#id' => 'field-request-presentation-designing_material_email-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_presentation_designing_material_phone($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['presentation']['designing_material_phone'])) {
    $values = $request_state['presentation']['designing_material_phone'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Phone Number of who will be Designing the Material';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'designing_material_phone',
    'group' => 'presentation',
    'path' => 'presentation][designing_material_phone',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['designing_material_phone'] = array(
      '#id' => 'field-request-presentation-designing_material_phone',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-presentation-designing_material_phone',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['designing_material_phone'][$key] = array(
        '#id' => 'field-request-presentation-designing_material_phone-' . $key,
        '#type' => 'telfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '[' . $key . ']' . '"]' => array('value' => 1),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '[' . $key . ']' . '"]' => array('value' => 1),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[designing_material]' . '[' . $key . ']' . '[97]' . '"]' => array('checked' => TRUE),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[designing_material]' . '[' . $key . ']' . '[97]' . '"]' => array('checked' => FALSE),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '[' . $key . ']' . '"]' => array('value' => NULL),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '[' . $key . ']' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['presentation']['designing_material_phone']);

    $form_state['conditionally_required'][1][] = 'presentation][designing_material_phone';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['presentation']['designing_material_phone'][$key] = array(
        '#id' => 'field-request-presentation-designing_material_phone-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_services_food_served($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['services']['food_served'])) {
    $values = $request_state['services']['food_served'];
  }

  $field_title = 'Will Food or Drink be Served? <span id="mfcs-markup-request-title_display-note-services_food_served" class="title_display-note some_venues_dont_permit">(Note some venues do not permit food and drink)</span>';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'food_served',
    'group' => 'services',
    'path' => 'services][food_served',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'details' && array_key_exists('services', $form['form']['sections']['#value'])) {
    $fields['services']['food_served'] = array(
      '#id' => 'field-request-services-food_served',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-services-food_served',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['services']['food_served'][$key] = array(
        '#id' => 'field-request-services-food_served-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['services']['food_served']);

    $form_state['conditionally_required'][0][] = 'services][food_served';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['services']['food_served'][$key] = array(
        '#id' => 'field-request-services-food_served-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_services_food_caterer($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['services']['food_caterer'])) {
    $values = $request_state['services']['food_caterer'];
  }

  $field_title = 'Provide the Name of the Caterer/Busineess';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'food_caterer',
    'group' => 'services',
    'path' => 'services][food_caterer',
    'visible' => TRUE,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'details' && array_key_exists('services', $form['form']['sections']['#value'])) {
    $fields['services']['food_caterer'] = array(
      '#id' => 'field-request-services-food_caterer',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-services-food_caterer',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['services']['food_caterer'][$key] = array(
        '#id' => 'field-request-services-food_caterer-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 48,
        '#maxlength' => 255,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[food_served]' . '[' . $key . ']' . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[food_served]' . '[' . $key . ']' . '"]' => array('value' => 0),
              ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[food_served]' . '[' . $key . ']' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['services']['food_caterer']);

    $form_state['conditionally_required'][1][] = 'services][food_caterer';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['services']['food_caterer'][$key] = array(
        '#id' => 'field-request-services-food_caterer-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_services_alcohol($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['services']['alcohol'])) {
    $values = $request_state['services']['alcohol'];
  }

  $field_title = 'Will Alcohol be Served?';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'alcohol',
    'group' => 'services',
    'path' => 'services][alcohol',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'details' && array_key_exists('services', $form['form']['sections']['#value'])) {
    $fields['services']['alcohol'] = array(
      '#id' => 'field-request-services-alcohol',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-services-alcohol',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['services']['alcohol'][$key] = array(
        '#id' => 'field-request-services-alcohol-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['services']['alcohol']);

    $form_state['conditionally_required'][0][] = 'services][alcohol';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['services']['alcohol'][$key] = array(
        '#id' => 'field-request-services-alcohol-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_services_alcohol_note($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('services', $form['form']['sections']['#value'])) {
    $fields['services']['alcohol_note'] = array(
      '#id' => 'field-request-services-alcohol_note',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-services-alcohol_note',
        ),
      ),
    );

    $fields['services']['alcohol_note'][0] = array(
      '#id' => 'field-request-services-alcohol_note-0',
      '#type' => 'container',
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[alcohol][0]' . '"]' => array('value' => 1),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[alcohol][0]' . '"]' => array('value' => 0),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[alcohol][0]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
      '#attributes' => array(
        'class' => array(
          'form-item-delta',
          'form-item-delta-0',
        ),
      ),
    );

    $fields['services']['alcohol_note'][0]['markup'] = array(
      '#markup' => '',
    );

    $fields['services']['alcohol_note'][0]['markup']['#markup'] .= '<div id="mfcs-markup-request-services-alcohol_note" class="markup-request">';
    $fields['services']['alcohol_note'][0]['markup']['#markup'] .= 'A formal written request must be submitted to the President for approval of any alcohol served on campus and may require police officer presence at your event for an additional charges. ';
    $fields['services']['alcohol_note'][0]['markup']['#markup'] .= 'Contact the President\'s Office at ext. 5556 for more information. ';
    $fields['services']['alcohol_note'][0]['markup']['#markup'] .= '</div>';
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_services_open_flames($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => 0);
  if (!empty($request_state['services']['open_flames'])) {
    $values = $request_state['services']['open_flames'];
  }

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  $field_title = 'Will Open Flames be Used? <span id="mfcs-markup-request-title_display-note-services_open_flames" class="title_display-note examples">(Examples: Sternos, Grill, Candles)</span>';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'open_flames',
    'group' => 'services',
    'path' => 'services][open_flames',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('services', $form['form']['sections']['#value'])) {
    $fields['services']['open_flames'] = array(
      '#id' => 'field-request-services-open_flames',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-services-open_flames',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['services']['open_flames'][$key] = array(
        '#id' => 'field-request-open_flames-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
      );
    }
    krsort($fields['services']['open_flames']);

    $form_state['conditionally_required'][0][] = 'services][open_flames';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['services']['open_flames'][$key] = array(
        '#id' => 'field-request-services-open_flames-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_services_open_flames_note($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);

  $build = FALSE;
  if (!isset($form['form']['information_type_meeting']['#value']) || !$form['form']['information_type_meeting']['#value']) {
    $build = TRUE;
  }

  if (!$build) {
    return $values;
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('services', $form['form']['sections']['#value'])) {
    $fields['services']['open_flames_note'] = array(
      '#id' => 'field-request-services-open_flames_note',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-services-open_flames_note',
        ),
      ),
    );

    $fields['services']['open_flames_note'][0] = array(
      '#id' => 'field-request-services-open_flames_note-0',
      '#type' => 'container',
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[open_flames][0]' . '"]' => array('value' => 1),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[open_flames][0]' . '"]' => array('value' => 0),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[open_flames][0]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
      '#attributes' => array(
        'class' => array(
          'form-item-delta',
          'form-item-delta-0',
        ),
      ),
    );

    $fields['services']['open_flames_note'][0]['markup'] = array(
      '#markup' => '',
    );

    $fields['services']['open_flames_note'][0]['markup']['#markup'] .= '<div id="mfcs-markup-request-services-open_flames_note" class="markup-request">';
    $fields['services']['open_flames_note'][0]['markup']['#markup'] .= 'You must fill out the <a href="//www.mcneese.edu/node/6309" target="_blank" title="Fill out Open Flame Permit" class="markup-request-link no_print_as_link">Open Flame Permit</a> before your request can be approved.';
    $fields['services']['open_flames_note'][0]['markup']['#markup'] .= '</div>';
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_additional($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($request_state['additional'])) {
    $values = $request_state['additional'];
  }

  $field_title = 'Please Provide any Other Information or Questions you may Have for Campus Officials';
  $limits_check = array(
    'title' => $field_title,
    'field' => 'additional',
    'group' => 'top',
    'path' => 'additional',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'details' && array_key_exists('top', $form['form']['sections']['#value'])) {
    $fields['additional'] = array(
      '#id' => 'field-request-additional',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-request-item',
          'field-request-item-additional',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['additional'][$key] = array(
        '#id' => 'field-request-additional-' . $key,
        '#type' => 'textarea',
        '#title' => $field_title,
        '#default_value' => $value,
        '#rows' => 4,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#description_before' => 'Please use this comment box to provide University Police with details about reserving spaces in parking lots and/or when buildings need to be opened/closed for weekend and after-hours events.',
      );
    }
    krsort($fields['additional']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['additional'][$key] = array(
        '#id' => 'field-request-additional-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_insurance_affiliated($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);

  if (isset($request_state['insurance']) && array_key_exists('affiliated', $request_state['insurance'])) {
    $values = $request_state['insurance']['affiliated'];
  }
  else {
    $values[0] = MFCS_INSURANCE_REQUIRED_NOT;
  }

  foreach ($values as $key => $value) {
    $fields['insurance']['affiliated'][$key] = array(
      '#id' => 'field-request-insurance-affiliated-' . $key,
      '#type' => 'value',
      '#value' => empty($value) ? NULL : (int) $value,
    );
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_insurance_contractor($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);

  if (isset($request_state['insurance']) && array_key_exists('contractor', $request_state['insurance'])) {
    $values = $request_state['insurance']['contractor'];
  }
  else {
    $values[0] = MFCS_INSURANCE_REQUIRED_NOT;
  }

  foreach ($values as $key => $value) {
    $fields['insurance']['contractor'][$key] = array(
      '#id' => 'field-request-insurance-contractor-' . $key,
      '#type' => 'value',
      '#value' => empty($value) ? NULL : (int) $value,
    );
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_insurance_provided($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);

  if (isset($request_state['insurance']) && array_key_exists('provided', $request_state['insurance'])) {
    $values = $request_state['insurance']['provided'];
  }
  else {
    $values[0] = MFCS_INSURANCE_REQUIRED_NOT;
  }

  foreach ($values as $key => $value) {
    $fields['insurance']['provided'][$key] = array(
      '#id' => 'field-request-insurance-provided-' . $key,
      '#type' => 'value',
      '#value' => empty($value) ? NULL : (int) $value,
    );
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mfcs_get_request_field_insurance_unaffiliated($form, &$form_state, &$fields, &$request_state, $ps) {
  $values = array(0 => NULL);

  if (isset($request_state['insurance']) && array_key_exists('unaffiliated', $request_state['insurance'])) {
    $values = $request_state['insurance']['unaffiliated'];
  }
  else {
    $values[0] = MFCS_INSURANCE_REQUIRED_NOT;
  }

  foreach ($values as $key => $value) {
    $fields['insurance']['unaffiliated'][$key] = array(
      '#id' => 'field-request-insurance-unaffiliated-' . $key,
      '#type' => 'value',
      '#value' => empty($value) ? NULL : (int) $value,
    );
  }

  return $values;
}

/**
 * Returns an array containing request information details.
 *
 * @param array $request
 *   The pre-loaded request values array.
 *
 * @return array|bool
 *   An array containing the request information details.
 *   FALSE is returned on error.
 */
function mfcs_get_request_existing_values($request) {
  if (!is_array($request)) {
    cf_error::invalid_array('request');
    return FALSE;
  }

  $processed = array();
  $structure = mfcs_table_structure();
  foreach ($structure as $structure_group => &$group_array) {
    foreach ($group_array as $field_name => &$field_structure) {
      unset($current);
      $path_parts = explode('][', $field_structure['path']);
      $current = &$processed;

      while (!empty($path_parts)) {
        $part = array_shift($path_parts);

        if (!array_key_exists($part, $current)) {
          $current[$part] = array();
        }

        if (is_array($current[$part])) {
          $current = &$current[$part];
        }
        else {
          // this is not a valid location, continue the foreach loop and not the while loop.
          continue 2;
        }
      }

      if (array_key_exists('storage', $field_structure) && $field_structure['storage'] == 'alternate') {
        if (array_key_exists('explode', $field_structure['alternate_method'])) {
          $alternate_array = array(0 => NULL);

          foreach ($field_structure['alternate'] as $alternate_name => $alternate_structure) {
            if (!empty($request[$structure_group][$alternate_name])) {
              foreach ($request[$structure_group][$alternate_name] as $alternate_key => $alternate_object) {
                $alternate_array[$alternate_key][$alternate_structure['key']] = NULL;

                if (property_exists($alternate_object, 'value')) {
                  $alternate_array[$alternate_key][$alternate_structure['key']] = $alternate_object->value;
                }
              }
            }
          }

          // handle 'exception' case.
          foreach ($field_structure['alternate_exception'] as $alternate_name => $alternate_structure) {
            if (empty($alternate_structure['exception_path'])) {
              continue;
            }

            $alternate_parts = explode('][', $alternate_structure['exception_path']);

            if (count($alternate_parts) != 2 || !isset($structure[$alternate_parts[0]][$alternate_parts[1]])) {
              continue;
            }

            $alternate_exception = &$structure[$alternate_parts[0]][$alternate_parts[1]];

            if (!empty($request[$alternate_parts[0]][$alternate_parts[1]])) {
              foreach ($request[$alternate_parts[0]][$alternate_parts[1]] as $alternate_key => $alternate_object) {
                $alternate_array[$alternate_key][$alternate_structure['key']] = NULL;

                if (property_exists($alternate_object, 'value')) {
                  $alternate_array[$alternate_key][$alternate_structure['key']] = $alternate_object->value;
                }
              }
            }

            unset($alternate_exception);
            unset($alternate_parts);
          }

          $current = array(0 => NULL);
          foreach ($alternate_array as $alternate_key => &$alternate_values) {
            if (!empty($field_structure['alternate_method']['explode'][0]) && !empty($alternate_values)) {
              ksort($alternate_values);
              $current[$alternate_key] = implode($field_structure['alternate_method']['explode'][0], $alternate_values);
            }
            else {
              $current[$alternate_key] = NULL;
            }
          }
        }
      }
      else {
        $current = array();

        if (!empty($request[$structure_group][$field_name])) {
          foreach ($request[$structure_group][$field_name] as $field_key => $field_object) {
            if (empty($field_structure['columns'])) {
              if (property_exists($field_object, 'value')) {
                $current[$field_object->delta] = $field_object->value;

                if (array_key_exists('storage', $field_structure) && is_string($field_structure['storage']) && !empty($field_structure['storage'])) {
                  $storage_method = $field_structure['storage'];

                  if (array_key_exists($storage_method, $field_structure) && is_array($field_structure[$storage_method])) {
                    $converted = mfcs_convert_value_from_database_format($current[$field_object->delta], $storage_method, $field_structure[$storage_method]);

                    if ($converted !== FALSE) {
                      $current[$field_object->delta] = $converted;
                    }
                  }
                }
              }
              else {
                $current[$field_object->delta] = NULL;
              }
            }
            else {
              $current[$field_object->delta] = array();

              foreach ($field_structure['columns'] as $column_id => $column_name) {
                $current[$field_object->delta][$column_id] = NULL;

                if (property_exists($field_object, $column_name)) {
                  $current[$field_object->delta][$column_id] = $field_object->{$column_name};

                  if (array_key_exists('storage', $field_structure) && is_string($field_structure['storage']) && !empty($field_structure['storage'])) {
                    $storage_method = $field_structure['storage'];

                    if (array_key_exists($storage_method, $field_structure) && is_array($field_structure[$storage_method])) {
                      $converted = mfcs_convert_value_from_database_format($current[$field_object->delta][$column_id], $storage_method, $field_structure[$storage_method]);

                      if ($converted !== FALSE) {
                        $current[$field_object->delta][$column_id] = $converted;
                      }
                    }
                  }
                }
              }
            }
          }

          if (empty($current)) {
            $current[0] = NULL;
          }
        }
      }
    }
  }

  return $processed;
}

/**
 * Build the table structure for saving new or existing information.
 *
 * @param array $structure
 *   Describes how the secondary tables are populated and processed.
 * @param array $structure_current
 *   The 'current' table with the fields to be updated.
 * @param array $structure_field_tables
 *   A collection of 'field' tables to create or update.
 * @param string $table_name
 *   Name of the current table being processed.
 * @param string $field_name
 *   Name of the current field being processed.
 * @param string $db_table_field
 *   The name of the 'field' table as defined the SQL database.
 * @param array $user_data
 *   An array of user related information:
 *   - 'user_id': The users ID.
 * @param array $form_state
 *   The form state for advanced processing of fields that depend on other
 *   fields when they are converted into the database format.
 * @param array|null $request
 *   (optional) When NULL, this created tables are new tables.
 *   When an array, the created tables are updates to existing values.
 *   The array should be populated by mfcs_load_request_by_id().
 *
 * @see: mfcs_table_structure()
 * @see: mfcs_load_request_by_id()
 */
function mfcs_build_table_structure($structure, &$structure_current, &$structure_field_tables, $table_name, $field_name, $db_table_field, $user_data, $values, $form_state, $request = NULL) {
  $instance = mfcs_instance();

  if (is_null($request)) {
    $request_id = NULL;
    $revision = 0;
  }
  else {
    $request_id = $request['mer']['id'][0]->value;

    $revision = mfcs_load_request_revision_number($request_id, $db_table_field);
    if ($revision === FALSE) {
      watchdog(MFCS_WATCHDOG_ID, 'Failed to load revision number for request %request_id while building the table structure, forcing revision number to be 0.', array('%request_id' => $request_id), WATCHDOG_ERROR);
      $revision = 0;
    }
    elseif (is_null($revision)) {
      $revision = 0;
    }
    else {
      $revision++;
    }
  }

  if (!isset($structure_field_tables[$db_table_field])) {
    $structure_field_tables[$db_table_field] = array();
  }

  if (array_key_exists('storage', $structure[$table_name][$field_name])) {
    if (!is_string($structure[$table_name][$field_name]['storage']) || empty($structure[$table_name][$field_name]['storage'])) {
      watchdog(MFCS_WATCHDOG_ID, 'Invalid storage structure specification for the field %field_name in the table %table_name. This must be a non-empty string.', array('%field_name' => $field_name, '%table_name' => $table_name), WATCHDOG_ERROR);
      return;
    }

    $storage_key = $structure[$table_name][$field_name]['storage'];
    if (!array_key_exists($storage_key, $structure[$table_name][$field_name]) || empty($structure[$table_name][$field_name][$storage_key])) {
      watchdog(MFCS_WATCHDOG_ID, 'The storage key %storage_key was not found in the structure specification for the field %field_name in the table %table_name. This must be a non-empty string.', array('%storage_key' => $storage_key, '%field_name' => $field_name, '%table_name' => $table_name), WATCHDOG_ERROR);
      return;
    }

    // alternate is used for handling fields that are stored in multiple separate tables.
    if ($storage_key == 'alternate') {
      unset($structure_current[$table_name . '_' . $field_name]);
      unset($structure_field_tables[$db_table_field]);

      foreach ($values as $delta => $value) {
        $alternate_values = array();
        if (!empty($structure[$table_name][$field_name]['alternate_method'])) {
          foreach ($structure[$table_name][$field_name]['alternate_method'] as $method_name => $method_values) {
            if ($method_name == 'explode') {
              $alternate_values = explode($method_values[0], $value, $method_values[1]);
            }

            break;
          }
        }

        foreach ($structure[$table_name][$field_name]['alternate'] as $alternate_name => &$alternate_data) {
          if (!array_key_exists('key', $alternate_data)) continue;

          $db_table_field = 'mfcs_field_' . $table_name . '_' . $alternate_name;

          if (!isset($structure_field_tables[$db_table_field])) {
            $structure_field_tables[$db_table_field] = array();
          }

          $structure_field_tables[$db_table_field][$delta] = array();
          $structure_field_tables[$db_table_field][$delta]['user_id'] = $user_data['user_id'];
          $structure_field_tables[$db_table_field][$delta]['date'] = $instance;
          $structure_field_tables[$db_table_field][$delta]['delta'] = $delta;
          $structure_field_tables[$db_table_field][$delta]['revision'] = $revision;

          if (isset($alternate_values[$alternate_data['key']]) && (is_numeric($alternate_values[$alternate_data['key']]) || !empty($alternate_values[$alternate_data['key']]))) {
            $structure_field_tables[$db_table_field][$delta]['value'] = $alternate_values[$alternate_data['key']];

            // fix_lines works by stripping out all carriage returns and replacing them with a single newline.
            if (array_key_exists('fix_lines', $structure[$table_name][$field_name]) && $structure[$table_name][$field_name]['fix_lines']) {
              $structure_field_tables[$db_table_field][$delta]['value'] = preg_replace("/\r\n/", "\n", $structure_field_tables[$db_table_field][$delta]['value']);
              $structure_field_tables[$db_table_field][$delta]['value'] = preg_replace("/\r/", "\n", $structure_field_tables[$db_table_field][$delta]['value']);
            }
          }

          if (!is_null($request_id)) {
            $structure_field_tables[$db_table_field][$delta]['request_id'] = $request_id;
          }

          $structure_current[$table_name . '_' . $alternate_name] = $revision;
        }
      }
    }
    else {
      $structure_current[$table_name . '_' . $field_name] = $revision;

      foreach ($values as $delta => $value) {
        $structure_field_tables[$db_table_field][$delta] = array();
        $structure_field_tables[$db_table_field][$delta]['user_id'] = $user_data['user_id'];
        $structure_field_tables[$db_table_field][$delta]['date'] = $instance;
        $structure_field_tables[$db_table_field][$delta]['delta'] = $delta;
        $structure_field_tables[$db_table_field][$delta]['revision'] = $revision;

        if (!is_null($request)) {
          $structure_field_tables[$db_table_field][$delta]['request_id'] = $request_id;
        }

        $storage_values = FALSE;
        if (isset($structure[$table_name][$field_name][$storage_key]) && is_array($structure[$table_name][$field_name][$storage_key])) {
          $storage_values = mfcs_convert_value_to_database_format($value, $storage_key, $structure[$table_name][$field_name][$storage_key], $form_state, $delta);
        }

        if ($storage_values !== FALSE && !is_null($storage_values)) {
          $structure_field_tables[$db_table_field][$delta]['value'] = $storage_values;

          // fix_lines works by stripping out all carriage returns and replacing them with a single newline.
          if (array_key_exists('fix_lines', $structure[$table_name][$field_name]) && $structure[$table_name][$field_name]['fix_lines']) {
            $structure_field_tables[$db_table_field][$delta]['value'] = preg_replace("/\r\n/", "\n", $structure_field_tables[$db_table_field][$delta]['value']);
            $structure_field_tables[$db_table_field][$delta]['value'] = preg_replace("/\r/", "\n", $structure_field_tables[$db_table_field][$delta]['value']);
          }
        }

        if (isset($structure[$table_name][$field_name]['type'])) {
          // exception case: the time type is also stored in a field with the y/m/d set to 0 for quick and easy relative searches at the cost of extra space usage.
          if ($structure[$table_name][$field_name]['type'] == 'time' && $storage_key == 'timestamp') {
            $storage_values = FALSE;
            if (isset($structure[$table_name][$field_name][$storage_key]) && is_array($structure[$table_name][$field_name][$storage_key])) {
              $storage_options = $structure[$table_name][$field_name][$storage_key];
              unset($storage_options['relative_to']);
              $storage_values = mfcs_convert_value_to_database_format($value, 'timestamp_relative', $storage_options);
              unset($storage_options);
            }

            if ($storage_values !== FALSE && !is_null($storage_values)) {
              $structure_field_tables[$db_table_field][$delta]['value_relative'] = $storage_values;
            }
          }
          // exception case: the date_popup type is also stored in a month and a day field for quick and easy relative searches at the cost of extra space usage.
          elseif ($structure[$table_name][$field_name]['type'] == 'date' && $storage_key == 'date_popup') {
            $storage_values = FALSE;
            if (isset($structure[$table_name][$field_name][$storage_key]) && is_array($structure[$table_name][$field_name][$storage_key])) {
              $storage_options = $structure[$table_name][$field_name][$storage_key];
              unset($storage_options['relative_to']);
              $storage_values = mfcs_convert_value_to_database_format($value, 'date_month', $storage_options);
              unset($storage_options);
            }

            if ($storage_values !== FALSE && !is_null($storage_values)) {
              $structure_field_tables[$db_table_field][$delta]['value_month'] = $storage_values;
            }

            $storage_values = FALSE;
            if (isset($structure[$table_name][$field_name][$storage_key]) && is_array($structure[$table_name][$field_name][$storage_key])) {
              $storage_options = $structure[$table_name][$field_name][$storage_key];
              unset($storage_options['relative_to']);
              $storage_values = mfcs_convert_value_to_database_format($value, 'date_day', $storage_options);
              unset($storage_options);
            }

            if ($storage_values !== FALSE && !is_null($storage_values)) {
              $structure_field_tables[$db_table_field][$delta]['value_day'] = $storage_values;
            }
          }
        }
      }
    }
  }
  else {
    $structure_current[$table_name . '_' . $field_name] = $revision;

    foreach ($values as $delta => $value) {
      $structure_field_tables[$db_table_field][$delta] = array();
      $structure_field_tables[$db_table_field][$delta]['user_id'] = $user_data['user_id'];
      $structure_field_tables[$db_table_field][$delta]['date'] = $instance;
      $structure_field_tables[$db_table_field][$delta]['delta'] = $delta;
      $structure_field_tables[$db_table_field][$delta]['revision'] = $revision;

      if (!is_null($request)) {
        $structure_field_tables[$db_table_field][$delta]['request_id'] = $request_id;
      }

      if (isset($structure[$table_name][$field_name]['columns'])) {
        foreach ($structure[$table_name][$field_name]['columns'] as $special_field_id => &$special_field_name) {
          if (isset($value[$special_field_id]) && $value[$special_field_id] > 0) {
            $structure_field_tables[$db_table_field][$delta][$special_field_name] = $special_field_id;
          }
        }
      }
      elseif (is_numeric($value) || !empty($value)) {
        $structure_field_tables[$db_table_field][$delta]['value'] = $value;

        // fix_lines works by stripping out all carriage returns and replacing them with a single newline.
        if (array_key_exists('fix_lines', $structure[$table_name][$field_name]) && $structure[$table_name][$field_name]['fix_lines']) {
          $structure_field_tables[$db_table_field][$delta]['value'] = preg_replace("/\r\n/", "\n", $structure_field_tables[$db_table_field][$delta]['value']);
          $structure_field_tables[$db_table_field][$delta]['value'] = preg_replace("/\r/", "\n", $structure_field_tables[$db_table_field][$delta]['value']);
        }
      }
    }
  }
}

/**
 * Prepare request values for use by mfcs_set_request_values()
 *
 * This is provided as a separate function so that the return value can be explititly handled on error.
 *
 * @return array|FALSE
 *   A combined array to be processed by mfcs_set_request_values().
 *   FALSE is returned on error.
 *
 * @see: mfcs_set_request_values()
 */
function mfcs_prepare_request_values($request, $changes) {
  $instance = mfcs_instance();
  $user = cf_current_user();

  $prepared = array(
    'mfcs_requests' => array(
      'updated' => $instance,
    ),
    'mfcs_request_revisions' => array(),
    'request_revision' => NULL,
    'request_revision_next' => NULL,
  );

  if (is_null($request)) {
    $prepared['mfcs_requests']['user_id'] = $user->uid;
    $prepared['mfcs_requests']['created'] = $instance;
  }
  else {
    $request_id = (int) $request['mer']['id'][0]->value;
  }

  foreach ($changes as $column => $value) {
    $prepared['mfcs_requests'][$column] = $value;
  }

  if (is_null($request)) {
    $prepared['request_revision'] = 0;
    $prepared['request_revision_next'] = 0;

    $prepared['mfcs_request_revisions'] = array(
      'date' => $instance,
      'revision' => 0,
      'user_id' => $user->uid,
    );
  }
  else {
    $prepared['request_revision'] = mfcs_load_request_revision_number($request_id);
    if ($prepared['request_revision'] === FALSE) {
      watchdog(MFCS_WATCHDOG_ID, 'Failed to load revision number for request %request_id while trying to save the request.', array('%request_id' => $request_id), WATCHDOG_ERROR);
      return FALSE;
    }
    elseif (is_null($prepared['request_revision'])) {
      $prepared['request_revision_next'] = 0;
    }
    else {
      $prepared['request_revision_next'] = $prepared['request_revision'] + 1;
    }

    $prepared['mfcs_request_revisions'] = array(
      'request_id' => $request_id,
      'date' => $instance,
      'revision' => $prepared['request_revision_next'],
      'user_id' => $user->uid,
    );
  }

  return $prepared;
}

/**
 * Assign changes to a request to the database.
 *
 * @param object|NULL $request
 *   The (unchanged) request object.
 *   If NULL, this is is a new request.
 * @param array $prepared
 *   An array as processed by mfcs_prepare_request_values()
 * @param array $existing
 *   An existing array as populated by mfcs_get_request_existing_values().
 *   This parameter is ignored when $request is NULL.
 * @param array $form_state
 *   An array of new values to assign using the same structure as that stored in $form_state.
 * @param bool $review_log_log
 *   (optional) When TRUE, review log is restarted depending on whether or not lock is in place.
 *   When FALSE, the review log lock is ignored.
 *   This parameter is ignored when $request is NULL.
 *
 * @return bool|NULL
 *   TRUE on success.
 *   NULL when nothing was done.
 *   FALSE on error.
 *
 * @see: mfcs_prepare_request_values()
 * @see: mfcs_get_request_existing_values()
 */
function mfcs_set_request_values($request, $prepared, $structure, $existing, &$form_state, $review_log_lock = FALSE) {
  $instance = mfcs_instance();
  $success = TRUE;

  $user = cf_current_user();
  $user_data = array(
    'user_id' => $user->uid,
  );

  if (!is_null($request)) {
    $request_id = (int) $request['mer']['id'][0]->value;
    $status = (int) $request['top']['status'][0]->value;
    $step = (int) $request['top']['step'][0]->value;
    $requirements = $request['current']['requirements'];
  }

  mfcs_include(MFCS_INCLUDE_TABLE);

  $structure_current = array();
  $structure_field_tables = array();

  if (is_null($request)) {
    foreach ($structure as $table_name => &$fields) {
      foreach ($fields as $field_name => &$field_values) {
        $structure_current[$table_name . '_' . $field_name] = 0;

        // manual processing fields must not be auto-populated through the normal form submit process.
        if (array_key_exists('manual_processing', $field_values) && $field_values['manual_processing']) {
          continue;
        }

        // store the values for each individual mfcs_field_* table.
        $db_table_field = 'mfcs_field_' . $table_name . '_' . $field_name;

        $current = &$form_state['values'];
        $parts = explode('][', $field_values['path']);
        if (empty($parts)) {
          unset($current);
          $current = NULL;
        }
        else {
          foreach ($parts as $part) {
            if (is_array($current) && array_key_exists($part, $current)) {
              $current = &$current[$part];

              if (empty($current)) {
                $current = NULL;
              }
            }
            else {
              watchdog(MFCS_WATCHDOG_ID, 'Failed to process \'@part\' for \'@field_name\' in \'@table_name\' during form submit. Invalid array or missing array key (new).', array('@part' => $part, '@field_name' => $field_name, '@table_name' => $table_name), WATCHDOG_ERROR);
              unset($current);
              $current = NULL;
              continue 2;
            }
          }
        }

        mfcs_build_table_structure($structure, $structure_current, $structure_field_tables, $table_name, $field_name, $db_table_field, $user_data, $current, $form_state);
      }
    }
  }
  else {
    foreach ($structure as $table_name => &$fields) {
      foreach ($fields as $field_name => &$field_values) {
        // manual processing fields must not be auto-populated through the normal form submit process.
        if (array_key_exists('manual_processing', $field_values) && $field_values['manual_processing']) {
          continue;
        }

        // load the path so that it can be checked to see if anything needs to be changed.
        $parts = explode('][', $field_values['path']);
        if (empty($parts)) {
          // this should not happen.
          continue;
        }

        unset($current_existing);
        unset($current_new);

        $current_existing = &$existing;
        $current_new = &$form_state['values'];

        while (!empty($parts)){
          $part = array_shift($parts);

          if (!is_array($current_existing) || !array_key_exists($part, $current_existing)) {
            watchdog(MFCS_WATCHDOG_ID, 'Failed to process \'@part\' for \'@field_name\' in \'@table_name\' during form submit. Invalid array or missing array key (existing).', array('@part' => $part, '@field_name' => $field_name, '@table_name' => $table_name), WATCHDOG_ERROR);
            continue 2;
          }

          if (!is_array($current_new) || !array_key_exists($part, $current_new)) {
            watchdog(MFCS_WATCHDOG_ID, 'Failed to process \'@part\' for \'@field_name\' in \'@table_name\' during form submit. Invalid array or missing array key (new).', array('@part' => $part, '@field_name' => $field_name, '@table_name' => $table_name), WATCHDOG_ERROR);
            continue 2;
          }

          $current_existing = &$current_existing[$part];
          $current_new = &$current_new[$part];
        }
        unset($part);

        // when the values are the same, do not update them.
        if ($current_existing == $current_new) {
          continue;
        }

        $db_table_field = 'mfcs_field_' . $table_name . '_' . $field_name;

        mfcs_build_table_structure($structure, $structure_current, $structure_field_tables, $table_name, $field_name, $db_table_field, $user_data, $current_new, $form_state, $request);
      }
    }
  }

  if (empty($structure_field_tables)) {
    return NULL;
  }

  $transaction = db_transaction();

  try {
    // now perform the operations.
    if (is_null($request)) {
      $query = db_insert('mfcs_requests');
      $prepared['mfcs_requests']['creator'] = $prepared['mfcs_requests']['user_id'];
      $query->fields($prepared['mfcs_requests']);
      $request_id = $query->execute();

      if (!is_numeric($request_id) || $request_id <= 0) {
        // the request was not properly saved, present an error here.
        $transaction->rollback();
        return FALSE;
      }
      unset($prepared['mfcs_requests']['creator']);

      // save the request id so that a programmatic caller can determine the request_id.
      $form_state['created_request_id'] = $request_id;

      $prepared['mfcs_request_revisions']['request_id'] = $request_id;
    }
    else {
      $query = db_update('mfcs_requests');
      $query->fields($prepared['mfcs_requests']);
      $query->condition('id', $request_id);
      $query->execute();
    }

    $processed_mfcs_request_revisions = &drupal_static('processed_mfcs_request_revisions_' . $instance . '_' . $user->uid . '_' . $request_id, FALSE);
    if (!$processed_mfcs_request_revisions) {
      $query = db_insert('mfcs_request_revisions');
      $query->fields($prepared['mfcs_request_revisions']);
      $query->execute();

      $processed_mfcs_request_revisions = TRUE;
    }

    foreach ($structure_field_tables as $table_name => $fields) {
      foreach ($fields as $delta => $values) {
        $values['request_id'] = $request_id;

        $query = db_insert($table_name);
        $query->fields($values);
        $query->execute();
      }
    }

    // if the request has been updated while it is in a locked status, the review log must be reset/restarted.
    /*
    if (!is_null($request) && $review_log_lock && $status === MFCS_REQUEST_STATUS_LOCKED && $step !== MFCS_REVIEW_STEP_VENUE_AVAILABLE) {
      $step_revision = $request['current']['top']['step'] + 1;
      $structure_current['top_step'] = $step_revision;

      $mfcs_field_top_step = array(
        'request_id' => $request_id,
        'revision' => $step_revision,
        'delta' => 0,
        'user_id' => $user->uid,
        'date' => $instance,
        'value' => MFCS_REVIEW_STEP_VENUE_AVAILABLE,
      );

      $mfcs_requests = array(
        'step' => MFCS_REVIEW_STEP_VENUE_AVAILABLE,
        'updated' => $instance,
      );

      $query = db_update('mfcs_requests');
      $query->fields($mfcs_requests);
      $query->condition('id', $request_id);
      $query->execute();

      $query = db_insert('mfcs_field_top_step');
      $query->fields($mfcs_field_top_step);
      $query->execute();

      $query = db_delete('mfcs_reviewed');
      $query->condition('request_id', $request_id);
      $query->execute();

      $review_revision = mfcs_load_request_revision_number($request_id, 'mfcs_field_review_review');

      if ($review_revision === FALSE || is_null($review_revision)) {
        $review_revision = 0;
      }
      else {
        $review_revision++;
      }

      $structure_current['review_review'] = $review_revision;

      $mfcs_field_review_review = array(
        'request_id' => $request_id,
        'revision' => $review_revision,
        'delta' => 0,
        'user_id' => $user->uid,
        'date' => $instance,
        'step' => $step,
        'decision' => MFCS_REVIEW_DECISION_AMENDMENT,
        'message' => '',
      );

      $query = db_insert('mfcs_field_review_review');
      $query->fields($mfcs_field_review_review);
      $query->execute();
    }
    */

    // update the current revisions table.
    if (is_null($request)) {
      $structure_current['request_id'] = $request_id;
      $query = db_insert('mfcs_current');
      $query->fields($structure_current);
      $query->execute();
    }
    else {
      $query = db_update('mfcs_current');
      $query->condition('request_id', $request_id);
      $query->fields($structure_current);
      $query->execute();
    }

    // enforce transaction execution
    unset($transaction);
  }
  catch (Error $e) {
    $transaction->rollback();
    cf_error::on_query_execution($e);

    $success = FALSE;
  }
  catch (Exception $e) {
    $transaction->rollback();
    cf_error::on_query_execution($e);

    $success = FALSE;
  }

  return $success;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
