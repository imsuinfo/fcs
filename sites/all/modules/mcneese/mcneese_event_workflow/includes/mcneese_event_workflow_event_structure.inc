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

  $information['fees'] = array(
    'steps' => array(
      'fees',
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

  mcneese_event_workflow_include(4);

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
  if (!isset($form['form']['sections']['#value']) || !is_array($form['form']['sections']['#value'])) {
    $form['form']['sections'] = array(
      '#type' => 'value',
      '#value' => mcneese_event_workflow_get_event_section_information(),
    );
  }


  // initialize conditionally_required 3, which defines range limits for each given field.
  $form_state['conditionally_required'][3] = array();


  // Fieldset: Status
  mcneese_event_workflow_get_event_field_status($form, $form_state, $fields, $event_state, $ps);


  // Fieldset: Step
  mcneese_event_workflow_get_event_field_step($form, $form_state, $fields, $event_state, $ps);


  // Fieldset: Location
  mcneese_event_workflow_get_event_field_location($form, $form_state, $fields, $event_state, $ps);


  // Fieldset: Room
  $room = mcneese_event_workflow_get_event_field_room($form, $form_state, $fields, $event_state, $ps);

  $form['form']['room_information'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );

  if (isset($room[0])) {
    $form['form']['room_information']['#value'] = mcneese_event_workflow_load_rooms(NULL, NULL, $room[0]);
  }


  // Fieldset: Building
  mcneese_event_workflow_get_event_field_building($form, $form_state, $fields, $event_state, $ps);


  // Fieldset: Review
  if ($form['form']['step']['#value'] == 'review' && array_key_exists('review', $form['form']['sections']['#value'])) {
    if (empty($form['form']['event']['#value']['mer']['id'][0]->value) || !mcneese_event_workflow_management_page_event_access('review', $form['form']['event']['#value']['mer']['id'][0]->value)) {
      drupal_access_denied();
      drupal_exit();
      return array();
    }

    $ps = 'review';
    if (!is_null($form['form']['parents']['#value']['path'])) {
      $ps .= ']';
    }

    if (!isset($event_state['review'])) {
      $event_state['review'] = array();
    }

    $fields['review'] = array(
      '#id' => 'section-event-review',
      '#tree' => TRUE,
    );

    mcneese_event_workflow_get_event_field_review_review($form, $form_state, $fields, $event_state, $ps);
  }

  // Fieldset: Fees
  if ($form['form']['step']['#value'] == 'fees' && array_key_exists('fees', $form['form']['sections']['#value'])) {
    if (empty($form['form']['event']['#value']['mer']['id'][0]->value) || !mcneese_event_workflow_management_page_event_access('fees', $form['form']['event']['#value']['mer']['id'][0]->value)) {
      drupal_access_denied();
      drupal_exit();
      return array();
    }

    $ps = 'fees';
    if (!is_null($form['form']['parents']['#value']['path'])) {
      $ps .= ']';
    }

    if (!isset($event_state['fees'])) {
      $event_state['fees'] = array();
    }

    $fields['fees'] = array(
      '#id' => 'section-event-fees',
      '#tree' => TRUE,
    );

    // @todo: implement fees in the same manner as the review.
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

  // date field contains different fields grouped togethor with it, process them simultanseously.
  mcneese_event_workflow_get_event_group_dates_date($form, $form_state, $fields, $event_state, $ps);

  // add/remove date fields
  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('dates', $form['form']['sections']['#value'])) {
    $dates = array(0 => NULL);
    if (!empty($event_state['dates']['date'])) {
      $dates = $event_state['dates']['date'];
    }

    if (count($dates) > 1) {
      foreach ($dates as $date_key => $date_value) {
        $fields['dates']['group_dates'][$date_key]['dates']['remove'] = array(
          '#id' => 'buttons-event-remove-date-' . $date_key,
          '#type' => 'container',
          '#attributes' => array(
            'class' => array(
              'buttons-event-remove-date',
            ),
          ),
        );

        $fields['dates']['group_dates'][$date_key]['dates']['remove']['button-' . $date_key] = array(
          '#id' => 'submit-event-remove-date-' . $date_key,
          '#name' => 'submit-event-remove-date-' . $date_key,
          '#type' => 'submit',
          '#default_value' => t("Remove Date"),
          '#attributes' => array(
            'class' => array(
              'buttons-event-remove-date-button',
            ),
          ),
        );
      }
    }

    $fields['dates']['add'] = array(
      '#id' => 'buttons-event-add-date',
      '#type' => 'container',
      '#attributes' => array(
        'class' => array(
          'buttons-event-add-date',
        ),
      ),
    );

    $fields['dates']['add']['button'] = array(
      '#id' => 'submit-event-add-date',
      '#name' => 'submit-event-add-date',
      '#type' => 'submit',
      '#default_value' => t("Add Date"),
      '#attributes' => array(
        'class' => array(
          'buttons-event-add-date-button',
        ),
      ),
    );
  }


  // Fieldset: Venue Coordinator
  $ps = 'venue_coordinator';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($event_state['venue_coordinator'])) {
    $event_state['venue_coordinator'] = array();
  }

  $fields['venue_coordinator'] = array(
    '#id' => 'section-event-venue_coordinator',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['venue_coordinator'] = array(
      '#id' => 'section-event-venue_coordinator',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Venue Coordinator Settings",
      '#description' => "The venue coordinator is either the room coordinator or if there is no room coordinator, then the venue coordinator.",
    );
  }

  mcneese_event_workflow_get_event_field_venue_coordinator_user_id($form, $form_state, $fields, $event_state, $ps);


  // Fieldset: Event Coordinator
  $ps = 'event_coordinator';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }

  if (!isset($event_state['event_coordinator'])) {
    $event_state['event_coordinator'] = array();
  }

  $fields['event_coordinator'] = array(
    '#id' => 'section-event-event_coordinator',
    '#tree' => TRUE,
  );

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['event_coordinator'] = array(
      '#id' => 'section-event-event_coordinator',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Event Coordinator Settings",
    );
  }

  mcneese_event_workflow_get_event_field_event_coordinator_name($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_event_coordinator_phone($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_event_coordinator_email($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_event_coordinator_classification($form, $form_state, $fields, $event_state, $ps);


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

  $information_title = mcneese_event_workflow_get_event_field_information_title($form, $form_state, $fields, $event_state, $ps);
  $information_attendance = mcneese_event_workflow_get_event_field_information_attendance($form, $form_state, $fields, $event_state, $ps);
  $information_type = mcneese_event_workflow_get_event_field_information_type($form, $form_state, $fields, $event_state, $ps);
  $information_organization = mcneese_event_workflow_get_event_field_information_organization($form, $form_state, $fields, $event_state, $ps);
  $information_adviser_approval = mcneese_event_workflow_get_event_field_information_adviser_approval($form, $form_state, $fields, $event_state, $ps);

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

  mcneese_event_workflow_get_event_field_plans_note($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_plans_description($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_plans_activities($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_plans_audience($form, $form_state, $fields, $event_state, $ps);


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
    if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
      $fields['registration'] = array(
        '#id' => 'section-event-registration',
        '#type' => 'fieldset',
        '#tree' => TRUE,
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#title' => "Event Registration",
      );
    }
  }

  mcneese_event_workflow_get_event_field_registration_require($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_registration_website($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_registration_phone($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_registration_ticket_price($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_registration_ticket_dates($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_registration_ticket_website($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_registration_ticket_phone($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_registration_generate_revenue($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_registration_revenue_generated($form, $form_state, $fields, $event_state, $ps);


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

  mcneese_event_workflow_get_event_field_setup_rectangular_tables_8ft($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_setup_round_tables_8ft($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_setup_other_tables($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_setup_standard_blue_chairs($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_setup_podium($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_setup_portable_stage($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_setup_portable_stage_configuration($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_setup_security($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_setup_parking_assistance($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_setup_parking_assistance_area($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_setup_road_closures($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_setup_road_closures_details($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_setup_special_requests($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_setup_special_requests_details($form, $form_state, $fields, $event_state, $ps);


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
  mcneese_event_workflow_get_event_field_presentation_technical_equipment($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_technical_equipment_details($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_technical_equipment_microphone($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_external_audio_person($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_external_audio_person_name($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_external_audio_person_email($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_external_audio_person_phone($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_production($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_production_name($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_production_email($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_production_phone($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_publicity($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_publicity_details($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_printed_material($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_university_logo($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_designing_material($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_designing_material_name($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_designing_material_email($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_presentation_designing_material_phone($form, $form_state, $fields, $event_state, $ps);


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

  mcneese_event_workflow_get_event_field_services_food_served($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_services_food_caterer($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_services_alcohol($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_services_alcohol_note($form, $form_state, $fields, $event_state, $ps);
  mcneese_event_workflow_get_event_field_services_open_flames($form, $form_state, $fields, $event_state, $ps);


  // Fieldset: Additional
  $ps = 'top';
  if (!is_null($form['form']['parents']['#value']['path'])) {
    $ps .= ']';
  }
  mcneese_event_workflow_get_event_field_additional($form, $form_state, $fields, $event_state, $ps);


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
function mcneese_event_workflow_get_event_field_status($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => MEW_EVENT_STATUS_UNLOCKED);

  if (empty($event_state['status'])) {
    if (isset($form['form']['existing']['#value']['event']['status'])) {
      $form['form']['status']['#value'] = $form['form']['existing']['#value']['event']['status'];
    }
  }
  else {
    $values = $event_state['status'];
  }

  $limits_check = array(
    'title' => "Status",
    'field' => 'status',
    'group' => 'top',
    'path' => 'status',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($form['form']['step']['#value'] == 'status' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['status'] = array(
      '#id' => 'field-event-status',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-status',
        ),
      ),
    );

    $limits_check['visible'] = TRUE;
  }

  foreach ($values as $key => $value) {
    $fields['status'][$key] = array(
      '#id' => 'field-event-status-' . $key,
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_step($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => MEW_REVIEW_STEP_VENUE_AVAILABLE);

  if (empty($event_state['step'])) {
    if (isset($form['form']['existing']['#value']['event']['step'])) {
      $form['form']['step']['#value'] = $form['form']['existing']['#value']['event']['step'];
    }
    else {
      $form['form']['step']['#value'] = array(MEW_REVIEW_STEP_VENUE_AVAILABLE);
    }
  }
  else {
    $values = $event_state['step'];
  }

  $limits_check = array(
    'title' => "Step",
    'field' => 'step',
    'group' => 'top',
    'path' => 'step',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($form['form']['step']['#value'] == 'step' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['step'] = array(
      '#id' => 'field-event-step',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-step',
        ),
      ),
    );

    $limits_check['visible'] = TRUE;
  }

  foreach ($values as $key => $value) {
    $fields['step'][$key] = array(
      '#id' => 'field-event-step-' . $key,
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_location($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['location'])) {
    $values = $event_state['location'];
  }

  $field_title = "Please indicate the requested facility or event location?";
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
      '#id' => 'field-event-location',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-location',
        ),
      ),
    );

    global $base_path;
    $module_path = drupal_get_path('module', 'mcneese_event_workflow');

    drupal_add_js($module_path . '/js/images-locations.js', array('type' => 'file', 'group' => JS_DEFAULT, 'preprocess' => TRUE));

    foreach ($values as $key => $value) {
      $fields['location'][$key] = array(
        '#id' => 'field-event-location-' . $key,
        '#type' => 'radios',
        '#title' => '<span class="strong underline">' . $field_title . '</span>',
        '#options' => event_workflow_get_event_location_list_options('radios'),
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
    }
    krsort($fields['location']);

    $form_state['conditionally_required'][0][] = 'location';

    $fields['location'][0]['location-images'] = array(
      '#tree' => TRUE,
    );

    $fields['location'][0]['location-images'][0] = array();
    $fields['location'][0]['location-images'][1] = array();

    $fields['location'][0]['location-images'][0]['#markup'] = '<img id="field-event-location-0-image-0" src="' . $base_path . $module_path . '/images/example-map.png" alt="Campus Map" class="location-image location-image-visible noscript">';
    $fields['location'][0]['location-images'][1]['#markup'] = '<img id="field-event-location-0-image-1" src="' . $base_path . $module_path . '/images/example-seed.png" alt="Campus Map, SEED Center" class="location-image location-image-invisible noscript">';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['location'][$key] = array(
        '#id' => 'field-event-location-' . $key,
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
function mcneese_event_workflow_get_event_field_building($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['building'])) {
    if (isset($form['form']['room_information']['#value']->building_id)) {
      $values[0] = $form['form']['room_information']['#value']->building_id;
    }
  }

  $limits_check = array(
    'title' => "Building",
    'field' => 'building',
    'group' => 'top',
    'path' => 'building',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;

  if ($form['form']['step']['#value'] == 'building' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['building'] = array(
      '#id' => 'field-event-building',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-building',
        ),
      ),
    );

    $limits_check['visible'] = TRUE;
  }

  // the building is determined by the room, therefore this is more or less a stub/placeholder.
  foreach ($values as $key => $value) {
    $fields['building'][$key] = array(
      '#id' => 'field-event-building-' . $key,
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_room($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['room'])) {
    $values = $event_state['room'];
  }

  $field_title = "Select a Room";
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
      '#id' => 'field-event-room',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-room',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $location = "";
      if (!empty($event_state['location'][$key])) {
        $location = $event_state['location'][$key];
      }

      $options = event_workflow_get_event_room_list_options($location, 'special');
      if (!is_array($options)) {
        $options = array();
        $value = NULL;
      }
      elseif (!array_key_exists($value, $options)) {
        $value = NULL;
      }

      $fields['room'][$key] = array(
        '#id' => 'field-event-room-' . $key,
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
    }
    krsort($fields['room']);

    $form_state['conditionally_required'][0][] = 'room';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['room'][$key] = array(
        '#id' => 'field-event-room-' . $key,
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
function mcneese_event_workflow_get_event_field_review_review($form, &$form_state, &$fields, &$event_state, $ps) {
  if ($form['form']['step']['#value'] != 'standard' || !array_key_exists('review', $form['form']['sections']['#value'])) {
    return FALSE;
  }

  // @todo: implement below to provide a custom field for altering the current review state.
  // @todo: implement special access control to ensure that only users added to the
  /*
  $values = array(0 => array('reviewer_id' => 0, 'decision' => 0, 'message' => NULL));

  if (empty($event_state['review']['review'])) {
    if (isset($form['form']['existing']['#value']['event']['review']['review'])) {
      $form['form']['review']['review']['#value'] = $form['form']['existing']['#value']['event']['review']['review'];
    }
  }
  else {
    $values = $event_state['review']['review'];
  }

  $fields['review'] = array(
    '#id' => 'field-event-review-review',
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array(
        'form-item-wrapper',
      ),
    ),
  );

  foreach ($values as $key => $value) {
    $fields['review']['review'][$key] = array(
      '#id' => 'field-event-review-review-' . $key,
      '#type' => 'value',
      '#value' => $value,
    );
  }

  return $values;
  */

  return FALSE;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_group_dates_date($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['dates']['date'])) {
    $values = $event_state['dates']['date'];
  }

  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('dates', $form['form']['sections']['#value'])) {
    $fields['dates']['group_dates'] = array(
      '#id' => 'group-event-dates-date',
      '#type' => 'container',
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'group-event-item',
          'group-event-item-dates-date',
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

      mcneese_event_workflow_get_event_field_dates_date($form, $form_state, $fields['dates']['group_dates'][$key], $event_state, $new_ps, $key);
      mcneese_event_workflow_get_event_field_dates_time_start($form, $form_state, $fields['dates']['group_dates'][$key], $event_state, $new_ps, $key);
      mcneese_event_workflow_get_event_field_dates_time_stop($form, $form_state, $fields['dates']['group_dates'][$key], $event_state, $new_ps, $key);
      mcneese_event_workflow_get_event_field_dates_category($form, $form_state, $fields['dates']['group_dates'][$key], $event_state, $new_ps, $key);
    }
  }
  else {
    foreach ($values as $key => $value) {
      $new_ps = $ps . '[group_dates][' . $key . ']';

      mcneese_event_workflow_get_event_field_dates_date($form, $form_state, $fields['dates']['group_dates'][$key], $event_state, $new_ps, $key);
      mcneese_event_workflow_get_event_field_dates_time_start($form, $form_state, $fields['dates']['group_dates'][$key], $event_state, $new_ps, $key);
      mcneese_event_workflow_get_event_field_dates_time_stop($form, $form_state, $fields['dates']['group_dates'][$key], $event_state, $new_ps, $key);
      mcneese_event_workflow_get_event_field_dates_category($form, $form_state, $fields['dates']['group_dates'][$key], $event_state, $new_ps, $key);
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_dates_date($form, &$form_state, &$fields, &$event_state, $ps, $delta = NULL) {
  $values = array(0 => NULL);
  if (!empty($event_state['dates']['date'])) {
    $values = $event_state['dates']['date'];
  }

  $delta_id = NULL;
  if (is_numeric($delta)) {
    $values = array($delta => NULL);

    if (!empty($event_state['dates']['date'][$delta])) {
      $values[$delta] = $event_state['dates']['date'][$delta];
    }

    $delta_id = '-delta_' . $delta;
  }

  $field_title = "Date of Event";
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
      '#id' => 'field-event-dates-date' . $delta_id,
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-dates-date',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['dates']['date'][$key] = array(
        '#id' => 'field-event-dates-date-' . $key,
        '#type' => 'date_popup',
        '#title' => $field_title,
        '#default_value' => $value,
        '#date_format' => "Y/m/d",
        '#date_label_position' => 'invisible',
        '#expected_format' => array('date' => "Y/m/d"),
        '#required' => TRUE,
        '#weight' => 0,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#parents' => array('event', 'dates', 'date', $delta),
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
        '#id' => 'field-event-dates-date-' . $key,
        '#type' => 'value',
        '#value' => $value,
        '#parents' => array('event', 'dates', 'date', $delta),
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_dates_time_start($form, &$form_state, &$fields, &$event_state, $ps, $delta = NULL) {
  $values = array(0 => NULL);
  if (!empty($event_state['dates']['time_start'])) {
    $values = $event_state['dates']['time_start'];
  }

  $delta_id = NULL;
  if (is_numeric($delta)) {
    $values = array($delta => NULL);

    if (!empty($event_state['dates']['time_start'][$delta])) {
      $values[$delta] = $event_state['dates']['time_start'][$delta];
    }

    $delta_id = '-delta_' . $delta;
  }

  $field_title = "Event Start Time";
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
      '#id' => 'field-event-dates-time_start' . $delta_id,
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-dates-time_start',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['dates']['time_start'][$key] = array(
        '#id' => 'field-event-dates-time_start-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 8,
        '#maxlength' => 8,
        '#expected_format' => array('date' => "h:ia"),
        '#description' => "E.g, 12:31pm",
        '#weight' => 1,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#parents' => array('event', 'dates', 'time_start', $delta),
      );
    }
    krsort($fields['dates']['time_start']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['dates']['time_start'][$key] = array(
        '#id' => 'field-event-dates-time_start-' . $key,
        '#type' => 'value',
        '#value' => $value,
        '#parents' => array('event', 'dates', 'time_start', $delta),
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_dates_time_stop($form, &$form_state, &$fields, &$event_state, $ps, $delta = NULL) {
  $values = array(0 => NULL);
  if (!empty($event_state['dates']['time_stop'])) {
    $values = $event_state['dates']['time_stop'];
  }

  $delta_id = NULL;
  if (is_numeric($delta)) {
    $values = array($delta => NULL);

    if (!empty($event_state['dates']['time_stop'][$delta])) {
      $values[$delta] = $event_state['dates']['time_stop'][$delta];
    }

    $delta_id = '-delta_' . $delta;
  }

  $field_title = "Event Stop Time";
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
      '#id' => 'field-event-dates-time_stop' . $delta_id,
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-dates-time_stop',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['dates']['time_stop'][$key] = array(
        '#id' => 'field-event-dates-time_stop-' . $key,
        '#type' => 'textfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#size' => 8,
        '#maxlength' => 8,
        '#expected_format' => array('date' => "h:ia"),
        '#description' => "E.g, 12:31pm",
        '#weight' => 2,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#parents' => array('event', 'dates', 'time_stop', $delta),
      );
    }
    krsort($fields['dates']['time_stop']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['dates']['time_stop'][$key] = array(
        '#id' => 'field-event-dates-time_stop-' . $key,
        '#type' => 'value',
        '#value' => $value,
        '#parents' => array('event', 'dates', 'time_stop', $delta),
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_dates_category($form, &$form_state, &$fields, &$event_state, $ps, $delta = NULL) {
  $values = array(0 => NULL);
  if (!empty($event_state['dates']['category'])) {
    $values = $event_state['dates']['category'];
  }

  $delta_id = NULL;
  if (is_numeric($delta)) {
    $values = array($delta => NULL);

    if (!empty($event_state['dates']['category'][$delta])) {
      $values[$delta] = $event_state['dates']['category'][$delta];
    }

    $delta_id = '-delta_' . $delta;
  }

  $field_title = "Category";
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
      '#id' => 'field-event-dates-category' . $delta_id,
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-dates-category',
        ),
      ),
    );

    $dates_category_options = event_workflow_get_dates_category_list_options('radios');

    foreach ($values as $key => $value) {
      $fields['dates']['category'][$key] = array(
        '#id' => 'field-event-dates-category-' . $key,
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
        '#parents' => array('event', 'dates', 'category', $delta),
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
        '#id' => 'field-event-dates-category-' . $key,
        '#type' => 'value',
        '#value' => $value,
        '#parents' => array('event', 'dates', 'category', $delta),
      );
    }
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_venue_coordinator_user_id($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);

  // never assign default here because the default is inforced only after the room is selected, not before.
  if (!empty($event_state['venue_coordinator']['user_id'][0])) {
    if (!empty($event_state['venue_coordinator']['user_id']) && array_key_exists(0, $event_state['venue_coordinator']['user_id'])) {
      $values[0] = $event_state['venue_coordinator']['user_id'][0];
    }
  }

  $fields['venue_coordinator']['user_id'] = array(
    '#tree' => TRUE,
  );

  $fields['venue_coordinator']['user_id'][0] = array(
    '#id' => 'field-event-venue_coordinator-user-0',
    '#type' => 'value',
    '#value' => $values[0],
  );

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_event_coordinator_name($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);

  if (isset($event_state['event_coordinator']) && array_key_exists('name', $event_state['event_coordinator'])) {
    $values = $event_state['event_coordinator']['name'];
  }
  else {
    if (!empty($form['form']['user']['#value']->field_user_last_name['und'][0]['safe_value'])) {
      $values[0] .= $form['form']['user']['#value']->field_user_last_name['und'][0]['safe_value'];
    }

    if (!empty($form['form']['user']['#value']->field_user_first_name['und'][0]['safe_value'])) {
      if (!empty($values[0])) {
        $values[0] .=  ', ';
      }

      $values[0] .= $form['form']['user']['#value']->field_user_first_name['und'][0]['safe_value'];
    }
  }

  $field_title = "Event Coordinator Name";
  $limits_check = array(
    'title' => $field_title,
    'field' => 'name',
    'group' => 'event_coordinator',
    'path' => 'event_coordinator][name',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['event_coordinator']['name'] = array(
      '#id' => 'field-event-event_coordinator-name',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-event_coordinator-name',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['event_coordinator']['name'][$key] = array(
        '#id' => 'field-event-event_coordinator-name-' . $key,
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
        '#autocomplete_path' => 'events/create-0/autocomplete/event_coordinator/name',
      );
    }
    krsort($fields['event_coordinator']['name']);

    $form_state['conditionally_required'][0][] = 'event_coordinator][name';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['event_coordinator']['name'][$key] = array(
        '#id' => 'field-event-event_coordinator-name-' . $key,
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
function mcneese_event_workflow_get_event_field_event_coordinator_phone($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);

  if (isset($event_state['event_coordinator']) && array_key_exists('phone', $event_state['event_coordinator'])) {
    $values = $event_state['event_coordinator']['phone'];
  }
  else {
    if (!empty($form['form']['user']['#value']->field_user_phone_number['und'][0]['safe_value'])) {
      $values[0] = $form['form']['user']['#value']->field_user_phone_number['und'][0]['safe_value'];
    }
  }

  $field_title = "Event Coordinator Phone";
  $limits_check = array(
    'title' => $field_title,
    'field' => 'phone',
    'group' => 'event_coordinator',
    'path' => 'event_coordinator][phone',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['event_coordinator']['phone'] = array(
      '#id' => 'field-event-event_coordinator-phone',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-event_coordinator-phone',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['event_coordinator']['phone'][$key] = array(
        '#id' => 'field-event-event_coordinator-phone-' . $key,
        '#type' => 'telfield',
        '#title' => $field_title,
        '#default_value' => $value,
        '#attributes' => array(
          'class' => array(
            'form-item-delta',
            'form-item-delta-' . $key,
          ),
        ),
        '#autocomplete_path' => 'events/create-0/autocomplete/event_coordinator/phone',
      );
    }
    krsort($fields['event_coordinator']['phone']);

    $values = $event_state['event_coordinator']['phone'];

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['event_coordinator']['phone'][$key] = array(
        '#id' => 'field-event-event_coordinator-phone-' . $key,
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
function mcneese_event_workflow_get_event_field_event_coordinator_email($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);

  if (isset($event_state['event_coordinator']) && array_key_exists('email', $event_state['event_coordinator'])) {
    $values = $event_state['event_coordinator']['email'];
  }
  else {
    if (!empty($form['form']['user']['#value']->mail)) {
      $values[0] = $form['form']['user']['#value']->mail;
    }
  }

  $field_title = "Event Coordinator E-Mail";
  $limits_check = array(
    'title' => $field_title,
    'field' => 'email',
    'group' => 'event_coordinator',
    'path' => 'event_coordinator][email',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['event_coordinator']['email'] = array(
      '#id' => 'field-event-event_coordinator-email',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-event_coordinator-email',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['event_coordinator']['email'][$key] = array(
        '#id' => 'field-event-event_coordinator-email-' . $key,
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
        '#autocomplete_path' => 'events/create-0/autocomplete/event_coordinator/email',
      );
    }
    krsort($fields['event_coordinator']['email']);

    $form_state['conditionally_required'][0][] = 'event_coordinator][email';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['event_coordinator']['email'][$key] = array(
        '#id' => 'field-event-event_coordinator-email-' . $key,
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
function mcneese_event_workflow_get_event_field_event_coordinator_classification($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);

  $user = NULL;
  if (isset($form['form']['user']['#value']) && is_object($form['form']['user']['#value'])) {
    $user = $form['form']['user']['#value'];
  }

  $state = array();
  if (isset($event_state['event_coordinator']) && is_array($event_state['event_coordinator'])) {
    $state = $event_state['event_coordinator'];
  }

  $classification = event_workflow_determine_user_classification($state, $user);

  if (isset($event_state['event_coordinator']) && array_key_exists('classification', $event_state['event_coordinator'])) {
    $values = $event_state['event_coordinator']['classification'];
  }
  else {
    $values = array(0 => $classification['value']);
  }

  $field_title = "Event Coordinator Classification";
  $limits_check = array(
    'title' => $field_title,
    'field' => 'classification',
    'group' => 'event_coordinator',
    'path' => 'event_coordinator][classification',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('coordinator', $form['form']['sections']['#value'])) {
    $fields['event_coordinator']['classification'] = array(
      '#id' => 'field-event-event_coordinator-classification',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-event_coordinator-classification',
        ),
      ),
    );

    // multiple classifications exists, so prompt user for desired classification.
    if ($classification['count'] > 1) {
      foreach ($values as $key => $value) {
        $fields['event_coordinator']['classification'][$key] = array(
          '#id' => 'field-event-event_coordinator-classification-' . $key,
          '#type' => 'select',
          '#title' => $field_title,
          '#description' => t("Associate this event request with a specific classification."),
          '#options' => $classification['options'],
          '#default_value' => $value,
          '#required' => TRUE,
          '#attributes' => array(
            'class' => array(
              'form-item-delta',
              'form-item-delta-' . $key,
            ),
          ),
        );
      }
      krsort($fields['event_coordinator']['classification']);

      $form_state['conditionally_required'][0][] = 'event_coordinator][classification';

      $limits_check['visible'] = TRUE;
    }
    else {
      foreach ($values as $key => $value) {
        $fields['event_coordinator']['classification'][$key] = array(
          '#id' => 'field-event-event_coordinator-classification-' . $key,
          '#type' => 'select',
          '#title' => 'Event Coordinator Classification',
          '#options' => $classification['options'],
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
          ),
        );
      }

      $form_state['conditionally_required'][0][] = 'event_coordinator][classification';
    }
  }
  else {
    foreach ($values as $key => $value) {
      $fields['event_coordinator']['classification'][$key] = array(
        '#id' => 'field-event-event_coordinator-classification-' . $key,
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
function mcneese_event_workflow_get_event_field_information_title($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['information']['title'])) {
    $values = $event_state['information']['title'];
  }

  $field_title = "Event Title";
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
      '#id' => 'field-event-information-title',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-information-title',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['information']['title'][$key] = array(
        '#id' => 'field-event-information-title-' . $key,
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
      );
    }
    krsort($fields['information']['title']);

    $form_state['conditionally_required'][0][] = 'information][title';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['information']['title'][$key] = array(
        '#id' => 'field-event-information-title-' . $key,
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
function mcneese_event_workflow_get_event_field_information_attendance($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['information']['attendance'])) {
    $values = $event_state['information']['attendance'];
  }

  $field_title = "Expected Number of Attendance";
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
      '#id' => 'field-event-information-attendance',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-information-attendance',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['information']['attendance'][$key] = array(
        '#id' => 'field-event-information-attendance-' . $key,
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
        '#id' => 'field-event-information-attendance-' . $key,
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
function mcneese_event_workflow_get_event_field_information_type($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['information']['type'])) {
    $values = $event_state['information']['type'];
  }

  $field_title = "Type of Event";
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
      '#id' => 'field-event-information-type',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-information-type',
        ),
      ),
    );

    $type_options = event_workflow_get_information_type_list_options('select');

    foreach ($values as $key => $value) {
      $fields['information']['type'][$key] = array(
        '#id' => 'field-event-information-type-' . $key,
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
    }
    krsort($fields['information']['type']);

    $form_state['conditionally_required'][0][] = 'information][type';

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['information']['type'][$key] = array(
        '#id' => 'field-event-information-type-' . $key,
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
function mcneese_event_workflow_get_event_field_information_organization($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['information']['organization'])) {
    $values = $event_state['information']['organization'];
  }

  $field_title = "Organization / Affiliation";
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
      '#id' => 'field-event-information-organization',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-information-organization',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['information']['organization'][$key] = array(
        '#id' => 'field-event-information-organization-' . $key,
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
        '#id' => 'field-event-information-organization-' . $key,
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
function mcneese_event_workflow_get_event_field_information_adviser_approval($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['information']['adviser_approval'])) {
    $values = $event_state['information']['adviser_approval'];
  }

  $field_title = "Has your adviser been notified and has approved this event?";
  $limits_check = array(
    'title' => $field_title,
    'field' => 'adviser_approval',
    'group' => 'information',
    'path' => 'information][adviser_approval',
    'visible' => FALSE,
    'ps' => $ps,
  );

  $form_state['conditionally_required'][3][] = &$limits_check;


  if ($form['form']['step']['#value'] == 'standard' && array_key_exists('information', $form['form']['sections']['#value'])) {
    $fields['information']['adviser_approval'] = array(
      '#id' => 'field-event-information-adviser_approval',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-information-adviser_approval',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['information']['adviser_approval'][$key] = array(
        '#id' => 'field-event-information-adviser_approval-' . $key,
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
            ':input[name="' . $form['form']['parents']['#value']['path'] . 'event_coordinator][classification]' . '[' . $key . ']'  . '"]' => array('value' => 1),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $form['form']['parents']['#value']['path'] . 'event_coordinator][classification]' . '[' . $key . ']'  . '"]' => array('value' => NULL),
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
        '#id' => 'field-event-information-adviser_approval-' . $key,
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
function mcneese_event_workflow_get_event_field_plans_note($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);

  $build = FALSE;
  if (isset($form['form']['information_type']['#value']) && $form['form']['information_type']['#value'] == 57) {
    $build = TRUE;
  }

  if ($build && $form['form']['step']['#value'] == 'details' && array_key_exists('plans', $form['form']['sections']['#value'])) {
    $fields['plans']['note'] = array(
      '#id' => 'field-event-plans-note',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-plans-note',
        ),
      ),
    );

    $fields['plans']['note'][0] = array(
      '#id' => 'field-event-plans-note-0',
      '#type' => 'container',
      '#attributes' => array(
        'class' => array(
          'form-item-delta',
          'form-item-delta-' . $key,
        ),
      ),
    );

    $fields['plans']['note'][0]['markup'] = array(
      '#markup' => '',
    );

    $fields['plans']['note'][0]['markup']['#markup'] .= '<div id="markup-event-plans-note" class="markup-event">';
    $fields['plans']['note'][0]['markup']['#markup'] .= '*Please note a specified drawing of setup requests is required to complete the reservation. ';
    $fields['plans']['note'][0]['markup']['#markup'] .= 'The reserving office will provide a venue layout form for setup submission. ';
    $fields['plans']['note'][0]['markup']['#markup'] .= '</div>';
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_plans_description($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['plans']['description'])) {
    $values = $event_state['plans']['description'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Event Description/Purpose";
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
      '#id' => 'field-event-plans-description',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-plans-description',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['plans']['description'][$key] = array(
        '#id' => 'field-event-plans-description-' . $key,
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
        '#id' => 'field-event-plans-description-' . $key,
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
function mcneese_event_workflow_get_event_field_plans_activities($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['plans']['activities'])) {
    $values = $event_state['plans']['activities'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "List all activities planned within the event";
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
      '#id' => 'field-event-plans-activities',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-plans-activities',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['plans']['activities'][$key] = array(
        '#id' => 'field-event-plans-activities-' . $key,
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
        '#id' => 'field-event-plans-activities-' . $key,
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
function mcneese_event_workflow_get_event_field_plans_audience($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['plans']['audience'])) {
    $values = $event_state['plans']['audience'];
  }

  $field_title = "Intended Audience";
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
      '#id' => 'field-event-plans-audience',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-plans-audience',
        ),
      ),
    );

    $audience_options = event_workflow_get_audience_list_options('select');

    foreach ($values as $key => $value) {
      $fields['plans']['audience'][$key] = array(
        '#id' => 'field-event-plans-audience-' . $key,
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
        '#id' => 'field-event-plans-audience-' . $key,
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
function mcneese_event_workflow_get_event_field_registration_require($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['registration']['require'])) {
    $values = $event_state['registration']['require'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Does the event require registration or ticket purchase?";
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
      '#id' => 'field-event-registration-require',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-registration-require',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['require'][$key] = array(
        '#id' => 'field-event-registration-require-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-registration-require-' . $key,
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
function mcneese_event_workflow_get_event_field_registration_website($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['registration']['website'])) {
    $values = $event_state['registration']['website'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Please indicate a <em>Registration Website</em>";
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
      '#id' => 'field-event-registration-website',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-registration-website',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['website'][$key] = array(
        '#id' => 'field-event-registration-website-' . $key,
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
          '#attributes' => array(
            'class' => array(
              'form-item-delta',
              'form-item-delta-' . $key,
            ),
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
        '#id' => 'field-event-registration-website-' . $key,
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
function mcneese_event_workflow_get_event_field_registration_phone($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['registration']['phone'])) {
    $values = $event_state['registration']['phone'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Please indicate a <em>Registration Phone</em>";
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
      '#id' => 'field-event-registration-phone',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-registration-phone',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['phone'][$key] = array(
        '#id' => 'field-event-registration-phone-' . $key,
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
        '#id' => 'field-event-registration-phone-' . $key,
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
function mcneese_event_workflow_get_event_field_registration_ticket_price($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['registration']['ticket_price'])) {
    $values = $event_state['registration']['ticket_price'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Please indicate a <em>Ticket Price</em>";
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
      '#id' => 'field-event-registration-ticket_price',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-registration-ticket_price',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['ticket_price'][$key] = array(
        '#id' => 'field-event-registration-ticket_price-' . $key,
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
        '#id' => 'field-event-registration-ticket_price-' . $key,
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
function mcneese_event_workflow_get_event_field_registration_ticket_dates($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['registration']['ticket_dates'])) {
    $values = $event_state['registration']['ticket_dates'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Please indicate <em>Dates</em> the tickets are one sale";
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
      '#id' => 'field-event-registration-ticket_dates',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-registration-ticket_dates',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['ticket_dates'][$key] = array(
        '#id' => 'field-event-registration-ticket_dates-' . $key,
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
        '#id' => 'field-event-registration-ticket_dates-' . $key,
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
function mcneese_event_workflow_get_event_field_registration_ticket_website($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['registration']['ticket_website'])) {
    $values = $event_state['registration']['ticket_website'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Please indicate a <em>Ticket Website</em>";
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
      '#id' => 'field-event-registration-ticket_website',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-registration-ticket_website',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['ticket_website'][$key] = array(
        '#id' => 'field-event-registration-ticket_website-' . $key,
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
        '#id' => 'field-event-registration-ticket_website-' . $key,
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
function mcneese_event_workflow_get_event_field_registration_ticket_phone($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['registration']['ticket_phone'])) {
    $values = $event_state['registration']['ticket_phone'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Please indicate a <em>Ticket Phone</em>";
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
      '#id' => 'field-event-registration-ticket_phone',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-registration-ticket_phone',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['ticket_phone'][$key] = array(
        '#id' => 'field-event-registration-ticket_phone-' . $key,
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
        '#id' => 'field-event-registration-ticket_phone-' . $key,
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
function mcneese_event_workflow_get_event_field_registration_generate_revenue($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['registration']['generate_revenue'])) {
    $values = $event_state['registration']['generate_revenue'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Will the Event Generate Revenue?";
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
      '#id' => 'field-event-registration-generate_revenue',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-registration-generate_revenue',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['registration']['generate_revenue'][$key] = array(
        '#id' => 'field-event-registration-generate_revenue-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-registration-generate_revenue-' . $key,
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
function mcneese_event_workflow_get_event_field_registration_revenue_generated($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => array());
  if (!empty($event_state['registration']['revenue_generated'])) {
    $values = $event_state['registration']['revenue_generated'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "How Will Revenue be Generated?";
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
      '#id' => 'field-event-registration-revenue_generated',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-registration-revenue_generated',
        ),
      ),
    );

    $revenue_generated_options = event_workflow_get_revenue_generated_list_options('checkboxes');

    foreach ($values as $key => $value) {
      $fields['registration']['revenue_generated'][$key] = array(
        '#id' => 'field-event-registration-revenue_generated-' . $key,
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
        '#id' => 'field-event-registration-revenue_generated-' . $key,
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
function mcneese_event_workflow_get_event_field_setup_rectangular_tables_8ft($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['setup']['rectangular_tables_8ft'])) {
    $values = $event_state['setup']['rectangular_tables_8ft'];
  }

  $field_title = "How Many 8 ft Rectangular Tables Will You Need?";
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
      '#id' => 'field-event-setup-rectangular_tables_8ft',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-setup-rectangular_tables_8ft',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['rectangular_tables_8ft'][$key] = array(
        '#id' => 'field-event-setup-rectangular_tables_8ft-' . $key,
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
        '#id' => 'field-event-setup-rectangular_tables_8ft-' . $key,
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
function mcneese_event_workflow_get_event_field_setup_round_tables_8ft($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['setup']['round_tables_8ft'])) {
    $values = $event_state['setup']['round_tables_8ft'];
  }

  $field_title = "How Many 8 ft Round Tables Will You Need?";
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
      '#id' => 'field-event-setup-round_tables_8ft',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-setup-round_tables_8ft',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['round_tables_8ft'][$key] = array(
        '#id' => 'field-event-setup-round_tables_8ft-' . $key,
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
        '#id' => 'field-event-setup-round_tables_8ft-' . $key,
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
function mcneese_event_workflow_get_event_field_setup_other_tables($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['setup']['other_tables'])) {
    $values = $event_state['setup']['other_tables'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "How Many Other Tables Will You Need? " . '<span class="title_display-note describe_the_table">' . "(Describe the Table)</span>";
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
      '#id' => 'field-event-setup-other_tables',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-setup-other_tables',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['other_tables'][$key] = array(
        '#id' => 'field-event-setup-other_tables-' . $key,
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
        '#id' => 'field-event-setup-other_tables-' . $key,
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
function mcneese_event_workflow_get_event_field_setup_standard_blue_chairs($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['setup']['standard_blue_chairs'])) {
    $values = $event_state['setup']['standard_blue_chairs'];
  }

  $field_title = "How Many Standard Blue Chairs Will You Need?";
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
      '#id' => 'field-event-setup-standard_blue_chairs',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-setup-standard_blue_chairs',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['standard_blue_chairs'][$key] = array(
        '#id' => 'field-event-setup-standard_blue_chairs-' . $key,
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
        '#id' => 'field-event-setup-standard_blue_chairs-' . $key,
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
function mcneese_event_workflow_get_event_field_setup_podium($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['setup']['podium'])) {
    $values = $event_state['setup']['podium'];
  }

  $field_title = "Will You Need a Podium?";
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
      '#id' => 'field-event-setup-podium',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-setup-podium',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['podium'][$key] = array(
        '#id' => 'field-event-setup-podium-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-setup-podium-' . $key,
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
function mcneese_event_workflow_get_event_field_setup_portable_stage($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['setup']['portable_stage'])) {
    $values = $event_state['setup']['portable_stage'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Will You Require a Portable Stage?";
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
      '#id' => 'field-event-setup-portable_stage',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-setup-portable_stage',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['portable_stage'][$key] = array(
        '#id' => 'field-event-setup-portable_stage-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-setup-portable_stage-' . $key,
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
function mcneese_event_workflow_get_event_field_setup_portable_stage_configuration($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['setup']['portable_stage_configuration'])) {
    $values = $event_state['setup']['portable_stage_configuration'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "What size stage and configuration is needed?";
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
      '#id' => 'field-event-setup-portable_stage_configuration',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-setup-portable_stage_configuration',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['portable_stage_configuration'][$key] = array(
        '#id' => 'field-event-setup-portable_stage_configuration-' . $key,
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
        '#id' => 'field-event-setup-portable_stage_configuration-' . $key,
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
function mcneese_event_workflow_get_event_field_setup_security($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['setup']['security'])) {
    $values = $event_state['setup']['security'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Will the Event Require Security? " . '<span class="title_display-note additional_fees_may_apply">' . "(Additional Fees May Apply)</span>";
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
      '#id' => 'field-event-setup-security',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-setup-security',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['security'][$key] = array(
        '#id' => 'field-event-setup-security-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-setup-security-' . $key,
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
function mcneese_event_workflow_get_event_field_setup_parking_assistance($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['setup']['parking_assistance'])) {
    $values = $event_state['setup']['parking_assistance'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Will the Event Require Parking Assistance or Restrictions?";
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
      '#id' => 'field-event-setup-parking_assistance',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-setup-parking_assistance',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['parking_assistance'][$key] = array(
        '#id' => 'field-event-setup-parking_assistance-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-setup-parking_assistance-' . $key,
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
function mcneese_event_workflow_get_event_field_setup_parking_assistance_area($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['setup']['parking_assistance_area'])) {
    $values = $event_state['setup']['parking_assistance_area'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "What Parking Area will be Accessed or Restricted?";
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
      '#id' => 'field-event-setup-parking_assistance_area',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-setup-parking_assistance_area',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['parking_assistance_area'][$key] = array(
        '#id' => 'field-event-setup-parking_assistance_area-' . $key,
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
        '#id' => 'field-event-setup-parking_assistance_area-' . $key,
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
function mcneese_event_workflow_get_event_field_setup_road_closures($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['setup']['road_closures'])) {
    $values = $event_state['setup']['road_closures'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Will the Event Require Road Closures or Barricades?";
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
      '#id' => 'field-event-setup-road_closures',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-setup-road_closures',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['road_closures'][$key] = array(
        '#id' => 'field-event-setup-road_closures-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-setup-road_closures-' . $key,
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
function mcneese_event_workflow_get_event_field_setup_road_closures_details($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['setup']['road_closures_details'])) {
    $values = $event_state['setup']['road_closures_details'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Provide an Explanation on the Road Closures or Barricades";
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
      '#id' => 'field-event-setup-road_closures_details',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-setup-road_closures_details',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['road_closures_details'][$key] = array(
        '#id' => 'field-event-setup-road_closures_details-' . $key,
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
        '#id' => 'field-event-setup-road_closures_details-' . $key,
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
function mcneese_event_workflow_get_event_field_setup_special_requests($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['setup']['special_requests'])) {
    $values = $event_state['setup']['special_requests'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Will the Event Require any Additional Special Requests for Setup?";
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
      '#id' => 'field-event-setup-special_requests',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-setup-special_requests',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['special_requests'][$key] = array(
        '#id' => 'field-event-setup-special_requests-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-setup-special_requests-' . $key,
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
function mcneese_event_workflow_get_event_field_setup_special_requests_details($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['setup']['special_requests_details'])) {
    $values = $event_state['setup']['special_requests_details'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Explanation on the Special Requests";
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
      '#id' => 'field-event-setup-special_requests_details',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-setup-special_requests_details',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['setup']['special_requests_details'][$key] = array(
        '#id' => 'field-event-setup-special_requests_details-' . $key,
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
        '#id' => 'field-event-setup-special_requests_details-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_technical_equipment($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['presentation']['technical_equipment'])) {
    $values = $event_state['presentation']['technical_equipment'];
  }

  $field_title = "Will the Event Require any Technical Equipment? " . '<span class="title_display-note additional_fees_may_apply">' . "(Additional Fees May Apply)</span>";
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
      '#id' => 'field-event-presentation-technical_equipment',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-technical_equipment',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['technical_equipment'][$key] = array(
        '#id' => 'field-event-presentation-technical_equipment-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-presentation-technical_equipment-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_technical_equipment_details($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => array());
  if (!empty($event_state['presentation']['technical_equipment_details'])) {
    $values = $event_state['presentation']['technical_equipment_details'];
  }

  $field_title = "Specify the Equipment Needed";
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
      '#id' => 'field-event-presentation-technical_equipment_details',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-technical_equipment_details',
        ),
      ),
    );

    $technical_equipment_details_options = event_workflow_get_technical_equipment_details_list_options('checkboxes');

    foreach ($values as $key => $value) {
      $fields['presentation']['technical_equipment_details'][$key] = array(
        '#id' => 'field-event-presentation-technical_equipment_details-' . $key,
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
        '#id' => 'field-event-presentation-technical_equipment_details-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_technical_equipment_microphone($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['presentation']['technical_equipment_microphone'])) {
    $values = $event_state['presentation']['technical_equipment_microphone'];
  }

  $field_title = "Specify the Quantity of Microphones";
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
      '#id' => 'field-event-presentation-technical_equipment_microphone',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-technical_equipment_microphone',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['technical_equipment_microphone'][$key] = array(
        '#id' => 'field-event-presentation-technical_equipment_microphone-' . $key,
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
        '#id' => 'field-event-presentation-technical_equipment_microphone-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_external_audio_person($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['presentation']['external_audio_person'])) {
    $values = $event_state['presentation']['external_audio_person'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Will an External Audio/Video Person/Company be Utilized?";
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
      '#id' => 'field-event-presentation-external_audio_person',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-external_audio_person',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['external_audio_person'][$key] = array(
        '#id' => 'field-event-presentation-external_audio_person-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-presentation-external_audio_person-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_external_audio_person_name($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['presentation']['external_audio_person_name'])) {
    $values = $event_state['presentation']['external_audio_person_name'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Provide the Name of the Person/Company Providing the External Audio/Video";
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
      '#id' => 'field-event-presentation-external_audio_person_name',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-external_audio_person_name',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['external_audio_person_name'][$key] = array(
        '#id' => 'field-event-presentation-external_audio_person_name-' . $key,
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
        '#id' => 'field-event-presentation-external_audio_person_name-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_external_audio_person_email($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['presentation']['external_audio_person_email'])) {
    $values = $event_state['presentation']['external_audio_person_email'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Provide the E-mail of the Person/Company Providing the External Audio/Video";
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
      '#id' => 'field-event-presentation-external_audio_person_email',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-external_audio_person_email',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['external_audio_person_email'][$key] = array(
        '#id' => 'field-event-presentation-external_audio_person_email-' . $key,
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
        '#id' => 'field-event-presentation-external_audio_person_email-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_external_audio_person_phone($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['presentation']['external_audio_person_phone'])) {
    $values = $event_state['presentation']['external_audio_person_phone'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Provide the Phone Number of the Person/Company Providing the External Audio/Video";
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
      '#id' => 'field-event-presentation-external_audio_person_phone',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-external_audio_person_phone',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['external_audio_person_phone'][$key] = array(
        '#id' => 'field-event-presentation-external_audio_person_phone-' . $key,
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
        '#id' => 'field-event-presentation-external_audio_person_phone-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_production($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['presentation']['production'])) {
    $values = $event_state['presentation']['production'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Is the Event Using a Production Company or Professional Promoter?";
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
      '#id' => 'field-event-presentation-production',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-production',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['production'][$key] = array(
        '#id' => 'field-event-presentation-production-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-presentation-production-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_production_name($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['presentation']['production_name'])) {
    $values = $event_state['presentation']['production_name'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Name of the Company/Individual";
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
      '#id' => 'field-event-presentation-production_name',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-production_name',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['production_name'][$key] = array(
        '#id' => 'field-event-presentation-production_name-' . $key,
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
        '#id' => 'field-event-presentation-production_name-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_production_email($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['presentation']['production_email'])) {
    $values = $event_state['presentation']['production_email'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Contact E-mail of the Company/Individual";
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
      '#id' => 'field-event-presentation-production_email',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-production_email',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['production_email'][$key] = array(
        '#id' => 'field-event-presentation-production_email-' . $key,
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
        '#id' => 'field-event-presentation-production_email-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_production_phone($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['presentation']['production_phone'])) {
    $values = $event_state['presentation']['production_phone'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Phone Number of the Company/Individual";
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
      '#id' => 'field-event-presentation-production_phone',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-production_phone',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['production_phone'][$key] = array(
        '#id' => 'field-event-presentation-production_phone-' . $key,
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
        '#id' => 'field-event-presentation-production_phone-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_publicity($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['presentation']['publicity'])) {
    $values = $event_state['presentation']['publicity'];
  }

  $field_title = "Will the Event Need Publicity?";
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
      '#id' => 'field-event-presentation-publicity',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-publicity',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['publicity'][$key] = array(
        '#id' => 'field-event-presentation-publicity-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-presentation-publicity-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_publicity_details($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => array());
  if (!empty($event_state['presentation']['publicity_details'])) {
    $values = $event_state['presentation']['publicity_details'];
  }

  $field_title = "Specify all Publicity Medium that Apply";
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
      '#id' => 'field-event-presentation-publicity_details',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-publicity_details',
        ),
      ),
    );

    $publicity_details_options = event_workflow_get_publicity_details_list_options('checkboxes');

    foreach ($values as $key => $value) {
      $fields['presentation']['publicity_details'][$key] = array(
        '#id' => 'field-event-presentation-publicity_details-' . $key,
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
        '#id' => 'field-event-presentation-publicity_details-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_printed_material($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['presentation']['printed_material'])) {
    $values = $event_state['presentation']['printed_material'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Will the Event use Printed Material?";
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
      '#id' => 'field-event-presentation-printed_material',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-printed_material',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['printed_material'][$key] = array(
        '#id' => 'field-event-presentation-printed_material-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-presentation-printed_material-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_university_logo($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['presentation']['university_logo'])) {
    $values = $event_state['presentation']['university_logo'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Will the Event use the University Logo?";
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
      '#id' => 'field-event-presentation-university_logo',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-university_logo',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['university_logo'][$key] = array(
        '#id' => 'field-event-presentation-university_logo-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-presentation-university_logo-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_designing_material($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => array());
  if (!empty($event_state['presentation']['designing_material'])) {
    $values = $event_state['presentation']['designing_material'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Who will be Designing the Material?";
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
      '#id' => 'field-event-presentation-designing_material',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-designing_material',
        ),
      ),
    );

    $desiging_material_options = event_workflow_get_designing_material_list_options('checkboxes');

    foreach ($values as $key => $value) {
      $fields['presentation']['designing_material'][$key] = array(
        '#id' => 'field-event-presentation-designing_material-' . $key,
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
        '#id' => 'field-event-presentation-designing_material-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_designing_material_name($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['presentation']['designing_material_name'])) {
    $values = $event_state['presentation']['designing_material_name'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Name of who will be Designing the Material";
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
      '#id' => 'field-event-presentation-designing_material_name',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-designing_material_name',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['designing_material_name'][$key] = array(
        '#id' => 'field-event-presentation-designing_material_name-' . $key,
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
        '#id' => 'field-event-presentation-designing_material_name-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_designing_material_email($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['presentation']['designing_material_email'])) {
    $values = $event_state['presentation']['designing_material_email'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "E-mail of who will be Designing the Material";
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
      '#id' => 'field-event-presentation-designing_material_email',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-designing_material_email',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['designing_material_email'][$key] = array(
        '#id' => 'field-event-presentation-designing_material_email-' . $key,
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
        '#id' => 'field-event-presentation-designing_material_email-' . $key,
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
function mcneese_event_workflow_get_event_field_presentation_designing_material_phone($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['presentation']['designing_material_phone'])) {
    $values = $event_state['presentation']['designing_material_phone'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Phone Number of who will be Designing the Material";
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
      '#id' => 'field-event-presentation-designing_material_phone',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-presentation-designing_material_phone',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['presentation']['designing_material_phone'][$key] = array(
        '#id' => 'field-event-presentation-designing_material_phone-' . $key,
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
        '#id' => 'field-event-presentation-designing_material_phone-' . $key,
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
function mcneese_event_workflow_get_event_field_services_food_served($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['services']['food_served'])) {
    $values = $event_state['services']['food_served'];
  }

  $field_title = "Will Food or Drink be Served? " . '<span class="title_display-note some_venues_dont_permit">' . "(Note some venues do not permit food and drink)</span>";
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
      '#id' => 'field-event-services-food_served',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-services-food_served',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['services']['food_served'][$key] = array(
        '#id' => 'field-event-services-food_served-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-services-food_served-' . $key,
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
function mcneese_event_workflow_get_event_field_services_food_caterer($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['services']['food_caterer'])) {
    $values = $event_state['services']['food_caterer'];
  }

  $field_title = "Provide the Name of the Caterer/Busineess";
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
      '#id' => 'field-event-services-food_caterer',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-services-food_caterer',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['services']['food_caterer'][$key] = array(
        '#id' => 'field-event-services-food_caterer-' . $key,
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
        '#id' => 'field-event-services-food_caterer-' . $key,
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
function mcneese_event_workflow_get_event_field_services_alcohol($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['services']['alcohol'])) {
    $values = $event_state['services']['alcohol'];
  }

  $field_title = "Will Alcohol be Served?";
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
      '#id' => 'field-event-services-alcohol',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-services-alcohol',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['services']['alcohol'][$key] = array(
        '#id' => 'field-event-services-alcohol-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-services-alcohol-' . $key,
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
function mcneese_event_workflow_get_event_field_services_alcohol_note($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);

  if ($form['form']['step']['#value'] == 'details' && array_key_exists('services', $form['form']['sections']['#value'])) {
    $fields['services']['alcohol_note'] = array(
      '#id' => 'field-event-services-alcohol_note',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-services-alcohol_note',
        ),
      ),
    );

    $fields['services']['alcohol_note'][0] = array(
      '#id' => 'field-event-services-alcohol_note-0',
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

    $fields['services']['alcohol_note'][0]['markup']['#markup'] .= '<div id="markup-event-services-alcohol_note" class="markup-event">';
    $fields['services']['alcohol_note'][0]['markup']['#markup'] .= 'A formal written request must be submitted to the President for approval of any alcohol served on campus and may require police officer presence at your event for an additional fee. ';
    $fields['services']['alcohol_note'][0]['markup']['#markup'] .= 'Contact the President\'s Office at ext. 5556 for more information. ';
    $fields['services']['alcohol_note'][0]['markup']['#markup'] .= '</div>';
  }

  return $values;
}

/**
 * Process and build a specific form field.
 */
function mcneese_event_workflow_get_event_field_services_open_flames($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => 0);
  if (!empty($event_state['services']['open_flames'])) {
    $values = $event_state['services']['open_flames'];
  }

  $build = FALSE;
  if (isset($form['form']['information_type']['#value'][0]) && $form['form']['information_type']['#value'][0] != 57) {
    $build = TRUE;
  }

  $field_title = "Will Open Flames be Used? " . '<span class="title_display-note examples">' . "(Examples: Sternos, Grill, Candles)</span>";
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
      '#id' => 'field-event-services-open_flames',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-services-open_flames',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['services']['open_flames'][$key] = array(
        '#id' => 'field-event-open_flames-' . $key,
        '#type' => 'radios',
        '#title' => $field_title,
        '#default_value' => $value,
        '#options' => array(1 => "Yes", 0 => "No"),
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
        '#id' => 'field-event-services-open_flames-' . $key,
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
function mcneese_event_workflow_get_event_field_additional($form, &$form_state, &$fields, &$event_state, $ps) {
  $values = array(0 => NULL);
  if (!empty($event_state['additional'])) {
    $values = $event_state['additional'];
  }

  $field_title = "Please Provide any Other Information or Questions you may Have for Campus Officials";
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
      '#id' => 'field-event-additional',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'form-item-wrapper',
          'field-event-item',
          'field-event-item-additional',
        ),
      ),
    );

    foreach ($values as $key => $value) {
      $fields['additional'][$key] = array(
        '#id' => 'field-event-additional-' . $key,
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
      );
    }
    krsort($fields['additional']);

    $limits_check['visible'] = TRUE;
  }
  else {
    foreach ($values as $key => $value) {
      $fields['additional'][$key] = array(
        '#id' => 'field-event-additional-' . $key,
        '#type' => 'value',
        '#value' => $value,
      );
    }
  }

  return $values;
}

/**
 * Returns an array containing classication information.
 *
 * @param array $state
 *   (optional) The form_state array containing the 'classsification' key and
 *   value.
 * @param object|null $user
 *   (optional) The loaded user object for the user to load the classification
 *   for. When NULL, returns all options available.
 *
 * @return array
 *   An array with the following keys:
 *   - count: the number of classifications available for the current user.
 *   - options: all available select list options.
 *   - value: the default value to use.
 */
function event_workflow_determine_user_classification($state = array(), $user = NULL) {
  $information = array();
  $information['count'] = 0;
  $information['options'] = array();
  $information['value'] = '';

  if (!is_array($state)) {
    cf_error:invalid_array('state');
    return $information;
  }

  if (!is_null($user) && !is_object($user)) {
    cf_error::invalid_object('user');

    return $information;
  }

  $information['options'] = event_workflow_get_event_classification_list_options(NULL, $user);
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
  $structure = mcneese_event_workflow_table_structure();
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
            if (!empty($event[$structure_group][$alternate_name])) {
              foreach ($event[$structure_group][$alternate_name] as $alternate_key => $alternate_object) {
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

            if (!empty($event[$alternate_parts[0]][$alternate_parts[1]])) {
              foreach ($event[$alternate_parts[0]][$alternate_parts[1]] as $alternate_key => $alternate_object) {
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
        $current = array(0 => NULL);

        if (!empty($event[$structure_group][$field_name])) {
          foreach ($event[$structure_group][$field_name] as $field_key => $field_object) {
            if (empty($field_structure['columns'])) {
              if (property_exists($field_object, 'value')) {
                $current[$field_key] = $field_object->value;

                if (array_key_exists('storage', $field_structure) && is_string($field_structure['storage']) && !empty($field_structure['storage'])) {
                  $storage_method = $field_structure['storage'];

                  if (array_key_exists($storage_method, $field_structure) && is_array($field_structure[$storage_method])) {
                    $converted = event_workflow_convert_value_from_database_format($current[$field_key], $storage_method, $field_structure[$storage_method]);

                    if ($converted !== FALSE) {
                      $current[$field_key] = $converted;
                    }
                  }
                }
              }
              else {
                $current[$field_key] = NULL;
              }
            }
            else {
              $current[$field_key] = array();

              foreach ($field_structure['columns'] as $column_id => $column_name) {
                $current[$field_key][$column_id] = NULL;

                if (property_exists($field_object, $column_name)) {
                  $current[$field_key][$column_id] = $field_object->$column_name;

                  if (array_key_exists('storage', $field_structure) && is_string($field_structure['storage']) && !empty($field_structure['storage'])) {
                    $storage_method = $field_structure['storage'];

                    if (array_key_exists($storage_method, $field_structure) && is_array($field_structure[$storage_method])) {
                      $converted = event_workflow_convert_value_from_database_format($current[$field_key][$column_id], $storage_method, $field_structure[$storage_method]);

                      if ($converted !== FALSE) {
                        $current[$field_key][$column_id] = $converted;
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
  }

  return $processed;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
