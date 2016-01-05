<?php

/**
 * @file
 * Defines McNeese facilities use secondary table functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Returns a list of secondary tables and their structure.
 *
 * @param bool $include_mer
 *   (optional) Provided as a means to allow for utilizing the mfcs_requests
 *   table that is generally referred to as mer.
 *   This is optional to prevent problems with code not expecting the mer
 *   expecting the mer table to exist can continue to function without
 *   further alterations to the code.
 *
 * @return array
 *  A list of secondary tables and their structure.
 */
function mfcs_table_structure($include_mer = FALSE) {
  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);

  $structure = array();

  if ($include_mer) {
    $structure['mer'] = array(
      'created' => array(
        'path' => 'request][created',
        'name' => "Created On",
        'type' => 'timestamp',
        'size' => 'big',
        'storage' => 'date_popup',
        'manual_processing' => TRUE,
      ),
      'updated' => array(
        'path' => 'request][updated',
        'name' => "Updated On",
        'type' => 'timestamp',
        'size' => 'big',
        'storage' => 'date_popup',
        'manual_processing' => TRUE,
      ),
      'status' => array(
        'path' => 'request][status',
        'name' => "Status",
        'type' => 'options',
        'size' => 'small',
        'manual_processing' => TRUE,
      ),
      'step' => array(
        'path' => 'request][step',
        'name' => "Step",
        'type' => 'options',
        'size' => 'normal',
        'manual_processing' => TRUE,
      ),
      'classification' => array(
        'path' => 'request][request_coordinator][classification',
        'name' => "Request Coordinator Classification",
        'type' => 'options',
        'size' => 'normal',
        'manual_processing' => TRUE,
      ),
      'type' => array(
        'path' => 'request][information][type',
        'name' => "Type",
        'type' => 'options',
        'size' => 'normal',
        'manual_processing' => TRUE,
      ),
      'location' => array(
        'path' => 'request][location',
        'name' => "Location",
        'type' => 'options',
        'size' => 'big',
        'manual_processing' => TRUE,
      ),
      'building' => array(
        'path' => 'request][building',
        'name' => "Building",
        'type' => 'options',
        'size' => 'big',
        'manual_processing' => TRUE,
      ),
      'room' => array(
        'path' => 'request][room',
        'name' => "Room",
        'type' => 'options',
        'size' => 'big',
        'manual_processing' => TRUE,
      ),
      'venue_coordinator' => array(
        'path' => 'request][venue_coordinator][user_id',
        'name' => "Venue Coordinator",
        'type' => 'venue_coordinator',
        'size' => 'normal',
        'manual_processing' => TRUE,
      ),
      'title' => array(
        'path' => 'request][information][title',
        'name' => "Request Name", // renamed from "Request Title" to "Request Name" as per fcs-23.
        'type' => 'text',
        'size' => 255,
        'manual_processing' => TRUE,
      ),
    );
  }

  $structure['top'] = array(
    'status' => array(
      'path' => 'request][status',
      'name' => "Status",
      'type' => 'options',
      'size' => 'small',
      'hide_from_search' => TRUE,
    ),
    'step' => array(
      'path' => 'request][step',
      'name' => "Step",
      'type' => 'options',
      'size' => 'normal',
      'hide_from_search' => TRUE,
    ),
    'location' => array(
      'path' => 'request][location',
      'name' => "Location",
      'type' => 'options',
      'size' => 'big',
      'hide_from_search' => TRUE,
    ),
    'building' => array(
      'path' => 'request][building',
      'name' => "Building",
      'type' => 'options',
      'size' => 'big',
      'hide_from_search' => TRUE,
    ),
    'room' => array(
      'path' => 'request][room',
      'name' => "Room",
      'type' => 'options',
      'size' => 'big',
      'hide_from_search' => TRUE,
    ),
    'additional' => array(
      'path' => 'request][additional',
      'name' => "Additional Information",
      'type' => 'text',
      'size' => 2048,
      'fix_lines' => TRUE,
    ),
  );

  $structure['review'] = array(
    'review' => array(
      'path' => 'request][review][review',
      'name' => "Review Log",
      'type' => 'review_log',
      'columns' => array('reviewer', 'step', 'decision', 'message'),
      'size' => array('normal', 'normal', 'normal', '4096'),
      'manual_processing' => TRUE,
    ),
  );

  $structure['requirements'] = array(
    'facilities' => array(
      'path' => 'request][requirements][facilities',
      'name' => "Facilities Fees",
      'type' => 'requirements',
      'manual_processing' => TRUE,
      'columns' => array('hours', 'days', 'amount'),
      'size' => array('normal', 'normal', 'normal'),
      'storage' => 'currency',
      'allows_null' => TRUE,
    ),
    'equipment' => array(
      'path' => 'request][requirements][equipment',
      'name' => "Equipment Fees",
      'type' => 'requirements',
      'manual_processing' => TRUE,
      'columns' => array('hours', 'days', 'amount'),
      'size' => array('normal', 'normal', 'normal'),
      'storage' => 'currency',
      'allows_null' => TRUE,
    ),
    'custodial' => array(
      'path' => 'request][requirements][custodial',
      'name' => "Custodial Fees",
      'type' => 'requirements',
      'manual_processing' => TRUE,
      'columns' => array('quantity', 'hours', 'days', 'amount'),
      'size' => array('normal', 'normal', 'normal', 'normal'),
      'storage' => 'currency',
      'allows_null' => TRUE,
    ),
    'maintenance' => array(
      'path' => 'request][requirements][maintenance',
      'name' => "Maintenance Fees",
      'type' => 'requirements',
      'manual_processing' => TRUE,
      'columns' => array('quantity', 'hours', 'days', 'amount'),
      'size' => array('normal', 'normal', 'normal', 'normal'),
      'storage' => 'currency',
      'allows_null' => TRUE,
    ),
    'grounds' => array(
      'path' => 'request][requirements][grounds',
      'name' => "Grounds Fees",
      'type' => 'requirements',
      'manual_processing' => TRUE,
      'columns' => array('quantity', 'hours', 'days', 'amount'),
      'size' => array('normal', 'normal', 'normal', 'normal'),
      'storage' => 'currency',
      'allows_null' => TRUE,
    ),
    'security' => array(
      'path' => 'request][requirements][security',
      'name' => "Security Fees",
      'type' => 'requirements',
      'manual_processing' => TRUE,
      'columns' => array('quantity', 'hours', 'days', 'amount'),
      'size' => array('normal', 'normal', 'normal', 'normal'),
      'storage' => 'currency',
      'allows_null' => TRUE,
    ),
    'other' => array(
      'name' => "Other Fees",
      'path' => 'request][requirements][other',
      'type' => 'bool',
      'size' => 'tiny',
      'manual_processing' => TRUE,
      'allows_null' => TRUE,
    ),
    'waived' => array(
      'name' => "Requirements / Fees Waived (with Presidents Approval)",
      'path' => 'request][requirements][waived',
      'type' => 'bool',
      'size' => 'tiny',
      'manual_processing' => TRUE,
      'allows_null' => TRUE,
    ),
    'university' => array(
      'name' => "University Fees",
      'type' => 'bool',
      'size' => 'tiny',
      'path' => 'request][requirements][university',
      'manual_processing' => TRUE,
      'allows_null' => TRUE,
    ),
  );

  $structure['insurance'] = array(
    'contractor' => array(
      'path' => 'request][insurance][contractor',
      'name' => "Contractor Insurance",
      'type' => 'bool',
      'size' => 'tiny',
      'manual_processing' => TRUE,
      'allows_null' => TRUE,
    ),
    'unaffiliated' => array(
      'name' => "Unaffiliated Insurance",
      'path' => 'request][insurance][unaffiliated',
      'type' => 'bool',
      'size' => 'tiny',
      'manual_processing' => TRUE,
      'allows_null' => TRUE,
    ),
    'provided' => array(
      'name' => "Insurance Has Been Provided",
      'path' => 'request][insurance][provided',
      'type' => 'bool',
      'size' => 'tiny',
      'manual_processing' => TRUE,
      'allows_null' => TRUE,
    ),
  );

  $structure['dates'] = array(
    'date' => array(
      'path' => 'request][dates][date',
      'name' => "Date",
      'type' => 'date',
      'size' => 'big',
      'storage' => 'date_popup',
      'date_popup' => array('view' => 'Y/m/d'),
      'multiple' => TRUE,
    ),
    'time_start' => array(
      'path' => 'request][dates][time_start',
      'name' => "Start Time",
      'type' => 'time',
      'size' => 'big',
      'storage' => 'timestamp',
      'timestamp' => array('view' => 'h:ia', 'relative_to' => 'request][dates][date'),
      'multiple' => TRUE,
    ),
    'time_stop' => array(
      'path' => 'request][dates][time_stop',
      'name' => "Stop Time",
      'type' => 'time',
      'size' => 'big',
      'storage' => 'timestamp',
      'timestamp' => array('view' => 'h:ia', 'relative_to' => 'request][dates][date'),
      'multiple' => TRUE,
    ),
    'category' => array(
      'path' => 'request][dates][category',
      'name' => "Date Category",
      'type' => 'options',
      'size' => 'normal',
      'multiple' => TRUE,
    ),
  );

  $structure['venue_coordinator'] = array(
    'user_id' => array(
      'path' => 'request][venue_coordinator][user_id',
      'name' => "Venue Coordinator",
      'type' => 'venue_coordinator',
      'size' => 'normal',
      'hide_from_search' => TRUE,
    ),
  );

  $structure['request_coordinator'] = array(
    'name' => array(
      'path' => 'request][request_coordinator][name',
      'name' => "Request Coordinator Name",
      'type' => 'text',
      'size' => 255,
    ),
    'phone' => array(
      'path' => 'request][request_coordinator][phone',
      'name' => "Request Coordinator Phone",
      'type' => 'text',
      'size' => 255,
    ),
    'email' => array(
      'path' => 'request][request_coordinator][email',
      'name' => "Request Coordinator E-mail",
      'type' => 'text',
      'size' => 255,
    ),
    'classification' => array(
      'path' => 'request][request_coordinator][classification',
      'name' => "Request Coordinator Classification",
      'type' => 'options',
      'size' => 'normal',
    ),
  );

  $structure['information'] = array(
    'title' => array(
      'path' => 'request][information][title',
      'name' => "Request Name", // renamed from "Request Title" to "Request Name" as per fcs-23.
      'type' => 'text',
      'size' => 255,
      'hide_from_search' => TRUE,
    ),
    'attendance' => array(
      'path' => 'request][information][attendance',
      'name' => "Expected Attendance",
      'type' => 'text',
      'size' => 'normal',
    ),
    'type' => array(
      'path' => 'request][information][type',
      'name' => "Type",
      'type' => 'options',
      'size' => 'normal',
      'hide_from_search' => TRUE,
    ),
    'organization' => array(
      'path' => 'request][information][organization',
      'name' => "Organization / Affiliation",
      'type' => 'text',
      'size' => 255,
    ),
    'adviser_approval' => array(
      'name' => "Adviser Approval",
      'type' => 'bool',
      'path' => 'request][information][adviser_approval',
      'size' => 'tiny',
    ),
  );

  $structure['plans'] = array(
    'audience' => array(
      'path' => 'request][plans][audience',
      'name' => "Intended Audience",
      'type' => 'options',
      'size' => 'normal',
    ),
    'description' => array(
      'path' => 'request][plans][description',
      'name' => "Description / Purpose",
      'type' => 'text',
      'size' => 2048,
      'fix_lines' => TRUE,
    ),
    'activities' => array(
      'path' => 'request][plans][activities',
      'name' => "Planned Activities",
      'type' => 'text',
      'size' => 2048,
      'fix_lines' => TRUE,
    ),
  );


  $revenue_generated_columns = mfcs_get_revenue_generated_list_options('column_name');
  $revenue_generated_size = array();
  foreach ($revenue_generated_columns as $key => $value) {
    $revenue_generated_size[$key] = 'normal';
  }

  $technical_equipment_details_columns = mfcs_get_technical_equipment_details_list_options('column_name');
  $technical_equipment_details_size = array();
  foreach ($technical_equipment_details_columns as $key => $value) {
    $technical_equipment_details_size[$key] = 'normal';
  }

  $structure['registration'] = array(
    'require' => array(
      'path' => 'request][registration][require',
      'name' => "Registration or Ticket Purchase",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'website' => array(
      'path' => 'request][registration][website',
      'name' => "Registration Website",
      'type' => 'text',
      'size' => 255,
    ),
    'phone' => array(
      'path' => 'request][registration][phone',
      'name' => "Registration Phone",
      'type' => 'text',
      'size' => 255,
    ),
    'ticket_price' => array(
      'path' => 'request][registration][ticket_price',
      'name' => "Ticket Prices",
      'type' => 'text',
      'size' => 255,
    ),
    'ticket_dates' => array(
      'path' => 'request][registration][ticket_dates',
      'name' => "Ticket Dates",
      'type' => 'text',
      'size' => 255,
    ),
    'ticket_website' => array(
      'path' => 'request][registration][ticket_website',
      'name' => "Ticket Website",
      'type' => 'text',
      'size' => 255,
    ),
    'ticket_phone' => array(
      'path' => 'request][registration][ticket_phone',
      'name' => "Ticket Phone",
      'type' => 'text',
      'size' => 255,
    ),
    'generate_revenue' => array(
      'path' => 'request][registration][generate_revenue',
      'name' => "Generate Revenue",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'revenue_generated' => array(
      'path' => 'request][registration][revenue_generated',
      'columns' => $revenue_generated_columns,
      'size' => $revenue_generated_size,
      'name' => "How Revenue is Generated",
      'type' => 'checkboxes',
    ),
  );


  $structure['setup'] = array(
    'rectangular_tables_8ft' => array(
      'path' => 'request][setup][rectangular_tables_8ft',
      'name' => "8ft Rectangular Tables",
      'type' => 'number',
      'size' => 'normal',
    ),
    'round_tables_8ft' => array(
      'path' => 'request][setup][round_tables_8ft',
      'name' => "8ft Round Tables",
      'type' => 'number',
      'size' => 'normal',
    ),
    'other_tables' => array(
      'path' => 'request][setup][other_tables',
      'name' => "Other Tables",
      'type' => 'text',
      'size' => 255,
      'fix_lines' => TRUE,
    ),
    'standard_blue_chairs' => array(
      'path' => 'request][setup][standard_blue_chairs',
      'name' => "Standard Chairs", // renamed "Blue Chairs" to "Chairs" as per fcs-22.
      'type' => 'number',
      'size' => 'normal',
    ),
    'podium' => array(
      'path' => 'request][setup][podium',
      'name' => "Podium Needed",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'portable_stage' => array(
      'path' => 'request][setup][portable_stage',
      'name' => "Portable Stage Needed",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'portable_stage_configuration' => array(
      'path' => 'request][setup][portable_stage_configuration',
      'name' => "Portable Stage Configuration",
      'type' => 'text',
      'size' => 255,
      'fix_lines' => TRUE,
    ),
    'security' => array(
      'path' => 'request][setup][security',
      'name' => "Security Needed",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'parking_assistance' => array(
      'path' => 'request][setup][parking_assistance',
      'name' => "Parking Assistance Needed",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'parking_assistance_area' => array(
      'path' => 'request][setup][parking_assistance_area',
      'name' => "Parking Assistance Area",
      'type' => 'text',
      'size' => 255,
      'fix_lines' => TRUE,
    ),
    'road_closures' => array(
      'path' => 'request][setup][road_closures',
      'name' => "Road Closures or Barricades",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'road_closures_details' => array(
      'path' => 'request][setup][road_closures_details',
      'name' => "Road Closure Details",
      'type' => 'text',
      'size' => 255,
      'fix_lines' => TRUE,
    ),
    'special_requests' => array(
      'path' => 'request][setup][special_requests',
      'name' => "Special Requests",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'special_requests_details' => array(
      'path' => 'request][setup][special_requests_details',
      'name' => "Special Request Details",
      'type' => 'text',
      'size' => 255,
      'fix_lines' => TRUE,
    ),
  );


  $publicity_details_columns = mfcs_get_publicity_details_list_options('column_name');
  $publicity_details_size = array();
  foreach ($publicity_details_columns as $key => $value) {
    $publicity_details_size[$key] = 'normal';
  }

  $designing_material_columns = mfcs_get_designing_material_list_options('column_name');
  $designing_material_size = array();
  foreach ($designing_material_columns as $key => $value) {
    $designing_material_size[$key] = 'normal';
  }



  $structure['presentation'] = array(
    'technical_equipment' => array(
      'path' => 'request][presentation][technical_equipment',
      'name' => "Need Technical Equipment",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'technical_equipment_details' => array(
      'path' => 'request][presentation][technical_equipment_details',
      'columns' => $technical_equipment_details_columns,
      'name' => "Technical Equipment Details",
      'type' => 'checkboxes',
      'size' => $technical_equipment_details_size,
    ),
    'technical_equipment_microphone' => array(
      'path' => 'request][presentation][technical_equipment_microphone',
      'name' => "Quantity of Microphones",
      'type' => 'text',
      'size' => 'normal',
    ),
    'external_audio_person' => array(
      'path' => 'request][presentation][external_audio_person',
      'name' => "External Audio Person/Company",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'external_audio_person_name' => array(
      'path' => 'request][presentation][external_audio_person_name',
      'name' => "Name of Audio Person/Company",
      'type' => 'text',
      'size' => 255,
    ),
    'external_audio_person_email' => array(
      'path' => 'request][presentation][external_audio_person_email',
      'name' => "E-Mail of Audio Person/Company",
      'type' => 'text',
      'size' => 255,
    ),
    'external_audio_person_phone' =>  array(
      'path' =>'request][presentation][external_audio_person_phone',
      'name' => "Phone of Audio Person/Company",
      'type' => 'text',
      'size' => 255,
    ),
    'publicity' => array(
      'path' => 'request][presentation][publicity',
      'name' => "Need Publicity",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'publicity_details' => array(
      'path' => 'request][presentation][publicity_details',
      'columns' => $publicity_details_columns,
      'name' => "Publicity Methods",
      'type' => 'checkboxes',
      'size' => $publicity_details_size,
    ),
    'production' => array(
      'path' => 'request][presentation][production',
      'name' => "Using Production Company/Promoter",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'production_name' => array(
      'path' => 'request][presentation][production_name',
      'name' => "Name of Production Company/Promoter",
      'type' => 'text',
      'size' => 255,
    ),
    'production_email' => array(
      'path' => 'request][presentation][production_email',
      'name' => "E-Mail of Production Company/Promoter",
      'type' => 'text',
      'size' => 255,
    ),
    'production_phone' => array(
      'path' => 'request][presentation][production_phone',
      'name' => "Phone of Production Company/Promoter",
      'type' => 'text',
      'size' => 255,
    ),
    'printed_material' => array(
      'path' => 'request][presentation][printed_material',
      'name' => "Using Printed Material",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'university_logo' => array(
      'path' => 'request][presentation][university_logo',
      'name' => "Using University Logo",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'designing_material' => array(
      'path' => 'request][presentation][designing_material',
      'columns' => $designing_material_columns,
      'name' => "Material Designer",
      'type' => 'checkboxes',
      'size' => $designing_material_size,
    ),
    'designing_material_name' => array(
      'path' => 'request][presentation][designing_material_name',
      'name' => "Material Designer Name",
      'type' => 'text',
      'size' => 255,
    ),
    'designing_material_email' => array(
      'path' => 'request][presentation][designing_material_email',
      'name' => "Material Designer E-Mail",
      'type' => 'text',
      'size' => 255,
    ),
    'designing_material_phone' => array(
      'path' => 'request][presentation][designing_material_phone',
      'name' => "Material Designer Phone",
      'type' => 'text',
      'size' => 255,
    ),
  );

  $structure['services'] = array(
    'food_served' => array(
      'path' => 'request][services][food_served',
      'name' => "Food Served",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'food_caterer' => array(
      'path' => 'request][services][food_caterer',
      'name' => "Food Caterer",
      'type' => 'text',
      'size' => 255,
    ),
    'alcohol' => array(
      'path' => 'request][services][alcohol',
      'name' => "Alcohol Served",
      'type' => 'bool',
      'size' => 'tiny',
    ),
    'open_flames' => array(
      'path' => 'request][services][open_flames',
      'name' => "Open Flames Used",
      'type' => 'bool',
      'size' => 'tiny',
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
function mfcs_table_presentation($option = NULL) {
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
        'request_type' => array(
          'hide_link' => array(
            MFCS_REQUEST_TYPE_MEETING,
            MFCS_REQUEST_TYPE_QUICK_MEETING,
          ),
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

      $presentation['requirements'] = array(
        'weight' => 0,
        'group' => 5,
        'label' => "Requirements / Fees",
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
      'request_type' => array(
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
        'value' => 'Y/m/d h:i:sa',
      ),
      'request_type' => array(
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
      'request_type' => array(
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
        'value' => 'Y/m/d h:i:sa',
      ),
      'request_type' => array(
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
      'request_type' => array(
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
      'request_type' => array(
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
      'request_type' => array(
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
      'request_type' => array(
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
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'additional' => array(
      'label' => 'Additional Information or Questions to Campus Officials',
      'size' => 'large',
      'type' => array(
        'value' => 'text_lined',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
  );

  $presentation['requirements'] = array(
    'facilities' => array(
      'label' => 'Facilities Use',
      'combined' => TRUE,
      'value_names' => array(
        'days',
        'amount',
      ),
      'type' => array(
        'days' => 'requirement_days',
        'amount' => 'requirement_amount',
       ),
      'group' => 'requirements',
      'request_type' => array(
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
        'days' => 'requirement_days',
        'amount' => 'requirement_amount',
       ),
      'group' => 'requirements',
      'request_type' => array(
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
        'quantity' => 'requirement_personnel',
        'hours' => 'requirement_hours',
        'days' => 'requirement_days',
        'amount' => 'requirement_amount',
      ),
      'group' => 'requirements',
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'maintenance' => array(
      'label' => 'Maintenance Use',
      'combined' => TRUE,
      'value_names' => array(
        'quantity',
        'hours',
        'days',
        'amount',
      ),
      'type' => array(
        'quantity' => 'requirement_personnel',
        'hours' => 'requirement_hours',
        'days' => 'requirement_days',
        'amount' => 'requirement_amount',
       ),
      'group' => 'requirements',
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'grounds' => array(
      'label' => 'Grounds Use',
      'combined' => TRUE,
      'value_names' => array(
        'quantity',
        'hours',
        'days',
        'amount',
      ),
      'type' => array(
        'quantity' => 'requirement_personnel',
        'hours' => 'requirement_hours',
        'days' => 'requirement_days',
        'amount' => 'requirement_amount',
       ),
      'group' => 'requirements',
      'request_type' => array(
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
        'quantity' => 'requirement_personnel',
        'hours' => 'requirement_hours',
        'days' => 'requirement_days',
        'amount' => 'requirement_amount',
      ),
      'group' => 'requirements',
      'request_type' => array(
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
      'group' => 'requirements',
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'waived' => array(
      'label' => 'Requirements / Fees Waived',
      'type' => array(
        'value' => 'boolean',
      ),
      'display' => array(
        'value' => 'when_true',
      ),
      'group' => 'requirements',
      'request_type' => array(
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
      'group' => 'requirements',
      'request_type' => array(
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
      'group' => 'requirements',
      'request_type' => array(
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
      'group' => 'requirements',
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'provided' => array(
      'label' => 'Insurance Provided',
      'type' => array(
        'value' => 'boolean',
      ),
      'group' => 'requirements',
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
  );

  $presentation['dates'] = array(
    'date' => array(
      'label' => 'Date',
      'type' => array(
        'value' => 'timestamp_linked_day',
      ),
      'display' => array(
        'value' => 'Y/m/d',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
      'collate' => 'date',
      'collate_order' => 1,
      'collate_into' => TRUE,
      'multiple' => TRUE,
      'cancellable' => TRUE,
    ),
    'time_start' => array(
      'label' => 'Start Time',
      'type' => array(
        'value' => 'timestamp_linked_hour',
      ),
      'display' => array(
        'value' => 'h:ia',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
      'collate' => 'date',
      'collate_order' => 2,
      'prefix' => ', ',
      'multiple' => TRUE,
    ),
    'time_stop' => array(
      'label' => 'Stop Time',
      'type' => array(
        'value' => 'timestamp_linked_hour',
      ),
      'display' => array(
        'value' => 'h:ia',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
      'collate' => 'date',
      'collate_order' => 3,
      'prefix' => ' to ',
      'multiple' => TRUE,
    ),
    'category' => array(
      'label' => 'Date Category',
      'type' => array(
        'value' => 'taxonomy',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
      'collate' => 'date',
      'collate_order' => 0,
      'prefix' => '(',
      'suffix' => ') ',
      'multiple' => TRUE,
    ),
  );

  $presentation['venue_coordinator'] = array(
    'user_id' => array(
      'label' => 'Venue Coordinator',
      'split' => array(
        'name' => 'venue_coordinator',
        'phone' => 'venue_coordinator',
        'email' => 'venue_coordinator',
      ),
      'type' => array(
        'value' => 'venue_coordinator',
      ),
      'display' => array(
        'value' => 'user_id',
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
      'request_type' => array(
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
      'request_type' => array(
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
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
  );

  $presentation['request_coordinator'] = array(
    'name' => array(
      'label' => 'Request Coordinator Name',
      'group' => 'coordinator',
      'weight' => 4,
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'phone' => array(
      'label' => 'Request Coordinator Phone',
      'group' => 'coordinator',
      'weight' => 5,
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'email' => array(
      'label' => 'Request Coordinator E-Mail',
      'group' => 'coordinator',
      'weight' => 6,
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'classification' => array(
      'label' => 'Request Coordinator Classification',
      'group' => 'coordinator',
      'weight' => 7,
      'type' => array(
        'value' => 'taxonomy',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
  );

  $presentation['information'] = array(
    'title' => array(
      'label' => 'Request Name', // renamed from "Request Title" to "Request Name" as per fcs-23.
      'weight' => 1,
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'attendance' => array(
      'label' => 'Expected Attendance',
      'weight' => 3,
      'request_type' => array(
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
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'organization' => array(
      'label' => 'Organization / Affiliation',
      'weight' => 4,
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'adviser_approval' => array(
      'label' => 'Adviser Approval',
      'weight' => 5,
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
      'request_classification' => array(
        'show' => array(1),
        'hide' => TRUE,
      ),
    ),
  );

  $presentation['plans'] = array(
    'audience' => array(
      'label' => 'Intended Audience',
      'type' => array(
        'value' => 'taxonomy',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'description' => array(
      'label' => 'Description/Purpose',
      'size' => 'large',
      'type' => array(
        'value' => 'text_lined',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'activities' => array(
      'label' => 'Planned Activities',
      'size' => 'large',
      'type' => array(
        'value' => 'text_lined',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
  );

  $presentation['registration'] = array(
    'require' => array(
      'label' => 'Registration or Ticket Purchase is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'website' => array(
      'label' => 'Registration Website',
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'phone' => array(
      'label' => 'Registration Phone',
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'ticket_price' => array(
      'label' => 'Ticket Price',
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'ticket_dates' => array(
      'label' => 'Ticket Dates',
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'ticket_website' => array(
      'label' => 'Ticket Website',
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'ticket_phone' => array(
      'label' => 'Ticket Phone',
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'generate_revenue' => array(
      'label' => 'Revenue is Generated',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
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
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
  );

  $presentation['setup'] = array(
    'rectangular_tables_8ft' => array(
      'label' => 'Quantity of 8ft Rectangular Tables',
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'round_tables_8ft' => array(
      'label' => 'Quantity of 8ft Round Tables',
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'other_tables' => array(
      'label' => 'Description and Quantity of Other Tables',
      'size' => 'large',
      'type' => array(
        'value' => 'text_lined',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'standard_blue_chairs' => array(
      'label' => 'Quantity of Chairs', // renamed "Blue Chairs" to "Chairs" as per fcs-22.
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'podium' => array(
      'label' => 'Podium is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'portable_stage' => array(
      'label' => 'Portable Stage is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'portable_stage_configuration' => array(
      'label' => 'Portable Stage Configuration',
      'size' => 'large',
      'type' => array(
        'value' => 'text_lined',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'security' => array(
      'label' => 'Security is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'parking_assistance' => array(
      'label' => 'Parking Assistance or Restrictions are Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'parking_assistance_area' => array(
      'label' => 'Parking Assistance or Restriction Explanation',
      'size' => 'large',
      'type' => array(
        'value' => 'text_lined',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'road_closures' => array(
      'label' => 'Road Closures or Barricades are Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'road_closures_details' => array(
      'label' => 'Road Closures or Barricades Explanation',
      'size' => 'large',
      'type' => array(
        'value' => 'text_lined',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'special_requests' => array(
      'label' => 'Special Requests are Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'special_requests_details' => array(
      'label' => 'Special Requests Explanation',
      'size' => 'large',
      'type' => array(
        'value' => 'text_lined',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
  );

  $presentation['presentation'] = array(
    'technical_equipment' => array(
      'label' => 'Technical Equipment is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
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
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'technical_equipment_microphone' => array(
      'label' => 'Quantity of Microphones',
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'external_audio_person' => array(
      'label' => 'External Audio/Video Person/Company is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'external_audio_person_name' => array(
      'label' => 'Name of Person/Company',
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'external_audio_person_email' => array(
      'label' => 'E-mail of Person/Company',
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'external_audio_person_phone' => array(
      'label' => 'Phone of Person/Company',
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'publicity' => array(
      'label' => 'Publicity is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
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
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'production' => array(
      'label' => 'Production Company or Professional Promoter is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'production_name' => array(
      'label' => 'Production Company or Professional Promoter Name',
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'production_email' => array(
      'label' => 'Production Company or Professional Promoter E-mail',
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'production_phone' => array(
      'label' => 'Production Company or Professional Promoter Phone',
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'printed_material' => array(
      'label' => 'Printed Material is Required',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'university_logo' => array(
      'label' => 'Printed Material Uses University Logo',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'designing_material' => array(
      'label' => 'Printed Material Designer',
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
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
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'designing_material_email' => array(
      'label' => 'E-mail of Printed Material Designer',
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
    'designing_material_phone' => array(
      'label' => 'Phone of Printed Material Designer',
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
      ),
    ),
  );

  $presentation['services'] = array(
    'food_served' => array(
      'label' => 'Food or Drink is Being Served',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'food_caterer' => array(
      'label' => 'Name of the Caterer/Business',
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'alcohol' => array(
      'label' => 'Alcohol is Being Served',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
    'open_flames' => array(
      'label' => 'Open Flames are Being Used',
      'type' => array(
        'value' => 'boolean',
      ),
      'request_type' => array(
        'show' => TRUE,
        'hide' => array(
          MFCS_REQUEST_TYPE_MEETING,
          MFCS_REQUEST_TYPE_QUICK_MEETING,
        ),
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
      'request_type' => array(
        'show' => TRUE,
        'hide' => FALSE,
      ),
    ),
  );

  return $presentation;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
