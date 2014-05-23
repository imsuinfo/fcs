<?php

/**
 * @file
 * Defines McNeese event workflow event commonly used form and structure functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

 /**
 * Return a list of sections and their relevant information.
 *
 * Manually updated to reflect mcneese_event_workflow_get_event_fields().
 *
 * @return array
 *   An array containing the generated sections.
 *
 * @see mcneese_event_workflow_get_event_fields()
 */
function mcneese_event_workflow_get_event_section_information() {
  $information = array();

  $information['information'] = array(
    'steps' => array(
      'location',
      'rooms',
      'standard',
      'details',
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

  return $information;
}

/**
 * Return a list of event form fields.
 *
 * This requires the following fields to be defined before calling:
 * - $form['form']['sections']
 * - $form['form']['step']
 * - $form['form']['paramater_1']
 * - $form['form']['paramater_2']
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
function mcneese_event_workflow_get_event_fields($form, &$form_state) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');

    // @todo: provide an error on the screen using the form markup.
    return array();
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    // @todo: provide an error on the screen using the form markup.
    return array();
  }

  $ps = NULL;

  if (!isset($form_state['values']['event'])) {
    if (isset($form['form']['existing']['#value']['event'])) {
      $form_state['values']['event'] = $form['form']['existing']['#value']['event'];
    }
    else {
      $form_state['values']['event'] = array();
    }
  }
  $event_state = &$form_state['values']['event'];

  $fields = array();
  if (isset($form['form']['paramater_1']['#value']) && isset($form['form']['paramater_2']['#value'])) {
    if ($form['form']['step']['#value'] == 'location' && !array_key_exists('location', $event_state)) {
      $catagory_list_options = event_workflow_get_location_list_options('radios');

      if (array_key_exists($form['form']['paramater_1']['#value'], $catagory_list_options)) {
        $room_list_options = event_workflow_get_room_list_options($event_state['location'], 'special');

        if (array_key_exists($form['form']['paramater_2']['#value'], $room_list_options)) {
          $event_state['location'] = $form['form']['paramater_1']['#value'];
          $event_state['room_list'] = $form['form']['paramater_2']['#value'];
          $form['form']['step']['#value'] = 'standard';
        }
      }
      else {
        $event_state['location'] = $form['form']['paramater_1']['#value'];
        $form['form']['step']['#value'] = 'rooms';
      }
    }
  }


  if (!isset($form['form']['sections']['#value']) || !is_array($form['form']['sections']['#value'])) {
    $form['form']['sections'] = array(
      '#type' => 'value',
      '#value' => mcneese_event_workflow_get_event_section_information(),
    );
  }


  // Fieldset: Location
  mcneese_event_workflow_get_event_field_location($form, $form_state, $fields, $ps);


  // Fieldset: Room List
  $room = mcneese_event_workflow_get_event_field_room_list($form, $form_state, $fields, $ps);

  $form['form']['room_information'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );

  if (!empty($room)) {
    $room_information_parts = explode('_', $room, 3);

    if (count($room_information_parts) == 3) {
      $form['form']['room_information']['#value'] = mcneese_event_workflow_load_buildings($room_information_parts[0], $room_information_parts[1], $room_information_parts[2]);

      if (empty($form['form']['room_information']['#value'])) {
        $form['form']['room_information']['#value'] = NULL;
      }
      else {
        $form['form']['room_information']['#value'] = array_pop($form['form']['room_information']['#value']);
      }
    }
    unset($room_information_parts);
  }


  // Fieldset: Dates
  $ps = 'dates';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($event_state['dates'])) {
    $event_state['dates'] = array();
  }

  $fields['dates'] = array(
    '#id' => 'section-event-dates',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('dates', $form['form']['sections']['#value'])) {
    $fields['dates'] = array(
      '#id' => 'section-event-dates',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Date Settings",
    );
  }

  mcneese_event_workflow_get_event_field_dates_date($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_dates_time_start($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_dates_time_stop($form, $form_state, $fields, $ps);


  // Fieldset: Coordinator
  $ps = 'coordinator';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($event_state['coordinator'])) {
    $event_state['coordinator'] = array();
  }

  $fields['coordinator'] = array(
    '#id' => 'section-event-coordinator',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['coordinator'] = array(
      '#id' => 'section-event-coordinator',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Coordinator Settings",
    );
  }

  mcneese_event_workflow_get_event_field_coordinator_name($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_coordinator_phone($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_coordinator_email($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_coordinator_classification($form, $form_state, $fields, $ps);


  // Fieldset: Information
  $ps = 'information';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($event_state['information'])) {
    $event_state['information'] = array();
  }

  $fields['information'] = array(
    '#id' => 'section-event-information',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['information'] = array(
      '#id' => 'section-event-information',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Event Information",
    );
  }

  $information_title = mcneese_event_workflow_get_event_field_information_title($form, $form_state, $fields, $ps);
  $information_attendance = mcneese_event_workflow_get_event_field_information_attendance($form, $form_state, $fields, $ps);
  $information_type = mcneese_event_workflow_get_event_field_information_type($form, $form_state, $fields, $ps);

  $form['form']['information_type'] = array(
    '#type' => 'value',
    '#value' => $information_type,
  );

  $type_meeting = FALSE;
  if ($form['form']['information_type']['#value'] == 57) {
    $type_meeting = TRUE;
  }


  // Fieldset: Plans
  $ps = 'plans';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($event_state['plans'])) {
    $event_state['plans'] = array();
  }

  $fields['plans'] = array(
    '#id' => 'section-event-plans',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('plans', $form['form']['sections']['#value'])) {
    $fields['plans'] = array(
      '#id' => 'section-event-plans',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Event Plans",
    );
  }

  mcneese_event_workflow_get_event_field_plans_note($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_plans_description($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_plans_activities($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_plans_dates($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_plans_audience($form, $form_state, $fields, $ps);


  // Fieldset: Registration
  $ps = 'registration';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($event_state['registration'])) {
    $event_state['registration'] = array();
  }

  $fields['registration'] = array(
    '#id' => 'section-event-registration',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration'] = array(
      '#id' => 'section-event-registration',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Event Registration",
    );
  }

  mcneese_event_workflow_get_event_field_registration_require($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_registration_website($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_registration_phone($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_registration_ticket_price($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_registration_ticket_dates($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_registration_ticket_website($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_registration_ticket_phone($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_registration_generate_revenue($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_registration_revenue_generated($form, $form_state, $fields, $ps);


  // Fieldset: Setup
  $ps = 'setup';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($event_state['setup'])) {
    $event_state['setup'] = array();
  }

  $fields['setup'] = array(
    '#id' => 'section-event-setup',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup'] = array(
      '#id' => 'section-event-setup',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Event Setup",
    );
  }

  mcneese_event_workflow_get_event_field_setup_rectangular_tables_8ft($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_setup_round_tables_8ft($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_setup_other_tables($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_setup_standard_blue_chairs($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_setup_podium($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_setup_portable_stage($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_setup_portable_stage_configuration($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_setup_security($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_setup_parking_assistance($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_setup_parking_assistance_area($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_setup_road_closures($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_setup_road_closures_details($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_setup_special_requests($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_setup_special_requests_details($form, $form_state, $fields, $ps);


  // Fieldset: Presentation
  $ps = 'presentation';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($event_state['presentation'])) {
    $event_state['presentation'] = array();
  }

  $fields['presentation'] = array(
    '#id' => 'section-event-presentation',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation'] = array(
      '#id' => 'section-event-presentation',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Event Presentation",
    );
  }

  // mising: external_audio_person  // if (!$meeting_event) {
  mcneese_event_workflow_get_event_field_presentation_technical_equipment($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_technical_equipment_details($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_technical_equipment_microphone($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_external_audio_person($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_external_audio_person_name($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_external_audio_person_email($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_external_audio_person_phone($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_production($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_production_name($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_production_email($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_production_phone($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_publicity($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_publicity_details($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_printed_material($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_university_logo($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_designing_material($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_designing_material_name($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_designing_material_email($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_presentation_designing_material_phone($form, $form_state, $fields, $ps);


  // Fieldset: Services
  $ps = 'services';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($event_state['services'])) {
    $event_state['services'] = array();
  }

  $fields['services'] = array(
    '#id' => 'section-event-services',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('services', $form['form']['sections']['#value'])) {
    $fields['services'] = array(
      '#id' => 'section-event-services',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Event Services",
    );
  }

  mcneese_event_workflow_get_event_field_services_food_served($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_services_food_caterer($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_services_alcohol($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_services_alcohol_note($form, $form_state, $fields, $ps);
  mcneese_event_workflow_get_event_field_services_open_flames($form, $form_state, $fields, $ps);


  // Fieldset: Additional
  mcneese_event_workflow_get_event_field_additional($form, $form_state, $fields, $ps);


  if ($form['form']['step']['#value'] == 'details' && array_key_exists('message', $form['form']['sections']['#value'])) {
    $fields['message'] = array(
      '#markup' => '',
    );

    $fields['message']['#markup'] .= '<div id="markup-event-message" class="markup-event">';
    $fields['message']['#markup'] .= 'Once your request has been approved appropriate campus personnel, you will be emailed with the outcome. ';
    $fields['message']['#markup'] .= 'If the event receives approval, you will also receive an itemized detail of any and all charges that will be assessed to you. ';
    $fields['message']['#markup'] .= 'You must agree and accept the terms offered for the venue to be reserved. ';
    $fields['message']['#markup'] .= '</div>';
  }

  return $fields;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_location($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('location', $form_state['values']['event'])) {
    $value = $form_state['values']['event']['location'];
  }

  if ($form['form']['step']['#value'] == 'location' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['location'] = array(
      '#id' => 'field-event-location',
      '#type' => 'radios',
      '#title' => '<span class="strong underline">Please indicate the requested facility or event location?</span>',
      '#options' => event_workflow_get_location_list_options('radios'),
      '#default_value' => $value,
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'location';
  }
  else {
    $fields['location'] = array(
      '#id' => 'field-event-location',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_room_list($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('room_list', $form_state['values']['event'])) {
    $value = $form_state['values']['event']['room_list'];
  }

  if ($form['form']['step']['#value'] == 'rooms' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['room_list'] = array(
      '#id' => 'field-event-room_list',
      '#type' => 'radios',
      '#title' => 'Select a Room',
      '#options' => event_workflow_get_room_list_options($form_state['values']['event']['location'], 'special'),
      '#default_value' => $value,
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'room_list';
  }
  else {
    $fields['room_list'] = array(
      '#id' => 'field-event-room_list',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_dates_date($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('date', $form_state['values']['event']['dates'])) {
    $value = $form_state['values']['event']['dates']['date'];
  }

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('dates', $form['form']['sections']['#value'])) {
    $fields['dates']['date'] = array(
      '#id' => 'field-event-date',
      '#type' => 'date_popup',
      '#title' => 'Date of Event',
      '#default_value' => $value,
      '#date_format' => "Y/m/d",
      '#date_label_position' => 'invisible',
      '#expected_format' => array('date' => "Y/m/d"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'dates][date';
  }
  else {
    $fields['dates']['date'] = array(
      '#id' => 'field-event-dates-date',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_dates_time_start($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('time_start', $form_state['values']['event']['dates'])) {
    $value = $form_state['values']['event']['dates']['time_start'];
  }

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('dates', $form['form']['sections']['#value'])) {
    $fields['dates']['time_start'] = array(
      '#id' => 'field-event-time_start',
      '#type' => 'textfield',
      '#title' => 'Event Start Time',
      '#default_value' => $value,
      '#size' => 8,
      '#maxlength' => 8,
      '#expected_format' => array('date' => "h:ia"),
      '#description' => "E.g, 12:31pm",
    );
  }
  else {
    $fields['dates']['time_start'] = array(
      '#id' => 'field-event-dates-time_start',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_dates_time_stop($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('time_stop', $form_state['values']['event']['dates'])) {
    $value = $form_state['values']['event']['dates']['time_stop'];
  }

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('dates', $form['form']['sections']['#value'])) {
    $fields['dates']['time_stop'] = array(
      '#id' => 'field-event-time_stop',
      '#type' => 'textfield',
      '#title' => 'Event Stop Time',
      '#default_value' => $value,
      '#size' => 8,
      '#maxlength' => 8,
      '#expected_format' => array('date' => "h:ia"),
      '#description' => "E.g, 12:31pm",
    );
  }
  else {
    $fields['dates']['time_stop'] = array(
      '#id' => 'field-event-dates-time_stop',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_coordinator_name($form, &$form_state, &$fields, $ps) {
  if (!array_key_exists('name', $form_state['values']['event']['coordinator'])) {
    $form_state['values']['event']['coordinator']['name'] = '';

    if (!empty($form['form']['user']['#value']->field_user_last_name['und'][0]['safe_value'])) {
      $form_state['values']['event']['coordinator']['name'] .= $form['form']['user']['#value']->field_user_last_name['und'][0]['safe_value'];
    }

    if (!empty($form['form']['user']['#value']->field_user_first_name['und'][0]['safe_value'])) {
      if (!empty($form_state['values']['event']['coordinator']['name'])) {
        $form_state['values']['event']['coordinator']['name'] .= ', ';
      }

      $form_state['values']['event']['coordinator']['name'] .= $form['form']['user']['#value']->field_user_first_name['und'][0]['safe_value'];
    }
  }

  $value = $form_state['values']['event']['coordinator']['name'];

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['coordinator']['name'] = array(
      '#id' => 'field-event-coordinator-name',
      '#type' => 'textfield',
      '#title' => 'Coordinator Name',
      '#default_value' => $value,
      '#required' => TRUE,
      '#size' => 48,
      '#maxlength' => 255,
    );

    $form_state['conditionally_required'][0][] = 'coordinator][name';
  }
  else {
    $fields['coordinator']['name'] = array(
      '#id' => 'field-event-coordinator-name',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_coordinator_phone($form, &$form_state, &$fields, $ps) {
  if (!array_key_exists('phone', $form_state['values']['event']['coordinator'])) {
    $form_state['values']['event']['coordinator']['phone'] = '';

    if (!empty($form['form']['user']['#value']->field_user_phone_number['und'][0]['safe_value'])) {
      $form_state['values']['event']['coordinator']['phone'] = $form['form']['user']['#value']->field_user_phone_number['und'][0]['safe_value'];
    }
  }

  $value = $form_state['values']['event']['coordinator']['phone'];

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['coordinator']['phone'] = array(
      '#id' => 'field-event-coordinator-phone',
      '#type' => 'telfield',
      '#title' => 'Coordinator Phone',
      '#default_value' => $value,
    );
  }
  else {
    $fields['coordinator']['phone'] = array(
      '#id' => 'field-event-coordinator-phone',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_coordinator_email($form, &$form_state, &$fields, $ps) {
  if (!array_key_exists('email', $form_state['values']['event']['coordinator'])) {
    $form_state['values']['event']['coordinator']['email'] = '';

    if (!empty($form['form']['user']['#value']->mail)) {
      $form_state['values']['event']['coordinator']['email'] = $form['form']['user']['#value']->mail;
    }
  }

  $value = $form_state['values']['event']['coordinator']['email'];

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['coordinator']['email'] = array(
      '#id' => 'field-event-coordinator-email',
      '#type' => 'emailfield',
      '#title' => 'Coordinator E-Mail',
      '#default_value' => $value,
      '#size' => 48,
      '#maxlength' => 255,
    );
  }
  else {
    $fields['coordinator']['email'] = array(
      '#id' => 'field-event-coordinator-email',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_plans_note($form, &$form_state, &$fields, $ps) {
  $value = NULL;

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] == 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('plans', $form['form']['sections']['#value'])) {
    $fields['plans']['note'] = array(
      '#id' => 'field-event-plans-note',
      '#type' => 'container',
    );

    $fields['plans']['note']['markup'] = array(
      '#markup' => '',
    );

    $fields['plans']['note']['markup']['#markup'] .= '<div id="markup-event-plans-note" class="markup-event">';
    $fields['plans']['note']['markup']['#markup'] .= '*Please note a specified drawing of setup requests is required to complete the reservation. ';
    $fields['plans']['note']['markup']['#markup'] .= 'The reserving office will provide a venue layout form for setup submission. ';
    $fields['plans']['note']['markup']['#markup'] .= '</div>';
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_plans_description($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('description', $form_state['values']['event']['plans'])) {
    $value = $form_state['values']['event']['plans']['description'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('plans', $form['form']['sections']['#value'])) {
    $fields['plans']['description'] = array(
      '#id' => 'field-event-plans-description',
      '#type' => 'textarea',
      '#title' => 'Event Description/Purpose',
      '#default_value' => $value,
      '#rows' => 2,
    );
  }
  else {
    $fields['plans']['description'] = array(
      '#id' => 'field-event-plans-description',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_plans_activities($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('activities', $form_state['values']['event']['plans'])) {
    $value = $form_state['values']['event']['plans']['activities'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('plans', $form['form']['sections']['#value'])) {
    $fields['plans']['activities'] = array(
      '#id' => 'field-event-plans-activities',
      '#type' => 'textarea',
      '#title' => 'List all activities planned within the event',
      '#default_value' => $value,
      '#rows' => 2,
    );
  }
  else {
    $fields['plans']['activities'] = array(
      '#id' => 'field-event-plans-activities',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_plans_dates($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('dates', $form_state['values']['event']['plans'])) {
    $value = $form_state['values']['event']['plans']['dates'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('plans', $form['form']['sections']['#value'])) {
    $fields['plans']['dates'] = array(
      '#id' => 'field-event-plans-dates',
      '#type' => 'textarea',
      '#title' => 'Please indicate all additional dates and times needed for plans and/or rehearsals',
      '#default_value' => $value,
      '#rows' => 2,
    );
  }
  else {
    $fields['plans']['dates'] = array(
      '#id' => 'field-event-plans-dates',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_plans_audience($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('audience', $form_state['values']['event']['plans'])) {
    $value = $form_state['values']['event']['plans']['audience'];
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('plans', $form['form']['sections']['#value'])) {
    $audience_options = event_workflow_get_audience_list_options();

    $fields['plans']['audience'] = array(
      '#id' => 'field-event-plans-audience',
      '#type' => 'select',
      '#title' => 'Intended Audience',
      '#default_value' => $value,
      '#options' => $audience_options,
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'plans][audience';
  }
  else {
    $fields['plans']['audience'] = array(
      '#id' => 'field-event-plans-audience',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_registration_require($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('require', $form_state['values']['event']['registration'])) {
    $value = $form_state['values']['event']['registration']['require'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['require'] = array(
      '#id' => 'field-event-registration-require',
      '#type' => 'radios',
      '#title' => 'Does the event require registration or ticket purchase?',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'registration][require';
  }
  else {
    $fields['registration']['require'] = array(
      '#id' => 'field-event-registration-require',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_registration_website($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('website', $form_state['values']['event']['registration'])) {
    $value = $form_state['values']['event']['registration']['website'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['website'] = array(
      '#id' => 'field-event-registration-website',
      '#type' => 'textfield',
      '#title' => 'Please indicate a <em>Registration Website</em>',
      '#default_value' => $value,
      '#size' => 48,
      '#maxlength' => 255,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );
  }
  else {
    $fields['registration']['website'] = array(
      '#id' => 'field-event-registration-website',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_registration_phone($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('phone', $form_state['values']['event']['registration'])) {
    $value = $form_state['values']['event']['registration']['phone'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['phone'] = array(
      '#id' => 'field-event-registration-phone',
      '#type' => 'telfield',
      '#title' => 'Please indicate a <em>Registration Phone</em>',
      '#default_value' => $value,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );
  }
  else {
    $fields['registration']['phone'] = array(
      '#id' => 'field-event-registration-phone',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_registration_ticket_price($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('ticket_price', $form_state['values']['event']['registration'])) {
    $value = $form_state['values']['event']['registration']['ticket_price'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['ticket_price'] = array(
      '#id' => 'field-event-registration-ticket_price',
      '#type' => 'textfield',
      '#title' => 'Please indicate a <em>Ticket Price</em>',
      '#default_value' => $value,
      '#size' => 48,
      '#maxlength' => 255,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );
  }
  else {
    $fields['registration']['ticket_price'] = array(
      '#id' => 'field-event-registration-ticket_price',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_registration_ticket_dates($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('ticket_dates', $form_state['values']['event']['registration'])) {
    $value = $form_state['values']['event']['registration']['ticket_dates'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['ticket_dates'] = array(
      '#id' => 'field-event-registration-ticket_dates',
      '#type' => 'textfield',
      '#title' => 'Please indicate <em>Dates</em> the tickets are one sale',
      '#default_value' => $value,
      '#size' => 48,
      '#maxlength' => 255,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );
  }
  else {
    $fields['registration']['ticket_dates'] = array(
      '#id' => 'field-event-registration-ticket_dates',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_registration_ticket_website($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('ticket_website', $form_state['values']['event']['registration'])) {
    $value = $form_state['values']['event']['registration']['ticket_website'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['ticket_website'] = array(
      '#id' => 'field-event-registration-ticket_website',
      '#type' => 'textfield',
      '#title' => 'Please indicate a <em>Ticket Website</em>',
      '#default_value' => $value,
      '#size' => 48,
      '#maxlength' => 255,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );
  }
  else {
    $fields['registration']['ticket_website'] = array(
      '#id' => 'field-event-registration-ticket_website',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_registration_ticket_phone($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('ticket_phone', $form_state['values']['event']['registration'])) {
    $value = $form_state['values']['event']['registration']['ticket_phone'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['ticket_phone'] = array(
      '#id' => 'field-event-registration-ticket_phone',
      '#type' => 'textfield',
      '#title' => 'Please indicate a <em>Ticket Phone</em>',
      '#default_value' => $value,
      '#size' => 48,
      '#maxlength' => 255,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[require]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );
  }
  else {
    $fields['registration']['ticket_phone'] = array(
      '#id' => 'field-event-registration-ticket_phone',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_registration_generate_revenue($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('generate_revenue', $form_state['values']['event']['registration'])) {
    $value = $form_state['values']['event']['registration']['generate_revenue'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $fields['registration']['generate_revenue'] = array(
      '#id' => 'field-event-registration-generate_revenue',
      '#type' => 'radios',
      '#title' => 'Will the Event Generate Revenue?',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'registration][generate_revenue';
  }
  else {
    $fields['registration']['generate_revenue'] = array(
      '#id' => 'field-event-registration-generate_revenue',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_registration_revenue_generated($form, &$form_state, &$fields, $ps) {
  $value = array();
  if (array_key_exists('revenue_generated', $form_state['values']['event']['registration'])) {
    $value = $form_state['values']['event']['registration']['revenue_generated'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('registration', $form['form']['sections']['#value'])) {
    $revenue_generated_options = event_workflow_get_revenue_generated_list_options('checkboxes');

    $fields['registration']['revenue_generated'] = array(
      '#id' => 'field-event-registration-revenue_generated',
      '#type' => 'checkboxes',
      '#title' => 'How Will Revenue be Generated?',
      '#default_value' => $value,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[generate_revenue]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[generate_revenue]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[generate_revenue]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
      '#options' => $revenue_generated_options,
    );
  }
  else {
    $fields['registration']['revenue_generated'] = array(
      '#id' => 'field-event-registration-revenue_generated',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_setup_rectangular_tables_8ft($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('rectangular_tables_8ft', $form_state['values']['event']['setup'])) {
    $value = $form_state['values']['event']['setup']['rectangular_tables_8ft'];
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['rectangular_tables_8ft'] = array(
      '#id' => 'field-event-setup-rectangular_tables_8ft',
      '#type' => 'numberfield',
      '#title' => 'How Many 8 ft Rectangular Tables Will You Need?',
      '#default_value' => $value,
      '#min' => 0,
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'setup][rectangular_tables_8ft';
  }
  else {
    $fields['setup']['rectangular_tables_8ft'] = array(
      '#id' => 'field-event-setup-rectangular_tables_8ft',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_setup_round_tables_8ft($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('round_tables_8ft', $form_state['values']['event']['setup'])) {
    $value = $form_state['values']['event']['setup']['round_tables_8ft'];
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['round_tables_8ft'] = array(
      '#id' => 'field-event-setup-round_tables_8ft',
      '#type' => 'numberfield',
      '#title' => 'How Many 8 ft Round Tables Will You Need?',
      '#default_value' => $value,
      '#min' => 0,
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'setup][round_tables_8ft';
  }
  else {
    $fields['setup']['round_tables_8ft'] = array(
      '#id' => 'field-event-setup-round_tables_8ft',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_setup_other_tables($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('other_tables', $form_state['values']['event']['setup'])) {
    $value = $form_state['values']['event']['setup']['other_tables'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['other_tables'] = array(
      '#id' => 'field-event-setup-other_tables',
      '#type' => 'textarea',
      '#title' => 'How Many Other Tables Will You Need? <span class="title_display-note describe_the_table">(Describe the Table)</span>',
      '#default_value' => $value,
      '#rows' => 2,
    );
  }
  else {
    $fields['setup']['other_tables'] = array(
      '#id' => 'field-event-setup-other_tables',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_setup_standard_blue_chairs($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('standard_blue_chairs', $form_state['values']['event']['setup'])) {
    $value = $form_state['values']['event']['setup']['standard_blue_chairs'];
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['standard_blue_chairs'] = array(
      '#id' => 'field-event-setup-standard_blue_chairs',
      '#type' => 'numberfield',
      '#title' => 'How Many Standard Blue Chairs Will You Need?',
      '#default_value' => $value,
      '#min' => 0,
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'setup][standard_blue_chairs';
  }
  else {
    $fields['setup']['standard_blue_chairs'] = array(
      '#id' => 'field-event-setup-standard_blue_chairs',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_setup_podium($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('podium', $form_state['values']['event']['setup'])) {
    $value = $form_state['values']['event']['setup']['podium'];
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['podium'] = array(
      '#id' => 'field-event-setup-podium',
      '#type' => 'radios',
      '#title' => 'Will You Need a Podium?',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'setup][podium';
  }
  else {
    $fields['setup']['podium'] = array(
      '#id' => 'field-event-setup-podium',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_setup_portable_stage($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('portable_stage', $form_state['values']['event']['setup'])) {
    $value = $form_state['values']['event']['setup']['portable_stage'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['portable_stage'] = array(
      '#id' => 'field-event-setup-portable_stage',
      '#type' => 'radios',
      '#title' => 'Will You Require a Portable Stage?',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'setup][portable_stage';
  }
  else {
    $fields['setup']['portable_stage'] = array(
      '#id' => 'field-event-setup-portable_stage',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_setup_portable_stage_configuration($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('portable_stage_configuration', $form_state['values']['event']['setup'])) {
    $value = $form_state['values']['event']['setup']['portable_stage_configuration'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['portable_stage_configuration'] = array(
      '#id' => 'field-event-setup-portable_stage_configuration',
      '#type' => 'textarea',
      '#title' => 'What size stage and configuration is needed?',
      '#default_value' => $value,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[portable_stage]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[portable_stage]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[portable_stage]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
      '#rows' => 2,
    );

    $form_state['conditionally_required'][1][] = 'setup][portable_stage_configuration';
  }
  else {
    $fields['setup']['portable_stage_configuration'] = array(
      '#id' => 'field-event-setup-portable_stage_configuration',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_setup_security($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('security', $form_state['values']['event']['setup'])) {
    $value = $form_state['values']['event']['setup']['security'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['security'] = array(
      '#id' => 'field-event-setup-security',
      '#type' => 'radios',
      '#title' => 'Will the Event Require Security? <span class="title_display-note additional_fees_may_apply">(Additional Fees May Apply)</span>',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'setup][security';
  }
  else {
    $fields['setup']['security'] = array(
      '#id' => 'field-event-setup-security',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_setup_parking_assistance($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('parking_assistance', $form_state['values']['event']['setup'])) {
    $value = $form_state['values']['event']['setup']['parking_assistance'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['parking_assistance'] = array(
      '#id' => 'field-event-setup-parking_assistance',
      '#type' => 'radios',
      '#title' => 'Will the Event Require Parking Assistance or Restrictions?',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'setup][parking_assistance';
  }
  else {
    $fields['setup']['parking_assistance'] = array(
      '#id' => 'field-event-setup-parking_assistance',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_setup_parking_assistance_area($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('parking_assistance_area', $form_state['values']['event']['setup'])) {
    $value = $form_state['values']['event']['setup']['parking_assistance_area'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['parking_assistance_area'] = array(
      '#id' => 'field-event-setup-parking_assistance_area',
      '#type' => 'textarea',
      '#title' => 'What Parking Area will be Accessed or Restricted?',
      '#default_value' => $value,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[parking_assistance]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[parking_assistance]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[parking_assistance]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
      '#rows' => 2,
    );

    $form_state['conditionally_required'][1][] = 'setup][parking_assistance_area';
  }
  else {
    $fields['setup']['parking_assistance_area'] = array(
      '#id' => 'field-event-setup-parking_assistance_area',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_setup_road_closures($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('road_closures', $form_state['values']['event']['setup'])) {
    $value = $form_state['values']['event']['setup']['road_closures'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['road_closures'] = array(
      '#id' => 'field-event-setup-road_closures',
      '#type' => 'radios',
      '#title' => 'Will the Event Require Road Closures or Barricades?',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'setup][road_closures';
  }
  else {
    $fields['setup']['road_closures'] = array(
      '#id' => 'field-event-setup-road_closures',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_setup_road_closures_details($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('road_closures_details', $form_state['values']['event']['setup'])) {
    $value = $form_state['values']['event']['setup']['road_closures_details'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['road_closures_details'] = array(
      '#id' => 'field-event-setup-road_closures_details',
      '#type' => 'textarea',
      '#title' => 'Provide an Explanation on the Road Closures or Barricades',
      '#default_value' => $value,
      '#rows' => 2,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[road_closures]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[road_closures]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[road_closures]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][1][] = 'setup][road_closures_details';
  }
  else {
    $fields['setup']['road_closures_details'] = array(
      '#id' => 'field-event-setup-road_closures_details',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_setup_special_requests($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('special_requests', $form_state['values']['event']['setup'])) {
    $value = $form_state['values']['event']['setup']['special_requests'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['special_requests'] = array(
      '#id' => 'field-event-setup-special_requests',
      '#type' => 'radios',
      '#title' => 'Will the Event Require any Additional Special Requests for Setup?',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'setup][special_requests';
  }
  else {
    $fields['setup']['special_requests'] = array(
      '#id' => 'field-event-setup-special_requests',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_setup_special_requests_details($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('special_requests_details', $form_state['values']['event']['setup'])) {
    $value = $form_state['values']['event']['setup']['special_requests_details'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('setup', $form['form']['sections']['#value'])) {
    $fields['setup']['special_requests_details'] = array(
      '#id' => 'field-event-setup-special_requests_details',
      '#type' => 'textarea',
      '#title' => 'Explanation on the Special Requests',
      '#default_value' => $value,
      '#rows' => 2,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[special_requests]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[special_requests]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[special_requests]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][1][] = 'setup][special_requests_details';
  }
  else {
    $fields['setup']['special_requests_details'] = array(
      '#id' => 'field-event-setup-special_requests_details',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_technical_equipment($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('technical_equipment', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['technical_equipment'];
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['technical_equipment'] = array(
      '#id' => 'field-event-presentation-technical_equipment',
      '#type' => 'radios',
      '#title' => 'Will the Event Require any Technical Equipment? <span class="title_display-note additional_fees_may_apply">(Additional Fees May Apply)</span>',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'presentation][technical_equipment';
  }
  else {
    $fields['presentation']['technical_equipment'] = array(
      '#id' => 'field-event-presentation-technical_equipment',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_technical_equipment_details($form, &$form_state, &$fields, $ps) {
  $value = array();
  if (array_key_exists('technical_equipment_details', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['technical_equipment_details'];
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $technical_equipment_details_options = event_workflow_get_technical_equipment_details_list_options('checkboxes');

    $fields['presentation']['technical_equipment_details'] = array(
      '#id' => 'field-event-presentation-technical_equipment_details',
      '#type' => 'checkboxes',
      '#title' => 'Specify the Equipment Needed',
      '#default_value' => $value,
      '#options' => $technical_equipment_details_options,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][0][] = 'presentation][technical_equipment_details';
    $form_state['conditionally_required'][1][] = 'presentation][technical_equipment_details';
  }
  else {
    $fields['presentation']['technical_equipment_details'] = array(
      '#id' => 'field-event-presentation-technical_equipment_details',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_technical_equipment_microphone($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('technical_equipment_microphone', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['technical_equipment_microphone'];
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['technical_equipment_microphone'] = array(
      '#id' => 'field-event-presentation-technical_equipment_microphone',
      '#type' => 'numberfield',
      '#title' => 'Specify the Quantity of Microphones',
      '#default_value' => $value,
      '#min' => 0,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment]' . '"]' => array('value' => 'yes'),
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment_details][76]' . '"]' => array('checked' => TRUE),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment_details][76]' . '"]' => array('checked' => FALSE),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment_details][76]' . '"]' => array('value' => NULL),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[technical_equipment]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][1][] = 'presentation][technical_equipment_microphone';
  }
  else {
    $fields['presentation']['technical_equipment_microphone'] = array(
      '#id' => 'field-event-presentation-technical_equipment_microphone',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_external_audio_person($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('external_audio_person', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['external_audio_person'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['external_audio_person'] = array(
      '#id' => 'field-event-presentation-external_audio_person',
      '#type' => 'radios',
      '#title' => 'Will an External Audio/Video Person/Company be Utilized?',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'presentation][external_audio_person';
  }
  else {
    $fields['presentation']['external_audio_person'] = array(
      '#id' => 'field-event-presentation-external_audio_person',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_external_audio_person_name($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('external_audio_person_name', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['external_audio_person_name'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['external_audio_person_name'] = array(
      '#id' => 'field-event-presentation-external_audio_person_name',
      '#type' => 'textfield',
      '#title' => 'Provide the Name of the Person/Company Providing the External Audio/Video',
      '#default_value' => $value,
      '#size' => 48,
      '#maxlength' => 255,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][1][] = 'presentation][external_audio_person_name';
  }
  else {
    $fields['presentation']['external_audio_person_name'] = array(
      '#id' => 'field-event-presentation-external_audio_person_name',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_external_audio_person_email($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('external_audio_person_email', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['external_audio_person_email'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['external_audio_person_email'] = array(
      '#id' => 'field-event-presentation-external_audio_person_email',
      '#type' => 'emailfield',
      '#title' => 'Provide the E-mail of the Person/Company Providing the External Audio/Video',
      '#default_value' => $value,
      '#size' => 48,
      '#maxlength' => 255,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][0][] = 'presentation][external_audio_person_email';
  }
  else {
    $fields['presentation']['external_audio_person_email'] = array(
      '#id' => 'field-event-presentation-external_audio_person_email',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_external_audio_person_phone($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('external_audio_person_phone', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['external_audio_person_phone'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['external_audio_person_phone'] = array(
      '#id' => 'field-event-presentation-external_audio_person_phone',
      '#type' => 'telfield',
      '#title' => 'Provide the Phone Number of the Person/Company Providing the External Audio/Video',
      '#default_value' => $value,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[external_audio_person]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][1][] = 'presentation][external_audio_person_phone';
  }
  else {
    $fields['presentation']['external_audio_person_phone'] = array(
      '#id' => 'field-event-presentation-external_audio_person_phone',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_production($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('production', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['production'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['production'] = array(
      '#id' => 'field-event-presentation-production',
      '#type' => 'radios',
      '#title' => 'Is the Event Using a Production Company or Professional Promoter?',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'presentation][production';
  }
  else {
    $fields['presentation']['production'] = array(
      '#id' => 'field-event-presentation-production',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_production_name($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('production_name', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['production_name'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['production_name'] = array(
      '#id' => 'field-event-presentation-production_name',
      '#type' => 'textfield',
      '#title' => 'Name of the Company/Individual',
      '#default_value' => $value,
      '#size' => 48,
      '#maxlength' => 255,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][0][] = 'presentation][production_name';
  }
  else {
    $fields['presentation']['production_name'] = array(
      '#id' => 'field-event-presentation-production_name',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_production_email($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('production_email', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['production_email'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['production_email'] = array(
      '#id' => 'field-event-presentation-production_email',
      '#type' => 'emailfield',
      '#title' => 'Contact E-mail of the Company/Individual',
      '#default_value' => $value,
      '#size' => 48,
      '#maxlength' => 255,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][0][] = 'presentation][production_email';
  }
  else {
    $fields['presentation']['production_email'] = array(
      '#id' => 'field-event-presentation-production_email',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_production_phone($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('production_phone', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['production_phone'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['production_phone'] = array(
      '#id' => 'field-event-presentation-production_phone',
      '#type' => 'telfield',
      '#title' => 'Phone Number of the Company/Individual',
      '#default_value' => $value,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[production]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][0][] = 'presentation][production_phone';
  }
  else {
    $fields['presentation']['production_phone'] = array(
      '#id' => 'field-event-presentation-production_phone',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_publicity($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('publicity', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['publicity'];
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['publicity'] = array(
      '#id' => 'field-event-presentation-publicity',
      '#type' => 'radios',
      '#title' => 'Will the Event Need Publicity?',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'presentation][publicity';
  }
  else {
    $fields['presentation']['publicity'] = array(
      '#id' => 'field-event-presentation-publicity',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_publicity_details($form, &$form_state, &$fields, $ps) {
  $value = array();
  if (array_key_exists('publicity_details', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['publicity_details'];
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $publicity_details_options = event_workflow_get_publicity_details_list_options('checkboxes');

    $fields['presentation']['publicity_details'] = array(
      '#id' => 'field-event-presentation-publicity_details',
      '#type' => 'checkboxes',
      '#title' => 'Specify all Publicity Medium that Apply',
      '#default_value' => $value,
      '#options' => $publicity_details_options,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[publicity]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[publicity]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[publicity]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][1][] = 'presentation][publicity_details';
  }
  else {
    $fields['presentation']['publicity_details'] = array(
      '#id' => 'field-event-presentation-publicity_details',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_printed_material($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('printed_material', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['printed_material'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['printed_material'] = array(
      '#id' => 'field-event-presentation-printed_material',
      '#type' => 'radios',
      '#title' => 'Will the Event use Printed Material?',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'presentation][printed_material';
  }
  else {
    $fields['presentation']['printed_material'] = array(
      '#id' => 'field-event-presentation-printed_material',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_university_logo($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('university_logo', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['university_logo'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['university_logo'] = array(
      '#id' => 'field-event-presentation-university_logo',
      '#type' => 'radios',
      '#title' => 'Will the Event use the University Logo?',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '"]' => array('value' => NULL),
          ),
        ),
        '#required' => TRUE,
      ),
    );

    $form_state['conditionally_required'][0][] = 'presentation][university_logo';
    $form_state['conditionally_required'][1][] = 'presentation][university_logo';
  }
  else {
    $fields['presentation']['university_logo'] = array(
      '#id' => 'field-event-presentation-university_logo',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_designing_material($form, &$form_state, &$fields, $ps) {
  $value = array();
  if (array_key_exists('designing_material', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['designing_material'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $desiging_material_options = event_workflow_get_designing_material_list_options('checkboxes');

    $fields['presentation']['designing_material'] = array(
      '#id' => 'field-event-presentation-designing_material',
      '#type' => 'checkboxes',
      '#title' => 'Who will be Designing the Material?',
      '#default_value' => $value,
      '#options' => $desiging_material_options,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '"]' => array('value' => 'yes'),
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '"]' => array('value' => NULL),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][1][] = 'presentation][designing_material';
  }
  else {
    $fields['presentation']['designing_material'] = array(
      '#id' => 'field-event-presentation-designing_material',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_designing_material_name($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('designing_material_name', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['designing_material_name'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['designing_material_name'] = array(
      '#id' => 'field-event-presentation-designing_material_name',
      '#type' => 'textfield',
      '#title' => 'Name of who will be Designing the Material',
      '#default_value' => $value,
      '#size' => 48,
      '#maxlength' => 255,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '"]' => array('value' => 'yes'),
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '"]' => array('value' => 'yes'),
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[designing_material][97]' . '"]' => array('checked' => TRUE),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[designing_material][97]' . '"]' => array('checked' => FALSE),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '"]' => array('value' => NULL),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][1][] = 'presentation][designing_material_name';
  }
  else {
    $fields['presentation']['designing_material_name'] = array(
      '#id' => 'field-event-presentation-designing_material_name',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_designing_material_email($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('designing_material_email', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['designing_material_email'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['designing_material_email'] = array(
      '#id' => 'field-event-presentation-designing_material_email',
      '#type' => 'emailfield',
      '#title' => 'E-mail of who will be Designing the Material',
      '#default_value' => $value,
      '#size' => 48,
      '#maxlength' => 255,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '"]' => array('value' => 'yes'),
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '"]' => array('value' => 'yes'),
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[designing_material][97]' . '"]' => array('checked' => TRUE),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[designing_material][97]' . '"]' => array('checked' => FALSE),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '"]' => array('value' => NULL),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][1][] = 'presentation][designing_material_email';
  }
  else {
    $fields['presentation']['designing_material_email'] = array(
      '#id' => 'field-event-presentation-designing_material_email',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_presentation_designing_material_phone($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('designing_material_phone', $form_state['values']['event']['presentation'])) {
    $value = $form_state['values']['event']['presentation']['designing_material_phone'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('presentation', $form['form']['sections']['#value'])) {
    $fields['presentation']['designing_material_phone'] = array(
      '#id' => 'field-event-presentation-designing_material_phone',
      '#type' => 'telfield',
      '#title' => 'Phone Number of who will be Designing the Material',
      '#default_value' => $value,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '"]' => array('value' => 'yes'),
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '"]' => array('value' => 'yes'),
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[designing_material][97]' . '"]' => array('checked' => TRUE),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[designing_material][97]' . '"]' => array('checked' => FALSE),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[printed_material]' . '"]' => array('value' => NULL),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[university_logo]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][1][] = 'presentation][designing_material_phone';
  }
  else {
    $fields['presentation']['designing_material_phone'] = array(
      '#id' => 'field-event-presentation-designing_material_phone',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_services_food_served($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('food_served', $form_state['values']['event']['services'])) {
    $value = $form_state['values']['event']['services']['food_served'];
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('services', $form['form']['sections']['#value'])) {
    $fields['services']['food_served'] = array(
      '#id' => 'field-event-services-food_served',
      '#type' => 'radios',
      '#title' => 'Will the Food or Drink be Served? <span class="title_display-note some_venues_dont_permit">(Note some venues do not permit food and drink)</span>',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'services][food_served';
  }
  else {
    $fields['services']['food_served'] = array(
      '#id' => 'field-event-services-food_served',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}
/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_services_food_caterer($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('food_caterer', $form_state['values']['event']['services'])) {
    $value = $form_state['values']['event']['services']['food_caterer'];
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('services', $form['form']['sections']['#value'])) {
    $fields['services']['food_caterer'] = array(
      '#id' => 'field-event-services-food_caterer',
      '#type' => 'textfield',
      '#title' => 'Provide the Name of the Caterer/Busineess',
      '#default_value' => $value,
      '#size' => 48,
      '#maxlength' => 255,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[food_served]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[food_served]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[food_served]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][1][] = 'services][food_caterer';
  }
  else {
    $fields['services']['food_caterer'] = array(
      '#id' => 'field-event-services-food_caterer',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_services_alcohol($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('alcohol', $form_state['values']['event']['services'])) {
    $value = $form_state['values']['event']['services']['alcohol'];
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('services', $form['form']['sections']['#value'])) {
    $fields['services']['alcohol'] = array(
      '#id' => 'field-event-services-alcohol',
      '#type' => 'radios',
      '#title' => 'Will Alcohol be Served?',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'services][alcohol';
  }
  else {
    $fields['services']['alcohol'] = array(
      '#id' => 'field-event-services-alcohol',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_services_alcohol_note($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('alcohol_note', $form_state['values']['event']['services'])) {
    $value = $form_state['values']['event']['services']['alcohol_note'];
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('services', $form['form']['sections']['#value'])) {
    $fields['services']['alcohol_note'] = array(
      '#id' => 'field-event-services-alcohol_note',
      '#type' => 'container',
      '#states' => array(
        'visible' => array(
          ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[alcohol]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[alcohol]' . '"]' => array('value' => 'no'),
            ':input[name="' . $form['form']['parents']['#value']['path'] . $ps . '[alcohol]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $fields['services']['alcohol_note']['markup'] = array(
      '#markup' => '',
    );

    $fields['services']['alcohol_note']['markup']['#markup'] .= '<div id="markup-event-services-alcohol_note" class="markup-event">';
    $fields['services']['alcohol_note']['markup']['#markup'] .= 'A formal written request must be submitted to the President for approval of any alcohol served on campus and may require police officer presence at your event for an additional fee. ';
    $fields['services']['alcohol_note']['markup']['#markup'] .= 'Contact the President\'s Office at ext. 5556 for more information. ';
    $fields['services']['alcohol_note']['markup']['#markup'] .= '</div>';
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_services_open_flames($form, &$form_state, &$fields, $ps) {
  $value = 'no';
  if (array_key_exists('open_flames', $form_state['values']['event']['services'])) {
    $value = $form_state['values']['event']['services']['open_flames'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] != 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('services', $form['form']['sections']['#value'])) {
    $fields['services']['open_flames'] = array(
      '#id' => 'field-event-open_flames',
      '#type' => 'radios',
      '#title' => 'Will Open Flames be Used? <span class="title_display-note examples">(Examples: Sternos, Grill, Candles)</span>',
      '#default_value' => $value,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'services][open_flames';
  }
  else {
    $fields['services']['open_flames'] = array(
      '#id' => 'field-event-services-open_flames',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_additional($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('additional', $form_state['values']['event'])) {
    $value = $form_state['values']['event']['additional'];
  }

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('top', $form['form']['sections']['#value'])) {
    $fields['additional'] = array(
      '#id' => 'field-event-additional',
      '#type' => 'textarea',
      '#title' => 'Please Provide any Other Information or Questions you may Have for Campus Officials',
      '#default_value' => $value,
      '#rows' => 4,
    );
  }
  else {
    $fields['additional'] = array(
      '#id' => 'field-event-additional',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_coordinator_classification($form, &$form_state, &$fields, $ps) {
  if (!array_key_exists('classification', $form_state['values']['event']['coordinator'])) {
    $classification = event_workflow_determine_coordinator_classification($form_state['values']['event']['coordinator'], $form['form']['user']['#value']);

    $form_state['values']['event']['coordinator']['classification'] = $classification['value'];
  }

  $value = $form_state['values']['event']['coordinator']['classification'];

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $classification = event_workflow_determine_coordinator_classification($form_state['values']['event']['coordinator'], $form['form']['user']['#value']);

    // multiple classifications exists, so prompt user for desired classification.
    if ($classification['count'] > 1) {
      $fields['coordinator']['classification'] = array(
        '#id' => 'field-event-coordinator-classification',
        '#type' => 'select',
        '#title' => t("Classification"),
        '#description' => t("Associate this event request with a specific classification."),
        '#options' => $classification['options'],
        '#default_value' => $value,
        '#required' => TRUE,
      );

      $form_state['conditionally_required'][0][] = 'coordinator][classification';
    }
    else {
      $fields['coordinator']['classification'] = array(
        '#id' => 'field-event-coordinator-classification',
        '#type' => 'select',
        '#title' => 'Coordinator Classification',
        '#options' => $classification['options'],
        '#default_value' => $value,
        '#readonly' => TRUE,
        '#attributes' => array('readonly' => 'readonly'),
        '#required' => TRUE,
      );

      $form_state['conditionally_required'][0][] = 'coordinator][classification';
    }
  }
  else {
    $fields['coordinator']['classification'] = array(
      '#id' => 'field-event-coordinator-classification',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_information_title($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('title', $form_state['values']['event']['information'])) {
    $value = $form_state['values']['event']['information']['title'];
  }

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['information']['title'] = array(
      '#id' => 'field-event-information-title',
      '#type' => 'textfield',
      '#title' => 'Event Title',
      '#default_value' => $value,
      '#required' => TRUE,
      '#size' => 48,
      '#maxlength' => 255,
    );

    $form_state['conditionally_required'][0][] = 'information][title';
  }
  else {
    $fields['information']['title'] = array(
      '#id' => 'field-event-information-title',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_information_attendance($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('attendance', $form_state['values']['event']['information']) && is_numeric($form_state['values']['event']['information']['attendance'])) {
    $value = $form_state['values']['event']['information']['attendance'];
  }

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['information']['attendance'] = array(
      '#id' => 'field-event-information-attendance',
      '#type' => 'numberfield',
      '#title' => 'Expected Number of Attendance',
      '#default_value' => $value,
      '#min' => 0,
    );

    if (!empty($form['form']['room_information']['#value']['capacity'])) {
      $fields['information']['attendance']['#max'] = $form['form']['room_information']['#value']['capacity'];
      $fields['information']['attendance']['#description'] = 'Current room has a max attendance of ' . check_plain($form['form']['room_information']['#value']['capacity']) . '.';
    }
  }
  else {
    $fields['information']['attendance'] = array(
      '#id' => 'field-event-information-attendance',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_information_type($form, &$form_state, &$fields, $ps) {
  $value = NULL;
  if (array_key_exists('type', $form_state['values']['event']['information'])) {
    $value = $form_state['values']['event']['information']['type'];
  }

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $type_options = event_workflow_get_information_type_list_options();

    $fields['information']['type'] = array(
      '#id' => 'field-event-information-type',
      '#type' => 'select',
      '#title' => 'Type of Event',
      '#default_value' => $value,
      '#options' => $type_options,
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'information][type';
  }
  else {
    $fields['information']['type'] = array(
      '#id' => 'field-event-information-type',
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $value;
}

/**
 * Returns an array containing classication information.
 *
 * @param array $state
 *   The form_state array containing the 'classsification' key and value.
 * @param object $user
 *   The loaded user object for the user to load the classification for.
 *
 * @return array
 *   An array with the following keys:
 *   - count: the number of classifications available for the current user.
 *   - options: all available select list options.
 *   - value: the default value to use.
 */
function event_workflow_determine_coordinator_classification($state, $user) {
  $information = array();
  $information['count'] = 0;
  $information['options'] = array();
  $information['value'] = '';

  if (!is_array($state)) {
    cf_error:invalid_array('state');
    return $information;
  }

  if (!is_object($user)) {
    cf_error::invalid_object('user');

    return $information;
  }

  $classifications_tree = taxonomy_get_tree(1, 0, NULL, TRUE);
  foreach ($classifications_tree as &$branch) {

    if (isset($branch->field_role_name['und'][0]['value'])) {
      if (in_array($branch->field_role_name['und'][0]['value'], $user->roles)) {
        $information['options'][$branch->tid] = $branch->name;
      }
    }
  }
  unset($classifications_tree);

  asort($information['options']);

  $information['count'] = count($information['options']);

  if ($information['count'] == 1) {
    $classification_keys = array_keys($information['options']);
    $information['value'] = array_pop($classification_keys);
  }
  else {
    $rebuild = array();
    $rebuild[''] = "- Select -";

    foreach ($information['options'] as $key => $value) {
      $rebuild[$key] = $value;
    }

    $information['options'] = $rebuild;

    if (array_key_exists('classification', $state)) {
      $information['value'] = $state['classification'];
    }
  }

  return $information;
}

/**
 * Returns an array containing event information details.
 *
 * @param array $event
 *   The pre-loaded event values array.
 *
 * @return array|bool
 *   An array containing the event information details.
 *   FALSE is returned on error.
 */
function event_workflow_get_event_existing_values($event) {
  if (!is_array($event)) {
    cf_error::invalid_array('event');
    return FALSE;
  }

  $processed = array();
  $structure = mcneese_event_workflow_secondary_table_structure();
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

          $alternate_array = array();
          foreach ($field_structure['alternate'] as $alternate_name => $alternate_structure) {
            $alternate_array[$alternate_structure['key']] = NULL;

            if (array_key_exists('multiple', $field_structure) && $field_structure['multiple'] != 0) {
              // @todo: impliment the multiple value logic.
            }
            else {
              if (isset($event[$structure_group][$alternate_name][0]) && is_object($event[$structure_group][$alternate_name][0])) {
                if (property_exists($event[$structure_group][$alternate_name][0], 'value')) {
                  $alternate_array[$alternate_structure['key']] = $event[$structure_group][$alternate_name][0]->value;
                }
              }
            }
          }

          // handle 'exception' case.
          foreach ($field_structure['alternate_exception'] as $alternate_name => $alternate_structure) {
            $alternate_array[$alternate_structure['key']] = NULL;

            if (empty($alternate_structure['exception_path'])) {
              continue;
            }

            $alternate_parts = explode('][', $alternate_structure['exception_path']);

            if (count($alternate_parts) != 2 || !isset($structure[$alternate_parts[0]][$alternate_parts[1]])) {
              continue;
            }

            $alternate_exception = &$structure[$alternate_parts[0]][$alternate_parts[1]];

            if (array_key_exists('multiple', $alternate_exception) && $alternate_exception['multiple'] != 0) {
              // @todo: impliment the multiple value logic.
            }
            else {
              if (isset($event[$alternate_parts[0]][$alternate_parts[1]][0]) && is_object($event[$alternate_parts[0]][$alternate_parts[1]][0])) {
                if (property_exists($event[$alternate_parts[0]][$alternate_parts[1]][0], 'value')) {
                  $alternate_array[$alternate_structure['key']] = $event[$alternate_parts[0]][$alternate_parts[1]][0]->value;
                }
              }
            }

            unset($alternate_exception);
            unset($alternate_parts);
          }

          if (!empty($field_structure['alternate_method']['explode'][0]) && !empty($alternate_array)) {
            ksort($alternate_array);
            $current = implode($field_structure['alternate_method']['explode'][0], $alternate_array);
          }
        }
      }
      else {
        if (array_key_exists('multiple', $field_structure) && $field_structure['multiple'] != 0) {
          // @todo: impliment the multiple value logic.
        }
        else {
          if (isset($event[$structure_group][$field_name][0]) && is_object($event[$structure_group][$field_name][0])) {

            if (empty($field_structure['columns'])) {
              if (property_exists($event[$structure_group][$field_name][0], 'value')) {
                $current = $event[$structure_group][$field_name][0]->value;

                if (!empty($field_structure['storage'])) {
                  foreach ($field_structure['storage'] as $storage_method => &$storage_options) {
                    $converted = event_workflow_convert_value_from_database_format($current, $storage_method, $storage_options);
                    if ($converted !== FALSE) {
                      $current = $converted;
                    }
                    break;
                  }
                }
              }
            }
            else {
              $current = array();
              foreach ($field_structure['columns'] as $column_id => $column_name) {
                $current[$column_id] = NULL;

                if (property_exists($event[$structure_group][$field_name][0], $column_name)) {
                  $current[$column_id] = $event[$structure_group][$field_name][0]->$column_name;

                  if (!empty($field_structure['storage'])) {
                    foreach ($field_structure['storage'] as $storage_method => &$storage_options) {
                      $converted = event_workflow_convert_value_from_database_format($current[$column_id], $storage_method, $storage_options);
                      if ($converted !== FALSE) {
                        $current[$column_id] = $converted;
                      }
                      break;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  return $processed;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
