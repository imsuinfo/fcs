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
 * Return a list of event form fields.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param string $step
 *   (optional) The step on the event fields to build.
 * @param array $parent_structure
 *   (optional) An array of strings representing the top-level structure used
 *   outside of this function. This is necessary to ensure dependent values and
 *   similar functionality point to the proper locations.
 *
 * @return array $form
 *   An array containing the generated form.
 */
function mcneese_event_workflow_get_event_fields($form, &$form_state, $step = 'category', $parent_structure = array('event')) {
  if (!is_string($step)) {
    cf_error::invalid_string('step');

    // @todo: provide an error on the screen using the form markup.
    return array();
  }

  if (!is_array($parent_structure)) {
    cf_error::invalid_array('parent_structure');

    // @todo: provide an error on the screen using the form markup.
    return array();
  }

  $ps_1 = implode('-', $parent_structure) . '-';
  $ps_2 = NULL;
  $ps_c = $parent_structure;
  while (!empty($ps_c)) {
    $ps = array_pop($ps_c);

    if (is_null($ps_2)) {
      $ps_2 = $ps . '[';
    }
    else {
      $ps_2 .= $ps . '][';
    }
  }
  unset($ps_c);
  unset($ps);


  $user = cf_current_user();

  if (!isset($form_state['values']['event'])) {
    $form_state['values']['event'] = array();
  }
  $event_state = &$form_state['values']['event'];

  $fields = array();

  if (isset($form['form']['paramater_1']['#value']) && isset($form['form']['paramater_2']['#value'])) {
    if ($step == 'category' && !array_key_exists('category', $event_state)) {
      $catagory_list_options = event_workflow_get_category_list_options('radios');

      if (array_key_exists($form['form']['paramater_1']['#value'], $catagory_list_options)) {
        $room_list_options = event_workflow_get_room_list_options($event_state['category'], 'special');

        if (array_key_exists($form['form']['paramater_2']['#value'], $room_list_options)) {
          $event_state['category'] = $form['form']['paramater_1']['#value'];
          $event_state['room_list'] = $form['form']['paramater_2']['#value'];
          $step = 'standard';
        }
      }
      else {
        $event_state['category'] = $form['form']['paramater_1']['#value'];
        $step = 'rooms';
      }
    }
  }

  if ($step == 'category') {
    $fields['category'] = array(
      '#id' => 'field-event-category',
      '#type' => 'radios',
      '#title' => '<span class="strong underline">Please indicate the requested facility or event location?</span>',
      '#options' => event_workflow_get_category_list_options('radios'),
      '#default_value' => array_key_exists('category', $event_state) ? $event_state['category'] : NULL,
      '#required' => TRUE,
      '#needs_validation' => FALSE,
    );

    $form_state['conditionally_required'][0][] = 'category';
  }
  else {
    $fields['category'] = array(
      '#id' => 'field-event-category',
      '#type' => 'value',
      '#default_value' => array_key_exists('category', $event_state) ? $event_state['category'] : NULL,
    );
  }


  if ($step == 'rooms') {
    $fields['room_list'] = array(
      '#id' => 'field-event-room_list',
      '#type' => 'radios',
      '#title' => 'Select a Room',
      '#options' => event_workflow_get_room_list_options($event_state['category'], 'special'),
      '#default_value' => array_key_exists('room_list', $event_state) ? $event_state['room_list'] : NULL,
      '#required' => TRUE,
      '#needs_validation' => FALSE,
    );

    $form_state['conditionally_required'][0][] = 'room_list';
  }
  else {
    $fields['room_list'] = array(
      '#id' => 'field-event-room_list',
      '#type' => 'value',
      '#default_value' => array_key_exists('room_list', $event_state) ? $event_state['room_list'] : NULL,
    );
  }

  $room_information = NULL;
  if (array_key_exists('room_list', $event_state) && !is_null($event_state['room_list'])) {
    $room_information_parts = explode('_', $event_state['room_list'], 3);

    if (count($room_information_parts) == 3) {
      $room_information = mcneese_event_workflow_load_buildings($room_information_parts[0], $room_information_parts[1], $room_information_parts[2]);

      if (empty($room_information)) {
        $room_information = NULL;
      }
      else {
        $room_information = array_pop($room_information);
      }
    }
    unset($room_information_parts);
  }

  if ($step == 'standard') {

    // Fieldset: Dates
    $ps_3 = 'dates';
    if (!is_null($ps_2)) {
      $ps_3 .= ']';
    }

    if (!isset($event_state['dates'])) {
      $event_state['dates'] = array();
    }
    $dates_state = &$event_state['dates'];

    $fields['dates'] = array(
     '#id' => 'section-event-dates',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Date Settings",
    );

    $fields['dates']['date'] = array(
      '#id' => 'field-event-date',
      '#type' => 'date_popup',
      '#title' => 'Date of Event',
      '#default_value' => array_key_exists('date', $dates_state) ? $dates_state['date'] : NULL,
      '#date_format' => "Y/m/d",
      '#date_label_position' => 'invisible',
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'dates][date';

    $fields['dates']['time_start'] = array(
      '#id' => 'field-event-time_start',
      '#type' => 'date_popup',
      '#title' => 'Time the Event Starts',
      '#default_value' => array_key_exists('time_start', $dates_state) ? $dates_state['time_start'] : NULL,
      '#date_format' => "h:ia",
      '#date_label_position' => 'invisible',
      '#description' => "FIXME: this field type is not properly saving the field values",
    );

    $fields['dates']['time_stop'] = array(
      '#id' => 'field-event-time_stop',
      '#type' => 'date_popup',
      '#title' => 'Time the Event Stops',
      '#default_value' => array_key_exists('time_stop', $dates_state) ? $dates_state['time_stop'] : NULL,
      '#date_format' => "h:ia",
      '#date_label_position' => 'invisible',
      '#description' => "FIXME: this field type is not properly saving the field values",
    );


    // Fieldset: Coordinator
    $ps_3 = 'coordinator';
    if (!is_null($ps_2)) {
      $ps_3 .= ']';
    }

    if (!isset($event_state['coordinator'])) {
      $event_state['coordinator'] = array();
    }
    $coordinator_state = &$event_state['coordinator'];

    $fields['coordinator'] = array(
      '#id' => 'section-event-coordinator',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Coordinator Settings",
    );

    $coordinator_name = '';
    if (array_key_exists('name', $coordinator_state)) {
      $coordinator_name = $coordinator_state['name'];
    }

    $fields['coordinator']['name'] = array(
      '#id' => 'field-event-coordinator-name',
      '#type' => 'textfield',
      '#title' => 'Coordinator Name',
      '#default_value' => $coordinator_name,
      '#required' => TRUE,
      '#size' => 48,
    );

    $form_state['conditionally_required'][0][] = 'coordinator][name';

    $coordinator_phone = '';
    if (array_key_exists('phone', $coordinator_state)) {
      $coordinator_phone = $coordinator_state['phone'];
    }

    $fields['coordinator']['phone'] = array(
      '#id' => 'field-event-coordinator-phone',
      '#type' => 'telfield',
      '#title' => 'Coordinator Phone',
      '#default_value' => $coordinator_phone,
    );

    $coordinator_email = '';
    if (array_key_exists('email', $coordinator_state)) {
      $coordinator_email = $coordinator_state['email'];
    }

    $fields['coordinator']['email'] = array(
      '#id' => 'field-event-coordinator-email',
      '#type' => 'emailfield',
      '#title' => 'Coordinator E-Mail',
      '#default_value' => $coordinator_email,
      '#size' => 48,
    );

    $classification = event_workflow_determine_coordinator_classification($coordinator_state);

    // multiple classifications exists, so prompt user for desired classification.
    if ($classification['count'] > 1) {
      $fields['coordinator']['classification'] = array(
        '#id' => 'field-event-coordinator-classification',
        '#type' => 'select',
        '#title' => t("Classification"),
        '#description' => t("You are associated with multiple classifications. Please specify which classification this event request form is to be associated with."),
        '#options' => $classification['options'],
        '#default_value' => $classification['value'],
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
        '#value' => $classification['value'],
        '#default_value' => $classification['value'],
        '#readonly' => TRUE,
        '#attributes' => array('readonly' => 'readonly'),
        '#required' => TRUE,
      );

      $form_state['conditionally_required'][0][] = 'coordinator][classification';
    }


    // Fieldset: Information
    $ps_3 = 'information';
    if (!is_null($ps_2)) {
      $ps_3 .= ']';
    }

    if (!isset($event_state['information'])) {
      $event_state['information'] = array();
    }
    $information_state = &$event_state['information'];

    $fields['information'] = array(
      '#id' => 'section-event-information',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Event Information",
    );

    $information_title = '';
    if (array_key_exists('title', $information_state)) {
      $information_title = $information_state['title'];
    }

    $fields['information']['title'] = array(
      '#id' => 'field-event-information-title',
      '#type' => 'textfield',
      '#title' => 'Event Title',
      '#default_value' => $information_title,
      '#required' => TRUE,
      '#size' => 48,
    );

    $form_state['conditionally_required'][0][] = 'information][title';

    $information_attendance = 0;
    if (array_key_exists('attendance', $information_state) && is_numeric($information_state['attendance'])) {
      $information_attendance = $information_state['attendance'];
    }

    $fields['information']['attendance'] = array(
      '#id' => 'field-event-information-attendance',
      '#type' => 'numberfield',
      '#title' => 'Expected Number of Attendance',
      '#description' => 'Current room has a max attendance of ' . check_plain($room_information['capacity']) . '.',
      '#default_value' => $information_attendance,
      '#min' => 0,
      '#max' => $room_information['capacity'],
    );

    $information_type = '';
    if (array_key_exists('type', $information_state)) {
      $information_type = $information_state['type'];
    }

    $type_options = event_workflow_get_information_type_list_options();

    $fields['information']['type'] = array(
      '#id' => 'field-event-information-type',
      '#type' => 'select',
      '#title' => 'Type of Event',
      '#default_value' => $information_type,
      '#options' => $type_options,
      '#required' => TRUE,
    );
    unset($type_options);

    $form_state['conditionally_required'][0][] = 'information][type';
  }
  else {
    if (!isset($event_state['dates'])) {
      $event_state['dates'] = array();
    }
    $dates_state = &$event_state['dates'];

    $fields['dates'] = array(
      '#tree' => TRUE,
    );

    foreach (array('date', 'time_start', 'time_stop') as $key) {
      $fields['dates'][$key] = array(
        '#id' => 'field-event-dates-' . $key,
        '#type' => 'value',
        '#default_value' => array_key_exists($key, $dates_state) ? $dates_state[$key] : NULL,
      );
    }


    if (!isset($event_state['coordinator'])) {
      $event_state['coordinator'] = array();
    }
    $coordinator_state = &$event_state['coordinator'];

    $fields['coordinator'] = array(
      '#tree' => TRUE,
    );

    if (!array_key_exists('name', $coordinator_state)) {
      $coordinator_state['name'] = '';
      if (!empty($user->field_user_last_name['und'][0]['safe_value'])) {
        $coordinator_state['name'] .= $user->field_user_last_name['und'][0]['safe_value'];
      }

      if (!empty($user->field_user_first_name['und'][0]['safe_value'])) {
        if (!empty($coordinator_state['name'])) {
          $coordinator_state['name'] .= ', ';
        }

        $coordinator_state['name'] .= $user->field_user_first_name['und'][0]['safe_value'];
      }
    }

    if (!array_key_exists('phone', $coordinator_state)) {
      if (!empty($user->field_user_phone_number['und'][0]['safe_value'])) {
        $coordinator_state['phone'] = $user->field_user_phone_number['und'][0]['safe_value'];
      }
    }

    if (!array_key_exists('email', $coordinator_state)) {
      if (!empty($user->mail)) {
        $coordinator_state['email'] = $user->mail;
      }
    }

    if (!array_key_exists('classification', $coordinator_state)) {
      $classification = event_workflow_determine_coordinator_classification($coordinator_state);

      $coordinator_state['classification'] = $classification['value'];
    }

    foreach (array('name', 'phone', 'email', 'classification') as $key) {
      $fields['coordinator'][$key] = array(
        '#id' => 'field-event-coordinator-' . $key,
        '#type' => 'value',
        '#default_value' => array_key_exists($key, $coordinator_state) ? $coordinator_state[$key] : NULL,
      );
    }


    if (!isset($event_state['information'])) {
      $event_state['information'] = array();
    }
    $information_state = &$event_state['information'];

    $fields['information'] = array(
      '#tree' => TRUE,
    );

    foreach (array('title', 'attendance', 'type') as $key) {
      $fields['information'][$key] = array(
        '#id' => 'field-event-information-' . $key,
        '#type' => 'value',
        '#default_value' => array_key_exists($key, $information_state) ? $information_state[$key] : NULL,
      );
    }

    $information_type = '';
    if (array_key_exists('type', $information_state)) {
      $information_type = $information_state['type'];
    }
  }

  // 57 = 'meeting'
  $meeting_event = FALSE;
  if ($information_type == 57) {
    $meeting_event = TRUE;
  }


  if ($step == 'details') {

    // Fieldset: Plans
    $ps_3 = 'plans';
    if (!is_null($ps_2)) {
      $ps_3 .= ']';
    }

    if ($meeting_event) {
      $fields['meeting_notice'] = array(
        '#markup' => '',
      );

      $fields['meeting_notice']['#markup'] .= '<div id="markup-event-meeting_notice" class="markup-event">';
      $fields['meeting_notice']['#markup'] .= "* Please note a specified drawing of setup requests is required to complete the reservation. ";
      $fields['meeting_notice']['#markup'] .= "The reserving office will provide a venue layout form for setup submission.";
      $fields['meeting_notice']['#markup'] .= "</div>";
    }

    $fields['plans'] = array(
      '#id' => 'section-event-plans',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Event Plans",
    );

    if (!isset($event_state['plans'])) {
      $event_state['plans'] = array();
    }
    $plans_state = &$event_state['plans'];

    $type_term = taxonomy_term_load($information_state['type']);
    $type = $type_term->name;

    if (!$meeting_event) {
      $plans_description = '';
      if (array_key_exists('description', $plans_state)) {
        $plans_description = $plans_state['description'];
      }

      $fields['plans']['description'] = array(
        '#id' => 'field-event-plans-description',
        '#type' => 'textarea',
        '#title' => 'Event Description/Purpose',
        '#default_value' => $plans_description,
        '#rows' => 2,
      );

      $plans_activities = '';
      if (array_key_exists('activities', $plans_state)) {
        $plans_activities = $plans_state['activities'];
      }

      $fields['plans']['activities'] = array(
        '#id' => 'field-event-plans-activities',
        '#type' => 'textarea',
        '#title' => 'List all activities planned within the event',
        '#default_value' => $plans_activities,
        '#rows' => 2,
      );

      $plans_dates = '';
      if (array_key_exists('dates', $plans_state)) {
        $plans_dates = $plans_state['dates'];
      }

      $fields['plans']['dates'] = array(
        '#id' => 'field-event-plans-dates',
        '#type' => 'textarea',
        '#title' => 'Please indicate all additional dates and times needed for plans and/or rehearsals',
        '#default_value' => $plans_dates,
        '#rows' => 2,
      );
    }

    $plans_audience = '';
    if (array_key_exists('audience', $plans_state)) {
      $plans_audience = $plans_state['audience'];
    }

    $audience_options = event_workflow_get_audience_list_options();

    $fields['plans']['audience'] = array(
      '#id' => 'field-event-plans-audience',
      '#type' => 'select',
      '#title' => 'Intended Audience',
      '#default_value' => $plans_audience,
      '#options' => $audience_options,
      '#required' => TRUE,
    );
    unset($audience_options);

    $form_state['conditionally_required'][0][] = 'plans][audience';


    // Fieldset: Registration
    $ps_3 = 'registration';
    if (!is_null($ps_2)) {
      $ps_3 .= ']';
    }

    $fields['registration'] = array(
      '#id' => 'section-event-registration',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Event Registration",
    );

    if (!isset($event_state['registration'])) {
      $event_state['registration'] = array();
    }
    $registration_state = &$event_state['registration'];

    if (!$meeting_event) {
      $registration_require = 'no';
      if (!empty($registration_state['require'])) {
        $registration_require = $registration_state['require'];
      }

      $fields['registration']['require'] = array(
        '#id' => 'field-event-registration-require',
        '#type' => 'radios',
        '#title' => 'Does the event require registration or ticket purchase?',
        '#default_value' => $registration_require,
        '#options' => array('yes' => "Yes", 'no' => "No"),
        '#required' => TRUE,
      );

      $form_state['conditionally_required'][0][] = 'registration][require';

      $registration_website = '';
      if (array_key_exists('website', $registration_state)) {
        $registration_website = $registration_state['website'];
      }

      $fields['registration']['website'] = array(
        '#id' => 'field-event-registration-website',
        '#type' => 'textfield',
        '#title' => 'Please indicate a <em>Registration Website</em>',
        '#default_value' => $registration_website,
        '#size' => 48,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $registration_phone = '';
      if (array_key_exists('phone', $registration_state)) {
        $registration_phone = $registration_state['phone'];
      }

      $fields['registration']['phone'] = array(
        '#id' => 'field-event-registration-phone',
        '#type' => 'telfield',
        '#title' => 'Please indicate a <em>Registration Phone</em>',
        '#default_value' => $registration_phone,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $registration_ticket_price = '';
      if (array_key_exists('ticket_price', $registration_state)) {
        $registration_ticket_price = $registration_state['ticket_price'];
      }

      $fields['registration']['ticket_price'] = array(
        '#id' => 'field-event-registration-ticket_price',
        '#type' => 'textfield',
        '#title' => 'Please indicate a <em>Ticket Price</em>',
        '#default_value' => $registration_ticket_price,
        '#size' => 48,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $registration_ticket_dates = '';
      if (array_key_exists('ticket_dates', $registration_state)) {
        $registration_ticket_dates = $registration_state['ticket_dates'];
      }

      $fields['registration']['ticket_dates'] = array(
        '#id' => 'field-event-registration-ticket_dates',
        '#type' => 'textfield',
        '#title' => 'Please indicate <em>Dates</em> the tickets are one sale',
        '#default_value' => $registration_ticket_dates,
        '#size' => 48,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $registration_ticket_website = '';
      if (array_key_exists('ticket_website', $registration_state)) {
        $registration_ticket_website = $registration_state['ticket_website'];
      }

      $fields['registration']['ticket_website'] = array(
        '#id' => 'field-event-registration-ticket_website',
        '#type' => 'textfield',
        '#title' => 'Please indicate a <em>Ticket Website</em>',
        '#default_value' => $registration_ticket_website,
        '#size' => 48,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $registration_ticket_phone = '';
      if (array_key_exists('ticket_phone', $registration_state)) {
        $registration_ticket_phone = $registration_state['ticket_phone'];
      }

      $fields['registration']['ticket_phone'] = array(
        '#id' => 'field-event-registration-ticket_phone',
        '#type' => 'textfield',
        '#title' => 'Please indicate a <em>Ticket Phone</em>',
        '#default_value' => $registration_ticket_phone,
        '#size' => 48,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[require]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $registration_generate_revenue = 'no';
      if (!empty($registration_state['generate_revenue'])) {
        $registration_generate_revenue = $registration_state['generate_revenue'];
      }

      $fields['registration']['generate_revenue'] = array(
        '#id' => 'field-event-registration-generate_revenue',
        '#type' => 'radios',
        '#title' => 'Will the Event Generate Revenue?',
        '#default_value' => $registration_generate_revenue,
        '#options' => array('yes' => "Yes", 'no' => "No"),
        '#required' => TRUE,
      );

      $form_state['conditionally_required'][0][] = 'registration][generate_revenue';

      $registration_revenue_generated = array();
      if (array_key_exists('revenue_generated', $registration_state)) {
        $registration_revenue_generated = $registration_state['revenue_generated'];
      }

      $revenue_generated_options = event_workflow_get_revenue_generated_list_options('checkboxes');

      $fields['registration']['revenue_generated'] = array(
        '#id' => 'field-event-registration-revenue_generated',
        '#type' => 'checkboxes',
        '#title' => 'How Will Revenue be Generated?',
        '#default_value' => $registration_revenue_generated,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[generate_revenue]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[generate_revenue]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[generate_revenue]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#options' => $revenue_generated_options,
      );

      unset($revenue_generated_options);
    }


    // Fieldset: Setup
    $ps_3 = 'setup';
    if (!is_null($ps_2)) {
      $ps_3 .= ']';
    }

    $fields['setup'] = array(
      '#id' => 'section-event-setup',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Event Setup",
    );

    if (!isset($event_state['setup'])) {
      $event_state['setup'] = array();
    }
    $setup_state = &$event_state['setup'];

    $fields['setup']['how_many_tables_needed'] = array(
      '#id' => 'section-event-setup-how_many_tables_needed',
      '#type' => 'fieldset',
      '#title' => 'How Many Tables Will You Need?',
      '#collapsible' => FALSE,
    );

    $setup_rectangular_tables_8ft = 0;
    if (array_key_exists('rectangular_tables_8ft', $setup_state) && is_numeric($setup_state['rectangular_tables_8ft'])) {
      $setup_rectangular_tables_8ft = $setup_state['rectangular_tables_8ft'];
    }

    $fields['setup']['rectangular_tables_8ft'] = array(
      '#id' => 'field-event-setup-rectangular_tables_8ft',
      '#type' => 'numberfield',
      '#title' => 'How Many 8 ft Rectangular Tables Will You Need?',
      '#default_value' => $setup_rectangular_tables_8ft,
      '#min' => 0,
      '#parent' => array('event', 'setup', 'how_many_tables_needed'),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'setup][rectangular_tables_8ft';

    $setup_round_tables_8ft = 0;
    if (array_key_exists('round_tables_8ft', $setup_state) && is_numeric($setup_state['round_tables_8ft'])) {
      $setup_round_tables_8ft = $setup_state['round_tables_8ft'];
    }

    $fields['setup']['round_tables_8ft'] = array(
      '#id' => 'field-event-setup-round_tables_8ft',
      '#type' => 'numberfield',
      '#title' => 'How Many 8 ft Round Tables Will You Need?',
      '#default_value' => $setup_round_tables_8ft,
      '#min' => 0,
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'setup][round_tables_8ft';


    if (!$meeting_event) {
      $setup_other_tables = '';
      if (array_key_exists('other_tables', $setup_state)) {
        $setup_other_tables = $setup_state['other_tables'];
      }

      $fields['setup']['other_tables'] = array(
        '#id' => 'field-event-setup-other_tables',
        '#type' => 'textarea',
        '#title' => 'How Many Other Tables Will You Need? <span class="title_display-note describe_the_table">(Describe the Table)</span>',
        '#default_value' => $setup_other_tables,
        '#rows' => 2,
      );
    }

    $setup_standard_blue_chairs = 0;
    if (array_key_exists('standard_blue_chairs', $setup_state) && is_numeric($setup_state['standard_blue_chairs'])) {
      $setup_standard_blue_chairs = $setup_state['standard_blue_chairs'];
    }

    $fields['setup']['standard_blue_chairs'] = array(
      '#id' => 'field-event-setup-standard_blue_chairs',
      '#type' => 'numberfield',
      '#title' => 'How Many Standard Blue Chairs Will You Need?',
      '#default_value' => $setup_standard_blue_chairs,
      '#min' => 0,
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'setup][standard_blue_chairs';

    $setup_podium = 'no';
    if (!empty($setup_state['podium'])) {
      $setup_podium = $setup_state['podium'];
    }

    $fields['setup']['podium'] = array(
      '#id' => 'field-event-setup-podium',
      '#type' => 'radios',
      '#title' => 'Will You Need a Podium?',
      '#default_value' => $setup_podium,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'setup][podium';

    if (!$meeting_event) {
      $setup_portable_stage = 'no';
      if (!empty($setup_state['portable_stage'])) {
        $setup_portable_stage = $setup_state['portable_stage'];
      }

      $fields['setup']['portable_stage'] = array(
        '#id' => 'field-event-setup-portable_stage',
        '#type' => 'radios',
        '#title' => 'Will You Require a Portable Stage?',
        '#default_value' => $setup_portable_stage,
        '#options' => array('yes' => "Yes", 'no' => "No"),
        '#required' => TRUE,
      );

      $setup_portable_stage_configuration = '';
      if (array_key_exists('portable_stage_configuration', $setup_state)) {
        $setup_portable_stage_configuration = $setup_state['portable_stage_configuration'];
      }

      $fields['setup']['portable_stage_configuration'] = array(
        '#id' => 'field-event-setup-portable_stage_configuration',
        '#type' => 'textarea',
        '#title' => 'What size stage and configuration is needed?',
        '#default_value' => $setup_portable_stage_configuration,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[portable_stage]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[portable_stage]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[portable_stage]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $form_state['conditionally_required'][0][] = 'setup][portable_stage_configuration';

      $setup_security = 'no';
      if (!empty($setup_state['security'])) {
        $setup_security = $setup_state['security'];
      }

      $fields['setup']['security'] = array(
        '#id' => 'field-event-setup-security',
        '#type' => 'radios',
        '#title' => 'Will the Event Require Security? <span class="title_display-note additional_fees_may_apply">(Additional Fees May Apply)</span>',
        '#default_value' => $setup_security,
        '#options' => array('yes' => "Yes", 'no' => "No"),
        '#required' => TRUE,
      );

      $form_state['conditionally_required'][0][] = 'setup][security';

      $setup_parking_assistance = 'no';
      if (!empty($setup_state['parking_assistance'])) {
        $setup_parking_assistance = $setup_state['parking_assistance'];
      }

      $fields['setup']['parking_assistance'] = array(
        '#id' => 'field-event-setup-parking_assistance',
        '#type' => 'radios',
        '#title' => 'Will the Event Require Parking Assistance or Restrictions?',
        '#default_value' => $setup_parking_assistance,
        '#options' => array('yes' => "Yes", 'no' => "No"),
        '#required' => TRUE,
      );

      $form_state['conditionally_required'][0][] = 'setup][parking_assistance';

      $setup_parking_assistance_area = '';
      if (array_key_exists('parking_assistance_area', $setup_state)) {
        $setup_parking_assistance_area = $setup_state['parking_assistance_area'];
      }

      $fields['setup']['parking_assistance_area'] = array(
        '#id' => 'field-event-setup-parking_assistance_area',
        '#type' => 'textarea',
        '#title' => 'What Parking Area will be Accessed or Restricted?',
        '#default_value' => $setup_parking_assistance_area,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[parking_assistance]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[parking_assistance]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[parking_assistance]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
        '#rows' => 2,
      );

      $setup_road_closures = 'no';
      if (!empty($setup_state['road_closures'])) {
        $setup_road_closures = $setup_state['road_closures'];
      }

      $fields['setup']['road_closures'] = array(
        '#id' => 'field-event-setup-road_closures',
        '#type' => 'radios',
        '#title' => 'Will the Event Require Road Closures or Barricades?',
        '#default_value' => $setup_road_closures,
        '#options' => array('yes' => "Yes", 'no' => "No"),
        '#required' => TRUE,
      );

      $setup_road_closures_details = '';
      if (array_key_exists('road_closures_details', $setup_state)) {
        $setup_road_closures_details = $setup_state['road_closures_details'];
      }

      $fields['setup']['road_closures_details'] = array(
        '#id' => 'field-event-setup-road_closures_details',
        '#type' => 'textarea',
        '#title' => 'Provide an Explanation on the Road Closures or Barricades',
        '#default_value' => $setup_road_closures_details,
        '#rows' => 2,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[road_closures]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[road_closures]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[road_closures]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $setup_special_requests = 'no';
      if (!empty($setup_state['special_requests'])) {
        $setup_special_requests = $setup_state['special_requests'];
      }

      $fields['setup']['special_requests'] = array(
        '#id' => 'field-event-setup-special_requests',
        '#type' => 'radios',
        '#title' => 'Will the Event Require any Additional Special Requests for Setup?',
        '#default_value' => $setup_special_requests,
        '#options' => array('yes' => "Yes", 'no' => "No"),
        '#required' => TRUE,
      );

      $form_state['conditionally_required'][0][] = 'setup][special_requests';

      $setup_special_requests_details = '';
      if (array_key_exists('special_requests_details', $setup_state)) {
        $setup_special_requests_details = $setup_state['special_requests_details'];
      }

      $fields['setup']['special_requests_details'] = array(
        '#id' => 'field-event-setup-special_requests_details',
        '#type' => 'textarea',
        '#title' => 'Explanation on the Special Requests',
        '#default_value' => $setup_special_requests_details,
        '#rows' => 2,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[special_requests]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[special_requests]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[special_requests]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );
    }

    $form_state['conditionally_required'][0][] = 'setup][special_requests_details';


    // Fieldset: Presentation
    $ps_3 = 'presentation';
    if (!is_null($ps_2)) {
      $ps_3 .= ']';
    }

    $fields['presentation'] = array(
      '#id' => 'section-event-presentation',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Event Presentation",
    );

    if (!isset($event_state['presentation'])) {
      $event_state['presentation'] = array();
    }
    $presentation_state = &$event_state['presentation'];

    $presentation_technical_equipment = 'no';
    if (!empty($presentation_state['technical_equipment'])) {
      $presentation_technical_equipment = $presentation_state['technical_equipment'];
    }

    $fields['presentation']['technical_equipment'] = array(
      '#id' => 'field-event-presentation-technical_equipment',
      '#type' => 'radios',
      '#title' => 'Will the Event Require any Technical Equipment? <span class="title_display-note additional_fees_may_apply">(Additional Fees May Apply)</span>',
      '#default_value' => $presentation_technical_equipment,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $presentation_technical_equipment_details = array();
    if (array_key_exists('technical_equipment_details', $presentation_state)) {
      $presentation_technical_equipment_details = $presentation_state['technical_equipment_details'];
    }

    $technical_equipment_details_options = event_workflow_get_technical_equipment_details_list_options('checkboxes');

    $fields['presentation']['technical_equipment_details'] = array(
      '#id' => 'field-event-presentation-technical_equipment_details',
      '#type' => 'checkboxes',
      '#title' => 'Specify the Equipment Needed',
      '#default_value' => $presentation_technical_equipment_details,
      '#options' => $technical_equipment_details_options,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $ps_2 . $ps_3 . '[technical_equipment]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[technical_equipment]' . '"]' => array('value' => 'no'),
            ':input[name="' . $ps_2 . $ps_3 . '[technical_equipment]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][0][] = 'presentation][technical_equipment_details';

    $presentation_technical_equipment_microphone = 0;
    if (array_key_exists('technical_equipment_microphone', $presentation_state) && is_numeric($presentation_state['technical_equipment_microphone'])) {
      $presentation_technical_equipment_microphone = $presentation_state['technical_equipment_microphone'];
    }

    $fields['presentation']['technical_equipment_microphone'] = array(
      '#id' => 'field-event-presentation-technical_equipment_microphone',
      '#type' => 'numberfield',
      '#title' => 'Specify the Quantity of Microphones',
      '#default_value' => $presentation_technical_equipment_microphone,
      '#min' => 0,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $ps_2 . $ps_3 . '[technical_equipment]' . '"]' => array('value' => 'yes'),
          ':input[name="' . $ps_2 . $ps_3 . '[technical_equipment_details][76]' . '"]' => array('checked' => TRUE),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[technical_equipment_details][76]' . '"]' => array('checked' => FALSE),
            ':input[name="' . $ps_2 . $ps_3 . '[technical_equipment_details][76]' . '"]' => array('value' => NULL),
            ':input[name="' . $ps_2 . $ps_3 . '[technical_equipment]' . '"]' => array('value' => 'no'),
            ':input[name="' . $ps_2 . $ps_3 . '[technical_equipment]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    if (!$meeting_event) {
      $presentation_external_audio_person = 'no';
      if (!empty($presentation_state['external_audio_person'])) {
        $presentation_external_audio_person = $presentation_state['external_audio_person'];
      }

      $fields['presentation']['external_audio_person'] = array(
        '#id' => 'field-event-presentation-external_audio_person',
        '#type' => 'radios',
        '#title' => 'Will an External Audio/Video Person/Company be Utilized?',
        '#default_value' => $presentation_external_audio_person,
        '#options' => array('yes' => "Yes", 'no' => "No"),
        '#required' => TRUE,
      );

      $presentation_external_audio_person_name = '';
      if (array_key_exists('external_audio_person_name', $presentation_state)) {
        $presentation_external_audio_person_name = $presentation_state['external_audio_person_name'];
      }

      $fields['presentation']['external_audio_person_name'] = array(
        '#id' => 'field-event-presentation-external_audio_person_name',
        '#type' => 'textfield',
        '#title' => 'Provide the Name of the Person/Company Providing the External Audio/Video',
        '#default_value' => $presentation_external_audio_person_name,
        '#size' => 48,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[external_audio_person]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[external_audio_person]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[external_audio_person]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $presentation_external_audio_person_email = '';
      if (array_key_exists('external_audio_person_email', $presentation_state)) {
        $presentation_external_audio_person_email = $presentation_state['external_audio_person_email'];
      }

      $fields['presentation']['external_audio_person_email'] = array(
        '#id' => 'field-event-presentation-external_audio_person_email',
        '#type' => 'emailfield',
        '#title' => 'Provide the E-mail of the Person/Company Providing the External Audio/Video',
        '#default_value' => $presentation_external_audio_person_email,
        '#size' => 48,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[external_audio_person]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[external_audio_person]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[external_audio_person]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $form_state['conditionally_required'][0][] = 'presentation][external_audio_person_email';

      $presentation_external_audio_person_phone = '';
      if (array_key_exists('external_audio_person_phone', $presentation_state)) {
        $presentation_external_audio_person_phone = $presentation_state['external_audio_person_phone'];
      }

      $fields['presentation']['external_audio_person_phone'] = array(
        '#id' => 'field-event-presentation-external_audio_person_phone',
        '#type' => 'telfield',
        '#title' => 'Provide the Phone Number of the Person/Company Providing the External Audio/Video',
        '#default_value' => $presentation_external_audio_person_phone,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[external_audio_person]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[external_audio_person]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[external_audio_person]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $presentation_production = 'no';
      if (!empty($presentation_state['production'])) {
        $presentation_production = $presentation_state['production'];
      }

      $fields['presentation']['production'] = array(
        '#id' => 'field-event-presentation-production',
        '#type' => 'radios',
        '#title' => 'Is the Event Using a Production Company or Professional Promoter?',
        '#default_value' => $presentation_production,
        '#options' => array('yes' => "Yes", 'no' => "No"),
        '#required' => TRUE,
      );

      $presentation_production_name = '';
      if (array_key_exists('production_name', $presentation_state)) {
        $presentation_production_name = $presentation_state['production_name'];
      }

      $fields['presentation']['production_name'] = array(
        '#id' => 'field-event-presentation-production_name',
        '#type' => 'textfield',
        '#title' => 'Name of the Company/Individual',
        '#default_value' => $presentation_production_name,
        '#size' => 48,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[production]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[production]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[production]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $presentation_production_email = '';
      if (array_key_exists('production_email', $presentation_state)) {
        $presentation_production_email = $presentation_state['production_email'];
      }

      $fields['presentation']['production_email'] = array(
        '#id' => 'field-event-presentation-production_email',
        '#type' => 'emailfield',
        '#title' => 'Contact E-mail of the Company/Individual',
        '#default_value' => $presentation_production_email,
        '#size' => 48,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[production]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[production]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[production]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $form_state['conditionally_required'][0][] = 'presentation][production_email';

      $presentation_production_phone = '';
      if (array_key_exists('production_phone', $presentation_state)) {
        $presentation_production_phone = $presentation_state['production_phone'];
      }

      $fields['presentation']['production_phone'] = array(
        '#id' => 'field-event-presentation-production_phone',
        '#type' => 'telfield',
        '#title' => 'Phone Number of the Company/Individual',
        '#default_value' => $presentation_production_phone,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[production]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[production]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[production]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );
    }

    $presentation_publicity = 'no';
    if (!empty($presentation_state['publicity'])) {
      $presentation_publicity = $presentation_state['publicity'];
    }

    $fields['presentation']['publicity'] = array(
      '#id' => 'field-event-presentation-publicity',
      '#type' => 'radios',
      '#title' => 'Will the Event Need Publicity?',
      '#default_value' => $presentation_publicity,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'presentation][publicity';

    $presentation_publicity_details = array();
    if (array_key_exists('publicity_details', $presentation_state)) {
      $presentation_publicity_details = $presentation_state['publicity_details'];
    }

    $publicity_details_options = event_workflow_get_publicity_details_list_options('checkboxes');

    $fields['presentation']['publicity_details'] = array(
      '#id' => 'field-event-presentation-publicity_details',
      '#type' => 'checkboxes',
      '#title' => 'Specify all Publicity Medium that Apply',
      '#default_value' => $presentation_publicity_details,
      '#options' => $publicity_details_options,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $ps_2 . $ps_3 . '[publicity]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[publicity]' . '"]' => array('value' => 'no'),
            ':input[name="' . $ps_2 . $ps_3 . '[publicity]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $form_state['conditionally_required'][0][] = 'presentation][publicity_details';

    if (!$meeting_event) {
      $presentation_printed_material = 'no';
      if (!empty($presentation_state['printed_material'])) {
        $presentation_printed_material = $presentation_state['printed_material'];
      }

      $fields['presentation']['printed_material'] = array(
        '#id' => 'field-event-presentation-printed_material',
        '#type' => 'radios',
        '#title' => 'Will the Event use Printed Material?',
        '#default_value' => $presentation_printed_material,
        '#options' => array('yes' => "Yes", 'no' => "No"),
        '#required' => TRUE,
      );

      $presentation_university_logo = 'no';
      if (!empty($presentation_state['university_logo'])) {
        $presentation_university_logo = $presentation_state['university_logo'];
      }

      $fields['presentation']['university_logo'] = array(
        '#id' => 'field-event-presentation-university_logo',
        '#type' => 'radios',
        '#title' => 'Will the Event use the University Logo?',
        '#default_value' => $presentation_university_logo,
        '#options' => array('yes' => "Yes", 'no' => "No"),
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[printed_material]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[printed_material]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[printed_material]' . '"]' => array('value' => NULL),
            ),
          ),
          '#required' => TRUE,
        ),
      );

      $presentation_designing_material = array();
      if (array_key_exists('designing_material', $presentation_state)) {
        $presentation_designing_material = $presentation_state['designing_material'];
      }

      $desiging_material_options = event_workflow_get_designing_material_list_options('checkboxes');

      $fields['presentation']['designing_material'] = array(
        '#id' => 'field-event-presentation-designing_material',
        '#type' => 'checkboxes',
        '#title' => 'Who will be Designing the Material?',
        '#default_value' => $presentation_designing_material,
        '#options' => $desiging_material_options,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[printed_material]' . '"]' => array('value' => 'yes'),
            ':input[name="' . $ps_2 . $ps_3 . '[university_logo]' . '"]' => array('value' => 'yes'),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[printed_material]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[printed_material]' . '"]' => array('value' => NULL),
              ':input[name="' . $ps_2 . $ps_3 . '[university_logo]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[university_logo]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $presentation_designing_material_name = '';
      if (array_key_exists('designing_material_name', $presentation_state)) {
        $presentation_designing_material_name = $presentation_state['designing_material_name'];
      }

      $fields['presentation']['designing_material_name'] = array(
        '#id' => 'field-event-presentation-designing_material_name',
        '#type' => 'textfield',
        '#title' => 'Name of who will be Designing the Material',
        '#default_value' => $presentation_designing_material_name,
        '#size' => 48,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[printed_material]' . '"]' => array('value' => 'yes'),
            ':input[name="' . $ps_2 . $ps_3 . '[university_logo]' . '"]' => array('value' => 'yes'),
            ':input[name="' . $ps_2 . $ps_3 . '[designing_material][97]' . '"]' => array('checked' => TRUE),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[designing_material][97]' . '"]' => array('checked' => FALSE),
              ':input[name="' . $ps_2 . $ps_3 . '[printed_material]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[printed_material]' . '"]' => array('value' => NULL),
              ':input[name="' . $ps_2 . $ps_3 . '[university_logo]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[university_logo]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $presentation_designing_material_email = '';
      if (array_key_exists('designing_material_email', $presentation_state)) {
        $presentation_designing_material_email = $presentation_state['designing_material_email'];
      }

      $fields['presentation']['designing_material_email'] = array(
        '#id' => 'field-event-presentation-designing_material_email',
        '#type' => 'emailfield',
        '#title' => 'E-mail of who will be Designing the Material',
        '#default_value' => $presentation_designing_material_email,
        '#size' => 48,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[printed_material]' . '"]' => array('value' => 'yes'),
            ':input[name="' . $ps_2 . $ps_3 . '[university_logo]' . '"]' => array('value' => 'yes'),
            ':input[name="' . $ps_2 . $ps_3 . '[designing_material][97]' . '"]' => array('checked' => TRUE),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[designing_material][97]' . '"]' => array('checked' => FALSE),
              ':input[name="' . $ps_2 . $ps_3 . '[printed_material]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[printed_material]' . '"]' => array('value' => NULL),
              ':input[name="' . $ps_2 . $ps_3 . '[university_logo]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[university_logo]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );

      $presentation_designing_material_phone = '';
      if (array_key_exists('designing_material_phone', $presentation_state)) {
        $presentation_designing_material_phone = $presentation_state['designing_material_phone'];
      }

      $fields['presentation']['designing_material_phone'] = array(
        '#id' => 'field-event-presentation-designing_material_phone',
        '#type' => 'telfield',
        '#title' => 'Phone Number of who will be Designing the Material',
        '#default_value' => $presentation_designing_material_phone,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[printed_material]' . '"]' => array('value' => 'yes'),
            ':input[name="' . $ps_2 . $ps_3 . '[university_logo]' . '"]' => array('value' => 'yes'),
            ':input[name="' . $ps_2 . $ps_3 . '[designing_material][97]' . '"]' => array('checked' => TRUE),
          ),
          'invisible' => array(
            'or' => array(
              ':input[name="' . $ps_2 . $ps_3 . '[designing_material][97]' . '"]' => array('checked' => FALSE),
              ':input[name="' . $ps_2 . $ps_3 . '[printed_material]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[printed_material]' . '"]' => array('value' => NULL),
              ':input[name="' . $ps_2 . $ps_3 . '[university_logo]' . '"]' => array('value' => 'no'),
              ':input[name="' . $ps_2 . $ps_3 . '[university_logo]' . '"]' => array('value' => NULL),
            ),
          ),
        ),
      );
    }

    $form_state['conditionally_required'][0][] = 'presentation][designing_material_phone';


    // Fieldset: Services
    $ps_3 = 'services';
    if (!is_null($ps_2)) {
      $ps_3 .= ']';
    }

    $fields['services'] = array(
      '#id' => 'section-event-services',
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => "Event Services",
    );

    if (!isset($event_state['services'])) {
      $event_state['services'] = array();
    }
    $services_state = &$event_state['services'];
    $services_food_served = 'no';
    if (!empty($services_state['food_served'])) {
      $services_food_served = $services_state['food_served'];
    }

    $fields['services']['food_served'] = array(
      '#id' => 'field-event-services-food_served',
      '#type' => 'radios',
      '#title' => 'Will the Food or Drink be Served? <span class="title_display-note some_venues_dont_permit">(Note some venues do not permit food and drink)</span>',
      '#default_value' => $services_food_served,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'services][food_served';

    $services_food_caterer = '';
    if (array_key_exists('food_caterer', $services_state)) {
      $services_food_caterer = $services_state['food_caterer'];
    }

    $fields['services']['food_caterer'] = array(
      '#id' => 'field-event-services-food_caterer',
      '#type' => 'textfield',
      '#title' => 'Provide the Name of the Caterer/Busineess',
      '#default_value' => $services_food_caterer,
      '#size' => 48,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $ps_2 . $ps_3 . '[food_served]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[food_served]' . '"]' => array('value' => 'no'),
            ':input[name="' . $ps_2 . $ps_3 . '[food_served]' . '"]' => array('value' => NULL),
          ),
        ),
      ),
    );

    $services_alcohol = 'no';
    if (!empty($services_state['alcohol'])) {
      $services_alcohol = $services_state['alcohol'];
    }

    $fields['services']['alcohol'] = array(
      '#id' => 'field-event-services-alcohol',
      '#type' => 'radios',
      '#title' => 'Will Alcohol be Served?',
      '#default_value' => $services_alcohol,
      '#options' => array('yes' => "Yes", 'no' => "No"),
      '#required' => TRUE,
    );

    $form_state['conditionally_required'][0][] = 'services][alcohol';

    $fields['services']['alcohol_note'] = array(
      '#id' => 'field-event-services-alcohol_note',
      '#type' => 'container',
      '#states' => array(
        'visible' => array(
          ':input[name="' . $ps_2 . $ps_3 . '[alcohol]' . '"]' => array('value' => 'yes'),
        ),
        'invisible' => array(
          'or' => array(
            ':input[name="' . $ps_2 . $ps_3 . '[alcohol]' . '"]' => array('value' => 'no'),
            ':input[name="' . $ps_2 . $ps_3 . '[alcohol]' . '"]' => array('value' => NULL),
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

    if (!$meeting_event) {
      $services_open_flames = 'no';
      if (!empty($services_state['open_flames'])) {
        $services_open_flames = $services_state['open_flames'];
      }

      $fields['services']['open_flames'] = array(
        '#id' => 'field-event-services-open_flames',
        '#type' => 'radios',
        '#title' => 'Will Open Flames be Used? <span class="title_display-note examples">(Examples: Sternos, Grill, Candles)</span>',
        '#default_value' => $services_open_flames,
        '#options' => array('yes' => "Yes", 'no' => "No"),
        '#required' => TRUE,
      );
    }


    // remaining fields
    $event_additional = '';
    if (array_key_exists('additional', $event_state)) {
      $event_additional = $event_state['additional'];
    }

    $fields['additional'] = array(
      '#id' => 'field-event-additional',
      '#type' => 'textarea',
      '#title' => 'Please Provide any Other Information or Questions you may Have for Campus Officials',
      '#default_value' => $event_additional,
      '#rows' => 4,
    );

    $fields['message'] = array(
      '#markup' => '',
    );

    $fields['message']['#markup'] .= '<div id="markup-event-message" class="markup-event">';
    $fields['message']['#markup'] .= 'Once your request has been approved appropriate campus personnel, you will be emailed with the outcome. ';
    $fields['message']['#markup'] .= 'If the event receives approval, you will also receive an itemized detail of any and all charges that will be assessed to you. ';
    $fields['message']['#markup'] .= 'You must agree and accept the terms offered for the venue to be reserved. ';
    $fields['message']['#markup'] .= '</div>';
  }
  else {
    $fields['plans'] = array(
      '#tree' => TRUE,
    );

    if (!isset($event_state['plans'])) {
      $event_state['plans'] = array();
    }
    $plans_state = &$event_state['plans'];


    $fields['registration'] = array(
      '#tree' => TRUE,
    );

    if (!isset($event_state['registration'])) {
      $event_state['registration'] = array();
    }
    $registration_state = &$event_state['registration'];


    $fields['setup'] = array(
      '#tree' => TRUE,
    );

    if (!isset($event_state['setup'])) {
      $event_state['setup'] = array();
    }
    $setup_state = &$event_state['setup'];


    $fields['presentation'] = array(
      '#tree' => TRUE,
    );

    if (!isset($event_state['presentation'])) {
      $event_state['presentation'] = array();
    }
    $presentation_state = &$event_state['presentation'];


    $fields['services'] = array(
      '#tree' => TRUE,
    );

    if (!isset($event_state['services'])) {
      $event_state['services'] = array();
    }
    $services_state = &$event_state['services'];
  }


  // always make sure the plans values exist.
  $plans_keys = array(
   'description',
   'activities',
   'dates',
   'audience',
  );

  foreach ($plans_keys as $key) {
    if (!array_key_exists($key, $fields['plans'])) {
      $fields['plans'][$key] = array(
        '#id' => 'field-event-plans-' . $key,
        '#type' => 'value',
        '#default_value' => array_key_exists($key, $plans_state) ? $plans_state[$key] : NULL,
      );
    }
  }


  // always make sure the registration values exist.
  $registration_keys = array(
   'require',
   'website',
   'phone',
   'ticket_price',
   'ticket_dates',
   'ticket_website',
   'ticket_phone',
   'generate_revenue',
   'revenue_generated',
  );

  foreach ($registration_keys as $key) {
    if (!array_key_exists($key, $fields['registration'])) {
      $fields['registration'][$key] = array(
        '#id' => 'field-event-registration-' . $key,
        '#type' => 'value',
        '#default_value' => array_key_exists($key, $registration_state) ? $registration_state[$key] : NULL,
      );
    }
  }


  // always make sure the setup values exist.
  $setup_keys = array(
   'rectangular_tables_8ft',
   'round_tables_8ft',
   'other_tables',
   'standard_blue_chairs',
   'podium',
   'portable_stage',
   'portable_stage_configuration',
   'security',
   'parking_assistance',
   'parking_assistance_area',
   'road_closures',
   'road_closures_details',
   'special_requests',
   'special_requests_details',
  );

  foreach ($setup_keys as $key) {
    if (!array_key_exists($key, $fields['setup'])) {
      $fields['setup'][$key] = array(
        '#id' => 'field-event-setup-' . $key,
        '#type' => 'value',
        '#default_value' => array_key_exists($key, $setup_state) ? $setup_state[$key] : NULL,
      );
    }
  }


  // always make sure the presentation values exist.
  $presentation_keys = array(
   'technical_equipment',
   'technical_equipment_details',
   'technical_equipment_microphone',
   'production',
   'production_name',
   'production_email',
   'production_phone',
   'publicity',
   'publicity_details',
   'printed_material',
   'university_logo',
   'designing_material',
   'designing_material_name',
   'designing_material_email',
   'designing_material_phone',
  );

  foreach ($presentation_keys as $key) {
    if (!array_key_exists($key, $fields['presentation'])) {
      $fields['presentation'][$key] = array(
        '#id' => 'field-event-presentation-' . $key,
        '#type' => 'value',
        '#default_value' => array_key_exists($key, $presentation_state) ? $presentation_state[$key] : NULL,
      );
    }
  }


  // always make sure the services values exist.
  $services_keys = array(
   'food_served',
   'food_caterer',
   'alcohol',
   'open_flames',
  );

  foreach ($services_keys as $key) {
    if (!array_key_exists($key, $fields['services'])) {
      $fields['services'][$key] = array(
        '#id' => 'field-event-services-' . $key,
        '#type' => 'value',
        '#default_value' => array_key_exists($key, $services_state) ? $services_state[$key] : NULL,
      );
    }
  }

  return $fields;
}

/**
 * Returns an array containing classication information.
 *
 * @param array $state
 *   The form_state array containing the 'classsification' key and value.
 *
 * @return array
 *   An array with the following keys:
 *   - count: the number of classifications available for the current user.
 *   - options: all available select list options.
 *   - value: the default value to use.
 */
function event_workflow_determine_coordinator_classification($state) {
  global $user;

  $information = array();
  $information['count'] = 0;
  $information['options'] = array();
  $information['value'] = '';

  if (!is_array($state)) {
    cf_error:invalid_array('state');
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
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
