<?php

/**
 * @file
 * Defines McNeese event workflow secondary table functions.
 */

/**
 * @addtogroup mcneese_event_workflow
 * @{
 */

/**
 * Returns a list of secondary tables and their structure.
 *
 * @return array
 *  A list of secondary tables and their structure.
 */
function mcneese_event_workflow_table_structure() {
  mcneese_event_workflow_include(4);

  $structure = array();
  $structure['top'] = array(
    'status' => array(
      'path' => 'event][status',
      'name' => "Event Status",
      'type' => 'options',
    ),
    'step' => array(
      'path' => 'event][step',
      'name' => "Event Step",
      'type' => 'options',
    ),
    'location' => array(
      'path' => 'event][location',
      'name' => "Event Location",
      'type' => 'options',
    ),
    'room' => array(
      'path' => 'event][room',
      'name' => "Event Room",
      'type' => 'options',
      'storage' => 'alternate',
      'alternate' => array(
        'building' => array(
          'path' => 'event][room',
          'key' => 1,
        ),
        'room' => array(
          'path' => 'event][room',
          'key' => 2,
        ),
      ),
      'alternate_exception' => array(
        'location' => array(
          'exception_path' => 'top][location',
          'path' => 'event][location',
          'key' => 0,
        ),
      ),
      'alternate_method' => array(
        'explode' => array('_', 3),
      ),
    ),
    'additional' => array(
      'path' => 'event][additional',
      'name' => "Additional Information",
      'type' => 'text',
    ),
  );

  $structure['review'] = array(
    'review' => array(
      'path' => 'event][review][review',
      'name' => "Review Log",
      'type' => 'review_log',
      'columns' => array('reviewer', 'step', 'decision', 'message'),
      'manual_processing' => TRUE,
    ),
  );

  $structure['use_fees'] = array(
    'facilities' => array(
      'path' => 'event][use_fees][facilities',
      'name' => "Facilities Use Fees",
      'type' => 'use_fees',
      'manual_processing' => TRUE,
      'columns' => array('hours', 'days', 'amount'),
      'storage' => 'currency',
    ),
    'equipment' => array(
      'path' => 'event][use_fees][equipment',
      'name' => "Equipment Use Fees",
      'type' => 'use_fees',
      'manual_processing' => TRUE,
      'columns' => array('hours', 'days', 'amount'),
      'storage' => 'currency',
    ),
    'custodial' => array(
      'path' => 'event][use_fees][custodial',
      'name' => "Custodial Use Fees",
      'type' => 'use_fees',
      'manual_processing' => TRUE,
      'columns' => array('hours', 'days', 'amount'),
      'storage' => 'currency',
    ),
    'security' => array(
      'path' => 'event][use_fees][security',
      'name' => "Security Use Fees",
      'type' => 'use_fees',
      'manual_processing' => TRUE,
      'columns' => array('hours', 'days', 'amount'),
      'storage' => 'currency',
    ),
    'other' => array(
      'name' => "Other Use Fees",
      'path' => 'event][use_fees][other',
      'type' => 'bool',
      'manual_processing' => TRUE,
    ),
    'waved' => array(
      'name' => "Use Fees Waved (with Presidents Approval)",
      'path' => 'event][use_fees][waved',
      'type' => 'bool',
      'manual_processing' => TRUE,
    ),
    'university' => array(
      'name' => "University Use Fees",
      'type' => 'bool',
      'path' => 'event][use_fees][university',
      'manual_processing' => TRUE,
    ),
  );

  $structure['insurance'] = array(
    'contractor' => array(
      'path' => 'event][insurance][contractor',
      'name' => "Contractor Insurance",
      'type' => 'bool',
      'manual_processing' => TRUE,
    ),
    'unaffiliated' => array(
      'name' => "Uniffiliated Insurance",
      'path' => 'event][insurance][unaffiliated',
      'type' => 'bool',
      'manual_processing' => TRUE,
    ),
  );

  $structure['dates'] = array(
    'date' => array(
      'path' => 'event][dates][date',
      'name' => "Date",
      'type' => 'date',
      'storage' => 'date_popup',
      'date_popup' => array('view' => 'Y/m/d'),
      'multiple' => TRUE,
    ),
    'time_start' => array(
      'path' => 'event][dates][time_start',
      'name' => "Start Time",
      'type' => 'time',
      'storage' => 'timestamp',
      'timestamp' => array('view' => 'h:ia', 'relative_to' => 'event][dates][date'),
      'multiple' => TRUE,
    ),
    'time_stop' => array(
      'path' => 'event][dates][time_stop',
      'name' => "Stop Time",
      'type' => 'time',
      'storage' => 'timestamp',
      'timestamp' => array('view' => 'h:ia', 'relative_to' => 'event][dates][date'),
      'multiple' => TRUE,
    ),
  );

  $structure['venue_coordinator'] = array(
    'user_id' => array(
      'path' => 'event][venue_coordinator][user_id',
      'name' => "Venue Coordinator",
      'type' => 'venue_coordinator',
    ),
  );

  $structure['event_coordinator'] = array(
    'name' => array(
      'path' => 'event][event_coordinator][name',
      'name' => "Event Coordinator Name",
      'type' => 'text',
    ),
    'phone' => array(
      'path' => 'event][event_coordinator][phone',
      'name' => "Event Coordinator Phone",
      'type' => 'text',
    ),
    'email' => array(
      'path' => 'event][event_coordinator][email',
      'name' => "Event Coordinator E-mail",
      'type' => 'text',
    ),
    'classification' => array(
      'path' => 'event][event_coordinator][classification',
      'name' => "Event Coordinator Classification",
      'type' => 'option',
    ),
  );

  $structure['information'] = array(
    'title' => array(
      'path' => 'event][information][title',
      'name' => "Event Title",
      'type' => 'text',
    ),
    'attendance' => array(
      'path' => 'event][information][attendance',
      'name' => "Expected Attendance",
      'type' => 'text',
    ),
    'type' => array(
      'path' => 'event][information][type',
      'name' => "Event Type",
      'type' => 'options',
    ),
    'organization' => array(
      'path' => 'event][information][organization',
      'name' => "Organization / Affiliation",
      'type' => 'text',
    ),
  );

  $structure['plans'] = array(
    'audience' => array(
      'path' => 'event][plans][audience',
      'name' => "Intended Audience",
      'type' => 'options',
    ),
    'description' => array(
      'path' => 'event][plans][description',
      'name' => "Description / Purpose",
      'type' => 'text',
    ),
    'activities' => array(
      'path' => 'event][plans][activities',
      'name' => "Planned Activities",
      'type' => 'text',
    ),
    'dates' => array(
      'path' => 'event][plans][dates',
      'name' => "Additional Dates and Times",
      'type' => 'text',
    ),
  );

  $structure['registration'] = array(
    'require' => array(
      'path' => 'event][registration][require',
      'name' => "Registration or Ticket Purchase",
      'type' => 'bool',
    ),
    'website' => array(
      'path' => 'event][registration][website',
      'name' => "Registration Website",
      'type' => 'text',
    ),
    'phone' => array(
      'path' => 'event][registration][phone',
      'name' => "Registration Phone",
      'type' => 'text',
    ),
    'ticket_price' => array(
      'path' => 'event][registration][ticket_price',
      'name' => "Ticket Prices",
      'type' => 'text',
    ),
    'ticket_dates' => array(
      'path' => 'event][registration][ticket_dates',
      'name' => "Ticket Dates",
      'type' => 'text',
    ),
    'ticket_website' => array(
      'path' => 'event][registration][ticket_website',
      'name' => "Ticket Website",
      'type' => 'text',
    ),
    'ticket_phone' => array(
      'path' => 'event][registration][ticket_phone',
      'name' => "Ticket Phone",
      'type' => 'text',
    ),
    'generate_revenue' => array(
      'path' => 'event][registration][generate_revenue',
      'name' => "Generate Revenue",
      'type' => 'bool',
    ),
    'revenue_generated' => array(
      'path' => 'event][registration][revenue_generated',
      'columns' => event_workflow_get_revenue_generated_list_options('column_name'),
      'name' => "How Revenue is Generated",
      'type' => 'checkboxes',
    ),
  );

  $structure['setup'] = array(
    'rectangular_tables_8ft' => array(
      'path' => 'event][setup][rectangular_tables_8ft',
      'name' => "8ft Rectangular Tables",
      'type' => 'number',
    ),
    'round_tables_8ft' => array(
      'path' => 'event][setup][round_tables_8ft',
      'name' => "8ft Round Tables",
      'type' => 'number',
    ),
    'other_tables' => array(
      'path' => 'event][setup][other_tables',
      'name' => "Other Tables",
      'type' => 'text',
    ),
    'standard_blue_chairs' => array(
      'path' => 'event][setup][standard_blue_chairs',
      'name' => "Standard Blue Chairs",
      'type' => 'number',
    ),
    'podium' => array(
      'path' => 'event][setup][podium',
      'name' => "Podium Needed",
      'type' => 'bool',
    ),
    'portable_stage' => array(
      'path' => 'event][setup][portable_stage',
      'name' => "Portable Stage Needed",
      'type' => 'bool',
    ),
    'portable_stage_configuration' => array(
      'path' => 'event][setup][portable_stage_configuration',
      'name' => "Portable Stage Configuration",
      'type' => 'text',
    ),
    'security' => array(
      'path' => 'event][setup][security',
      'name' => "Security Needed",
      'type' => 'bool',
    ),
    'parking_assistance' => array(
      'path' => 'event][setup][parking_assistance',
      'name' => "Parking Assistance Needed",
      'type' => 'bool',
    ),
    'parking_assistance_area' => array(
      'path' => 'event][setup][parking_assistance_area',
      'name' => "Parking Assistance Area",
      'type' => 'text',
    ),
    'road_closures' => array(
      'path' => 'event][setup][road_closures',
      'name' => "Road Closures or Barricades",
      'type' => 'bool',
    ),
    'road_closures_details' => array(
      'path' => 'event][setup][road_closures_details',
      'name' => "Road Closure Details",
      'type' => 'text',
    ),
    'special_requests' => array(
      'path' => 'event][setup][special_requests',
      'name' => "Special Requests",
      'type' => 'bool',
    ),
    'special_requests_details' => array(
      'path' => 'event][setup][special_requests_details',
      'name' => "Special Request Details",
      'type' => 'text',
    ),
  );

  $structure['presentation'] = array(
    'technical_equipment' => array(
      'path' => 'event][presentation][technical_equipment',
      'name' => "Need Technical Equipment",
      'type' => 'bool',
    ),
    'technical_equipment_details' => array(
      'path' => 'event][presentation][technical_equipment_details',
      'columns' => event_workflow_get_technical_equipment_details_list_options('column_name'),
      'name' => "Technical Equipment Details",
      'type' => 'checkboxes',
    ),
    'technical_equipment_microphone' => array(
      'path' => 'event][presentation][technical_equipment_microphone',
      'name' => "Quantity of Microphones",
      'type' => 'text',
    ),
    'external_audio_person' => array(
      'path' => 'event][presentation][external_audio_person',
      'name' => "External Audio Person/Company",
      'type' => 'bool',
    ),
    'external_audio_person_name' => array(
      'path' => 'event][presentation][external_audio_person_name',
      'name' => "Name of Audio Person/Company",
      'type' => 'text',
    ),
    'external_audio_person_email' => array(
      'path' => 'event][presentation][external_audio_person_email',
      'name' => "E-Mail of Audio Person/Company",
      'type' => 'text',
    ),
    'external_audio_person_phone' =>  array(
      'path' =>'event][presentation][external_audio_person_phone',
      'name' => "Phone of Audio Person/Company",
      'type' => 'text',
    ),
    'publicity' => array(
      'path' => 'event][presentation][publicity',
      'name' => "Need Publicity",
      'type' => 'bool',
    ),
    'publicity_details' => array(
      'path' => 'event][presentation][publicity_details',
      'columns' => event_workflow_get_publicity_details_list_options('column_name'),
      'name' => "Publicity Methods",
      'type' => 'checkboxes',
    ),
    'production' => array(
      'path' => 'event][presentation][production',
      'name' => "Using Production Company/Promoter",
      'type' => 'bool',
    ),
    'production_name' => array(
      'path' => 'event][presentation][production_name',
      'name' => "Name of Production Company/Promoter",
      'type' => 'text',
    ),
    'production_email' => array(
      'path' => 'event][presentation][production_email',
      'name' => "E-Mail of Production Company/Promoter",
      'type' => 'text',
    ),
    'production_phone' => array(
      'path' => 'event][presentation][production_phone',
      'name' => "Phone of Production Company/Promoter",
      'type' => 'text',
    ),
    'printed_material' => array(
      'path' => 'event][presentation][printed_material',
      'name' => "Using Printed Material",
      'type' => 'bool',
    ),
    'university_logo' => array(
      'path' => 'event][presentation][university_logo',
      'name' => "Using University Logo",
      'type' => 'bool',
    ),
    'designing_material' => array(
      'path' => 'event][presentation][designing_material',
      'columns' => event_workflow_get_designing_material_list_options('column_name'),
      'name' => "Material Designer",
      'type' => 'checkboxes',
    ),
    'designing_material_name' => array(
      'path' => 'event][presentation][designing_material_name',
      'name' => "Material Designer Name",
      'type' => 'text',
    ),
    'designing_material_email' => array(
      'path' => 'event][presentation][designing_material_email',
      'name' => "Material Designer E-Mail",
      'type' => 'text',
    ),
    'designing_material_phone' => array(
      'path' => 'event][presentation][designing_material_phone',
      'name' => "Material Designer Phone",
      'type' => 'text',
    ),
  );

  $structure['services'] = array(
    'food_served' => array(
      'path' => 'event][services][food_served',
      'name' => "Food Served",
      'type' => 'bool',
    ),
    'food_caterer' => array(
      'path' => 'event][services][food_caterer',
      'name' => "Food Caterer",
      'type' => 'text',
    ),
    'alcohol' => array(
      'path' => 'event][services][alcohol',
      'name' => "Alcohol Served",
      'type' => 'bool',
    ),
    'open_flames' => array(
      'path' => 'event][services][open_flames',
      'name' => "Open Flames Used",
      'type' => 'bool',
    ),
  );

  return $structure;
}

/**
 * Returns a list of secondary tables and their presentation options.
 *
 * @params int|null $option
 *   (optional) If NULL, present the table presentation structure of all fields
 *   and nested within their appropriate group.
 *   1: Provide group presentation options.
 *
 * @return array
 *  A list of secondary tables and their presentation options.
 */
function mcneese_event_workflow_table_presentation($option = NULL) {
  $presentation = array();

  if (!is_null($option)) {
    if ($option == 1) {
      $presentation['request'] = array(
        'weight' => 0,
        'group' => 0,
        'label' => "Request",
      );

      $presentation['dates'] = array(
        'weight' => 1,
        'group' => 0,
        'label' => "Dates",
      );

      $presentation['information'] = array(
        'weight' => 0,
        'group' => 1,
        'label' => "Information",
      );

      $presentation['coordinator'] = array(
        'weight' => 1,
        'group' => 1,
        'label' => "Coordinator",
      );

      $presentation['plans'] = array(
        'weight' => 0,
        'group' => 2,
        'label' => "Plans",
      );

      $presentation['registration'] = array(
        'weight' => 1,
        'group' => 2,
        'label' => "Registration",
        'event_type' => array(
          'hide_link' => array(57),
        ),
      );

      $presentation['setup'] = array(
        'weight' => 0,
        'group' => 3,
        'label' => "Setup",
      );

      $presentation['presentation'] = array(
        'weight' => 1,
        'group' => 3,
        'label' => "Presentation",
      );

      $presentation['services'] = array(
        'weight' => 0,
        'group' => 4,
        'label' => "Services",
      );

      $presentation['top'] = array(
        'weight' => 1,
        'group' => 4,
        'label' => "Miscellaneous",
      );

      $presentation['fees'] = array(
        'weight' => 0,
        'group' => 5,
        'label' => "Fees",
        'editable' => FALSE,
      );

      $presentation['reviews'] = array(
        'weight' => 1,
        'group' => 5,
        'label' => "Reviews",
        'editable' => FALSE,
      );
    }

    return $presentation;
  }

  $presentation['mer'] = array(
    'id' => array(
      'label' => 'Request ID',
      'group' => 'request',
      'weight' => 0,
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'created' => array(
      'label' => 'Created On',
      'group' => 'request',
      'weight' => 8,
      'type' => array(
        'value' => 'timestamp',
      ),
      'display' => array(
        'value' => 'Y/m/d h:ia',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'user_id' => array(
      'label' => 'Created By',
      'group' => 'request',
      'weight' => 7,
      'type' => array(
        'value' => 'user_id',
      ),
      'display' => array(
        'value' => 'account_name',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'updated' => array(
      'label' => 'Updated On',
      'group' => 'request',
      'weight' => 9,
      'type' => array(
        'value' => 'timestamp',
      ),
      'display' => array(
        'value' => 'Y/m/d h:ia',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
  );

  $presentation['top'] = array(
    'status' => array(
      'label' => 'Status',
      'group' => 'request',
      'weight' => 10,
      'type' => array(
        'value' => 'status',
      ),
      'display' => array(
        'value' => NULL,
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'step' => array(
      'label' => 'Step',
      'group' => 'request',
      'weight' => 11,
      'type' => array(
        'value' => 'step',
      ),
      'display' => array(
        'value' => NULL,
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'location' => array(
      'label' => 'Location',
      'group' => 'request',
      'weight' => 100,
      'type' => array(
        'value' => 'location',
      ),
      'display' => array(
        'value' => NULL,
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'building' => array(
      'label' => 'Building',
      'group' => 'request',
      'weight' => 101,
      'type' => array(
        'value' => 'building',
      ),
      'display' => array(
        'value' => NULL,
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'room' => array(
      'label' => 'Room',
      'group' => 'request',
      'weight' => 102,
      'type' => array(
        'value' => 'room',
      ),
      'display' => array(
        'value' => NULL,
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'additional' => array(
      'label' => 'Additional Information or Questions to Campus Officials',
      'size' => 'small',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
  );

  $presentation['use_fees'] = array(
    'facilities' => array(
      'label' => 'Facilities Use',
      'combined' => TRUE,
      'value_names' => array(
        'days',
        'amount',
      ),
      'type' => array(
        'days' => 'use_fee_days',
        'amount' => 'use_fee_amount',
       ),
      'group' => 'fees',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'equipment' => array(
      'label' => 'Additional Equipment Use',
      'combined' => TRUE,
      'value_names' => array(
        'days',
        'amount',
      ),
      'type' => array(
        'days' => 'use_fee_days',
        'amount' => 'use_fee_amount',
       ),
      'group' => 'fees',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'custodial' => array(
      'label' => 'Custodial Use',
      'combined' => TRUE,
      'value_names' => array(
        'quantity',
        'hours',
        'days',
        'amount',
      ),
      'type' => array(
        'quantity' => 'use_fee_personnel',
        'hours' => 'use_fee_hours',
        'days' => 'use_fee_days',
        'amount' => 'use_fee_amount',
      ),
      'group' => 'fees',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'security' => array(
      'label' => 'Security Use',
      'combined' => TRUE,
      'value_names' => array(
        'quantity',
        'hours',
        'days',
        'amount',
      ),
      'type' => array(
        'quantity' => 'use_fee_personnel',
        'hours' => 'use_fee_hours',
        'days' => 'use_fee_days',
        'amount' => 'use_fee_amount',
      ),
      'group' => 'fees',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'other' => array(
      'label' => 'Other Use',
      'type' => array(
        'value' => 'boolean',
      ),
      'display' => array(
        'value' => 'when_true',
      ),
      'group' => 'fees',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'waved' => array(
      'label' => 'Fees Waved',
      'type' => array(
        'value' => 'boolean',
      ),
      'display' => array(
        'value' => 'when_true',
      ),
      'group' => 'fees',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'university' => array(
      'label' => 'Strictly University Business',
      'type' => array(
        'value' => 'boolean',
      ),
      'display' => array(
        'value' => 'when_true',
      ),
      'group' => 'fees',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
  );

  $presentation['insurance'] = array(
    'contractor' => array(
      'label' => 'Contractor Insurance',
      'type' => array(
        'value' => 'boolean',
      ),
      'display' => array(
        'value' => 'when_true',
      ),
      'group' => 'fees',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'unaffiliated' => array(
      'label' => 'Unaffiliated Insurance',
      'type' => array(
        'value' => 'boolean',
      ),
      'display' => array(
        'value' => 'when_true',
      ),
      'group' => 'fees',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
  );

  $presentation['dates'] = array(
    'date' => array(
      'label' => 'Date',
      'type' => array(
        'value' => 'timestamp',
      ),
      'display' => array(
        'value' => 'Y/m/d',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
      'collate' => 'date',
      'collate_order' => 0,
    ),
    'time_start' => array(
      'label' => 'Start Time',
      'type' => array(
        'value' => 'timestamp',
      ),
      'display' => array(
        'value' => 'h:ia',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
      'collate' => 'date',
      'collate_order' => 1,
      'prefix' => ', ',
    ),
    'time_stop' => array(
      'label' => 'Stop Time',
      'type' => array(
        'value' => 'timestamp',
      ),
      'display' => array(
        'value' => 'h:ia',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
      'collate' => 'date',
      'collate_order' => 2,
      'prefix' => ' to ',
    ),
  );

  $presentation['venue_coordinator'] = array(
    'user_id' => array(
      'split' => array(
        'name' => 'venue_coordinator',
        'phone' => 'venue_coordinator',
        'email' => 'venue_coordinator',
      ),
    ),
    'name' => array(
      'label' => 'Venue Coordinator Name',
      'group' => 'coordinator',
      'weight' => 1,
      'type' => array(
        'value' => 'venue_coordinator',
      ),
      'display' => array(
        'value' => 'name',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'phone' => array(
      'label' => 'Venue Coordinator Phone',
      'group' => 'coordinator',
      'weight' => 2,
      'type' => array(
        'value' => 'venue_coordinator',
      ),
      'display' => array(
        'value' => 'phone',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'email' => array(
      'label' => 'Venue Coordinator E-Mail',
      'group' => 'coordinator',
      'weight' => 3,
      'type' => array(
        'value' => 'venue_coordinator',
      ),
      'display' => array(
        'value' => 'email',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
  );

  $presentation['event_coordinator'] = array(
    'name' => array(
      'label' => 'Event Coordinator Name',
      'group' => 'coordinator',
      'weight' => 4,
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'phone' => array(
      'label' => 'Event Coordinator Phone',
      'group' => 'coordinator',
      'weight' => 5,
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'email' => array(
      'label' => 'Event Coordinator E-Mail',
      'group' => 'coordinator',
      'weight' => 6,
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'classification' => array(
      'label' => 'Event Coordinator Classification',
      'group' => 'coordinator',
      'weight' => 7,
      'type' => array(
        'value' => 'taxonomy',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
  );

  $presentation['information'] = array(
    'title' => array(
      'label' => 'Request Title',
      'weight' => 1,
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'attendance' => array(
      'label' => 'Expected Attendance',
      'weight' => 3,
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'type' => array(
      'label' => 'Request Type',
      'weight' => 2,
      'type' => array(
        'value' => 'taxonomy',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'organization' => array(
      'label' => 'Organization / Affiliation',
      'weight' => 4,
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
  );

  $presentation['plans'] = array(
    'audience' => array(
      'label' => 'Intended Audience',
      'type' => array(
        'value' => 'taxonomy',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'description' => array(
      'label' => 'Event Description/Purpose',
      'size' => 'large',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'activities' => array(
      'label' => 'Planned Activities',
      'size' => 'large',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'dates' => array(
      'label' => 'Dates and Times',
      'size' => 'large',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
  );

  $presentation['registration'] = array(
    'require' => array(
      'label' => 'Registration or Ticket Purchase is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'website' => array(
      'label' => 'Registration Website',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'phone' => array(
      'label' => 'Registration Phone',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'ticket_price' => array(
      'label' => 'Ticket Price',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'ticket_dates' => array(
      'label' => 'Ticket Dates',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'ticket_website' => array(
      'label' => 'Ticket Website',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'ticket_phone' => array(
      'label' => 'Ticket Phone',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'generate_revenue' => array(
      'label' => 'Revenue is Generated',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'revenue_generated' => array(
      'label' => 'How Revenue is Generated',
      'value_names' => array(
        'ticket',
        'merchandise',
        'concession',
      ),
      'type' => array(
        'ticket' => 'taxonomy',
        'merchandise' => 'taxonomy',
        'concession' => 'taxonomy',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
  );

  $presentation['setup'] = array(
    'rectangular_tables_8ft' => array(
      'label' => 'Quantity of 8ft Rectangular Tables',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'round_tables_8ft' => array(
      'label' => 'Quantity of 8ft Round Tables',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'other_tables' => array(
      'label' => 'Description and Quantity of Other Tables',
      'size' => 'large',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'standard_blue_chairs' => array(
      'label' => 'Quantity of Blue Chairs',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'podium' => array(
      'label' => 'Podium is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'portable_stage' => array(
      'label' => 'Portable Stage is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'portable_stage_configuration' => array(
      'label' => 'Portable Stage Configuration',
      'size' => 'large',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'security' => array(
      'label' => 'Security is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'parking_assistance' => array(
      'label' => 'Parking Assistance or Restrictions are Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'parking_assistance_area' => array(
      'label' => 'Parking Assistance or Restriction Explanation',
      'size' => 'large',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'road_closures' => array(
      'label' => 'Road Closures or Barricades are Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'road_closures_details' => array(
      'label' => 'Road Closures or Barricades Explanation',
      'size' => 'large',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'special_requests' => array(
      'label' => 'Special Requests are Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'special_requests_details' => array(
      'label' => 'Special Requests Explanation',
      'size' => 'large',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
  );

  $presentation['presentation'] = array(
    'technical_equipment' => array(
      'label' => 'Technical Equipment is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'technical_equipment_details' => array(
      'label' => 'Technical Equipment Explanation',
      'value_names' => array(
        'microphone',
        'screen',
        'computer',
        'sound',
      ),
      'type' => array(
        'microphone' => 'taxonomy',
        'screen' => 'taxonomy',
        'computer' => 'taxonomy',
        'sound' => 'taxonomy',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'technical_equipment_microphone' => array(
      'label' => 'Quantity of Microphones',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'external_audio_person' => array(
      'label' => 'External Audio/Video Person/Company is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'external_audio_person_name' => array(
      'label' => 'Name of Person/Company',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'external_audio_person_email' => array(
      'label' => 'E-mail of Person/Company',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'external_audio_person_phone' => array(
      'label' => 'Phone of Person/Company',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'publicity' => array(
      'label' => 'Publicity is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'publicity_details' => array(
      'label' => 'Publicity Methods',
      'value_names' => array(
        'campus_digest',
        'student_digest',
        'website',
        'social_media',
        'axis_tv',
        'press_release',
      ),
      'type' => array(
        'campus_digest' => 'taxonomy',
        'student_digest' => 'taxonomy',
        'website' => 'taxonomy',
        'social_media' => 'taxonomy',
        'axis_tv' => 'taxonomy',
        'press_release' => 'taxonomy',
       ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'production' => array(
      'label' => 'Production Company or Professional Promoter is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'production_name' => array(
      'label' => 'Production Company or Professional Promoter Name',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'production_email' => array(
      'label' => 'Production Company or Professional Promoter E-mail',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'production_phone' => array(
      'label' => 'Production Company or Professional Promoter Phone',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'printed_material' => array(
      'label' => 'Printed Material is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'university_logo' => array(
      'label' => 'Printed Material Uses University Logo',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'designing_material' => array(
      'label' => 'Printed Material Designer',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'designing_material_name' => array(
      'label' => 'Name of Printer Material Designer',
      'value_names' => array(
        'public_relations',
        'marketing',
        'other',
      ),
      'type' => array(
        'public_relations' => 'taxonomy',
        'marketing' => 'taxonomy',
        'other' => 'taxonomy',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'designing_material_email' => array(
      'label' => 'E-mail of Printed Material Designer',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'designing_material_phone' => array(
      'label' => 'Phone of Printed Material Designer',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
  );

  $presentation['services'] = array(
    'food_served' => array(
      'label' => 'Food or Drink is Being Served',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'food_caterer' => array(
      'label' => 'Name of the Caterer/Business',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'alcohol' => array(
      'label' => 'Alcohol is Being Served',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'open_flames' => array(
      'label' => 'Open Flames are Being Used',
      'type' => array(
        'value' => 'boolean',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
  );

  $presentation['reviews'] = array(
    'decision' => array(
      'label' => 'Decision',
      'value_names' => array(
        'decision'
      ),
      'type' => array(
        'decision' => 'review_decision',
      ),
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
  );

  return $presentation;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
