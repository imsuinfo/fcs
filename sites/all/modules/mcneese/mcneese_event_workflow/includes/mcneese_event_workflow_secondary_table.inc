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
function mcneese_event_workflow_secondary_table_structure() {
  mcneese_event_workflow_include(4);

  $structure = array();
  $structure['top'] = array(
    'status' => array(
      'path' => 'event][status',
    ),
    'step' => array(
      'path' => 'event][step',
    ),
    'location' => array(
      'path' => 'event][location',
    ),
    'room_list' => array(
      'path' => 'event][room_list',
      'storage' => 'alternate',
      'alternate' => array(
        'building' => array(
          'path' => 'event][room_list',
          'key' => 1,
        ),
        'room' => array(
          'path' => 'event][room_list',
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
    ),
  );

  $structure['review'] = array(
    'review' => array(
      'path' => 'event][review][review',
      'columns' => array('reviewer', 'decision', 'message'),
      'manual_processing' => TRUE,
    ),
    'facilities_use_fee' => array(
      'path' => 'event][review][facilities_use_fee',
      'manual_processing' => TRUE,
    ),
    'equipment_use_fee' => array(
      'path' => 'event][review][equipment_use_fee',
      'manual_processing' => TRUE,
    ),
  );

  $structure['dates'] = array(
    'date' => array(
      'path' => 'event][dates][date',
      'storage' => 'date_popup',
      'date_popup' => array('view' => 'Y/m/d'),
    ),
    'time_start' => array(
      'path' => 'event][dates][time_start',
      'storage' => 'timestamp',
      'timestamp' => array('view' => 'h:ia', 'relative_to' => 'event][dates][date'),
    ),
    'time_stop' => array(
      'path' => 'event][dates][time_stop',
      'storage' => 'timestamp',
      'timestamp' => array('view' => 'h:ia', 'relative_to' => 'event][dates][date'),
    ),
  );

  $structure['coordinator'] = array(
    'name' => array(
      'path' => 'event][coordinator][name',
    ),
    'phone' => array(
      'path' => 'event][coordinator][phone',
    ),
    'email' => array(
      'path' => 'event][coordinator][email',
    ),
    'classification' => array(
      'path' => 'event][coordinator][classification',
    ),
  );

  $structure['information'] = array(
    'title' => array(
      'path' => 'event][information][title',
    ),
    'attendance' => array(
      'path' => 'event][information][attendance',
    ),
    'type' => array(
      'path' => 'event][information][type',
    ),
  );

  $structure['plans'] = array(
    'audience' => array(
      'path' => 'event][plans][audience',
    ),
    'description' => array(
      'path' => 'event][plans][description',
    ),
    'activities' => array(
      'path' => 'event][plans][activities',
    ),
    'dates' => array(
      'path' => 'event][plans][dates',
    ),
  );

  $structure['registration'] = array(
    'require' => array(
      'path' => 'event][registration][require',
    ),
    'website' => array(
      'path' => 'event][registration][website',
    ),
    'phone' => array(
      'path' => 'event][registration][phone',
    ),
    'ticket_price' => array(
      'path' => 'event][registration][ticket_price',
    ),
    'ticket_dates' => array(
      'path' => 'event][registration][ticket_dates',
    ),
    'ticket_website' => array(
      'path' => 'event][registration][ticket_website',
    ),
    'ticket_phone' => array(
      'path' => 'event][registration][ticket_phone',
    ),
    'generate_revenue' => array(
      'path' => 'event][registration][generate_revenue',
    ),
    'revenue_generated' => array(
      'path' => 'event][registration][revenue_generated',
      'columns' => event_workflow_get_revenue_generated_list_options('column_name'),
    ),
  );

  $structure['setup'] = array(
    'rectangular_tables_8ft' => array(
      'path' => 'event][setup][rectangular_tables_8ft',
    ),
    'round_tables_8ft' => array(
      'path' => 'event][setup][round_tables_8ft',
    ),
    'other_tables' => array(
      'path' => 'event][setup][other_tables',
    ),
    'standard_blue_chairs' => array(
      'path' => 'event][setup][standard_blue_chairs',
    ),
    'podium' => array(
      'path' => 'event][setup][podium',
    ),
    'portable_stage' => array(
      'path' => 'event][setup][portable_stage',
    ),
    'portable_stage_configuration' => array(
      'path' => 'event][setup][portable_stage_configuration',
    ),
    'security' => array(
      'path' => 'event][setup][security',
    ),
    'parking_assistance' => array(
      'path' => 'event][setup][parking_assistance',
    ),
    'parking_assistance_area' => array(
      'path' => 'event][setup][parking_assistance_area',
    ),
    'road_closures' => array(
      'path' => 'event][setup][road_closures',
    ),
    'road_closures_details' => array(
      'path' => 'event][setup][road_closures_details',
    ),
    'special_requests' => array(
      'path' => 'event][setup][special_requests',
    ),
    'special_requests_details' => array(
      'path' => 'event][setup][special_requests_details',
    ),
  );

  $structure['presentation'] = array(
    'technical_equipment' => array(
      'path' => 'event][presentation][technical_equipment',
    ),
    'technical_equipment_details' => array(
      'path' => 'event][presentation][technical_equipment_details',
      'columns' => event_workflow_get_technical_equipment_details_list_options('column_name'),
    ),
    'technical_equipment_microphone' => array(
      'path' => 'event][presentation][technical_equipment_microphone',
    ),
    'external_audio_person' => array(
      'path' => 'event][presentation][external_audio_person',
    ),
    'external_audio_person_name' => array(
      'path' => 'event][presentation][external_audio_person_name',
    ),
    'external_audio_person_email' => array(
      'path' => 'event][presentation][external_audio_person_email',
    ),
    'external_audio_person_phone' =>  array(
      'path' =>'event][presentation][external_audio_person_phone',
    ),
    'publicity' => array(
      'path' => 'event][presentation][publicity',
    ),
    'publicity_details' => array(
      'path' => 'event][presentation][publicity_details',
      'columns' => event_workflow_get_publicity_details_list_options('column_name'),
    ),
    'production' => array(
      'path' => 'event][presentation][production',
    ),
    'production_name' => array(
      'path' => 'event][presentation][production_name',
    ),
    'production_email' => array(
      'path' => 'event][presentation][production_email',
    ),
    'production_phone' => array(
      'path' => 'event][presentation][production_phone',
    ),
    'printed_material' => array(
      'path' => 'event][presentation][printed_material',
    ),
    'university_logo' => array(
      'path' => 'event][presentation][university_logo',
    ),
    'designing_material' => array(
      'path' => 'event][presentation][designing_material',
      'columns' => event_workflow_get_designing_material_list_options('column_name'),
    ),
    'designing_material_name' => array(
      'path' => 'event][presentation][designing_material_name',
    ),
    'designing_material_email' => array(
      'path' => 'event][presentation][designing_material_email',
    ),
    'designing_material_phone' => array(
      'path' => 'event][presentation][designing_material_phone',
    ),
  );

  $structure['services'] = array(
    'food_served' => array(
      'path' => 'event][services][food_served',
    ),
    'food_caterer' => array(
      'path' => 'event][services][food_caterer',
    ),
    'alcohol' => array(
      'path' => 'event][services][alcohol',
    ),
    'open_flames' => array(
      'path' => 'event][services][open_flames',
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
function mcneese_event_workflow_secondary_table_presentation($option = NULL) {
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

      $presentation['fees'] = array(
        'weight' => 1,
        'group' => 4,
        'label' => "Fees",
        'editable' => FALSE,
      );

      $presentation['top'] = array(
        'weight' => 0,
        'group' => 5,
        'label' => "Miscellaneous",
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
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'additional' => array(
      'label' => 'Additional Information or Questions to Campus Officials',
      'size' => 'large',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
  );

  $presentation['review'] = array(
    'facilities_use_fee' => array(
      'label' => 'Facilities Use',
      'group' => 'fees',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'equipment_use_fee' => array(
      'label' => 'Equipment Use',
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

  $presentation['coordinator'] = array(
    'name' => array(
      'label' => 'Coordinator Name',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'phone' => array(
      'label' => 'Coordinator Phone',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'email' => array(
      'label' => 'Coordinator E-Mail',
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'classification' => array(
      'label' => 'Coordinator Classification',
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
      'weight' => 6,
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
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'portable_stage' => array(
      'label' => 'Portable Stage is Required',
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
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'parking_assistance' => array(
      'label' => 'Parking Assistance or Restrictions are Required',
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
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'production_name' => array(
      'label' => 'Production Company or Professional Promoter Name',
      'size' => 'large',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'production_email' => array(
      'label' => 'Production Company or Professional Promoter E-mail',
      'size' => 'large',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'production_phone' => array(
      'label' => 'Production Company or Professional Promoter Phone',
      'size' => 'large',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'printed_material' => array(
      'label' => 'Printed Material is Required',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
    'university_logo' => array(
      'label' => 'Printed Material Uses University Logo',
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
      'event_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'open_flames' => array(
      'label' => 'Open Flames are Being Used',
      'event_type' => array(
        'show' => TRUE,
        'hide' => array(57),
      ),
    ),
  );

  return $presentation;
}

/**
 * @} End of '@addtogroup mcneese_event_workflow'.
 */
