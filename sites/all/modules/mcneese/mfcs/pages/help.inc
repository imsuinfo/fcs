<?php

/**
 * @file
 * Defines McNeese facilities use help page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request help page.
 *
 * @param string|null $topic
 *   When NULL, represents the default topic. Otherwise, this is a string
 *   representing the topic to reference.
 * @param string|null $section
 *   When NULL, represents the default section. Otherwise, this is a string
 *   representing the section to reference.
 * @param string|null $topic
 *   When NULL, represents the default sub_section. Otherwise, this is a string
 *   representing the sub_section to reference.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_help_0_page($topic = NULL, $section = NULL, $sub_section = NULL) {
  if (!is_null($topic) && !is_string($topic)) {
    cf_error::invalid_string('topic');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($section) && !is_string($section)) {
    cf_error::invalid_string('section');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($sub_section) && !is_string($sub_section)) {
    cf_error::invalid_string('sub_section');

    drupal_not_found();
    drupal_exit();
  }

  if (is_null($topic) && (!is_null($section) || !is_null($sub_section))) {
    drupal_not_found();
    drupal_exit();
  }

  if (is_null($section) && !is_null($sub_section)) {
    drupal_not_found();
    drupal_exit();
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $user = cf_current_user();

  $title = 'Help';
  if (is_null($topic)) {
    mfcs_add_canonical_headers('help');

    $pdf_url = 'glossary';
  }
  elseif (is_null($section)) {
    $topic = strtolower($topic);

    mfcs_add_canonical_headers('help/' . $topic);

    $title = 'Help - ' . mfcs_help_url_cleanup_for_title($topic);
    $pdf_url = $topic;
  }
  elseif (is_null($sub_section)) {
    $topic = strtolower($topic);
    $section = strtolower($section);

    mfcs_add_canonical_headers('help/' . $topic . '/' . $section);

    $title = 'Help - ' . mfcs_help_url_cleanup_for_title($topic) . ' - ' . mfcs_help_url_cleanup_for_title($section);
    $pdf_url = $topic . '/' . $section;
  }
  else {
    $topic = strtolower($topic);
    $section = strtolower($section);
    $sub_section = strtolower($sub_section);

    mfcs_add_canonical_headers('help/' . $topic . '/' . $section . '/' . $sub_section);

    $title = 'Help - ' . mfcs_help_url_cleanup_for_title($topic) . ' - ' . mfcs_help_url_cleanup_for_title($section) . ' - ' . mfcs_help_url_cleanup_for_title($sub_section);
    $pdf_url = $topic . '/' . $section . '/' . $sub_section;
  }

  drupal_set_title($title);

  $allowed_topics = mfcs_help_0_allowed_topics($user);
  if (!is_array($allowed_topics)) {
    return mfcs_help_topic_not_found();
  }
  elseif (!is_null($topic) && !in_array($topic, $allowed_topics)) {
    return mfcs_help_topic_not_found($topic, $section, $sub_section);
  }

  // prepare section information.
  $section_information = array(
    'id' => 'mfcs-section-help-main',
    'class' => NULL,
    'headers' => array(
      0 => array(
        'markup' => 'Facilities Use Documentation',
        'attributes' => array(
          'class' => array(
            'header-1',
            'title',
          ),
        ),
      ),
    ),
    'header_attributes' => array(
    ),
    'navigation' => NULL,
    'start_count' => NULL,
  );

  $base_class = mfcs_help_base_class($topic, $section, $sub_section);


  // build the navigation markup.
  $navigation_urls = mfcs_help_determine_next_previous_top($allowed_topics, $topic, $section, $sub_section);
  $section_information['navigation'] = '<nav id="mfcs-help-navigation_links" class="help-navigation_links no-print">';

  if (!is_null($navigation_urls['top'])) {
    if (!empty($navigation_urls['previous'])) {
      $section_information['navigation'] .= '<a href="' . $navigation_urls['previous'] . '" title="' . $navigation_urls['tooltip-previous'] . '" class="help-navigation_link link-previous">Previous</a>';
    }

    $section_information['navigation'] .= '<a href="' . $navigation_urls['top'] . '" title="' . $navigation_urls['tooltip-top'] . '" class="help-navigation_link link-top">Top</a>';

    if (!empty($navigation_urls['next'])) {
      $section_information['navigation'] .= '<a href="' . $navigation_urls['next'] . '" title="' . $navigation_urls['tooltip-next'] . '" class="help-navigation_link link-next">Next</a>';
    }
  }

  if (file_exists(DRUPAL_ROOT . '/sites/all/modules/mcneese/mfcs/images/pdf/32.png')) {
    $module_path = drupal_get_path('module', 'mfcs');

    $image_file_path = $base_path . $module_path . '/images/pdf/32.png';

    $section_information['navigation'] .= '<a href="' . $base_path . 'help-0/presentation/pdf/' . $pdf_url . $url_arguments . '" title="Download as PDF" class="help-navigation_link link-pdf"><img src="' . $image_file_path . '" alt="PDF" width="32" height="32" class="help-navigation_image image-pdf"></a>';

    unset($image_file_path);
    unset($module_path);
  }

  $section_information['navigation'] .= '</nav>';


  // build the page.
  $markup = '';
  if (is_null($topic)) {
    $section_information['headers'][1] = array('markup' => 'Glossary', 'attributes' => array('class' => array('header-2', 'topic')));
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-break';

    if (is_null($section)) {
      $section_information['class'][] = 'section-help-topic';

      $markup .= mfcs_help_topic_glossary($allowed_topics, $section_information);
    }
    else {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
  }
  elseif ($topic == 'introduction' && in_array('introduction', $allowed_topics)) {
    $section_information['headers'][1] = array('markup' => 'Introduction', 'attributes' => array('class' => array('header-2', 'topic')));
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-break';

    if (is_null($section)) {
      $section_information['class'][] = 'section-help-topic-introduction';

      $markup .= mfcs_help_topic_introduction($allowed_topics, $section_information);
    }
    elseif (!is_null($sub_section)) {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
    else {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
  }
  elseif ($topic == 'requests' && in_array('requests', $allowed_topics)) {
    $section_information['headers'][1] = array('markup' => 'Requests', 'attributes' => array('class' => array('header-2', 'topic')));
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-break';

    if (is_null($section)) {
      $section_information['class'][] = 'section-help-topic-requests';

      $markup .= mfcs_help_topic_requests($allowed_topics, $section_information);
    }
    elseif (!is_null($sub_section)) {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
    elseif ($section == 'employee') {
      $section_information['class'][] = 'section-help-topic-requests-employee';
      $section_information['headers'][2] = array('markup' => 'Faculty, Staff, and Athletics Requests', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_requests_section_employee($allowed_topics, $section_information);
    }
    elseif ($section == 'student') {
      $section_information['class'][] = 'section-help-topic-requests-student';
      $section_information['headers'][2] = array('markup' => 'Student Organization Requests', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_requests_section_student_organization($allowed_topics, $section_information);
    }
    elseif ($section == 'camps') {
      $section_information['class'][] = 'section-help-topic-requests-camps';
      $section_information['headers'][2] = array('markup' => 'Camps Requests', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_requests_section_camps($allowed_topics, $section_information);
    }
    elseif ($section == 'external') {
      $section_information['class'][] = 'section-help-topic-requests-external';
      $section_information['headers'][2] = array('markup' => 'External Requests', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_requests_section_external($allowed_topics, $section_information);
    }
    else {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
  }
  elseif ($topic == 'review' && in_array('review', $allowed_topics)) {
    $section_information['headers'][1] = array('markup' => 'Review', 'attributes' => array('class' => array('header-2', 'topic')));
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-break';

    if (is_null($section)) {
      $section_information['class'][] = 'section-help-topic-review';

      $markup .= mfcs_help_topic_review($allowed_topics, $section_information);
    }
    elseif (!is_null($sub_section)) {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
    elseif ($section == 'venue_available') {
      $section_information['class'][] = 'section-help-topic-review-venue_available';
      $section_information['headers'][2] = array('markup' => 'Venue Available Step', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_review_section_venue_available($allowed_topics, $section_information);
    }
    elseif ($section == 'review') {
      $section_information['class'][] = 'section-help-topic-review-review';
      $section_information['headers'][2] = array('markup' => 'Review Step', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_review_section_review($allowed_topics, $section_information);
    }
    elseif ($section == 'final_decision') {
      $section_information['class'][] = 'section-help-topic-review-final_decision';
      $section_information['headers'][2] = array('markup' => 'Final Decision Step', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_review_section_final_decision($allowed_topics, $section_information);
    }
    elseif ($section == 'completed') {
      $section_information['class'][] = 'section-help-topic-review-completed';
      $section_information['headers'][2] = array('markup' => 'Completed Step', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_review_section_completed($allowed_topics, $section_information);
    }
    else {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
  }
  elseif ($topic == 'manage' && in_array('manage', $allowed_topics)) {
    $section_information['headers'][1] = array('markup' => 'Manage', 'attributes' => array('class' => array('header-2', 'topic')));
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-break';

    if (is_null($section)) {
      $section_information['class'][] = 'section-help-topic-manage';

      $markup .= mfcs_help_topic_manage($allowed_topics, $section_information);
    }
    elseif (!is_null($sub_section)) {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
    elseif ($section == 'requests') {
      $section_information['class'][] = 'section-help-topic-manage-requests';
      $section_information['headers'][2] = array('markup' => 'Requests Management', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_manage_section_requests($allowed_topics, $section_information);
    }
    elseif ($section == 'users') {
      $section_information['class'][] = 'section-help-topic-manage-users';
      $section_information['headers'][2] = array('markup' => 'Users Management', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_manage_section_users($allowed_topics, $section_information);
    }
    elseif ($section == 'problems') {
      $section_information['class'][] = 'section-help-topic-manage-problems';
      $section_information['headers'][2] = array('markup' => 'Problems Management', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_manage_section_problems($allowed_topics, $section_information);
    }
    elseif ($section == 'availability') {
      $section_information['class'][] = 'section-help-topic-manage-availability';
      $section_information['headers'][2] = array('markup' => 'Availability Management', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_manage_section_availability($allowed_topics, $section_information);
    }
    elseif ($section == 'logs') {
      $section_information['class'][] = 'section-help-topic-manage-logs';
      $section_information['headers'][2] = array('markup' => 'Logs Management', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_manage_section_logs($allowed_topics, $section_information);
    }
    else {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
  }
  elseif ($topic == 'email' && in_array('email', $allowed_topics)) {
    $section_information['headers'][1] = array('markup' => 'E-Mail', 'attributes' => array('class' => array('header-2', 'topic')));
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-break';

    if (is_null($section)) {
      $section_information['class'][] = 'section-help-topic-email';

      $markup .= mfcs_help_topic_email($allowed_topics, $section_information);
    }
    elseif ($section == 'request') {
      if (!is_null($sub_section)) {
        // failsafe.
        return mfcs_help_topic_not_found($topic, $section, $sub_section);
      }

      $section_information['class'][] = 'section-help-topic-email-request';
      $section_information['headers'][2] = array('markup' => 'Request E-Mail', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_email_section_request($allowed_topics, $section_information);
    }
    elseif ($section == 'advanced') {
      $section_information['class'][] = 'section-help-topic-email-advanced';
      $section_information['headers'][2] = array('markup' => 'Advanced E-Mail', 'attributes' => array('class' => array('header-3', 'section')));

      if (is_null($sub_section)) {
        $markup .= mfcs_help_topic_email_section_advanced($allowed_topics, $section_information);
      }
      elseif ($sub_section == 'request_status_codes') {
        $section_information['class'][] = 'section-help-topic-email-advanced-request_status_codes';
        $section_information['headers'][3] = array('markup' => 'Request Status Codes', 'attributes' => array('class' => array('header-4', 'sub_section')));

        $markup .= mfcs_help_topic_email_section_advanced_sub_section_status_codes($allowed_topics, $section_information);
      }
      elseif ($sub_section == 'review_step_codes') {
        $section_information['class'][] = 'section-help-topic-email-advanced-review_step_codes';
        $section_information['headers'][3] = array('markup' => 'Review Step Codes', 'attributes' => array('class' => array('header-4', 'sub_section')));

        $markup .= mfcs_help_topic_email_section_advanced_sub_section_step_codes($allowed_topics, $section_information);
      }
      elseif ($sub_section == 'request_classification_codes') {
        $section_information['class'][] = 'section-help-topic-email-advanced-request_classification_codes';
        $section_information['headers'][3] = array('markup' => 'Request Classification Codes', 'attributes' => array('class' => array('header-4', 'sub_section')));

        $markup .= mfcs_help_topic_email_section_advanced_sub_section_classification_codes($allowed_topics, $section_information);
      }
      elseif ($sub_section == 'request_type_codes') {
        $section_information['class'][] = 'section-help-topic-email-advanced-request_type_codes';
        $section_information['headers'][3] = array('markup' => 'Request Type Codes', 'attributes' => array('class' => array('header-4', 'sub_section')));

        $markup .= mfcs_help_topic_email_section_advanced_sub_section_request_type_codes($allowed_topics, $section_information);
      }
      elseif ($sub_section == 'email_type_codes') {
        $section_information['class'][] = 'section-help-topic-email-advanced-email_type_codes';
        $section_information['headers'][3] = array('markup' => 'E-Mail Type Codes', 'attributes' => array('class' => array('header-4', 'sub_section')));

        $markup .= mfcs_help_topic_email_section_advanced_sub_section_email_type_codes($allowed_topics, $section_information);
      }
      else {
        // failsafe.
        return mfcs_help_topic_not_found($topic, $section, $sub_section);
      }
    }
    else {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
  }
  elseif ($topic == 'calendar' && in_array('calendar', $allowed_topics)) {
    $section_information['headers'][1] = array('markup' => 'Calendar', 'attributes' => array('class' => array('header-2', 'topic')));
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-break';

    if (is_null($section)) {
      $section_information['class'][] = 'section-help-topic-calendar';

      $markup .= mfcs_help_topic_calendar($allowed_topics, $section_information);
    }
    elseif (!is_null($sub_section)) {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
    else {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
  }
  elseif ($topic == 'faq' && in_array('faq', $allowed_topics)) {
    $section_information['headers'][1] = array('markup' => 'Frequently Asked Questions', 'attributes' => array('class' => array('header-2', 'topic')));

    if (is_null($section)) {
      $section_information['class'] = $base_class;
      $section_information['class'][] = 'section-help-topic-faq';

      $markup .= mfcs_help_topic_faq($allowed_topics, $section_information);
    }
    elseif (!is_null($sub_section)) {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
    else {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
  }
  elseif ($topic == 'terminology' && in_array('terminology', $allowed_topics)) {
    $section_information['headers'][1] = array('markup' => 'Terminology', 'attributes' => array('class' => array('header-2', 'topic')));
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-break';

    if (is_null($section)) {
      $section_information['class'][] = 'section-help-topic-terminology';

      $markup .= mfcs_help_topic_terminology($allowed_topics, $section_information);
    }
    elseif (!is_null($sub_section)) {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
    elseif ($section == 'facilities_use') {
      $section_information['class'][] = 'section-help-topic-terminology-facilities_use';
      $section_information['class'][] = 'section-help-break';
      $section_information['headers'][2] = array('markup' => 'Facilities Use Terminology', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_terminology_section_facilities_use($allowed_topics, $section_information);
    }
    elseif ($section == 'system') {
      $section_information['class'][] = 'section-help-topic-terminology-system';
      $section_information['class'][] = 'section-help-break';
      $section_information['headers'][2] = array('markup' => 'System Terminology', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_terminology_section_system($allowed_topics, $section_information);
    }
    elseif ($section == 'documentation') {
      $section_information['class'][] = 'section-help-topic-terminology-documentation';
      $section_information['class'][] = 'section-help-break';
      $section_information['headers'][2] = array('markup' => 'Documentation Terminology', 'attributes' => array('class' => array('header-3', 'section')));

      $markup .= mfcs_help_topic_terminology_section_documentation($allowed_topics, $section_information);
    }
    else {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
  }
  else {
    // failsafe.
    return mfcs_help_topic_not_found($topic, $section, $sub_section);
  }

  return $markup;
}

/**
 * Provides the facilities use request help page.
 *
 * @param string $presentation_type
 *   The type of presentation, such as: 'pdf' or 'single'.
 * @param string|null $topic
 *   When NULL, represents the default topic. Otherwise, this is a string
 *   representing the topic to reference.
 * @param string|null $section
 *   When NULL, represents the default section. Otherwise, this is a string
 *   representing the section to reference.
 * @param string|null $topic
 *   When NULL, represents the default sub_section. Otherwise, this is a string
 *   representing the sub_section to reference.
 *
 * @return string
 *   The HTML output representing the page.
 *   This function may exit instead of returning (such as when outputting a PDF file).
 */
function mfcs_help_0_page_presentation($presentation_type, $topic = NULL, $section = NULL, $sub_section = NULL) {
  if (!is_string($presentation_type)) {
    cf_error::invalid_string('presentation_type');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($topic) && !is_string($topic)) {
    cf_error::invalid_string('topic');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($section) && !is_string($section)) {
    cf_error::invalid_string('section');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_null($sub_section) && !is_string($sub_section)) {
    cf_error::invalid_string('sub_section');

    drupal_not_found();
    drupal_exit();
  }

  if (is_null($topic) && (!is_null($section) || !is_null($sub_section))) {
    drupal_not_found();
    drupal_exit();
  }

  if (is_null($section) && !is_null($sub_section)) {
    drupal_not_found();
    drupal_exit();
  }

  drupal_set_title('Help');

  $user = cf_current_user();

  $allowed_topics = mfcs_help_0_allowed_topics($user);
  if (!is_array($allowed_topics)) {
    return mfcs_help_topic_not_found();
  }

  $markup = '';

  if ($presentation_type == 'pdf') {
    if (is_null($topic) && is_null($section) && is_null($sub_section)) {
      $markup = mfcs_help_0_page_presentation_single($allowed_topics);
      mfcs_help_0_page_presentation_pdf('fcs-documentation-single.pdf', 'Facilities Use Documentation', $markup);
    }

    if ($topic == 'glossary') {
      $markup = mfcs_help_0_page();
      mfcs_help_0_page_presentation_pdf('fcs-documentation-glossary.pdf', 'Facilities Use Documentation - Glossary', $markup);
    }

    if (is_null($topic)) {
      drupal_not_found();
      drupal_exit();
    }

    $name = $topic;
    $label = mcneese_fcs_text_capitalization($topic);
    if (!is_null($section)) {
      $name .= '-' . $section;
      $label  .= ' - ' . mcneese_fcs_text_capitalization($section);
    }
    if (!is_null($sub_sction)) {
      $name .= '-' . $sub_sction;
      $label  .= ' - ' . mcneese_fcs_text_capitalization($sub_sction);
    }

    $markup = mfcs_help_0_page($topic, $section, $sub_sction);
    mfcs_help_0_page_presentation_pdf('fcs-documentation-' . $name . '.pdf', 'Facilities Use Documentation - ' . $label, $markup);
  }
  elseif ($presentation_type == 'single') {
    // 'single' does not accept any topic, section, or sub-section.
    if (!is_null($topic) || !is_null($section) || !is_null($sub_section)) {
      drupal_not_found();
      drupal_exit();
    }

    $markup = mfcs_help_0_page_presentation_single($allowed_topics);
  }
  else {
    drupal_not_found();
    drupal_exit();
  }

  return $markup;
}

/**
 * Build the entire documentation section as a single page.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 *
 * @return string
 *   The entire documentation markup.
 */
function mfcs_help_0_page_presentation_single($allowed_topics) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }


  // build the navigation markup.
  $navigation_markup = '<nav id="mfcs-help-navigation_links" class="help-navigation_links no-print">';

  if (file_exists(DRUPAL_ROOT . '/sites/all/modules/mcneese/mfcs/images/pdf/32.png')) {
    $module_path = drupal_get_path('module', 'mfcs');

    $image_file_path = $base_path . $module_path . '/images/pdf/32.png';

    $navigation_markup .= '<a href="' . $base_path . 'help-0/presentation/pdf' . $url_arguments . '" title="Download as PDF" class="help-navigation_link link-pdf"><img src="' . $image_file_path . '" alt="PDF" width="32" height="32" class="help-navigation_image image-pdf"></a>';

    unset($image_file_path);
    unset($module_path);
  }

  $navigation_markup .= '</nav>';


  // prepare the header information.
  $section_id = 'mfcs-section-help-main';
  $base_class = array();
  $base_class[] = 'section-help';
  $base_class[] = 'section-help-break';
  $sub_base_class = array();
  $sub_base_class[] = 'section-help';

  $headers = array(
    0 => array(
      'markup' => 'Facilities Use Documentation',
      'attributes' => array(
        'class' => array(
          'header-1',
          'title',
        ),
      ),
    ),
  );

  $header_attributes = array();

  $section_class = $base_class;
  $section_class[] = 'section-help-main';
  $section_class[] = 'section-help-presentation-single';

  $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
  $markup .= $navigation_markup;
  $markup .= mfcs_help_render_headers('topic', $header_attributes, $headers, 0);
  unset($section_class);

  $headers = array(
    0 => array(
      'markup' => '',
      'attributes' => array(
        'class' => array(
          'header-2',
          'section',
        ),
      ),
    ),
  );

  $section_id = 'help-documentation-topic';
  $base_class[] = 'section-help-child';
  $base_class[] = 'section-help-documentation';


  // prepare section information.
  $section_information = array(
    'id' => 'mfcs-section-help-main',
    'class' => NULL,
    'headers' => $headers,
    'header_attributes' => array(
    ),
    'navigation' => NULL,
    'start_count' => NULL,
  );


  // glossary
  $section_information['headers'] = $headers;
  $section_information['headers'][0]['markup'] = 'Glossary';
  $section_information['id'] = $section_id . '-glossary';
  $section_information['class'] = $base_class;
  $section_information['class'][] = 'section-help-topic-glossary';
  $section_information['start_count'] = 1;

  $markup .= mfcs_help_topic_glossary($allowed_topics, $section_information, TRUE);


  // introduction
  if (in_array('introduction', $allowed_topics)) {
    $section_information['headers'][0]['markup'] = 'Introduction';
    $section_information['id'] = $section_id . '-introduction';
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-topic-introduction';
    $section_information['start_count'] = 1;

    $markup .= mfcs_help_topic_introduction($allowed_topics, $section_information, TRUE);
  }


  // requests
  if (in_array('requests', $allowed_topics)) {
    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Requests';
    $section_information['id'] = $section_id . '-requests';
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-topic-requests';
    $section_information['start_count'] = 1;

    $markup .= mfcs_help_topic_requests($allowed_topics, $section_information, TRUE, FALSE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Faculty, Staff, and Athletics Requests';
    $section_information['id'] = $section_id . '-requests-section-employee';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-requests-section-employee';
    $section_information['class'][] = 'no_first_of_type';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_requests_section_employee($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Student Organization Requests';
    $section_information['id'] = $section_id . '-requests-section-student';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-requests-section-student';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_requests_section_student_organization($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Camps Requests';
    $section_information['id'] = $section_id . '-requests-section-camps';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-requests-section-camps';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_requests_section_camps($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'External Requests';
    $section_information['id'] = $section_id . '-requests-section-external';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-requests-section-external';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_requests_section_external($allowed_topics, $section_information, TRUE);

    // close the requests section.
    $markup .= '</section>';
  }


  // review
  if (in_array('review', $allowed_topics)) {
    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Review';
    $section_information['id'] = $section_id . '-review';
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-topic-review';
    $section_information['start_count'] = 1;

    $markup .= mfcs_help_topic_review($allowed_topics, $section_information, TRUE, FALSE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Venue Available Step';
    $section_information['id'] = $section_id . '-review-section-venue_available';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-review-section-venue_available';
    $section_information['class'][] = 'no_first_of_type';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_review_section_venue_available($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Review Step';
    $section_information['id'] = $section_id . '-review-section-venue_available';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-review-section-venue_available';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_review_section_review($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Final Decision Step';
    $section_information['id'] = $section_id . '-review-section-venue_available';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-review-section-venue_available';
    $section_information['start_count'] = 2;
    $markup .= mfcs_help_topic_review_section_final_decision($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Completed Step';
    $section_information['id'] = $section_id . '-review-section-venue_available';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-review-section-venue_available';
    $section_information['start_count'] = 2;
    $markup .= mfcs_help_topic_review_section_completed($allowed_topics, $section_information, TRUE);

    // close the review section.
    $markup .= '</section>';
  }


  // manage
  if (in_array('manage', $allowed_topics)) {
    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Manage';
    $section_information['id'] = $section_id . '-manage';
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-topic-manage';
    $section_information['start_count'] = 1;

    $markup .= mfcs_help_topic_manage($allowed_topics, $section_information, TRUE, FALSE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Requests Management';
    $section_information['id'] = $section_id . '-manage-section-requests';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-manage-section-requests';
    $section_information['class'][] = 'no_first_of_type';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_manage_section_requests($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Users Management';
    $section_information['id'] = $section_id . '-manage-section-users';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-manage-section-users';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_manage_section_users($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Problems Management';
    $section_information['id'] = $section_id . '-manage-section-problems';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-manage-section-problems';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_manage_section_problems($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Availability Management';
    $section_information['id'] = $section_id . '-manage-section-availability';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-manage-section-availability';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_manage_section_availability($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Logs Management';
    $section_information['id'] = $section_id . '-manage-section-logs';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-manage-section-logs';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_manage_section_logs($allowed_topics, $section_information, TRUE);

    // close the manage section.
    $markup .= '</section>';
  }

  // email
  if (in_array('email', $allowed_topics)) {
    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'E-Mail';
    $section_information['id'] = $section_id . '-email';
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-topic-email';
    $section_information['start_count'] = 1;

    $markup .= mfcs_help_topic_email($allowed_topics, $section_information, TRUE, FALSE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Request E-Mail';
    $section_information['id'] = $section_id . '-email-section-request';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-email-section-request';
    $section_information['class'][] = 'no_first_of_type';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_email_section_request($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Advanced E-Mail';
    $section_information['id'] = $section_id . '-email-section-advanced';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-email-section-advanced';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_email_section_advanced($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Request Status Codes';
    $section_information['id'] = $section_id . '-email-section-advanced-sub_section-request_status_codes';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-email-section-advanced-sub_section-request_status_codes';
    $section_information['start_count'] = 3;

    $markup .= mfcs_help_topic_email_section_advanced_sub_section_status_codes($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Review Step Codes';
    $section_information['id'] = $section_id . '-email-section-advanced-sub_section-review_step_codes';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-email-section-advanced-sub_section-review_step_codes';
    $section_information['start_count'] = 3;

    $markup .= mfcs_help_topic_email_section_advanced_sub_section_step_codes($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Request Classification Codes';
    $section_information['id'] = $section_id . '-email-section-advanced-sub_section-request_classification_codes';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-email-section-advanced-sub_section-request_classification_codes';
    $section_information['start_count'] = 3;

    $markup .= mfcs_help_topic_email_section_advanced_sub_section_classification_codes($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Request Type Codes';
    $section_information['id'] = $section_id . '-email-section-advanced-sub_section-request_type_codes';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-email-section-advanced-sub_section-request_type_codes';
    $section_information['start_count'] = 3;

    $markup .= mfcs_help_topic_email_section_advanced_sub_section_request_type_codes($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'E-Mail Type Codes';
    $section_information['id'] = $section_id . '-email-section-advanced-sub_section-email_type_codes';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-email-section-advanced-sub_section-email_type_codes';
    $section_information['start_count'] = 3;

    $markup .= mfcs_help_topic_email_section_advanced_sub_section_email_type_codes($allowed_topics, $section_information, TRUE);

    // close the email section.
    $markup .= '</section>';
  }

  // calendar
  if (in_array('calendar', $allowed_topics)) {
    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Calendar';
    $section_information['id'] = $section_id . '-calendar';
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-topic-calendar';
    $section_information['start_count'] = 1;

    $markup .= mfcs_help_topic_calendar($allowed_topics, $section_information, TRUE, FALSE);

    // close the calendar section.
    $markup .= '</section>';
  }


  // frequently asked questions
  if (in_array('faq', $allowed_topics)) {
    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Frequently Asked Questions';
    $section_information['id'] = $section_id . '-faq';
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-topic-faq';
    $section_information['start_count'] = 1;

    $markup .= mfcs_help_topic_faq($allowed_topics, $section_information, TRUE);
  }


  // terminology
  if (in_array('terminology', $allowed_topics)) {
    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Terminology';
    $section_information['id'] = $section_id . '-terminology';
    $section_information['class'] = $base_class;
    $section_information['class'][] = 'section-help-topic-terminology';
    $section_information['start_count'] = 1;

    $markup .= mfcs_help_topic_terminology($allowed_topics, $section_information, TRUE, FALSE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Facilities Use Terminology';
    $section_information['id'] = $section_id . '-terminology-section-facilities_use';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-terminology-section-facilities_use';
    $section_information['class'][] = 'section-help-break';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_terminology_section_facilities_use($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'System Terminology';
    $section_information['id'] = $section_id . '-terminology-section-system';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-terminology-section-system';
    $section_information['class'][] = 'section-help-break';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_terminology_section_system($allowed_topics, $section_information, TRUE);

    $section_information['headers'] = $headers;
    $section_information['headers'][0]['markup'] = 'Documentation Terminology';
    $section_information['id'] = $section_id . '-terminology-section-documentation';
    $section_information['class'] = $sub_base_class;
    $section_information['class'][] = 'section-help-topic-terminology-section-documentation';
    $section_information['class'][] = 'section-help-break';
    $section_information['start_count'] = 2;

    $markup .= mfcs_help_topic_terminology_section_documentation($allowed_topics, $section_information, TRUE);

    // close the terminology section.
    $markup .= '</section>';
  }

  // close the main section.
  $markup .= '</section>';

  return $markup;
}

/**
 * Returns the given markup as a PDF download.
 *
 * @param string $filename
 *   The name of the file to use.
 * @param string $title
 *   The title of the dpcument.
 * @param string $markup
 *   The HTML markup to convert to a pdf.
 *
 * @return
 *   Does not return.
 */
function mfcs_help_0_page_presentation_pdf($filename, $title, $markup) {
  if (!is_string($filename)) {
    cf_error::invalid_string('filename');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_string($title)) {
    cf_error::invalid_string('title');

    drupal_not_found();
    drupal_exit();
  }

  if (!is_string($markup)) {
    cf_error::invalid_string('markup');

    drupal_not_found();
    drupal_exit();
  }

  $page['html']['content'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );
  unset($markup);

  $renderred = drupal_render($page);
  $html = drupal_render_page($renderred);
  unset($renderred);

  mfcs_include(MFCS_INCLUDE_OUTPUT);

  $parameters = array(
    'page_title' => FALSE,
    'title' => $title,
  );

  $contained = mfcs_output_print_page_view($html, $parameters);

  if (!is_string($contained)) {
    drupal_not_found();
    drupal_exit();
  }

  $result = mfcs_request_pdf_0_page_using_wkhtmltopdf_generic($filename, $contained, FALSE);
  if ($result) {
    exit();
  }

  drupal_not_found();
  drupal_exit();
}

/**
 * Determines the base class from the topic/section/sub_section.
 *
 * @param string|null $topic
 *   When NULL, represents the default topic. Otherwise, this is a string
 *   representing the topic to reference.
 * @param string|null $section
 *   When NULL, represents the default section. Otherwise, this is a string
 *   representing the section to reference.
 * @param string|null $topic
 *   When NULL, represents the default sub_section. Otherwise, this is a string
 *   representing the sub_section to reference.
 *
 * @return array
 *   An array of the base class.
 */
function mfcs_help_base_class($topic = NULL, $section = NULL, $sub_section = NULL) {
  if (!is_null($topic) && !is_string($topic)) {
    cf_error::invalid_string('topic');

    return array();
  }

  if (!is_null($section) && !is_string($section)) {
    cf_error::invalid_string('section');

    return array();
  }

  if (!is_null($sub_section) && !is_string($sub_section)) {
    cf_error::invalid_string('sub_section');

    return array();
  }

  $base_class = array('section-help-main', 'section-help');
  if (is_null($topic)) {
    $base_class[] = 'NULL-topic';
    $base_class[] = 'NULL-section';
    $base_class[] = 'NULL-sub_section';
  }
  elseif (is_null($section)) {
    $base_class[] = 'topic-' . cf_theme_safe_css_string_part($topic);
    $base_class[] = 'NULL-section';
    $base_class[] = 'NULL-sub_section';
  }
  elseif (is_null($sub_section)) {
    $base_class[] = 'topic-' . cf_theme_safe_css_string_part($topic);
    $base_class[] = 'section-' . cf_theme_safe_css_string_part($section);
    $base_class[] = 'NULL-sub_section';
  }
  else {
    $base_class[] = 'topic-' . cf_theme_safe_css_string_part($topic);
    $base_class[] = 'section-' . cf_theme_safe_css_string_part($section);
    $base_class[] = 'sub_section-' . cf_theme_safe_css_string_part($sub_section);
  }

  return $base_class;
}

/**
 * Converts a given text message into a more title-friendly format.
 *
 * Underscores are converted to spaces.
 * Dashes have spaces added to either side.
 *
 * @return string
 *   A converted string.
 */
function mfcs_help_url_cleanup_for_title($text) {
  if (!is_string($text)) {
    return '';
  }

  // remove leading and trailing spaces.
  $text = trim($text);

  // add spacing before and after dashes.
  $text = str_replace('-', ' - ', $text);

  // replace underscores with spaces
  $text = str_replace('_', ' ', $text);

  // replace all consecutive spaces with a single space (also replacing tabs and other whitespace with a single space).
  $text = preg_replace('/\s+/i', ' ', $text);

  // convert each word to have the first character uppercased.
  $parts = explode(' ', $text);

  if (empty($parts)) {
    return ucfirst($text);
  }

  $text = NULL;
  foreach ($parts as $part) {
    if (is_null($text)) {
      $text = '';
    }
    else {
      $text .= ' ';
    }

    $text .= ucfirst($part);
  }
  unset($part);
  unset($parts);

  return $text;
}

/**
 * Determine the next, previous, and top paths.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param string|null $topic
 *   When NULL, represents the default topic. Otherwise, this is a string
 *   representing the topic to reference.
 * @param string|null $section
 *   When NULL, represents the default section. Otherwise, this is a string
 *   representing the section to reference.
 * @param string|null $topic
 *   When NULL, represents the default sub_section. Otherwise, this is a string
 *   representing the sub_section to reference.
 *
 * @return array
 *   An array containing the following keys:
 *   - top: Either NULL or a string. The string represents the top/parent url path.
 *   - next: Either NULL or a string. The string represents the next url path within the same depth.
 *   - previous: Either NULL or a string. The string represents the previous url path within the same depth.
 *   - tooltip-top: Either NULL or a string. The string represents the top/parent url tooltip.
 *   - tooltip-next: Either NULL or a string. The string represents the next url tooltip within the same depth.
 *   - tooltip-previous: Either NULL or a string. The string represents the previous url tooltip within the same depth.
 */
function mfcs_help_determine_next_previous_top($allowed_topics, $topic = NULL, $section = NULL, $sub_section = NULL) {
  $determined = array(
    'top' => NULL,
    'next' => NULL,
    'previous' => NULL,
    'tooltip-top' => NULL,
    'tooltip-next' => NULL,
    'tooltip-previous' => NULL,
  );

  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return $determined;
  }

  if (!is_null($topic) && !is_string($topic)) {
    cf_error::invalid_string('topic');

    return $determined;
  }

  if (!is_null($section) && !is_string($section)) {
    cf_error::invalid_string('section');

    return $determined;
  }

  if (!is_null($sub_section) && !is_string($sub_section)) {
    cf_error::invalid_string('sub_section');

    return $determined;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  // at the topmost path, such as glossary, so there is nowhere to go.
  if (is_null($topic)) {
    return $determined;
  }

  if ($topic == 'introduction') {
    $determined['top'] = $base_path . 'help-0' . $url_arguments;
    $determined['tooltip-top'] = 'Glossary';

    $determined['next'] = $base_path . 'help-0/requests' . $url_arguments;
    $determined['tooltip-next'] = 'Documentation: Requests';
  }
  elseif ($topic == 'requests') {
    $determined['top'] = $base_path . 'help-0' . $url_arguments;
    $determined['tooltip-top'] = 'Glossary';

    if ($section == 'employee') {
      $determined['top'] = $base_path . 'help-0/requests' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Requests';

      $determined['next'] = $base_path . 'help-0/requests/student' . $url_arguments;
      $determined['tooltip-next'] = 'Section: Student Organization Requests';
    }
    elseif ($section == 'student') {
      $determined['top'] = $base_path . 'help-0/requests' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Requests';

      $determined['next'] = $base_path . 'help-0/requests/camps' . $url_arguments;
      $determined['tooltip-next'] = 'Section: Camps Requests';

      $determined['previous'] = $base_path . 'help-0/requests/employee' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: Faculty, Staff, and Athletics Requests';
    }
    elseif ($section == 'camps') {
      $determined['top'] = $base_path . 'help-0/requests' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Requests';

      $determined['next'] = $base_path . 'help-0/requests/external' . $url_arguments;
      $determined['tooltip-next'] = 'Section: External Requests';

      $determined['previous'] = $base_path . 'help-0/requests/student' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: Student Organization Requests';
    }
    elseif ($section == 'external') {
      $determined['top'] = $base_path . 'help-0/requests' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Requests';

      $determined['previous'] = $base_path . 'help-0/requests/camps' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: Camps Requests';
    }
    else {
      if (in_array('review', $allowed_topics)) {
        $determined['next'] = $base_path . 'help-0/review' . $url_arguments;
        $determined['tooltip-next'] = 'Documentation: Review';
      }
      elseif (in_array('manage', $allowed_topics)) {
        $determined['next'] = $base_path . 'help-0/manage' . $url_arguments;
        $determined['tooltip-next'] = 'Documentation: Manage';
      }
      else {
        $determined['next'] = $base_path . 'help-0/email' . $url_arguments;
        $determined['tooltip-next'] = 'Documentation: E-Mail';
      }

      $determined['previous'] = $base_path . 'help-0/introduction' . $url_arguments;
      $determined['tooltip-previous'] = 'Documentation: Introduction';
    }
  }
  elseif ($topic == 'review' && in_array('review', $allowed_topics)) {
    $determined['top'] = $base_path . 'help-0' . $url_arguments;
    $determined['tooltip-top'] = 'Glossary';

    if ($section == 'venue_available') {
      $determined['top'] = $base_path . 'help-0/review' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Review';

      $determined['next'] = $base_path . 'help-0/review/review' . $url_arguments;
      $determined['tooltip-next'] = 'Section: Review Step';
    }
    elseif ($section == 'review') {
      $determined['top'] = $base_path . 'help-0/review' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Review';

      $determined['next'] = $base_path . 'help-0/review/final_decision' . $url_arguments;
      $determined['tooltip-next'] = 'Section: Final Decision Step';

      $determined['previous'] = $base_path . 'help-0/review/venue_available' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: Venue Available Step';
    }
    elseif ($section == 'final_decision') {
      $determined['top'] = $base_path . 'help-0/review' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Review';

      $determined['next'] = $base_path . 'help-0/review/completed' . $url_arguments;
      $determined['tooltip-next'] = 'Section: Completed Step';

      $determined['previous'] = $base_path . 'help-0/review/review' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: Review Step';
    }
    elseif ($section == 'completed') {
      $determined['top'] = $base_path . 'help-0/review' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Review';

      $determined['previous'] = $base_path . 'help-0/review/final_decision' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: Final Decision Step';
    }
    else {
      if (in_array('manage', $allowed_topics)) {
        $determined['next'] = $base_path . 'help-0/manage' . $url_arguments;
        $determined['tooltip-next'] = 'Documentation: Manage';
      }
      else {
        $determined['next'] = $base_path . 'help-0/email' . $url_arguments;
        $determined['tooltip-next'] = 'Documentation: E-Mail';
      }

      $determined['previous'] = $base_path . 'help-0/requests' . $url_arguments;
      $determined['tooltip-previous'] = 'Documentation: Requests';
    }
  }
  elseif ($topic == 'manage' && in_array('manage', $allowed_topics)) {
    $determined['top'] = $base_path . 'help-0' . $url_arguments;
    $determined['tooltip-top'] = 'Glossary';

    if ($section == 'requests') {
      $determined['top'] = $base_path . 'help-0/manage' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Manage';

      $determined['next'] = $base_path . 'help-0/manage/users' . $url_arguments;
      $determined['tooltip-next'] = 'Section: Users Management';
    }
    elseif ($section == 'users') {
      $determined['top'] = $base_path . 'help-0/manage' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Manage';

      $determined['next'] = $base_path . 'help-0/manage/problems' . $url_arguments;
      $determined['tooltip-next'] = 'Section: Problems Management';

      $determined['previous'] = $base_path . 'help-0/manage/requests' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: Requests Management';
    }
    elseif ($section == 'problems') {
      $determined['top'] = $base_path . 'help-0/manage' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Manage';

      $determined['next'] = $base_path . 'help-0/manage/availability' . $url_arguments;
      $determined['tooltip-next'] = 'Section: Availability Management';

      $determined['previous'] = $base_path . 'help-0/manage/users' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: Users Management';
    }
    elseif ($section == 'availability') {
      $determined['top'] = $base_path . 'help-0/manage' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Manage';

      $determined['next'] = $base_path . 'help-0/manage/logs' . $url_arguments;
      $determined['tooltip-next'] = 'Section: Logs Management';

      $determined['previous'] = $base_path . 'help-0/manage/problems' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: Problems Management';
    }
    elseif ($section == 'logs') {
      $determined['top'] = $base_path . 'help-0/manage' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Manage';

      $determined['previous'] = $base_path . 'help-0/manage/availability' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: Availability Management';
    }
    else {
      $determined['next'] = $base_path . 'help-0/email' . $url_arguments;
      $determined['tooltip-next'] = 'Documentation: E-Mail';

      if (in_array('review', $allowed_topics)) {
        $determined['previous'] = $base_path . 'help-0/review' . $url_arguments;
        $determined['tooltip-previous'] = 'Documentation: Review';
      }
      else {
        $determined['previous'] = $base_path . 'help-0/requests' . $url_arguments;
        $determined['tooltip-previous'] = 'Documentation: Requests';
      }
    }
  }
  elseif ($topic == 'email') {
    $determined['top'] = $base_path . 'help-0' . $url_arguments;
    $determined['tooltip-top'] = 'Glossary';

    if ($section == 'request') {
      $determined['top'] = $base_path . 'help-0/email' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: E-Mail';

      $determined['next'] = $base_path . 'help-0/email/advanced' . $url_arguments;
      $determined['tooltip-next'] = 'Section: Advanced E-mail';
    }
    elseif ($section == 'advanced') {
      $determined['top'] = $base_path . 'help-0/email/advanced' . $url_arguments;
      $determined['tooltip-top'] = 'Section: Advanced E-mail';

      if ($sub_section == 'request_status_codes') {
        $determined['next'] = $base_path . 'help-0/email/advanced/review_step_codes' . $url_arguments;
        $determined['tooltip-next'] = 'Sub-Section: Review Step Codes';
      }
      elseif ($sub_section == 'review_step_codes') {
        $determined['next'] = $base_path . 'help-0/email/advanced/request_classification_codes' . $url_arguments;
        $determined['tooltip-next'] = 'Sub-Section: Request Classification Codes';

        $determined['previous'] = $base_path . 'help-0/email/advanced/request_status_codes' . $url_arguments;
        $determined['tooltip-previous'] = 'Sub-Section: Request Status Codes';
      }
      elseif ($sub_section == 'request_classification_codes') {
        $determined['next'] = $base_path . 'help-0/email/advanced/request_type_codes' . $url_arguments;
        $determined['tooltip-next'] = 'Sub-Section: Request Type Codes';

        $determined['previous'] = $base_path . 'help-0/email/advanced/review_step_codes' . $url_arguments;
        $determined['tooltip-previous'] = 'Sub-Section: Review Step Codes';
      }
      elseif ($sub_section == 'request_type_codes') {
        $determined['next'] = $base_path . 'help-0/email/advanced/email_type_codes' . $url_arguments;
        $determined['tooltip-next'] = 'Sub-Section: E-Mail Type Codes';

        $determined['previous'] = $base_path . 'help-0/email/advanced/request_classification_codes' . $url_arguments;
        $determined['tooltip-previous'] = 'Sub-Section: Request Calassification Codes';
      }
      elseif ($sub_section == 'email_type_codes') {
        $determined['previous'] = $base_path . 'help-0/email/advanced/request_type_codes' . $url_arguments;
        $determined['tooltip-previous'] = 'Sub-Section: Request Type Codes';
      }
      else {
        $determined['top'] = $base_path . 'help-0/email' . $url_arguments;
        $determined['tooltip-top'] = 'Documentation: E-Mail';

        $determined['previous'] = $base_path . 'help-0/email/request' . $url_arguments;
        $determined['tooltip-previous'] = 'Section: Request E-mail';
      }
    }
    else {
      $determined['next'] = $base_path . 'help-0/calendar' . $url_arguments;
      $determined['tooltip-next'] = 'Documentation: Calendar';

      if (in_array('manage', $allowed_topics)) {
        $determined['previous'] = $base_path . 'help-0/manage' . $url_arguments;
        $determined['tooltip-previous'] = 'Documentation: Manage';
      }
      elseif (in_array('review', $allowed_topics)) {
        $determined['previous'] = $base_path . 'help-0/review' . $url_arguments;
        $determined['tooltip-previous'] = 'Documentation: Review';
      }
      else {
        $determined['previous'] = $base_path . 'help-0/requests' . $url_arguments;
        $determined['tooltip-previous'] = 'Documentation: Requests';
      }
    }
  }
  elseif ($topic == 'calendar') {
    $determined['top'] = $base_path . 'help-0' . $url_arguments;
    $determined['tooltip-top'] = 'Glossary';

    $determined['next'] = $base_path . 'help-0/faq' . $url_arguments;
    $determined['tooltip-next'] = 'Documentation: Frequently Asked Questions';

    $determined['previous'] = $base_path . 'help-0/email' . $url_arguments;
    $determined['tooltip-previous'] = 'Documentation: E-Mail';
  }
  elseif ($topic == 'faq') {
    $determined['top'] = $base_path . 'help-0' . $url_arguments;
    $determined['tooltip-top'] = 'Glossary';

    $determined['next'] = $base_path . 'help-0/terminology' . $url_arguments;
    $determined['tooltip-next'] = 'Documentation: Terminology';

    $determined['previous'] = $base_path . 'help-0/calendar' . $url_arguments;
    $determined['tooltip-previous'] = 'Documentation: Calendar';
  }
  elseif ($topic == 'terminology') {
    $determined['top'] = $base_path . 'help-0' . $url_arguments;
    $determined['tooltip-top'] = 'Glossary';

    if ($section == 'facilities_use') {
      $determined['top'] = $base_path . 'help-0/terminology' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Terminology';

      $determined['next'] = $base_path . 'help-0/terminology/system' . $url_arguments;
      $determined['tooltip-next'] = 'Section: System Terminology';
    }
    elseif ($section == 'system') {
      $determined['top'] = $base_path . 'help-0/terminology' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Terminology';

      $determined['next'] = $base_path . 'help-0/terminology/documentation' . $url_arguments;
      $determined['tooltip-next'] = 'Section: Documentation Terminology';

      $determined['previous'] = $base_path . 'help-0/terminology/facilities_use' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: Facilities Use Terminology';
    }
    elseif ($section == 'documentation') {
      $determined['top'] = $base_path . 'help-0/terminology' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Terminology';

      $determined['previous'] = $base_path . 'help-0/terminology/system' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: System Terminology';
    }
    else {
      $determined['previous'] = $base_path . 'help-0/faq' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: Frequently Asked Questions';
    }
  }

  return $determined;
}

/**
 * Provides a "Not Found" section for the appropriate topic/section/sub_section.
 *
 * @todo: this needs to send the appropriate HTTP header, such as 404.
 *
 * @param string|null $topic
 *   When NULL, represents the default topic. Otherwise, this is a string
 *   representing the topic to reference.
 * @param string|null $section
 *   When NULL, represents the default section. Otherwise, this is a string
 *   representing the section to reference.
 * @param string|null $topic
 *   When NULL, represents the default sub_section. Otherwise, this is a string
 *   representing the sub_section to reference.
 *
 * @return string|bool
 *   Return the HTML markup on success, otherwise FALSE is returned.
 *
 * @see: mfcs_help_0_page()
 */
function mfcs_help_topic_not_found($topic = NULL, $section = NULL, $sub_section = NULL) {
  if (!is_null($topic) && !is_string($topic)) {
    cf_error::invalid_string('topic');

    return FALSE;
  }

  if (!is_null($section) && !is_string($section)) {
    cf_error::invalid_string('section');

    return FALSE;
  }

  if (!is_null($sub_section) && !is_string($sub_section)) {
    cf_error::invalid_string('sub_section');

    return FALSE;
  }

  $headers = array(
    0 => array('markup' => 'No Valid Help Path Found'),
  );

  if (is_null($topic)) {
    $first_line = 'The help documentation you were looking for was not found.<br>';
  }
  elseif (is_null($section)) {
    $headers[0]['markup'] = 'No Valid Topic Found';
    $first_line = 'The topic "' . $topic . '" was not found in the help documentation.<br>';
  }
  elseif (is_null($sub_section)) {
    $headers[0]['markup'] = 'No Valid Section Found';
    $first_line = 'The section "' . $section . '" was not found under topic "' . $topic . '" of the help documentation.<br>';
  }
  else {
    $headers[0]['markup'] = 'No Valid Sub-Section Found';
    $first_line = 'The sub-section "' . $sub_section . '" was not found under section "' . $section . '" of topic "' . $topic . '" of the help documentation.<br>';
  }

  $section_class = mfcs_help_base_class($topic, $section, $sub_section);
  $section_class[] = 'section-help-topic_not_found';

  $markup = '<section id="mfcs-section-help-main" class="' . implode(' ', $section_class) . '">';
  $markup .= mfcs_help_render_headers('section-', array(), $headers, 1);
  $markup .= '<div class="paragraph">';
  $markup .= $first_line;
  $markup .= 'Perhaps you have specified an invalid url. ';
  $markup .= '</div>';

  $markup .= '</section>';

  return $markup;
}

/**
 * Renders HTML markup for HTML5 headers.
 *
 * Note: the HTML5 standard only goes up to 6 header tags. having an arbitrary
 * restriction like this is ridiculous. This will generate header tags as
 * unlimited.
 *
 * @param string $headers_id
 *   If a non-empty string, then this is the id to be attatched to the
 *   outermost header tag.
 * @param array $attributes
 *   (optional) An array of classes to attatch to the outermost header tag. The
 *   'id' attribute will be unset.
 * @param array $headers
 *   (optional) An array of header tags, each header with its own array structure:
 *   - 'id' => (optional) id string.
 *   - 'attributes' => (optional) tag attributes, 'id' attribute is unset.
 *   - 'markup' => The HTML markup or plain text to display inside the header.
 * @param int $start_count
 *   (optional) The header counting start point. The first header written will
 *   be this value+1. Therefore, to start with <h1>, this must be 0, and so on.
 *
 * @param string|bool
 *   The renderred HTML5 header tag markup or FALSE on error.
 *
 * @see: mfcs_help_0_page()
 */
function mfcs_help_render_headers($headers_id, $attributes = array(), $headers = array(), $start_count = 0) {
  if (!is_string($headers_id)) {
    cf_error::invalid_string('headers_id');

    return FALSE;
  }

  if (!is_array($attributes)) {
    cf_error::invalid_array('attributes');

    return FALSE;
  }

  if (!is_array($headers)) {
    cf_error::invalid_array('headers');

    return FALSE;
  }

  if (!cf_is_integer($start_count)) {
    cf_error::invalid_integer('start_count');

    return FALSE;
  }

  // Only generate headers markup when headers are defined.
  if (empty($headers)) {
    return '';
  }

  if (!isset($attributes['class']) || !is_array($attributes['class'])) {
    $attributes['class'] = array();
  }

  if (!in_array('help-headers', $attributes['class'])) {
    $attributes['class'][] = 'help-headers';
  }

  $headers_group_class = array(
    'help-headers_group',
  );

  $even_odd = FALSE;
  $treven_trodd = FALSE;

  $id_string = '';
  unset($attributes['id']);
  if (!empty($headers_id)) {
    $id_string = 'id="' . $headers_id . '" ';
    $attributes['id'] = $headers_id;
  }

  $attributes_string = ' ';
  foreach ($attributes as $name => $data) {
    if (strlen($name) == 0) continue;

    if (is_string($data)) {
      $fixed = str_replace('"', '\'', $data);

      if (strlen($fixed) > 0) {
        $attributes_string .= $name . '="' . $fixed . '" ';
      }
    }
    elseif (is_array($data) && !empty($data)) {
      $fixed = str_replace('"', '\'', implode(' ', $data));

      if (strlen($fixed) > 0) {
        $attributes_string .= $name . '="' . $fixed . '" ';
      }

      unset($fixed);
    }
  }

  $attributes_string = trim($attributes_string);
  if (!empty($attributes_string)) {
    $attributes_string = ' ' . $attributes_string;
  }

  $markup = '<header' . $attributes_string . '>';
  $markup .= '<hgroup class="' . implode(' ', $headers_group_class) . '">';

  $headers_count = $start_count;
  foreach ($headers as $header_id => $header) {
    if (empty($header['markup'])) {
      // do not allow for empty header tags.
      continue;
    }

    $headers_count++;

    $header_attributes = array();
    if (isset($header['attributes']) && is_array($header['attributes'])) {
      $header_attributes = $header['attributes'];
    }

    $header_id_string = '';
    unset($header_attributes['id']);
    if (!empty($header['id'])) {
      $header_id_string = 'id="' . $header['id'] . '" ';
      $header_attributes['id'] = $header['id'];
    }

    if (!isset($header['attributes']['class']) || !is_array($header['attributes']['class'])) {
      $header['attributes']['class'] = array();
    }

    if (!in_array('tag-header', $header['attributes']['class'])) {
      $header['attributes']['class'][] = 'header';
    }

    if (!in_array('delta-' . $headers_count, $header['attributes']['class'])) {
      $header['attributes']['class'][] = 'delta-' . $headers_count;
    }

    $attributes_string = ' ';
    foreach ($header_attributes as $name => $data) {
      if (is_string($data)) {
        $attributes_string .= $name . '="' . str_replace('"', '\'', $data) . '" ';
      }
      elseif (is_array($data)) {
        $attributes_string .= $name . '="' . str_replace('"', '\'', implode(' ', $data)) . '" ';
      }
    }

    if ($attributes_string == ' ') {
      $attributes_string = '';
    }

    $markup .= '<h' . $headers_count . $attributes_string . '>' . $header['markup'] . '</h' . $headers_count . '>';
  }

  $markup .= '</hgroup>';
  $markup .= '</header>';

  if ($headers_count > 0) {
    return $markup;
  }

  // when there are no header tags, then return no markup.
  return '';
}

/**
 * Provide a list of topics the user is allowed to access.
 *
 * @param object $user
 *   The loaded drupal user object.
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 *
 * @return array|bool
 *   An array of allowed topics, or FALSE on error.
 */
function mfcs_help_0_allowed_topics($user) {
  if (!is_object($user)) {
    cf_error::invalid_object('user');
    return FALSE;
  }

  // documentation presented will be filtered based on user's access roles.
  // this will help simplify the documentation for less technical users.
  $is_administer = user_access('mfcs administer', $user);
  $is_manager = user_access('mfcs manage', $user);
  $is_reviewer = user_access('mfcs review', $user);

  $topics = array();

  // user guides
  $topics[] = 'request';
  $topics[] = 'request-create';
  $topics[] = 'request-edit';
  $topics[] = 'request-cancel';
  $topics[] = 'request-copy';
  $topics[] = 'request-find';
  $topics[] = 'calendar-import';
  $topics[] = 'calendar-export';

  // documentation
  $topics[] = 'introduction';
  $topics[] = 'requests';
  $topics[] = 'requests-create';
  $topics[] = 'requests-edit';
  $topics[] = 'requests-cancel';
  $topics[] = 'requests-copy';
  $topics[] = 'requests-find';
  $topics[] = 'email';
  $topics[] = 'calendar';
  $topics[] = 'faq';
  $topics[] = 'terminology';

  // add reviewing-specific topics.
  if ($is_reviewer || $is_manager || $is_administer) {
    // user guides
    $topics[] = 'request-review';

    // documentation
    $topics[] = 'review';
  }

  // add managing-specific topics.
  if ($is_manager || $is_administer) {
    // user guides
    $topics[] = 'request-manage';

    // documentation
    $topics[] = 'manage';
  }

  // add administer-specific topics.
  if ($is_administer) {
  }

  return $topics;
}

/**
 * Provides the main/primary topic for the help page.
 *
 * This should be used as the start/home page for help/documentation.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 * @param bool $user_guides
 *   (optional) When TRUE, user guides will be linked as inline when $as_inline is TRUE.
 *   When FALSE, documentation will be linked as inline when $as_inline is TRUE.
 *
 * @return string|bool
 *   Return the HTML markup on success, otherwise FALSE is returned.
 *
 * @see: mfcs_help_0_page()
 */
function mfcs_help_topic_glossary($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE, $user_guides = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  if (!is_bool($user_guides)) {
    cf_error::invalid_bool('user_guides');
    $user_guides = FALSE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-glossary', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $sub_section_class = array(
    'section',
  );


  // create a list of links, when $as_inline is TRUE, then the urls can be more easily altered.
  $links = array(
    'request-create' => array(
      'url' => $base_path . 'help-0/request-create' . $url_arguments,
      'tooltip' => 'User Guide: Create a Request',
      'text' => 'Create a Request',
      'class' => 'item-link no_print_links_as_link',
    ),
    'request-create/employee' => array(
      'url' => $base_path . 'help-0/request-create/employee' . $url_arguments,
      'tooltip' => 'User Guide: Create Faculty, Staff, and Athletics Requests',
      'text' => 'Faculty, Staff, and Athletics',
      'class' => 'item-link no_print_links_as_link',
    ),
    'request-create/student' => array(
      'url' => $base_path . 'help-0/request-create/student' . $url_arguments,
      'tooltip' => 'User Guide: Create Student Organization Requests',
      'text' => 'Student Organization',
      'class' => 'item-link no_print_links_as_link',
    ),
    'request-create/camps' => array(
      'url' => $base_path . 'help-0/request-create/camps' . $url_arguments,
      'tooltip' => 'User Guide: Create Camps Requests',
      'text' => 'Create Camps Requests',
      'class' => 'item-link no_print_links_as_link',
    ),
    'request-create/external' => array(
      'url' => $base_path . 'help-0/request-create/external' . $url_arguments,
      'tooltip' => 'User Guide: Create External Requests',
      'text' => 'Create External Requests',
      'class' => 'item-link no_print_links_as_link',
    ),
    'request-edit' => array(
      'url' => $base_path . 'help-0/request-edit' . $url_arguments,
      'tooltip' => 'User Guide: Edit a Request',
      'text' => 'Edit a Request',
      'class' => 'item-link no_print_links_as_link',
    ),
    'request-cancel' => array(
      'url' => $base_path . 'help-0/request-cancel' . $url_arguments,
      'tooltip' => 'User Guide: Cancel or Uncancel a Request',
      'text' => 'Cancel or Uncancel a Request',
      'class' => 'item-link no_print_links_as_link',
    ),
    'request-copy' => array(
      'url' => $base_path . 'help-0/request-copy' . $url_arguments,
      'tooltip' => 'User Guide: Copy a Request',
      'text' => 'Copy a Request',
      'class' => 'item-link no_print_links_as_link',
    ),
    'request-find' => array(
      'url' => $base_path . 'help-0/request-find' . $url_arguments,
      'tooltip' => 'User Guide: Find a Request',
      'text' => 'Find a Request',
      'class' => 'item-link no_print_links_as_link',
    ),
    'request-review' => array(
      'url' => $base_path . 'help-0/request-review' . $url_arguments,
      'tooltip' => 'User Guide: Review a Request',
      'text' => 'Review a Request',
      'class' => 'item-link no_print_links_as_link',
    ),
    'request-manage' => array(
      'url' => $base_path . 'help-0/request-manage' . $url_arguments,
      'tooltip' => 'User Guide: Manage System',
      'text' => 'Manage System',
      'class' => 'item-link no_print_links_as_link',
    ),
    'calendar-use' => array(
      'url' => $base_path . 'help-0/calendar-use' . $url_arguments,
      'tooltip' => 'User Guide: Using the Calendar',
      'text' => 'Using the Calendar',
      'class' => 'item-link no_print_links_as_link',
    ),
    'calendar-export' => array(
      'url' => $base_path . 'help-0/calendar-export' . $url_arguments,
      'tooltip' => 'User Guide: Exporting the Calendar',
      'text' => 'Exporting the Calendar',
      'class' => 'item-link no_print_links_as_link',
    ),
    'introduction' => array(
      'url' => $base_path . 'help-0/introduction' . $url_arguments,
      'tooltip' => 'Documentation: Introduction',
      'text' => 'Introduction',
      'class' => 'item-link no_print_links_as_link',
    ),
    'requests' => array(
      'url' => $base_path . 'help-0/requests' . $url_arguments,
      'tooltip' => 'Documentation: Requests',
      'text' => 'Requests',
      'class' => 'item-link no_print_links_as_link',
    ),
    'requests/employee' => array(
      'url' => $base_path . 'help-0/requests/employee' . $url_arguments,
      'tooltip' => 'Section: Faculty, Staff, and Athletics Requests',
      'text' => 'Faculty, Staff, and Athletics',
      'class' => 'item-link no_print_links_as_link',
    ),
    'requests/student' => array(
      'url' => $base_path . 'help-0/requests/student' . $url_arguments,
      'tooltip' => 'Section: Student Organization Requests',
      'text' => 'Student Organization',
      'class' => 'item-link no_print_links_as_link',
    ),
    'requests/camps' => array(
      'url' => $base_path . 'help-0/requests/camps' . $url_arguments,
      'tooltip' => 'Section: Camps Requests',
      'text' => 'Camps',
      'class' => 'item-link no_print_links_as_link',
    ),
    'requests/external' => array(
      'url' => $base_path . 'help-0/requests/external' . $url_arguments,
      'tooltip' => 'Section: External',
      'text' => 'External',
      'class' => 'item-link no_print_links_as_link',
    ),
    'review' => array(
      'url' => $base_path . 'help-0/review' . $url_arguments,
      'tooltip' => 'Documentation: Review',
      'text' => 'Review',
      'class' => 'item-link no_print_links_as_link',
    ),
    'review/venue_available' => array(
      'url' => $base_path . 'help-0/review/venue_available' . $url_arguments,
      'tooltip' => 'Section: Venue Available Step',
      'text' => 'Venue Available',
      'class' => 'item-link no_print_links_as_link',
    ),
    'review/review' => array(
      'url' => $base_path . 'help-0/review/review' . $url_arguments,
      'tooltip' => 'Section: Review Step',
      'text' => 'Review',
      'class' => 'item-link no_print_links_as_link',
    ),
    'review/final_decision' => array(
      'url' => $base_path . 'help-0/review/final_decision' . $url_arguments,
      'tooltip' => 'Section: Final Decision Step',
      'text' => 'Final Decision',
      'class' => 'item-link no_print_links_as_link',
    ),
    'review/completed' => array(
      'url' => $base_path . 'help-0/review/completed' . $url_arguments,
      'tooltip' => 'Section: Completed Step',
      'text' => 'Completed',
      'class' => 'item-link no_print_links_as_link',
    ),
    'manage' => array(
      'url' => $base_path . 'help-0/manage' . $url_arguments,
      'tooltip' => 'Documentation: Manage',
      'text' => 'Manage',
      'class' => 'item-link no_print_links_as_link',
    ),
    'manage/requests' => array(
      'url' => $base_path . 'help-0/manage/requests' . $url_arguments,
      'tooltip' => 'Section: Requests',
      'text' => 'Requests',
      'class' => 'item-link no_print_links_as_link',
    ),
    'manage/users' => array(
      'url' => $base_path . 'help-0/manage/users' . $url_arguments,
      'tooltip' => 'Section: Users',
      'text' => 'Users',
      'class' => 'item-link no_print_links_as_link',
    ),
    'manage/problems' => array(
      'url' => $base_path . 'help-0/manage/problems' . $url_arguments,
      'tooltip' => 'Section: Problems',
      'text' => 'Problems',
      'class' => 'item-link no_print_links_as_link',
    ),
    'manage/availability' => array(
      'url' => $base_path . 'help-0/manage/availability' . $url_arguments,
      'tooltip' => 'Section: Availability',
      'text' => 'Availability',
      'class' => 'item-link no_print_links_as_link',
    ),
    'manage/logs' => array(
      'url' => $base_path . 'help-0/manage/logs' . $url_arguments,
      'tooltip' => 'Section: Logs',
      'text' => 'Logs',
      'class' => 'item-link no_print_links_as_link',
    ),
    'email' => array(
      'url' => $base_path . 'help-0/email' . $url_arguments,
      'tooltip' => 'Documentation: E-Mail',
      'text' => 'E-Mail',
      'class' => 'item-link no_print_links_as_link',
    ),
    'email/request' => array(
      'url' => $base_path . 'help-0/email/request' . $url_arguments,
      'tooltip' => 'Section: Request E-mail',
      'text' => 'Request',
      'class' => 'item-link no_print_links_as_link',
    ),
    'email/advanced' => array(
      'url' => $base_path . 'help-0/email/advanced' . $url_arguments,
      'tooltip' => 'Section: Advanced E-mail',
      'text' => 'Advanced',
      'class' => 'item-link no_print_links_as_link',
    ),
    'email/advanced/request_status_codes' => array(
      'url' => $base_path . 'help-0/email/advanced/request_status_codes' . $url_arguments,
      'tooltip' => 'Sub-Section: Request Status Codes',
      'text' => 'Request Status Codes',
      'class' => 'item-link no_print_links_as_link',
    ),
    'email/advanced/review_step_codes' => array(
      'url' => $base_path . 'help-0/email/advanced/review_step_codes' . $url_arguments,
      'tooltip' => 'Sub-Section: Review Step Codes',
      'text' => 'Review Step Codes',
      'class' => 'item-link no_print_links_as_link',
    ),
    'email/advanced/request_classification_codes' => array(
      'url' => $base_path . 'help-0/email/advanced/request_classification_codes' . $url_arguments,
      'tooltip' => 'Sub-Section: Request Classification Codes',
      'text' => 'Request Classification Codes',
      'class' => 'item-link no_print_links_as_link',
    ),
    'email/advanced/request_type_codes' => array(
      'url' => $base_path . 'help-0/email/advanced/request_type_codes' . $url_arguments,
      'tooltip' => 'Sub-Section: Request Type Codes',
      'text' => 'Request Type Codes',
      'class' => 'item-link no_print_links_as_link',
    ),
    'email/advanced/email_type_codes' => array(
      'url' => $base_path . 'help-0/email/advanced/email_type_codes' . $url_arguments,
      'tooltip' => 'Sub-Section: E-Mail Type Codes',
      'text' => 'E-Mail Type Codes',
      'class' => 'item-link no_print_links_as_link',
    ),
    'calendar' => array(
      'url' => $base_path . 'help-0/calendar' . $url_arguments,
      'tooltip' => 'Documentation: Calendar',
      'text' => 'Calendar',
      'class' => 'item-link no_print_links_as_link',
    ),
    'faq' => array(
      'url' => $base_path . 'help-0/faq' . $url_arguments,
      'tooltip' => 'Documentation: Frequently Asked Questions',
      'text' => 'Frequently Asked Questions',
      'class' => 'item-link no_print_links_as_link',
    ),
    'terminology' => array(
      'url' => $base_path . 'help-0/terminology' . $url_arguments,
      'tooltip' => 'Documentation: Terminology',
      'text' => 'Terminology',
      'class' => 'item-link no_print_links_as_link',
    ),
    'terminology/facilities_use' => array(
      'url' => $base_path . 'help-0/terminology/facilities_use' . $url_arguments,
      'tooltip' => 'Section: Facilities Use',
      'text' => 'Facilities Use',
      'class' => 'item-link no_print_links_as_link',
    ),
    'terminology/system' => array(
      'url' => $base_path . 'help-0/terminology/system' . $url_arguments,
      'tooltip' => 'Section: System',
      'text' => 'System',
      'class' => 'item-link no_print_links_as_link',
    ),
    'terminology/documentation' => array(
      'url' => $base_path . 'help-0/terminology/documentation' . $url_arguments,
      'tooltip' => 'Section: Documentation',
      'text' => 'Documentation',
      'class' => 'item-link no_print_links_as_link',
    ),
  );

  // change urls to links to an id on the current page for the inline glossary.
  if ($as_inline) {
    if ($user_guides) {
      // @todo:
    }
    else {
      $links['introduction']['url'] = '#help-documentation-topic-introduction' . $url_arguments;
      $links['requests']['url'] = '#help-documentation-topic-requests' . $url_arguments;
      $links['requests/employee']['url'] = '#help-documentation-topic-requests-section-employee' . $url_arguments;
      $links['requests/student']['url'] = '#help-documentation-topic-requests-section-student' . $url_arguments;
      $links['requests/camps']['url'] = '#help-documentation-topic-requests-section-camps' . $url_arguments;
      $links['requests/external']['url'] = '#help-documentation-topic-requests-section-external' . $url_arguments;
      $links['review']['url'] = '#help-documentation-topic-review' . $url_arguments;
      $links['review/venue_available']['url'] = '#help-documentation-topic-review-section-venue_available' . $url_arguments;
      $links['review/review']['url'] = '#help-documentation-topic-review-section-review' . $url_arguments;
      $links['review/final_decision']['url'] = '#help-documentation-topic-review-section-final_decision' . $url_arguments;
      $links['review/completed']['url'] = '#help-documentation-topic-review-section-completed' . $url_arguments;
      $links['manage']['url'] = '#help-documentation-topic-manage' . $url_arguments;
      $links['manage/requests']['url'] = '#help-documentation-topic-manage-section-requests' . $url_arguments;
      $links['manage/users']['url'] = '#help-documentation-topic-manage-section-users' . $url_arguments;
      $links['manage/problems']['url'] = '#help-documentation-topic-manage-section-problems' . $url_arguments;
      $links['manage/availability']['url'] = '#help-documentation-topic-manage-section-availability' . $url_arguments;
      $links['manage/logs']['url'] = '#help-documentation-topic-manage-section-logs' . $url_arguments;
      $links['email']['url'] = '#help-documentation-topic-email' . $url_arguments;
      $links['email/request']['url'] = '#help-documentation-topic-email-section-request' . $url_arguments;
      $links['email/advanced']['url'] = '#help-documentation-topic-email-section-advanced' . $url_arguments;
      $links['email/advanced/request_status_codes']['url'] = '#help-documentation-topic-email-section-advanced-sub_section-request_status_codes' . $url_arguments;
      $links['email/advanced/review_step_codes']['url'] = '#help-documentation-topic-email-section-advanced-sub_section-review_step_codes' . $url_arguments;
      $links['email/advanced/request_classification_codes']['url'] = '#help-documentation-topic-email-section-advanced-sub_section-request_classification_codes' . $url_arguments;
      $links['email/advanced/request_type_codes']['url'] = '#help-documentation-topic-email-section-advanced-sub_section-request_type_codes' . $url_arguments;
      $links['email/advanced/email_type_codes']['url'] = '#help-documentation-topic-email-section-advanced-sub_section-email_type_codes' . $url_arguments;
      $links['calendar']['url'] = '#help-documentation-topic-calendar' . $url_arguments;
      $links['faq']['url'] = '#help-documentation-topic-faq' . $url_arguments;
      $links['terminology']['url'] = '#help-documentation-topic-terminology' . $url_arguments;
      $links['terminology/facilities_use']['url'] = '#help-documentation-topic-terminology-section-facilities_use' . $url_arguments;
      $links['terminology/system']['url'] = '#help-documentation-topic-terminology-section-system' . $url_arguments;
      $links['terminology/documentation']['url'] = '#help-documentation-topic-terminology-section-documentation' . $url_arguments;
    }
  }

  $markup .= '<div class="paragraph">';
  $markup .= 'The documentation is broken up into multiple <dfn class="term">Topics</dfn>.<br>';
  $markup .= 'Each <dfn class="term">Topic</dfn> may have multiple <dfn class="term">Sections</dfn>, each of which may contain their own <dfn class="term">Sub-Sections</dfn>.<br>';
  $markup .= '<dfn class="term">Sections</dfn> and <dfn class="term">Sub-Sections</dfn> will provide specific information while the <dfn class="term">Topic</dfn> provides general information.<br>';
  $markup .= '<br>';
  $markup .= 'If you are simply looking for a step-by-step guide, then use the <dfn class="term">User Guide</dfn> section.<br>';
  $markup .= 'For more details on the how and the why or to gain a better understanding of the <dfn class="term">Facilities Use System</dfn>, view the <dfn class="term">Documentation</dfn> section.<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph paragraph-user_guides">';
  $markup .= '<nav class="navigation navigation-help_glossary" role="navigation">';

  $headers = array(
    0 => array('markup' => 'User Guides', 'attributes' => array('class' => array('header-2', 'section'))),
  );

  $markup .= mfcs_help_render_headers('topic-glossary-user_guides', array('class' => $sub_section_class), $headers, 2);
  $markup .= '<ul class="navigation-list">';

  if (in_array('requests-create', $allowed_topics) && array_key_exists('request-create', $links)) {
    $markup .= '<li class="list-item">';
    $markup .= '<a href="' . $links['request-create']['url'] . '" class="' . $links['request-create']['class'] . '" title="' . $links['request-create']['tooltip'] . '">' . $links['request-create']['text'] . '</a>';
    $markup .= '<ul class="item-navigation navigation-list">';
    $markup .= '<li class="list-item"><a href="' . $links['request-create/employee']['url'] . '" class="' . $links['request-create/employee']['class'] . '" title="' . $links['request-create/employee']['tooltip'] . '">' . $links['request-create/employee']['text'] . '</a></li>';
    $markup .= '<li class="list-item"><a href="' . $links['request-create/student']['url'] . '" class="' . $links['request-create/student']['class'] . '" title="' . $links['request-create/student']['tooltip'] . '">' . $links['request-create/student']['text'] . '</a></li>';
    $markup .= '<li class="list-item"><a href="' . $links['request-create/camps']['url'] . '" class="' . $links['request-create/camps']['class'] . '" title="' . $links['request-create/camps']['tooltip'] . '">' . $links['request-create/camps']['text'] . '</a></li>';
    $markup .= '<li class="list-item"><a href="' . $links['request-create/external']['url'] . '" class="' . $links['request-create/external']['class'] . '" title="' . $links['request-create/external']['tooltip'] . '">' . $links['request-create/external']['text'] . '</a></li>';
    $markup .= '</ul>';
    $markup .= '</li>';
  }

  foreach (array('request-edit', 'request-cancel', 'request-cancel', 'request-copy', 'request-find', 'request-review', 'request-manage', 'calendar-use', 'calendar-export') as $topic_name) {
    if (!in_array($topic_name, $allowed_topics) || !array_key_exists($topic_name, $links)) {
      continue;
    }

    $markup .= '<li class="list-item"><a href="' . $links[$topic_name]['url'] . '" class="' . $links[$topic_name]['class'] . '" title="' . $links[$topic_name]['tooltip'] . '">' . $links[$topic_name]['text'] . '</a></li>';
  }

  $markup .= '</ul>';
  $markup .= '</nav>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph paragraph-documentation">';
  $markup .= '<nav class="navigation navigation-help_glossary" role="navigation">';

  $headers = array(
    0 => array('markup' => 'Documentation', 'attributes' => array('class' => array('header-2', 'section'))),
  );

  $markup .= mfcs_help_render_headers('topic-glossary-documentation', array('class' => $sub_section_class), $headers, 2);
  $markup .= '<ul class="navigation-list">';
  $markup .= '<li class="list-item"><a href="' . $links['introduction']['url'] . '" class="' . $links['introduction']['class'] . '" title="' . $links['introduction']['tooltip'] . '">' . $links['introduction']['text'] . '</a></li>';

  $markup .= '<li class="list-item">';
  $markup .= '<a href="' . $links['requests']['url'] . '" class="' . $links['requests']['class'] . '" title="' . $links['requests']['tooltip'] . '">' . $links['requests']['text'] . '</a>';
  $markup .= '<ul class="item-navigation navigation-list">';
  $markup .= '<li class="list-item"><a href="' . $links['requests/employee']['url'] . '" class="' . $links['requests/employee']['class'] . '" title="' . $links['requests/employee']['tooltip'] . '">' . $links['requests/employee']['text'] . '</a></li>';
  $markup .= '<li class="list-item"><a href="' . $links['requests/student']['url'] . '" class="' . $links['requests/student']['class'] . '" title="' . $links['requests/student']['tooltip'] . '">' . $links['requests/student']['text'] . '</a></li>';
  $markup .= '<li class="list-item"><a href="' . $links['requests/camps']['url'] . '" class="' . $links['requests/camps']['class'] . '" title="' . $links['requests/camps']['tooltip'] . '">' . $links['requests/camps']['text'] . '</a></li>';
  $markup .= '<li class="list-item"><a href="' . $links['requests/external']['url'] . '" class="' . $links['requests/external']['class'] . '" title="' . $links['requests/external']['tooltip'] . '">' . $links['requests/external']['text'] . '</a></li>';
  $markup .= '</ul>';
  $markup .= '</li>';

  if (in_array('review', $allowed_topics)) {
    $markup .= '<li class="list-item">';
    $markup .= '<a href="' . $links['review']['url'] . '" class="' . $links['review']['class'] . '" title="' . $links['review']['tooltip'] . '">' . $links['review']['text'] . '</a>';
    $markup .= '<ul class="item-navigation navigation-list">';
    $markup .= '<li class="list-item"><a href="' . $links['review/venue_available']['url'] . '" class="' . $links['review/venue_available']['class'] . '" title="' . $links['review/venue_available']['tooltip'] . '">' . $links['review/venue_available']['text'] . '</a></li>';
    $markup .= '<li class="list-item"><a href="' . $links['review/review']['url'] . '" class="' . $links['review/review']['class'] . '" title="' . $links['review/review']['tooltip'] . '">' . $links['review/review']['text'] . '</a></li>';
    $markup .= '<li class="list-item"><a href="' . $links['review/final_decision']['url'] . '" class="' . $links['review/final_decision']['class'] . '" title="' . $links['review/final_decision']['tooltip'] . '">' . $links['review/final_decision']['text'] . '</a></li>';
    $markup .= '<li class="list-item"><a href="' . $links['review/completed']['url'] . '" class="' . $links['review/completed']['class'] . '" title="' . $links['review/completed']['tooltip'] . '">' . $links['review/completed']['text'] . '</a></li>';
    $markup .= '</ul>';
    $markup .= '</li>';
  }

  if (in_array('manage', $allowed_topics)) {
    $markup .= '<li class="list-item">';
    $markup .= '<a href="' . $links['manage']['url'] . '" class="' . $links['manage']['class'] . '" title="' . $links['manage']['tooltip'] . '">' . $links['manage']['text'] . '</a>';
    $markup .= '<ul class="item-navigation navigation-list">';
    $markup .= '<li class="list-item"><a href="' . $links['manage/requests']['url'] . '" class="' . $links['manage/requests']['class'] . '" title="' . $links['manage/requests']['tooltip'] . '">' . $links['manage/requests']['text'] . '</a></li>';
    $markup .= '<li class="list-item"><a href="' . $links['manage/users']['url'] . '" class="' . $links['manage/users']['class'] . '" title="' . $links['manage/users']['tooltip'] . '">' . $links['manage/users']['text'] . '</a></li>';
    $markup .= '<li class="list-item"><a href="' . $links['manage/problems']['url'] . '" class="' . $links['manage/problems']['class'] . '" title="' . $links['manage/problems']['tooltip'] . '">' . $links['manage/problems']['text'] . '</a></li>';
    $markup .= '<li class="list-item"><a href="' . $links['manage/availability']['url'] . '" class="' . $links['manage/availability']['class'] . '" title="' . $links['manage/availability']['tooltip'] . '">' . $links['manage/availability']['text'] . '</a></li>';
    $markup .= '<li class="list-item"><a href="' . $links['manage/logs']['url'] . '" class="' . $links['manage/logs']['class'] . '" title="' . $links['manage/logs']['tooltip'] . '">' . $links['manage/logs']['text'] . '</a></li>';
    $markup .= '</ul>';
    $markup .= '</li>';
  }

  $markup .= '<li class="list-item">';
  $markup .= '<a href="' . $links['email']['url'] . '" class="' . $links['email']['class'] . '" title="' . $links['email']['tooltip'] . '">' . $links['email']['text'] . '</a>';
  $markup .= '<ul class="item-navigation navigation-list">';
  $markup .= '<li class="list-item"><a href="' . $links['email/request']['url'] . '" class="' . $links['email/request']['class'] . '" title="' . $links['email/request']['tooltip'] . '">' . $links['email/request']['text'] . '</a></li>';

  $markup .= '<li class="list-item">';
  $markup .= '<a href="' . $links['email/advanced']['url'] . '" class="' . $links['email/advanced']['class'] . '" title="' . $links['email/advanced']['tooltip'] . '">' . $links['email/advanced']['text'] . '</a>';
  $markup .= '<ul class="item-navigation navigation-list">';
  $markup .= '<li class="list-item"><a href="' . $links['email/advanced/request_status_codes']['url'] . '" class="' . $links['email/advanced/request_status_codes']['class'] . '" title="' . $links['email/advanced/request_status_codes']['tooltip'] . '">' . $links['email/advanced/request_status_codes']['text'] . '</a></li>';
  $markup .= '<li class="list-item"><a href="' . $links['email/advanced/review_step_codes']['url'] . '" class="' . $links['email/advanced/review_step_codes']['class'] . '" title="' . $links['email/advanced/review_step_codes']['tooltip'] . '">' . $links['email/advanced/review_step_codes']['text'] . '</a></li>';
  $markup .= '<li class="list-item"><a href="' . $links['email/advanced/request_classification_codes']['url'] . '" class="' . $links['email/advanced/request_classification_codes']['class'] . '" title="' . $links['email/advanced/request_classification_codes']['tooltip'] . '">' . $links['email/advanced/request_classification_codes']['text'] . '</a></li>';
  $markup .= '<li class="list-item"><a href="' . $links['email/advanced/request_type_codes']['url'] . '" class="' . $links['email/advanced/request_type_codes']['class'] . '" title="' . $links['email/advanced/request_type_codes']['tooltip'] . '">' . $links['email/advanced/request_type_codes']['text'] . '</a></li>';
  $markup .= '<li class="list-item"><a href="' . $links['email/advanced/email_type_codes']['url'] . '" class="' . $links['email/advanced/email_type_codes']['class'] . '" title="' . $links['email/advanced/email_type_codes']['tooltip'] . '">' . $links['email/advanced/email_type_codes']['text'] . '</a></li>';
  $markup .= '</ul>';
  $markup .= '</li>';

  $markup .= '</ul>';
  $markup .= '</li>';

  $markup .= '<li class="list-item"><a href="' . $links['calendar']['url'] . '" class="' . $links['calendar']['class'] . '" title="' . $links['calendar']['tooltip'] . '">' . $links['calendar']['text'] . '</a></li>';

  $markup .= '<li class="list-item"><a href="' . $links['faq']['url'] . '" class="' . $links['faq']['class'] . '" title="' . $links['faq']['tooltip'] . '">' . $links['faq']['text'] . '</a></li>';

  $markup .= '<li class="list-item">';
  $markup .= '<a href="' . $links['terminology']['url'] . '" class="' . $links['terminology']['class'] . '" title="' . $links['terminology']['tooltip'] . '">' . $links['terminology']['text'] . '</a>';
  $markup .= '<ul class="item-navigation navigation-list">';
  $markup .= '<li class="list-item"><a href="' . $links['terminology/facilities_use']['url'] . '" class="' . $links['terminology/facilities_use']['class'] . '" title="' . $links['terminology/facilities_use']['tooltip'] . '">' . $links['terminology/facilities_use']['text'] . '</a></li>';
  $markup .= '<li class="list-item"><a href="' . $links['terminology/system']['url'] . '" class="' . $links['terminology/system']['class'] . '" title="' . $links['terminology/system']['tooltip'] . '">' . $links['terminology/system']['text'] . '</a></li>';
  $markup .= '<li class="list-item"><a href="' . $links['terminology/documentation']['url'] . '" class="' . $links['terminology/documentation']['class'] . '" title="' . $links['terminology/documentation']['tooltip'] . '">' . $links['terminology/documentation']['text'] . '</a></li>';
  $markup .= '</ul>';
  $markup .= '</li>';
  $markup .= '</ul>';
  $markup .= '</nav>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="paragraph paragraph-presentation no-print">';
    $markup .= '<nav class="navigation navigation-help_glossary" role="navigation">';

    $headers = array(
      0 => array('markup' => 'Presentation', 'attributes' => array('class' => array('header-2', 'section'))),
    );

    $markup .= mfcs_help_render_headers('topic-glossary-presentation', array('class' => $sub_section_class), $headers, 2);
    $markup .= '<ul class="navigation-list">';
    $markup .= '<li class="list-item"><a href="' . $base_path . 'help-0/presentation/single' . $url_arguments . '" class="item-link no_print_links_as_link">View Documentation as a Single Page</a></li>';
    $markup .= '<li class="list-item"><a href="' . $base_path . 'help-0/presentation/pdf' . $url_arguments . '" class="item-link no_print_links_as_link">Download Entire Documentation as a PDF</a></li>';
    $markup .= '</ul>';

    $markup .= '</nav>';
    $markup .= '</div>';
  }

  $markup .= '</section>';

  return $markup;
}

/**
 * Build the markup for the help topic: introduction.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_introduction($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-introduction', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'The <dfn class="term">Facilities Use System</dfn>, or <dfn class="term"><abbr title="Facilities Use System" class="abbreviation">FCS</abbr></dfn>, is a product of <em class="emphasis">McNeese State University\'s</em> culture and needs in tracking special events. ';
  $markup .= 'Through this system, <em class="emphasis">McNeese State University</em> provides facilities and resources to our <strong class="bold">Students</strong>, <strong class="bold">Employees</strong>, and the <strong class="bold">Community</strong> to further educational, cultural, and recreational initiatives through events held on campus. ';
  $markup .= 'All activities, meetings, or requests, excluding scheduled academic classes, must be scheduled through the facilities use special event request form. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'This system provides methods for requesting, reviewing, and viewing such events through numerous technologies, such as e-mail, iCalendar, <abbr title="Open Pretty Good Privacy" class="abbreviation">OpenPGP</abbr>, and <abbr title="Hypertext Markup Language 5" class="abbreviation">HTML5</abbr>. ';
  $markup .= '<strong class="bold">OpenPGP</strong> signed e-mails are are sent out for many actions performed on or by this system to help keep users informed. ';
  $markup .= '<strong class="bold">iCalendar</strong> is provided to allow users to download a copy of approved requests and add them to their own personal calendars. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/requests' . $url_arguments . '" class="item-link no_print_links_as_link" title="Documentation: Requests">Documentation: Requests</a>. ';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: requests.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_requests($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-requests', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'A <dfn class="term">Request</dfn> is a web form entry for reserving a particular place owned or operated by <em class="emphasis">McNeese State University</em> at some specified date and time. ';
  $markup .= 'Each <dfn class="term">Request</dfn> goes through a <dfn class="term">Review</dfn> process for approval to help ensure that the <dfn class="term">Room</dfn> is available, all appropriate expenses are paid, and all conflicts are reasonably avoided. ';
  $markup .= 'All users who make requests, called the <dfn class="term">Request Coordinator</dfn> (or simply <dfn class="term">Requester</dfn> for short), are expected to read, agree to, and abide by the policies put forth by <em class="emphasis">McNeese State University</em>. ';
  $markup .= 'It is important to read and agree to the <a href="//www.mcneese.edu/node/3217" class="link no_print_links_as_link">Facility Use Policy</a>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'In addition to the <a href="//www.mcneese.edu/policy" class="link no_print_links_as_link">McNeese State University policies</a>, there are also other forms that may need to be filled out, such as the <a href="//www.mcneese.edu/node/6309" class="link no_print_links_as_link">Open Flame Permit</a>, the <a href="//www.mcneese.edu/node/5434" class="link no_print_links_as_link">Table Request Form</a>, the <a href="//www.mcneese.edu/node/5506" class="link no_print_links_as_link">Technical Request Form</a>, and the <a href="//www.mcneese.edu/node/768" class="link no_print_links_as_link">Student Union Reservation Forms</a>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'Each <dfn class="term">Request</dfn> must be associated with a specific <dfn class="term">Room</dfn>. Each <dfn class="term">Room</dfn> is associated with a <dfn class="term">Building</dfn> and each <dfn class="term">Building</dfn> falls under one or more <dfn class="term">Locations</dfn>. ';
  $markup .= 'The <dfn class="term">Buildings</dfn> themselves are more of an abstract term because they may reference a generic place such as a parking lot (making a <dfn class="term">Room</dfn> also an abstract concept). ';
  $markup .= 'These places, or venues, are managed by their <dfn class="term">Venue Coordinator</dfn> or, if applicable, are managed by their <dfn class="term">Room Coordinator</dfn>. ';
  $markup .= 'These <dfn class="term">Coordinators</dfn> are the primary point of contact for their respective venue. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'In some cases a <dfn class="term">Requester</dfn> may instead go through a <dfn class="term">Coordinator</dfn> to have their <dfn class="term">Request</dfn> submitted. ';
  $markup .= 'This is a common practice for student organizations. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'Specific dates and times must be specified for each event. ';
  $markup .= 'A date category must be provided when specifying a date and time. ';
  $markup .= 'It is highly recommended to assign a rehearsal/setup and a cleanup/break-down time for the requested event. ';
  $markup .= 'There are a number of restrictions on what dates and times may be specified, such as: room conflicts, holidays, and room unavailabilities. ';
  $markup .= 'Room conflicts represent other approved requests (or other <dfn class="term">Requests</dfn> currently under review) for a given <dfn class="term">Room</dfn> on the specified date and time. ';
  $markup .= 'Holidays are intended to inform the <dfn class="term">Requester</dfn> that a given <dfn class="term">Room</dfn> may be unavailable for the requested date and time. ';
  $markup .= 'Room unavailabilities designate that the room is unavailable and may include classes or class-related activities. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'Each <dfn class="term">Request</dfn> is assigned a <dfn class="term">Request Classification</dfn> (or just <dfn class="term">Classification</dfn> for short) based on the <dfn class="term">Requesters Classification</dfn>. ';
  $markup .= 'There are 4 types of <dfn class="term">Classifications</dfn>: <dfn class="term">Faculty, Staff, and Athletics</dfn>, <dfn class="term">Student Organization</dfn>, <dfn class="term">Camps</dfn>, and <dfn class="term">External</dfn>. ';
  $markup .= '<dfn class="term">Managers</dfn> and <dfn class="term">Coordinators</dfn> may be able to alter the <dfn class="term">Classification</dfn> assigned to a given <dfn class="term">Request</dfn>. ';
  $markup .= 'Each of these <dfn class="term">Classifications</dfn> may pose their own questions and form fields. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'In addition to the <dfn class="term">Classifications</dfn>, each <dfn class="term">Request</dfn> has a <dfn class="term">Request Type</dfn>. ';
  $markup .= 'Unlike the <dfn class="term">Classifications</dfn>, each <dfn class="term">Request Type</dfn> generally refers to the purpose of the <dfn class="term">Request</dfn>. ';
  $markup .= 'Some <dfn class="term">Request Types</dfn> will provide new fields to fill out, fewer fields to fill out, or even allow for quick submittal of a <dfn class="term">Request</dfn>. ';
  $markup .= 'The special <dfn class="term">Request Type</dfn>, called a <dfn class="term">Quick Meeting</dfn>, does not go through the <dfn class="term">Review</dfn> process and may only be assigned by <dfn class="term">Venue Coordinators</dfn> and <dfn class="term">Managers</dfn>. ';
  $markup .= 'To better accommodate the needs of students, student organizations are able to create a special <dfn class="term">Request Type</dfn> called a <dfn class="term">Club Meeting</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'E-mails sent by the <dfn class="term">Facilities Use System</dfn> should not be replied to. ';
  $markup .= 'Your replies will not be received. ';
  $markup .= 'Instead, all responses to received e-mails should be sent via <dfn class="term">Comments</dfn> through the <dfn class="term"><abbr class="abbreviation" title="Facilities Use System">FCS</abbr></dfn> for the appropraite <dfn class="term">Request</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'Once a <dfn class="term">Request</dfn> is created it enters into what is called the <dfn class="term">Review</dfn> process. ';
  $markup .= 'The <dfn class="term">Review</dfn> process is broken up into multiple <dfn class="term">Steps</dfn>: <dfn class="term">Venue Available</dfn>, <dfn class="term">Review</dfn>, <dfn class="term">Final Decision</dfn>, and <dfn class="term">Completed</dfn>. ';
  $markup .= 'The first <dfn class="term">Step</dfn> is the <dfn class="term">Venue Available</dfn> <dfn class="term">Step</dfn>. ';
  $markup .= 'The first action performed during this <dfn class="term">Step</dfn> is an automated check to see if the venue is available and if not then appropriately close the <dfn class="term">Request</dfn> and change the <dfn class="term">Step</dfn> to <dfn class="term">Completed</dfn>. ';
  $markup .= 'After this automated check, the <dfn class="term">Venue Coordinator</dfn> is informed of the request and may then designate the <dfn class="term">Request</dfn> as either <dfn class="term">Available</dfn> or <dfn class="term">Unavailable</dfn>. ';
  $markup .= '<dfn class="term">Requests</dfn> marked as <dfn class="term">Unavailable</dfn> are immediately closed. ';
  $markup .= '<dfn class="term">Requests</dfn> marked as <dfn class="term">Available</dfn> are moved to the <dfn class="term">Review</dfn> <dfn class="term">Step</dfn>. ';
  $markup .= 'During the <dfn class="term">Review</dfn> <dfn class="term">Step</dfn>, different <dfn class="term">Reviewers</dfn> will designate whether they have <dfn class="term">Issues</dfn> or <dfn class="term">No Issues</dfn> with the <dfn class="term">Request</dfn>. ';
  $markup .= 'These responses are generally sent to the <dfn class="term">Requester</dfn> so that they may be informed of the current <dfn class="term">Decisions</dfn>, answer any questions, or even help solve any <dfn class="term">Issues</dfn>. ';
  $markup .= 'Once all of the <dfn class="term">Reviewers</dfn> have made at least one <dfn class="term">Decision</dfn>, the <dfn class="term">Request</dfn> is moved into the <dfn class="term">Final Decision</dfn> <dfn class="term">Step</dfn>. ';
  $markup .= 'It is during this final <dfn class="term">Step</dfn> that all <dfn class="term">Issues</dfn> should be resolved (and in some cases additional <dfn class="term">Requirements</dfn> may also be specified). ';
  $markup .= 'The <dfn class="term">Request</dfn> may either be <dfn class="term">Approved</dfn> or <dfn class="term">Denied</dfn>, but regardless of what the <dfn class="term">Final Decision</dfn> is, the <dfn class="term">Request</dfn> will be moved into the <dfn class="term">Completed</dfn> <dfn class="term">Step</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'The <dfn class="term">Completed</dfn> <dfn class="term">Step</dfn> only identifies a <dfn class="term">Request</dfn> as having been fully processed. ';
  $markup .= 'It does <strong class="bold">not</strong> represent whether or not a given <dfn class="term">Request</dfn> is <dfn class="term">Approved</dfn> or <dfn class="term">Denied</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= ' <dfn class="term">Completed</dfn> and <dfn class="term">Approved</dfn> <dfn class="term">Requests</dfn> appear on the calendars, the time slots are reserved, and an e-mail is sent to the <dfn class="term">Requester</dfn>. ';
  $markup .= ' These e-mails also contain additional files, such as an iCalendar file (a file with an ICS file extension), for importing the <dfn class="term">Approved</dfn> <dfn class="term">Request</dfn> into the persons own personal calendars. ';
  $markup .= ' Under certain conditions, <dfn class="term">Requests</dfn> may be altered and even individual dates may be cancelled or uncancelled. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'When a <dfn class="term">Request</dfn> (or one of its dates) is <dfn class="term">Cancelled</dfn>, the requested dates and times may no longer being available for use. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'View Section: <a href="' . $base_path . 'help-0/requests/employee' . $url_arguments . '" class="item-link">Faculty, Staff, and Athletics Requests</a>. ';
    $markup .= '<br>';
    $markup .= 'View Section: <a href="' . $base_path . 'help-0/requests/student' . $url_arguments . '" class="item-link">Student Organization Requests</a>. ';
    $markup .= '<br>';
    $markup .= 'View Section: <a href="' . $base_path . 'help-0/requests/camps' . $url_arguments . '" class="item-link">Camps Requests</a>. ';
    $markup .= '<br>';
    $markup .= 'View Section: <a href="' . $base_path . 'help-0/requests/external' . $url_arguments . '" class="item-link">External Requests</a>. ';
    $markup .= '<br>';
    $markup .= '<br>';

    if (in_array('review', $allowed_topics)) {
      $markup .= 'Continue on to <a href="' . $base_path . 'help-0/review' . $url_arguments . '" class="item-link">Documentation: Review</a>. ';
    }
    elseif (in_array('manage', $allowed_topics)) {
      $markup .= 'Continue on to <a href="' . $base_path . 'help-0/manage' . $url_arguments . '" class="item-link">Documentation: Manage</a>. ';
    }
    else {
      $markup .= 'Continue on to <a href="' . $base_path . 'help-0/email' . $url_arguments . '" class="item-link">Documentation: E-Mail</a>. ';
    }
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic - faculty, staff, and athletics.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_requests_section_employee($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-requests-section-employee', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'These <dfn class="term">Requests</dfn> generally represent <em class="emphasis">McNeese State University</em> employee or official university related activity. ';
  $markup .= 'There are no modifications to the <dfn class="term">Request</dfn> when using this <dfn class="term">Request Classification</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/requests/student' . $url_arguments . '" class="item-link">Documentation: Student Organization Requests</a>. ';
    $markup .= '<br>';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: requests - student organization.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_requests_section_student_organization($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-requests-section-student', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'These <dfn class="term">Requests</dfn> are associated with <em class="emphasis">McNeese State University</em> student organizations. ';
  $markup .= 'This form requires that the <strong class="bold">Organization / Affiliation</strong> field to be filled out, providing the name of the student organization. ';
  $markup .= 'This form requires that the <strong class="bold">Adviser Approval</strong> checkbox be selected. ';
  $markup .= 'By selecting this checkbox, the <dfn class="term">Requester</dfn> is claiming that the adviser for their student organization has given the approval for the requested event. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'In addition to the mentioned fields, there is also a new <dfn class="term">Request Type</dfn> available for student organizations. ';
  $markup .= 'This new type is called a <dfn class="term">Club Meeting</dfn>. ';
  $markup .= '<dfn class="term">Club Meetings</dfn> are intended to assist students in creating <dfn class="term">Requests</dfn> by simplifying certain aspects of the <dfn class="term">Request</dfn> process. ';
  $markup .= 'This type uses the same, simplied form, as the <dfn class="term">Meeting</dfn> type. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/requests/camps' . $url_arguments . '" class="item-link">Documentation: Camps Requests</a>. ';
    $markup .= '<br>';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: requests - camps.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_requests_section_camps($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-requests-section-camps', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'These <dfn class="term">Requests</dfn> generally represent events associated with camps hosted by or related to <em class="emphasis">McNeese State University</em>. ';
  $markup .= 'There are no modifications to these forms. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/requests/external' . $url_arguments . '" class="item-link">Documentation: External Requests</a>. ';
    $markup .= '<br>';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: requests, sub-section: external.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_requests_section_external($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-requests-section-external', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'These <dfn class="term">Requests</dfn> generally represent people or organizations not directly affiliated with <em class="emphasis">McNeese State University</em> or are otherwise consider a 3rd-party. ';
  $markup .= 'While there are no modifications to these forms, the <dfn class="term">Requester</dfn> is expected to go through the appropriate point of contact for filling out the forms and generally does not directly create <dfn class="term">Requests</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: review.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_review($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-review', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'The <dfn class="term">Facilities Use System</dfn> is centerred around the <dfn class="term">Review</dfn> process to help ensure <dfn class="term">Room</dfn> availability, issue resolution, fee assignments, insurance confirmation, and any other situation including but not limited to closures due to emergencies. ';
  $markup .= 'This process is intended to increase communication between the different parties in a consistent and centralized manner. ';
  $markup .= 'Many actions, decisions, comments, and other changes to <dfn class="term">Requests</dfn> are stored on the system in the <dfn class="term">Review Log</dfn> (or in the <dfn class="term">Request History</dfn>) and may also be sent via e-mail to relevant parties. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'E-mails sent by the <dfn class="term">Facilities Use System</dfn> should not be replied to. ';
  $markup .= 'Your replies will not be received. ';
  $markup .= 'Instead, all responses to received e-mails should be sent via <dfn class="term">Comments</dfn> through the <dfn class="term"><abbr class="abbreviation" title="Facilities Use System">FCS</abbr></dfn> for the appropraite <dfn class="term">Request</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'The <dfn class="term">Review</dfn> process is broken down into multiple <dfn class="term">Review Steps</dfn>. ';
  $markup .= 'These <dfn class="term">Steps</dfn> are: <dfn class="term">Venue Available</dfn>, <dfn class="term">Review</dfn>, <dfn class="term">Final Decision</dfn>, and <dfn class="term">Completed</dfn>. ';
  $markup .= 'Sometimes the system will automatically alter a <dfn class="term">Requests</dfn> <dfn class="term">Review Step</dfn>. ';
  $markup .= 'This commonly happens with incomplete <dfn class="term">Requests</dfn> with dates and times in the past. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'There is a <dfn class="term">Comment</dfn> decision that may be used during all <dfn class="term">Review Steps</dfn>. ';
  $markup .= 'All <dfn class="term">Comments</dfn> are sent to the <dfn class="term">Venue Coordinator</dfn>, every <dfn class="term">Reviewer</dfn>, and also the <dfn class="term">Requester</dfn>. ';
  $markup .= 'There is an option to restrict messages such that the <dfn class="term">Requester</dfn> does not receive the <dfn class="term">Comment</dfn> (and also hiding the message in the <dfn class="term">Review Log</dfn>) for more private messages between <dfn class="term">Reviewers</dfn>, the <dfn class="term">Venue Coordinator</dfn>, and <dfn class="term">Managers</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $section_information_child = $section_information;
    $section_information_child['id'] = 'help-documentation-topic-review-section-venue_available';
    $section_information_child['class'] = array('section-help', 'section-help-review-venue_available');
    $section_information_child['headers'] = array(
      0 => array('markup' => 'Venue Available Step', 'attributes' => array('class' => array('header-3', 'section'))),
    );
    $section_information_child['navigation'] = NULL;
    $section_information_child['start_count'] = 2;

    $markup .= mfcs_help_topic_review_section_venue_available($allowed_topics, $section_information_child, TRUE);

    $section_information_child['id'] = 'help-documentation-topic-review-section-review';
    $section_information_child['class'] = array('section-help', 'section-help-review-review');
    $section_information_child['headers'][0]['markup'] = 'Review Step';

    $markup .= mfcs_help_topic_review_section_review($allowed_topics, $section_information_child, TRUE);

    $section_information_child['id'] = 'help-documentation-topic-review-section-final_decision';
    $section_information_child['class'] = array('section-help', 'section-help-review-final_decision');
    $section_information_child['headers'][0]['markup'] = 'Final Decision Step';

    $markup .= mfcs_help_topic_review_section_final_decision($allowed_topics, $section_information_child, TRUE);

    $section_information_child['id'] = 'help-documentation-topic-review-section-completed';
    $section_information_child['class'] = array('section-help', 'section-help-review-completed');
    $section_information_child['headers'][0]['markup'] = 'Completed Step';

    $markup .= mfcs_help_topic_review_section_completed($allowed_topics, $section_information_child, TRUE);

    $markup .= '<div class="continue_on no-print">';
    if (in_array('manage', $allowed_topics)) {
      $markup .= 'Continue on to <a href="' . $base_path . 'help-0/manage' . $url_arguments . '" class="item-link">Documentation: Manage</a>. ';
    }
    else {
      $markup .= 'Continue on to <a href="' . $base_path . 'help-0/email' . $url_arguments . '" class="item-link">Documentation: E-Mail</a>. ';
    }
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: review - venue available step.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_review_section_venue_available($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-review-section-venue_available', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'The <dfn class="term">Venue Available</dfn> <dfn class="term">Review Step</dfn> is the very first step following the creation of a <dfn class="term">Request</dfn>. ';
  $markup .= 'During this <dfn class="term">Step</dfn>, <dfn class="term">Venue Coordinators</dfn> must determine if the requested venue is available. ';
  $markup .= 'A decision of either <strong class="bold"><dfn class="term">Available</dfn></strong> or <strong class="bold"><dfn class="term">Unavailable</dfn></strong> must be specified before the <dfn class="term">Request</dfn> can move into the next <dfn class="term">Review Step</dfn>. ';
  $markup .= 'The next <dfn class="term">Review Step</dfn> is <dfn class="term">Review</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'When the <strong class="bold"><dfn class="term">Unavailable</dfn></strong> decision is made, an e-mail is sent out informing the <dfn class="term">Requester</dfn> that their <dfn class="term">Request</dfn> has been closed because the <dfn class="term">Room</dfn> is <dfn class="term">Unavailable</dfn>. ';
  $markup .= 'When the <strong class="bold"><dfn class="term">Available</dfn></strong> decision is made, an e-mail is sent out informing the <dfn class="term">Requester</dfn> that their <dfn class="term">Request</dfn> has been made available for review. ';
  $markup .= 'These <dfn class="term">Requests</dfn> have their <dfn class="term">Request State</dfn> immediately set to <dfn class="term">Queued (Locked)</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= '<dfn class="term">Requests</dfn> in the <dfn class="term">Venue Available</dfn> <dfn class="term">Step</dfn> may sometimes be auto-closed as <dfn class="term">Room Unavailable</dfn> when another <dfn class="term">Request</dfn> is <dfn class="term">Approved</dfn> for the same <dfn class="term">Room</dfn>, dates, and times. ';
  $markup .= '<dfn class="term">Requests</dfn> in this <dfn class="term">Step</dfn> may become auto-cancelled if their dates and times are in the past. ';
  $markup .= '<dfn class="term">Requirements</dfn>, such as usage fees, may be specified by the appropriate <dfn class="term">Financer</dfn>, <dfn class="term">Insurerer</dfn>, or <dfn class="term">Manager</dfn> during this <dfn class="term">Step</dfn>. ';
  $markup .= 'The <dfn class="term">Requirement</dfn> of <em class="emphasis">Strictly University Business</em> may be specified by the <dfn class="term">Venue Coordinator</dfn> to inform the <dfn class="term">Financer</dfn> and <dfn class="term">Insurer</dfn> that fees and insurance may not be required. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/review/review' . $url_arguments . '" class="item-link">Documentation: Review - Review</a>. ';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: review - review step.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_review_section_review($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-review-section-review', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'The <dfn class="term">Review</dfn> <dfn class="term">Review Step</dfn> (or just <dfn class="term">Review Step</dfn> to be less confusing) is where <dfn class="term">Reviewers</dfn> are expected to make decisions. ';
  $markup .= 'Every <dfn class="term">Reviewer</dfn> associated with the <dfn class="term">Request\'s</dfn> <dfn class="term">Classification</dfn> will revieve an e-mail informing them that a new <dfn class="term">Request</dfn> has been made available for <dfn class="term">Review</dfn>. ';
  $markup .= 'A decision of either <strong class="bold"><dfn class="term">Issues</dfn></strong> or <strong class="bold"><dfn class="term">No Issues</dfn></strong> must be specified by each <dfn class="term">Reviewer</dfn> before the <dfn class="term">Request</dfn> can move into the next <dfn class="term">Review Step</dfn>. ';
  $markup .= 'The next <dfn class="term">Review Step</dfn> is <dfn class="term">Final Decision</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'When the <strong class="bold"><dfn class="term">Issues</dfn></strong> decision is made, an e-mail is sent out informing the <dfn class="term">Requester</dfn> and other <dfn class="term">Reviewers</dfn> of the decision. ';
  $markup .= 'When the <strong class="bold"><dfn class="term">No Issues</dfn></strong> decision is made, an e-mail is sent out informing the <dfn class="term">Requester</dfn> and other <dfn class="term">Reviewers</dfn> of the decision. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'It is highly recommended to use the <strong class="bold"><dfn class="term">Comment</dfn></strong> decision to ask the <dfn class="term">Requester</dfn> questions or give additional instructions during the <dfn class="term">Review Step</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'A warning is presented near the <dfn class="term">Review</dfn> decision fields when there are any existing conflicts. ';
  $markup .= '<dfn class="term">Requests</dfn> in this <dfn class="term">Step</dfn> may become auto-cancelled if their dates and times are in the past. ';
  $markup .= '<dfn class="term">Requirements</dfn>, such as usage fees, may be specified by the appropriate <dfn class="term">Financer</dfn>, <dfn class="term">Insurerer</dfn>, or <dfn class="term">Manager</dfn> during this <dfn class="term">Step</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/review/final_decision' . $url_arguments . '" class="item-link">Documentation: Review - Final Decision</a>. ';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: review - final decision step.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_review_section_final_decision($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-review-section-final_decision', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'The <dfn class="term">Final Decision</dfn> <dfn class="term">Review Step</dfn> is where <dfn class="term">Reviewers</dfn> make the final decision for <dfn class="term">Approving</dfn> or <dfn class="term">Denying</dfn> the <dfn class="term">Request</dfn> is made. ';
  $markup .= 'The only differences between this <dfn class="term">Step</dfn> and the <dfn class="term">Review Step</dfn> is that every <dfn class="term">Reviewer</dfn> has now made at least one decision of either <dfn class="term">Issues</dfn> or <dfn class="term">No Issued</dfn> and certain <dfn class="term">Reviewers</dfn> now have the <strong class="bold"><dfn class="term">Approve</dfn></strong> or <strong class="bold"><dfn class="term">Deny</dfn></strong> decision. ';
  $markup .= 'The <dfn class="term">Reviewer</dfn> may still update their <dfn class="term">Issues</dfn> or <dfn class="term">No Issues</dfn> decision during this <dfn class="term">Step</dfn>. ';
  $markup .= 'The next <dfn class="term">Review Step</dfn> is <dfn class="term">Completed</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'When the <strong class="bold"><dfn class="term">Approved</dfn></strong> decision is made, an e-mail is sent out informing the <dfn class="term">Requester</dfn> and other <dfn class="term">Reviewers</dfn> of the decision and the <dfn class="term">Request State</dfn> is set to <dfn class="term">Accepted</dfn>. ';
  $markup .= 'When the <strong class="bold"><dfn class="term">Denied</dfn></strong> decision is made, an e-mail is sent out informing the <dfn class="term">Requester</dfn> and other <dfn class="term">Reviewers</dfn> of the decision and the <dfn class="term">Request State</dfn> is set to <dfn class="term">Denied</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'A warning is presented near the <dfn class="term">Review</dfn> decision fields when there are any existing conflicts. ';
  $markup .= '<dfn class="term">Requests</dfn> in this <dfn class="term">Step</dfn> may become auto-cancelled if their dates and times are in the past. ';
  $markup .= '<dfn class="term">Requirements</dfn>, such as usage fees, may be specified by the appropriate <dfn class="term">Financer</dfn>, <dfn class="term">Insurerer</dfn>, or <dfn class="term">Manager</dfn> during this <dfn class="term">Step</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/review/completed' . $url_arguments . '" class="item-link">Documentation: Review - Completed</a>. ';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: review - completed step.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_review_section_completed($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-review-section-completed', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'The <dfn class="term">Completed</dfn> <dfn class="term">Review Step</dfn> is where the <dfn class="term">Review</dfn> process is complete. ';
  $markup .= 'No further decisions may be made except for <dfn class="term">Comments</dfn> and <dfn class="term">Requirements</dfn>. ';
  $markup .= 'Completed <dfn class="term">Requests</dfn> are generally either in the <dfn class="term">Approved</dfn> or <dfn class="term">Denied</dfn> <dfn class="term">State</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= '<dfn class="term">Requirements</dfn>, such as usage fees, may be specified by the appropriate <dfn class="term">Financer</dfn>, <dfn class="term">Insurerer</dfn>, or <dfn class="term">Manager</dfn> during this <dfn class="term">Step</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: manage.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_manage($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-manage', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'The system may be <dfn class="term">managed</dfn> through various administrative tasks, such as forcibly changing the <dfn class="term">Review Step</dfn>, adding and modifying users, altering content, creating <dfn class="term">Room</dfn> unavailabilities, and troubleshooting. ';
  $markup .= 'These administrative tasks are performed by <dfn class="term">Managers</dfn>. ';
  $markup .= '<dfn class="term">Managers</dfn> are essentially non-technical administrators. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'View Section: <a href="' . $base_path . 'help-0/manage/requests' . $url_arguments . '" class="item-link">Requests Management</a>. ';
    $markup .= '<br>';
    $markup .= 'View Section: <a href="' . $base_path . 'help-0/manage/users' . $url_arguments . '" class="item-link">Users Management</a>. ';
    $markup .= '<br>';
    $markup .= 'View Section: <a href="' . $base_path . 'help-0/manage/problems' . $url_arguments . '" class="item-link">Problems Management</a>. ';
    $markup .= '<br>';
    $markup .= 'View Section: <a href="' . $base_path . 'help-0/manage/availability' . $url_arguments . '" class="item-link">Availability Management</a>. ';
    $markup .= '<br>';
    $markup .= 'View Section: <a href="' . $base_path . 'help-0/manage/logs' . $url_arguments . '" class="item-link">Logs Management</a>. ';
    $markup .= '<br>';
    $markup .= '<br>';

    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/email' . $url_arguments . '" class="item-link">Documentation: E-Mail</a>. ';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: manage - requests.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_manage_section_requests($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-manage-section-requests', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= '<dfn class="term">Managers</dfn> are allowed to make changes to any and all <dfn class="term">Requests</dfn> at any point in time with only a few exceptions. ';
  $markup .= 'Some conditions that are prohibited even to <dfn class="term">Managers</dfn>, such as <dfn class="term">Uncancelling</dfn> a <dfn class="term">Request</dfn> that conflicts with already <dfn class="term">Accepted</dfn> <dfn class="term">Requests</dfn>. ';
  $markup .= 'Operations performed by a <dfn class="term">Manager</dfn> for a particular <dfn class="term">Request</dfn> are generally recorded in the <dfn class="term">Review Log</dfn> as well as the <dfn class="term">Request History</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';


  $markup .= '<div class="paragraph">';
  $markup .= 'Each <dfn class="term">Request</dfn> has a special <em class="emphasis">manage</em> interface for viewing more technical <dfn class="term">Request</dfn> and user information and performing special management actions. ';
  $markup .= 'Such actions include: manual override of <dfn class="term">Review Step</dfn> and <dfn class="term">Status</dfn>, customizing the <dfn class="term">Request\'s</dfn> auto-locking behavior, moving a <dfn class="term">Request</dfn> to a different <dfn class="term">Room</dfn>, <dfn class="term">Building</dfn>, or <dfn class="term">Location</dfn>, and re-assigning the <dfn class="term">Venue Coordinator</dfn> or <dfn class="term">Request Coordinator</dfn>. ';
  $markup .= 'Manually overriding allows for forcing a new <dfn class="term">Review Step</dfn> and <dfn class="term">Status</dfn>, effectively bypassing the <dfn class="term">Review</dfn> process. ';
  $markup .= 'Auto-locking behavior defines if a <dfn class="term">Request</dfn> will be auto-locked when it is in the past (or for any other condition that triggers the <dfn class="term">Request</dfn> to be automatically locked, cancelled, or otherwise closed). ';
  $markup .= 'Moving a <dfn class="term">Request</dfn> entails forcibly changing the <dfn class="term">Request</dfn> to a new location (even if the <dfn class="term">Request</dfn> is <dfn class="term">Accepted</dfn>). ';
  $markup .= 'The <dfn class="term">Venue Coordinator</dfn> may be changed, but only to the <dfn class="term">Venue Coordinator</dfn> or the <dfn class="term">Room Coordinator</dfn> defined by <dfn class="term">Banner</dfn>. ';
  $markup .= 'The <dfn class="term">Request Coordinator</dfn>, or <dfn class="term">Requester</dfn> for short, may be updated through this management interface. ';
  $markup .= 'In the case of both the <dfn class="term">Venue Coordinator</dfn> and the <dfn class="term">Request Coordinator</dfn>, a notification e-mail is sent out to both the old and the new coordinators. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'When performing changes related to the <dfn class="term">Venue Coordinator</dfn> or <dfn class="term">Room Coordinator</dfn> make sure the <dfn class="term">Banner</dfn> information is up to date. ';
  $markup .= 'This can be ensured by performing <strong class="bold">troubleshooting options 3 or 5</strong>. ';
  $markup .= 'At any point in time, the currently cached <dfn class="term">Banner</dfn> information may be viewed under <strong class="bold">troubleshooting option 5</strong>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  #$markup .= 'Many of the <dfn class="term">Review</dfn> operations are available to <dfn class="term">Managers</dfn> even if that <dfn class="term">Manager</dfn> is not given explicit access to. ';
  #$markup .= 'For example, <dfn class="term">Managers</dfn> may act as both <dfn class="term">Insurer</dfn> and <dfn class="term">Financer</dfn> even when they are not explicitly assigned such roles. ';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/manage/users' . $url_arguments . '" class="item-link">Section: Users Management</a>. ';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: manage - users.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_manage_section_users($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-manage-section-users', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'The <dfn class="term"><abbr class="abbreviation" title="Facilities Use System">FCS</abbr></dfn> provides a few different ways to manage users. ';
  $markup .= 'The first is the management of the user account itself, simply called <strong class="bold">manage users</strong>. ';
  $markup .= 'The second is the management of <dfn class="term">Reviewers</dfn>. ';
  $markup .= 'The third is the management of <dfn class="term">Venue Coordinator</dfn> proxies. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'Each user has a small set of fields describing their account. ';
  $markup .= 'Each user must have an e-mail account and that e-mail account is required to be an <em class="emphasis">@mcneese.edu</em> address or an <em class="emphasis">@student.mcneese.edu</em> address. ';
  $markup .= 'The <dfn class="term">Banner ID</dfn> is required for all accounts unless an exception is flagged. ';
  $markup .= 'The phone number is optional. ';
  $markup .= 'A primary account checkbox is available for designating that an account is or is not a primary account. ';
  $markup .= 'Primary accounts are tied to the <dfn class="term">Banner ID</dfn>. ';
  $markup .= 'Most accounts should be a primary account and primary accounts are the user accounts that a person should use for accessing this system. ';
  $markup .= 'Accounts may be locked and locking prevents further access to this system even if <dfn class="term">Banner</dfn> allows states that the user should have access. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'Each account has a set of roles that may be assigned to the user. ';
  $markup .= 'These roles are broken up into 4 groups: basic roles, venue-specific roles, special/exceptional roles, and administration roles. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'The basic roles group is for defining how a user is classified. ';
  $markup .= '<ol class="list">';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Requester Role</strong>: ';
  $markup .= '<br>';
  $markup .= 'This role is auto-assigned by the system. ';
  $markup .= 'The user may access the system, navigate <dfn class="term">Accepted</dfn> <dfn class="term">Requests</dfn>, and make <dfn class="term">Requests</dfn>. ';
  $markup .= 'Bannerless exception allows for manual control over this role. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Reviewer Role</strong>: ';
  $markup .= '<br>';
  $markup .= 'This role is auto-assigned by the system. ';
  $markup .= 'The user may act as a <dfn class="term">Reviewer</dfn> or a <dfn class="term">Venue Coordinator</dfn>. ';
  $markup .= 'Bannerless exception allows for manual control over this role. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Manager Role</strong>: ';
  $markup .= '<br>';
  $markup .= 'The user may act as a <dfn class="term">Manager</dfn>. ';
  $markup .= 'Grant this role only to authorized users. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Financer Role</strong>: ';
  $markup .= '<br>';
  $markup .= 'The user may act as a <dfn class="term">Financer</dfn>. ';
  $markup .= 'This is a subset of <dfn class="term">Reviewer</dfn> and requires the user to have the reviewer role to properly function. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Insurer Role</strong>: ';
  $markup .= '<br>';
  $markup .= 'The user may act as an <dfn class="term">Insurer</dfn>. ';
  $markup .= 'This is a subset of <dfn class="term">Reviewer</dfn> and requires the user to have the reviewer role to properly function. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '</ol>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'The venue-specific roles group is for defining the <dfn class="term">Request Classification</dfn> a user belongs to and is allowed to operate with. ';
  $markup .= 'All of these roles are auto-populated unless bannerless exception role is assigned. ';
  $markup .= '<ol class="list">';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Camp Role</strong>: ';
  $markup .= '<br>';
  $markup .= 'The user is assigned the <dfn class="term">Camp</dfn> <dfn class="term">Request Classification</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Employee Role</strong>: ';
  $markup .= '<br>';
  $markup .= 'The user is assigned the <dfn class="term">Faculty, Staff, and Athletics</dfn> <dfn class="term">Request Classification</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">External Role</strong>: ';
  $markup .= '<br>';
  $markup .= 'The user is assigned the <dfn class="term">External</dfn> <dfn class="term">Request Classification</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Student Role</strong>: ';
  $markup .= '<br>';
  $markup .= 'The user is assigned the <dfn class="term">Student Organization</dfn> <dfn class="term">Request Classification</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '</ol>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'The special/exception roles group is for special or exceptional cases. ';
  $markup .= 'These are commonly assigned for testing or for when the auto-assignment of user information is not working properly for a given user. ';
  $markup .= '<ol class="list">';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Bannerless Exception Role</strong>: ';
  $markup .= '<br>';
  $markup .= 'All <dfn class="term">Banner</dfn> related information normally auto-populated for the user no longer becomes auto-populated. ';
  $markup .= 'This allows for the user account to not require a <dfn class="term">Banner ID</dfn>';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Locked Exception Role</strong>: ';
  $markup .= '<br>';
  $markup .= 'There may be situations where accounts get auto-locked (and auto-unlocked). ';
  $markup .= 'This role prevents the account from being auto-locked or auto-unlocked. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Reviewer Exception Role</strong>: ';
  $markup .= '<br>';
  $markup .= 'When assigned, the reviewer role will not be auto-added or auto-removed. ';
  $markup .= 'This prevents <dfn class="term">Banner</dfn> updates, <dfn class="term">Venue Coordinator</dfn> updates, or <dfn class="term">Reviewer</dfn> updates from altering a users reviewer role. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '</ol>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'The administrations roles group is for administrative cases. ';
  $markup .= '<ol class="list">';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Administrator Role</strong>: ';
  $markup .= '<br>';
  $markup .= 'This role grants the user full administrative access to the system (including technical aspects not available to <dfn class="term">Managers</dfn>. ';
  $markup .= '<dfn class="term">Managers</dfn> are not allowed to add or remove this role. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Masquerade Role</strong>: ';
  $markup .= '<br>';
  $markup .= 'This allows a user to switch to (or masquerade as) another user. ';
  $markup .= 'This functionality is only available on testing systems. ';
  $markup .= 'This role may only be applied to users with the administrator role or the manager role. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '</ol>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'While <dfn class="term">Venue Coordinator</dfn> roles are assigned via <dfn class="term">Banner</dfn>, proxies for each <dfn class="term">Venue Coordinator</dfn> may be assigned via this system. ';
  $markup .= 'These proxies are able to perform all tasks as the specified <dfn class="term">Venue Coordinator</dfn> in their place. ';
  $markup .= 'The proxy status only applies to <dfn class="term">Venue Coordinators</dfn> and does not grant the user the ability to proxy as a <dfn class="term">Reviewer</dfn> or a <dfn class="term">Requester</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'Each <dfn class="term">Request Classification</dfn> has a set of <dfn class="term">Reviewers</dfn> who are allowed to <dfn class="term">Review</dfn> a particular <dfn class="term">Classification</dfn>. ';
  $markup .= 'Each <dfn class="term">Reviewer</dfn> is assigned a <dfn class="term">Reviewer Classification</dfn>. ';
  $markup .= 'Multiple users with the same <dfn class="term">Reviewer Classification</dfn> count towards the same <dfn class="term">Review</dfn> decision. ';
  $markup .= 'That is to say that each <dfn class="term">Review</dfn> decision is tied to the <dfn class="term">Reviewer Classification</dfn> instead of the individual <dfn class="term">Reviewer</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/manage/problems' . $url_arguments . '" class="item-link">Section: Problems Management</a>. ';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: manage - problems.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_manage_section_problems($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-manage-section-problems', $section_information['header_attributes'], $section_information['headers'], $start_count);



  $markup .= '<div class="paragraph">';
  $markup .= 'The website has a troubleshooting page where special operations are provided. ';
  $markup .= 'Each of these troubleshooting options is assigned a number to uniquely identify it for easy reference. ';
  $markup .= '<br>';

  $markup .= '<ol class="list">';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Troubleshooting Option 1</strong>: Requests Review Recheck. ';
  $markup .= '<br>';
  $markup .= 'Whenever <dfn class="term">Reviewer</dfn> types are added or removed on the <dfn class="term">Reviewer</dfn> management pages, <dfn class="term">Requests</dfn> currently under <dfn class="term">Review</dfn> may become stuck in an unusual state. ';
  $markup .= 'Performing the provided task will find problematic <dfn class="term">Requests</dfn> and re-process the <dfn class="term">Review</dfn> decisions. ';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Troubleshooting Option 2</strong>: Requests Cache Synchronize. ';
  $markup .= '<br>';
  $markup .= '<dfn class="term">Requests</dfn> have common information, such as <dfn class="term">Status</dfn> and <dfn class="term">Step</dfn>, cached for performance improvements. ';
  $markup .= 'In rare cases, this can become inconsistent or otherwise corrupt. ';
  $markup .= 'Performing the provided task will re-create the cache, fixing any such problems. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Troubleshooting Option 3</strong>: Banner Cache Synchronize. ';
  $markup .= '<br>';
  $markup .= 'The servers that provide the <dfn class="term">Banner</dfn> information have scheduled downtimes, everday. ';
  $markup .= 'This makes access to the <dfn class="term">Banner</dfn> information impossible during such downtimes. ';
  $markup .= 'To ensure that this system can operate while <dfn class="term">Banner</dfn> is unavailable, a cache is used. ';
  $markup .= 'This cache takes a long time to build and under normal circumstances is only updated once a day. ';
  $markup .= 'Performing the provided task should cause this cache to be re-created, ensuring the most up to date information. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Troubleshooting Option 4</strong>: Banner Role Cache Synchronize. ';
  $markup .= '<br>';
  $markup .= 'This cache is part of the <dfn class="term">Banner</dfn> information, but due to its common use has been added as a separate option. ';
  $markup .= 'Like the rest of the <dfn class="term">Banner</dfn> cache, this is updated once a day. ';
  $markup .= 'Unlike the rest of the <dfn class="term">Banner</dfn> cache, this should only take a few seconds to perform. ';
  $markup .= 'Performing the provided task will re-create only the banner cache associated with user roles. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Troubleshooting Option 5</strong>: Banner Information. ';
  $markup .= '<br>';
  $markup .= 'To make it easier to identify <dfn class="term">Banner</dfn> cache bugs, this exposes all cached data related to <dfn class="term">Banner</dfn>. ';
  $markup .= 'This should make it easier to tell if the cache is up to date or if the cache has some incorrect data in it. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Troubleshooting Option 6</strong>: Requests Problem Check. ';
  $markup .= '<br>';
  $markup .= 'The system has extensive constraints, locks, and restrictions to prevent problems from happening. ';
  $markup .= 'Depsite this, there are still some situations where problems may still occur. ';
  $markup .= 'Most of these problems will require a user to make a decision on how to fix them. ';
  $markup .= 'Performing the provided task will scan the system for known request-related problems and report any discovered problems so that they may be manually resolved. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Troubleshooting Option 7</strong>: Users Problem Check. ';
  $markup .= '<br>';
  $markup .= 'The system has extensive constraints, locks, and restrictions to prevent problems from happening. ';
  $markup .= 'Depsite this, there are still some situations where problems may still occur. ';
  $markup .= 'Most of these problems will require a user to make a decision on how to fix them. ';
  $markup .= 'Performing the provided task will scan the system for known user-related problems and report any discovered problems so that they may be manually resolved. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '</ol>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'All detected problems are stored in their respective <dfn class="term">Request</dfn> problems or user problems pages. ';
  $markup .= 'Each of the problems are assigned a problem category (such as Conflict, Room Invalid, Old/Duplicate Account, Invalid Banner ID, etc..). ';
  $markup .= 'All of the <dfn class="term">Request</dfn> problems have the ability to be ignored or reset. ';
  $markup .= 'Some of the <dfn class="term">Request</dfn> problems have options for automatically fixing them while others require manual fixes. ';
  $markup .= 'Most of the user problems only have the option to be reset. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'The following are the <dfn class="term">Request</dfn> problem categories: ';
  $markup .= '<ul class="list">';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Conflict</strong>: ';
  $markup .= '<br>';
  $markup .= 'The dates and time requested for the given <dfn class="term">Room</dfn> are coinciding with the dates and time for another <dfn class="term">Request</dfn> for the same <dfn class="term">Room</dfn>. ';
  $markup .= 'This is only flagged for conflicts between other <dfn class="term">Requests</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Stale</strong>: ';
  $markup .= '<br>';
  $markup .= '<dfn class="term">Requests</dfn> that have not reached the <dfn class="term">Completed</dfn> <dfn class="term">Review Step</dfn> and have scheduled dates in the past are considered stale. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Stuck</strong>: ';
  $markup .= '<br>';
  $markup .= 'There is something preventing the <dfn class="term">Request</dfn> from being able to move to the next <dfn class="term">Review Step</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Blocked</strong>: ';
  $markup .= '<br>';
  $markup .= 'The <dfn class="term">Requested</dfn> is prevented from being able to move to the next <dfn class="term">Review Step</dfn> specifically because of a conflict with another <dfn class="term">Request</dfn> for the specified dates, times, and <dfn class="term">Room</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Coordinator Invalid</strong>: ';
  $markup .= '<br>';
  $markup .= 'The <dfn class="term">Venue Coordinator</dfn> assigned to the <dfn class="term">Request</dfn> is no longer a <dfn class="term">Venue Coordinator</dfn> or otherwise lacks access to perform <dfn class="term">Venue Coordinator</dfn> duties. ';
  $markup .= 'This happens most often when the <dfn class="term">Banner</dfn> information related to the <dfn class="term">Venue Coordinator</dfn> changes or the user no longer has access to this system. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Requester Invalid</strong>: ';
  $markup .= '<br>';
  $markup .= 'The <dfn class="term">Request Coordinator</dfn> assigned to the <dfn class="term">Request</dfn> no longer has access to the <dfn class="term">Request</dfn> or the user no longer has access to this system. ';
  $markup .= 'This happens most often when the <dfn class="term">Banner</dfn> information related to the <dfn class="term">Request Coordinator</dfn> changes. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Room Invalid</strong>: ';
  $markup .= '<br>';
  $markup .= 'The <dfn class="term">Room</dfn> associated with a given <dfn class="term">Request</dfn> no longer exists, is disabled, or is otherwise invalid. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Building Invalid</strong>: ';
  $markup .= '<br>';
  $markup .= 'The <dfn class="term">Building</dfn> associated with a given <dfn class="term">Request</dfn> no longer exists, is disabled, or is otherwise invalid. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Location Invalid</strong>: ';
  $markup .= '<br>';
  $markup .= 'The <dfn class="term">Location</dfn> associated with a given <dfn class="term">Request</dfn> no longer exists, is disabled, or is otherwise invalid. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Cache Invalid</strong>: ';
  $markup .= '<br>';
  $markup .= 'The cached information for a given <dfn class="term">Request</dfn> is inconsistent or otherwise inaccurate. ';
  $markup .= 'The <dfn class="term">Request</dfn> cache needs to be re-synchronized. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Room Unavailable</strong>: ';
  $markup .= '<br>';
  $markup .= 'The dates and times requested for the given <dfn class="term">Room</dfn> are coinciding with an unavailability (such as a class room or emergency campus closure). ';
  $markup .= 'This does not get flagged for conflicts between <dfn class="term">Requests</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '</ul>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'The following are the user problem categories: ';
  $markup .= '<ul class="list">';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Core System Problem</strong>: ';
  $markup .= '<br>';
  $markup .= 'Any non-specific, generic, system related problems are associated with this category. ';
  $markup .= 'These problems are related to the technical structure and functioning of the system. ';
  $markup .= 'Should they be detected, the system <dfn class="term">Administrators</dfn> will have to address the issue. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Facilities Use Problem</strong>: ';
  $markup .= '<br>';
  $markup .= 'Any non-specific, generic, <dfn class="term">Facilities Use System</dfn> problems are associated with this category. ';
  $markup .= 'These problems are related to the logic, flow, and presentation of the system. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Banner Unavailable on Create</strong>: ';
  $markup .= '<br>';
  $markup .= 'This problem is flagged when the user account does not yet exist on the system, <dfn class="term">Banner</dfn> is unavailable during account creation, and the specified user currently does not exist in the <dfn class="term">Banner</dfn> cache. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Banner Unavailable on Login</strong>: ';
  $markup .= '<br>';
  $markup .= 'This problem is flagged when <dfn class="term">Banner</dfn> is unavailable during login and the specified user currently does not exist in the <dfn class="term">Banner</dfn> cache. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Multiple Login Attempt Problems</strong>: ';
  $markup .= '<br>';
  $markup .= 'This problem is flagged when a large number of login problems are detected for a given user. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Reviewer/Venue-Coordinator Role Not Allowed</strong>: ';
  $markup .= '<br>';
  $markup .= 'This problem is flagged when a user has been assigned the <dfn class="term">Reviewer</dfn> or <dfn class="term">Venue Coordiantor</dfn> roles but according to the cached <dfn class="term">Banner</dfn> information, they should not be allowed to do so. ';
  $markup .= 'This flag is information because when this problem is flagged, the unallowed roles have (or should have been) auto-removed for the <dfn class="term">Reviewer</dfn> or the <dfn class="term">Venue Coordiantor</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Old/Duplicate Account</strong>: ';
  $markup .= '<br>';
  $markup .= 'The user information is populated through <abbr class="abbreviation" title="Lightweight Directory Access Protocol">LDAP</abbr>, using the user\'s assigned <dfn class="term">Banner ID</dfn>. ';
  $markup .= 'Multiple accounts may exist per <dfn class="term">Banner ID</dfn> in <abbr class="abbreviation" title="Lightweight Directory Access Protocol">LDAP</abbr>. ';
  $markup .= 'This system is not designed to operate with multiple accounts per <dfn class="term">Banner ID</dfn>, so when multiple accounts are detected, this flag is assigned. ';
  $markup .= 'This can be resolved by assigning one account as the primary account and the all other accounts as not the primary account. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Disabled Account in Use</strong>: ';
  $markup .= '<br>';
  $markup .= 'Accounts that are locked are disabled accounts. ';
  $markup .= 'If a user with a disabled account is still on the system after the account is disabled, this is considered a security problem. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Invalid Banner ID</strong>: ';
  $markup .= '<br>';
  $markup .= 'This flag is assigned when the <dfn class="term">Banner ID</dfn> provided by <abbr class="abbreviation" title="Lightweight Directory Access Protocol">LDAP</abbr> is not found in <dfn class="term">Banner</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Insufficient Banner Access</strong>: ';
  $markup .= '<br>';
  $markup .= 'Users who have have logged into the system but can perform no actions due to a lack of privileges provided by <dfn class="term">Banner</dfn> are assigned this flag. ';
  $markup .= 'This happens most often with students who may not yet have enrolled in courses between semesters. ';
  $markup .= 'This flag is informative as it must be resolved in <dfn class="term">Banner</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Core System Form Errors</strong>: ';
  $markup .= '<br>';
  $markup .= 'An technical problems detected while filling out a form on the system are assigned this flag. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Facilities Use Form Errors</strong>: ';
  $markup .= '<br>';
  $markup .= 'An logical, flow, and presentation problems detected while filling out a form on the system are assigned this flag. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '</ul>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/manage/availability' . $url_arguments . '" class="item-link">Section: Availability Management</a>. ';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: manage - availability.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_manage_section_availability($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-manage-section-availability', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= '<dfn class="term">Managers</dfn> are allowed to assign the availability, or more specifically the unavailability, for a given <dfn class="term">Room</dfn> at a specified date and time. ';
  $markup .= 'This availability interface also allows for specifying holidays. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'The <dfn class="term">Room</dfn> unavailability is broken down into <dfn class="term">Building</dfn> and <dfn class="term">Room</dfn> listings. ';
  $markup .= 'Each unavailability entry is assigned to a specific <dfn class="term">Room</dfn> within a given <dfn class="term">Building</dfn>. ';
  $markup .= 'Unavailabilities may be specified using a start and stop date and time range, but for multiple separate dates, a new unavailability must be used. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'There are the following types of unavailabilities: ';
  $markup .= '<ul class="list">';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Renovation</strong>: ';
  $markup .= '<br>';
  $markup .= 'This is intended to designate renovations to a given <dfn class="term">Room</dfn>. ';
  $markup .= 'The specific interpretation and use is left to the <dfn class="term">Manager</dfn> to define. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Repair</strong>: ';
  $markup .= '<br>';
  $markup .= 'This is intended to designate repairs to a given <dfn class="term">Room</dfn>. ';
  $markup .= 'The specific interpretation and use is left to the <dfn class="term">Manager</dfn> to define. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Planned Refurbishment</strong>: ';
  $markup .= '<br>';
  $markup .= 'This is intended to designate planned changes, redesigns, etc.. to a given <dfn class="term">Room</dfn>. ';
  $markup .= 'The specific interpretation and use is left to the <dfn class="term">Manager</dfn> to define. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Special Event</strong>: ';
  $markup .= '<br>';
  $markup .= 'There is a special event taking place that requires the use of the specified <dfn class="term">Room</dfn>. ';
  $markup .= 'The specific interpretation and use is left to the <dfn class="term">Manager</dfn> to define. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Outage</strong>: ';
  $markup .= '<br>';
  $markup .= 'Some service provided for, by, or within the specified <dfn class="term">Room</dfn> is unavailable (such as electricity). ';
  $markup .= 'The specific interpretation and use is left to the <dfn class="term">Manager</dfn> to define. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Emergency</strong>: ';
  $markup .= '<br>';
  $markup .= 'There is an emergency related to the <dfn class="term">Room</dfn>, the campus, or the surrounding areas making this <dfn class="term">Room</dfn> unavailable. ';
  $markup .= 'The specific interpretation and use is left to the <dfn class="term">Manager</dfn> to define. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">Class</strong>: ';
  $markup .= '<br>';
  $markup .= 'This <dfn class="term">Room</dfn> is being used as a class room, or perhaps class related activities, as defined by <dfn class="term">Banner</dfn>. ';
  $markup .= '<dfn class="term">Managers</dfn> are not allowed to assign this unavailability as this is assigned automatically via <dfn class="term">Banner</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '</ul>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'Holidays are not assigned to any specific <dfn class="term">Room</dfn>. ';
  $markup .= 'Each holiday must have an entry created for each day and are all day events. ';
  $markup .= '<dfn class="term">Requests</dfn> may still be reserved on holidays. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/manage/logs' . $url_arguments . '" class="item-link">Section: Logs Management</a>. ';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: manage - logs.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_manage_section_logs($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_url;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-manage-section-logs', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'Nearly every action performed on this system is logged in some way. ';
  $markup .= '<dfn class="term">Managers</dfn> have access to view most of these logs. ';
  $markup .= '<dfn class="term">Requests</dfn> have both a <dfn class="term">Review Log</dfn> and a <dfn class="term">Request History</dfn>. ';
  $markup .= 'E-mails sent by this system are recorded and logged. ';
  $markup .= 'Special management tasks such as changes to holidays, changes to unavailabilities, troubleshooting actions, and request problem actions are logged. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'This system is primarily designed around <dfn class="term">Requests</dfn> and the <dfn class="term">Review</dfn> of <dfn class="term">Requests</dfn>. ';
  $markup .= 'This makes the <dfn class="term">Request</dfn> and <dfn class="term">Review</dfn> related log entries the most thorough, most complex, and most readily available logs. ';
  $markup .= 'The <dfn class="term">Review Log</dfn> provides provides a log of all <dfn class="term">Review</dfn> decisions made and any notable action that should be made visible to the involved parties. ';
  $markup .= 'The <dfn class="term">Request History</dfn> is intended as a complete log of all changes performed directly to a given <dfn class="term">Request</dfn>. ';
  $markup .= 'Only a limited set of users may view the <dfn class="term">Request History</dfn>. ';
  $markup .= '<dfn class="term">Manages</dfn> may see the <dfn class="term">Request History</dfn> for all <dfn class="term">Requests</dfn>. ';
  $markup .= '<dfn class="term">Venue Coordinators</dfn> may see the <dfn class="term">Request History</dfn> only for <dfn class="term">Requests</dfn> that they are allowed to coordinate. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'The e-mail log provides a way to view the complete details of an e-mail (including technical information such as headers) prior to leaving the system. ';
  $markup .= 'These e-mails may also be re-sent in case without modification. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'The other logged tasks currently do not have an interface written for viewing but can be obtained directly from the database as necessary. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'There is also a statistics page on the site that is tweaked for processing the <dfn class="term">Request</dfn> logs. ';
  $markup .= 'The <dfn class="term">Request</dfn> statistics log breaks <dfn class="term">Requests</dfn> down by <dfn class="term">Classification</dfn>, <dfn class="term">Type</dfn>, <dfn class="term">Step</dfn>, and <dfn class="term">Status</dfn>. ';
  $markup .= 'The information is presented on a yearly basis, but monthly and daily information may be viewed. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'No links have been added for easily selecting the monthly or daily <dfn class="term">Request</dfn> statistics. ';
  $markup .= 'To view these statistics, go to the url bar while viewing the <dfn class="term">Request</dfn> statistics page and add in <strong class="bold">/month_name</strong> or <strong class="bold">/month_name/day_number</strong>. ';
  $markup .= 'An example for <em class="emphasis">October 1, 2016</em> would be: <strong class="bold">' . $base_url . '/requests/statistics-0/2016/october/01</strong>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: email.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_email($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-email', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'The <dfn class="term"><abbr class="abbreviation" title="Facilities Use System">FCS</abbr></dfn> sends e-mails for a large number of actions performed. ';
  $markup .= 'These e-mails are designed to assist the users, keeping them up to date of the ongoings of the system without having to login to the system. ';
  $markup .= 'The e-mails themselves are purpely informative and cannot be used to perform any action. ';
  $markup .= 'For improved compatibility, each e-mail presents its information in two formats: plain text and <abbr class="abbreviation" title="Hypertext Markup Language">HTML</abbr>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'Do not reply to e-mails system from this system. ';
  $markup .= 'Your replies will not be received. ';
  $markup .= 'Instead, all responses to received e-mails should be sent via <dfn class="term">Comments</dfn> in the <dfn class="term"><abbr class="abbreviation" title="Facilities Use System">FCS</abbr></dfn> for the appropraite <dfn class="term">Request</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'All e-mails sent by this system are saved. ';
  $markup .= 'If you believe that you did not receive an e-mail (or lost an e-mail), then you can ask a <dfn class="term">Manager</dfn> to re-send a particular e-mail. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'View Section: <a href="' . $base_path . 'help-0/email/request' . $url_arguments . '" class="item-link">Request E-Mail</a>. ';
    $markup .= '<br>';
    $markup .= 'View Section: <a href="' . $base_path . 'help-0/email/advanced' . $url_arguments . '" class="item-link">Advanced E-Mail</a>. ';
    $markup .= '<br>';
    $markup .= '<br>';

    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/calendar' . $url_arguments . '" class="item-link">Documentation: Calendar</a>. ';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: email - requests.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_email_section_request($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_url;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-email-section-requests', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'Most e-mails sent by this system are directly related to a particular <dfn class="term">Request</dfn>. ';
  $markup .= 'These e-mails contain a special structure to make identifying and reading the e-mails relatively easy. ';
  $markup .= 'The subject of each e-mail contains generic text, such as <strong class="bold">Facilities Use Request</strong>. ';
  $markup .= 'This generic text is then followed by the <dfn class="term">Request ID</dfn>, the <dfn class="term">Request</dfn> name, and finally an action or purpose that the e-mail represents. ';
  $markup .= 'The contents of each e-mail is broken down into multiple parts. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'The first part is often a generated message describing the purpose of the e-mail, such as <em class="emphasis">The System has cancelled the request</em>. ';
  $markup .= 'This is then followed by any comments made or associated with the action. ';
  $markup .= 'The second part contains common <dfn class="term">Request</dfn> information. ';
  $markup .= 'This part contains the following information: <dfn class="term">Request ID</dfn>, <dfn class="term">Request</dfn> name, <dfn class="term">Location</dfn>, <dfn class="term">Building</dfn>, <dfn class="term">Room</dfn>, <dfn class="term">Classification</dfn>, <dfn class="term">Type</dfn>, and all dates and times. ';
  $markup .= 'The third part presents a link to the <dfn class="term">Request</dfn> for easily accessing the entire <dfn class="term">Request</dfn>. ';
  $markup .= 'The fourth an final part is a date representing the exact time this e-mail was generated. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: email - advanced.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_email_section_advanced($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_url;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-email-section-advanced', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'Each e-mails contains additional information to assist in validity, integrity, and use of the information provided. ';
  $markup .= 'Some e-mails contain an <dfn class="term">iCalendar</dfn> file for importing the <dfn class="term">Request</dfn> into your own private calendar. ';
  $markup .= 'All e-mails should contain an <dfn class="term">OpenPG</dfn> file for helping insure the validity and integrity of the e-mail. ';
  $markup .= 'All e-mails contain special headers for consistent advanced filtering. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'Received e-mails are not always the same as the original e-mail and may contain additional header information not specified by this system. ';
  $markup .= 'E-mail services that transfer the e-mail to the recipient can (and often do) alter the header information. ';
  $markup .= 'Most people will never notice this because the header information is normally not displayed. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'When a <dfn class="term">Request</dfn> completes the <dfn class="term">Final Approval Step</dfn> and becomes <dfn class="term">Accepted</dfn> an e-mail is sent out informing the <dfn class="term">Requester</dfn> of the acceptance. ';
  $markup .= 'That e-mail should contain an <dfn class="term">iCalendar</dfn> file attachment for easy import of the <dfn class="term">Accepted</dfn> <dfn class="term">Request</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'Every e-mail should have an <dfn class="term">OpenPG</dfn> download attachment as a digital signature for that e-mail. ';
  $markup .= 'These signatures only applies to the contents of the e-mail and does not guarantee anything else. ';
  $markup .= 'Not all e-mail clients support this and if yours does not, this digital signature should not cause any problem. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'If your e-mail client allows for advanced filtering of e-mails based on custom header information, then you can filter by the following header names: ';
  $markup .= '<ul class="list">';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">fcs-request-id</strong>: ';
  $markup .= '<br>';
  $markup .= 'This is a number representing the <dfn class="term">Request ID</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">fcs-request-revision</strong>: ';
  $markup .= '<br>';
  $markup .= 'This is a number representing the <dfn class="term">Request Revision</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">fcs-request-name</strong>: ';
  $markup .= '<br>';
  $markup .= 'This is a string representing the <dfn class="term">Request</dfn> name. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">fcs-request-status</strong>: ';
  $markup .= '<br>';
  $markup .= 'This is a number used to uniquely represent the <dfn class="term">Request Status</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">fcs-request-step</strong>: ';
  $markup .= '<br>';
  $markup .= 'This is a number used to uniquely represent the <dfn class="term">Review Step</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">fcs-request-classification</strong>: ';
  $markup .= '<br>';
  $markup .= 'This is a number used to uniquely represent the <dfn class="term">Request Classification</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">fcs-request-type</strong>: ';
  $markup .= '<br>';
  $markup .= 'This is a number used to uniquely represent the <dfn class="term">Request Type</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">fcs-request-building</strong>: ';
  $markup .= '<br>';
  $markup .= 'This is a number used to uniquely represent the <dfn class="term">Building</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">fcs-request-room</strong>: ';
  $markup .= '<br>';
  $markup .= 'This is a number used to uniquely represent the <dfn class="term">Room</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">fcs-request-url</strong>: ';
  $markup .= '<br>';
  $markup .= 'This is a string representing a <dfn class="term"><abbr class="abbreviation" title="Universal Resource Locator">URL</abbr></dfn> associated with the <dfn class="term">Request</dfn>. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">fcs-email-generated</strong>: ';
  $markup .= '<br>';
  $markup .= 'This is a number representing the UNIX timestamp of the date in which the e-mail was generated. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '<li class="list-item">';
  $markup .= '<strong class="bold">fcs-email-type</strong>: ';
  $markup .= '<br>';
  $markup .= 'This is a number used to uniquely define the purpose of the e-mail. ';
  $markup .= '<br>';
  $markup .= '</li>';
  $markup .= '</ul>';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'View Sub-Section: <a href="' . $base_path . 'help-0/email/advanced/request_status_codes' . $url_arguments . '" class="item-link">Request Status Codes</a>. ';
    $markup .= '<br>';
    $markup .= 'View Sub-Section: <a href="' . $base_path . 'help-0/email/advanced/review_step_codes' . $url_arguments . '" class="item-link">Review Step Codes</a>. ';
    $markup .= '<br>';
    $markup .= 'View Sub-Section: <a href="' . $base_path . 'help-0/email/advanced/request_classification_codes' . $url_arguments . '" class="item-link">Request Classification Codes</a>. ';
    $markup .= '<br>';
    $markup .= 'View Sub-Section: <a href="' . $base_path . 'help-0/email/advanced/request_type_codes' . $url_arguments . '" class="item-link">Request Type Codes</a>. ';
    $markup .= '<br>';
    $markup .= 'View Sub-Section: <a href="' . $base_path . 'help-0/email/advanced/email_type_codes' . $url_arguments . '" class="item-link">E-Mail Type Codes</a>. ';
    $markup .= '<br>';
    $markup .= '<br>';

    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/calendar' . $url_arguments . '" class="item-link">Documentation: Calendar</a>. ';
    $markup .= '</div>';
  }


  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: email - advanced - request status codes.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_email_section_advanced_sub_section_status_codes($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_url;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  $options =  mfcs_get_request_status_list_options(NULL, TRUE);

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-email-section-advanced-sub_section-request_status_codes', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';

  $codes = array();

  if (array_key_exists(MFCS_REQUEST_STATUS_DELETED, $options)) {
    $codes[MFCS_REQUEST_STATUS_DELETED] = '';
    $codes[MFCS_REQUEST_STATUS_DELETED] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_STATUS_DELETED] .= '<strong class="bold">Code ' . MFCS_REQUEST_STATUS_DELETED . ', ' . $options[MFCS_REQUEST_STATUS_DELETED] . '</strong>: ';
    $codes[MFCS_REQUEST_STATUS_DELETED] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_DELETED] .= 'The <dfn class="term">Facilities Use System</dfn> does not permanently delete <dfn class="term">Requests</dfn>. ';
    $codes[MFCS_REQUEST_STATUS_DELETED] .= 'Instead, deleted <dfn class="term">Requests</dfn> are assigned the deleted status. ';
    $codes[MFCS_REQUEST_STATUS_DELETED] .= 'There currently is no interface for viewing or accessing deleted content. ';
    $codes[MFCS_REQUEST_STATUS_DELETED] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_DELETED] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_STATUS_LOCKED, $options)) {
    $codes[MFCS_REQUEST_STATUS_LOCKED] = '';
    $codes[MFCS_REQUEST_STATUS_LOCKED] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_STATUS_LOCKED] .= '<strong class="bold">Code ' . MFCS_REQUEST_STATUS_LOCKED . ', ' . $options[MFCS_REQUEST_STATUS_LOCKED] . '</strong>: ';
    $codes[MFCS_REQUEST_STATUS_LOCKED] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_LOCKED] .= 'Designates that the <dfn class="term">Request</dfn> is restricted or otherwise preventing other <dfn class="term">Requests</dfn> from requesting reservation for the same dates and times as this <dfn class="term">Request</dfn>. ';
    $codes[MFCS_REQUEST_STATUS_LOCKED] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_LOCKED] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_STATUS_UNLOCKED, $options)) {
    $codes[MFCS_REQUEST_STATUS_UNLOCKED] = '';
    $codes[MFCS_REQUEST_STATUS_UNLOCKED] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_STATUS_UNLOCKED] .= '<strong class="bold">Code ' . MFCS_REQUEST_STATUS_UNLOCKED . ', ' . $options[MFCS_REQUEST_STATUS_UNLOCKED] . '</strong>: ';
    $codes[MFCS_REQUEST_STATUS_UNLOCKED] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_UNLOCKED] .= 'Designates that the <dfn class="term">Request</dfn> is currently not restricted or preventing other <dfn class="term">Requests</dfn> from requesting reservation for the same dates and times as this <dfn class="term">Request</dfn>. ';
    $codes[MFCS_REQUEST_STATUS_UNLOCKED] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_UNLOCKED] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_STATUS_CLOSED_ACCEPTED, $options)) {
    $codes[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] = '';
    $codes[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] .= '<strong class="bold">Code ' . MFCS_REQUEST_STATUS_CLOSED_ACCEPTED . ', ' . $options[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] . '</strong>: ';
    $codes[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] .= 'Designates that the <dfn class="term">Request</dfn> has been <dfn class="term">Accepted</dfn>. ';
    $codes[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_STATUS_CLOSED_DENIED, $options)) {
    $codes[MFCS_REQUEST_STATUS_CLOSED_DENIED] = '';
    $codes[MFCS_REQUEST_STATUS_CLOSED_DENIED] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_STATUS_CLOSED_DENIED] .= '<strong class="bold">Code ' . MFCS_REQUEST_STATUS_CLOSED_DENIED . ', ' . $options[MFCS_REQUEST_STATUS_CLOSED_DENIED] . '</strong>: ';
    $codes[MFCS_REQUEST_STATUS_CLOSED_DENIED] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_CLOSED_DENIED] .= 'Designates that the <dfn class="term">Request</dfn> has been <dfn class="term">Denied</dfn>. ';
    $codes[MFCS_REQUEST_STATUS_CLOSED_DENIED] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_CLOSED_DENIED] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE, $options)) {
    $codes[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE] = '';
    $codes[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE] .= '<strong class="bold">Code ' . MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE . ', ' . $options[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE] . '</strong>: ';
    $codes[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE] .= 'Designates that the <dfn class="term">Request</dfn> has been <dfn class="term">Denied</dfn> because the <dfn class="term">Room</dfn> is unavailable. ';
    $codes[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_CLOSED_UNAVAILABLE] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK, $options)) {
    $codes[MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK] = '';
    $codes[MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK] .= '<strong class="bold">Code ' . MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK . ', ' . $options[MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK] . '</strong>: ';
    $codes[MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK] .= 'Designates that the <dfn class="term">Request</dfn> has been <dfn class="term">Denied</dfn> because another <dfn class="term">Request</dfn> is restricting or otherwise preventing this <dfn class="term">Request</dfn> from being made. ';
    $codes[MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_CLOSED_DUE_TO_LOCK] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_STATUS_CANCELLED, $options)) {
    $codes[MFCS_REQUEST_STATUS_CANCELLED] = '';
    $codes[MFCS_REQUEST_STATUS_CANCELLED] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_STATUS_CANCELLED] .= '<strong class="bold">Code ' . MFCS_REQUEST_STATUS_CANCELLED . ', ' . $options[MFCS_REQUEST_STATUS_CANCELLED] . '</strong>: ';
    $codes[MFCS_REQUEST_STATUS_CANCELLED] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_CANCELLED] .= 'Designates that the <dfn class="term">Request</dfn> has been <dfn class="term">Cancelled</dfn>. ';
    $codes[MFCS_REQUEST_STATUS_CANCELLED] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_CANCELLED] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED, $options)) {
    $codes[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED] = '';
    $codes[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED] .= '<strong class="bold">Code ' . MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED . ', ' . $options[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED] . '</strong>: ';
    $codes[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED] .= 'Designates that the <dfn class="term">Request</dfn> has been <dfn class="term">Cancelled</dfn> after it has already been <dfn class="term">Accepted</dfn>. ';
    $codes[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED] .= 'This is important for technical reasons regarding iCalendar (or similar technologies) to designate that the <dfn class="term">Request</dfn> should be removed or marked as cancelled. ';
    $codes[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED] .= '<br>';
    $codes[MFCS_REQUEST_STATUS_CLOSED_ACCEPTED_CANCELLED] .= '</li>';
  }

  if (empty($codes)) {
    $markup .= '<br>';
    $markup .= 'The system currently has no codes available. ';
  }
  else {
    ksort($codes);

    $markup .= '<ul class="list">';
    $markup .= implode('', $codes);
    $markup .= '</ul>';
  }
  unset($codes);

  $markup .= '<br>';
  $markup .= '</div>';


  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: email - advanced - review step codes.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_email_section_advanced_sub_section_step_codes($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_url;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  $options = mfcs_get_review_step_list_options(NULL, TRUE);

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-email-section-advanced-sub_section-review_step_codes', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';

  $codes = array();

  if (array_key_exists(MFCS_REVIEW_STEP_VENUE_AVAILABLE, $options)) {
    $codes[MFCS_REVIEW_STEP_VENUE_AVAILABLE] = '';
    $codes[MFCS_REVIEW_STEP_VENUE_AVAILABLE] .= '<li class="list-item">';
    $codes[MFCS_REVIEW_STEP_VENUE_AVAILABLE] .= '<strong class="bold">Code ' . MFCS_REVIEW_STEP_VENUE_AVAILABLE . ', ' . $options[MFCS_REVIEW_STEP_VENUE_AVAILABLE] . '</strong>: ';
    $codes[MFCS_REVIEW_STEP_VENUE_AVAILABLE] .= '<br>';
    $codes[MFCS_REVIEW_STEP_VENUE_AVAILABLE] .= 'The <dfn class="term">Request</dfn> is currently in the <dfn class="term">Venue Available Step</dfn>. ';
    $codes[MFCS_REVIEW_STEP_VENUE_AVAILABLE] .= '<br>';
    $codes[MFCS_REVIEW_STEP_VENUE_AVAILABLE] .= '</li>';
  }

  if (array_key_exists(MFCS_REVIEW_STEP_REVIEW, $options)) {
    $codes[MFCS_REVIEW_STEP_REVIEW] = '';
    $codes[MFCS_REVIEW_STEP_REVIEW] .= '<li class="list-item">';
    $codes[MFCS_REVIEW_STEP_REVIEW] .= '<strong class="bold">Code ' . MFCS_REVIEW_STEP_REVIEW . ', ' . $options[MFCS_REVIEW_STEP_REVIEW] . '</strong>: ';
    $codes[MFCS_REVIEW_STEP_REVIEW] .= '<br>';
    $codes[MFCS_REVIEW_STEP_REVIEW] .= 'The <dfn class="term">Request</dfn> is currently in the <dfn class="term">Review Step</dfn>. ';
    $codes[MFCS_REVIEW_STEP_REVIEW] .= '<br>';
    $codes[MFCS_REVIEW_STEP_REVIEW] .= '</li>';
  }

  if (array_key_exists(MFCS_REVIEW_STEP_FINAL_DECISION, $options)) {
    $codes[MFCS_REVIEW_STEP_FINAL_DECISION] = '';
    $codes[MFCS_REVIEW_STEP_FINAL_DECISION] .= '<li class="list-item">';
    $codes[MFCS_REVIEW_STEP_FINAL_DECISION] .= '<strong class="bold">Code ' . MFCS_REVIEW_STEP_FINAL_DECISION . ', ' . $options[MFCS_REVIEW_STEP_FINAL_DECISION] . '</strong>: ';
    $codes[MFCS_REVIEW_STEP_FINAL_DECISION] .= '<br>';
    $codes[MFCS_REVIEW_STEP_FINAL_DECISION] .= 'The <dfn class="term">Request</dfn> is currently in the <dfn class="term">Final Decision Step</dfn>. ';
    $codes[MFCS_REVIEW_STEP_FINAL_DECISION] .= '<br>';
    $codes[MFCS_REVIEW_STEP_FINAL_DECISION] .= '</li>';
  }

  if (array_key_exists(MFCS_REVIEW_STEP_COMPLETED, $options)) {
    $codes[MFCS_REVIEW_STEP_COMPLETED] = '';
    $codes[MFCS_REVIEW_STEP_COMPLETED] .= '<li class="list-item">';
    $codes[MFCS_REVIEW_STEP_COMPLETED] .= '<strong class="bold">Code ' . MFCS_REVIEW_STEP_COMPLETED . ', ' . $options[MFCS_REVIEW_STEP_COMPLETED] . '</strong>: ';
    $codes[MFCS_REVIEW_STEP_COMPLETED] .= '<br>';
    $codes[MFCS_REVIEW_STEP_COMPLETED] .= 'The <dfn class="term">Request</dfn> is currently in the <dfn class="term">Completed Step</dfn>. ';
    $codes[MFCS_REVIEW_STEP_COMPLETED] .= '<br>';
    $codes[MFCS_REVIEW_STEP_COMPLETED] .= '</li>';
  }

  if (empty($codes)) {
    $markup .= '<br>';
    $markup .= 'The system currently has no codes available. ';
  }
  else {
    ksort($codes);

    $markup .= '<ul class="list">';
    $markup .= implode('', $codes);
    $markup .= '</ul>';
  }
  unset($codes);

  $markup .= '<br>';
  $markup .= '</div>';


  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: email - advanced - request classification codes.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_email_section_advanced_sub_section_classification_codes($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_url;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  $options =  mfcs_get_request_classification_list_options(NULL, NULL, TRUE);

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-email-section-advanced-sub_section-request_classification_codes', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';

  $codes = array();

  if (array_key_exists(MFCS_REQUEST_CLASSIFICATION_STUDENT, $options)) {
    $codes[MFCS_REQUEST_CLASSIFICATION_STUDENT] = '';
    $codes[MFCS_REQUEST_CLASSIFICATION_STUDENT] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_CLASSIFICATION_STUDENT] .= '<strong class="bold">Code ' . MFCS_REQUEST_CLASSIFICATION_STUDENT . ', ' . $options[MFCS_REQUEST_CLASSIFICATION_STUDENT] . '</strong>: ';
    $codes[MFCS_REQUEST_CLASSIFICATION_STUDENT] .= '<br>';
    $codes[MFCS_REQUEST_CLASSIFICATION_STUDENT] .= 'The <dfn class="term">Request</dfn> is categorized as a <dfn class="term">' . $options[MFCS_REQUEST_CLASSIFICATION_STUDENT] . '</dfn>. ';
    $codes[MFCS_REQUEST_CLASSIFICATION_STUDENT] .= '<br>';
    $codes[MFCS_REQUEST_CLASSIFICATION_STUDENT] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_CLASSIFICATION_CAMPS, $options)) {
    $codes[MFCS_REQUEST_CLASSIFICATION_CAMPS] = '';
    $codes[MFCS_REQUEST_CLASSIFICATION_CAMPS] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_CLASSIFICATION_CAMPS] .= '<strong class="bold">Code ' . MFCS_REQUEST_CLASSIFICATION_CAMPS . ', ' . $options[MFCS_REQUEST_CLASSIFICATION_CAMPS] . '</strong>: ';
    $codes[MFCS_REQUEST_CLASSIFICATION_CAMPS] .= '<br>';
    $codes[MFCS_REQUEST_CLASSIFICATION_CAMPS] .= 'The <dfn class="term">Request</dfn> is categorized as a <dfn class="term">' . $options[MFCS_REQUEST_CLASSIFICATION_CAMPS] . '</dfn>. ';
    $codes[MFCS_REQUEST_CLASSIFICATION_CAMPS] .= '<br>';
    $codes[MFCS_REQUEST_CLASSIFICATION_CAMPS] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_CLASSIFICATION_FACULTY, $options)) {
    $codes[MFCS_REQUEST_CLASSIFICATION_FACULTY] = '';
    $codes[MFCS_REQUEST_CLASSIFICATION_FACULTY] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_CLASSIFICATION_FACULTY] .= '<strong class="bold">Code ' . MFCS_REQUEST_CLASSIFICATION_FACULTY . ', ' . $options[MFCS_REQUEST_CLASSIFICATION_FACULTY] . '</strong>: ';
    $codes[MFCS_REQUEST_CLASSIFICATION_FACULTY] .= '<br>';
    $codes[MFCS_REQUEST_CLASSIFICATION_FACULTY] .= 'The <dfn class="term">Request</dfn> is categorized as a <dfn class="term">' . $options[MFCS_REQUEST_CLASSIFICATION_FACULTY] . '</dfn>. ';
    $codes[MFCS_REQUEST_CLASSIFICATION_FACULTY] .= '<br>';
    $codes[MFCS_REQUEST_CLASSIFICATION_FACULTY] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_CLASSIFICATION_EXTERNAL, $options)) {
    $codes[MFCS_REQUEST_CLASSIFICATION_EXTERNAL] = '';
    $codes[MFCS_REQUEST_CLASSIFICATION_EXTERNAL] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_CLASSIFICATION_EXTERNAL] .= '<strong class="bold">Code ' . MFCS_REQUEST_CLASSIFICATION_EXTERNAL . ', ' . $options[MFCS_REQUEST_CLASSIFICATION_EXTERNAL] . '</strong>: ';
    $codes[MFCS_REQUEST_CLASSIFICATION_EXTERNAL] .= '<br>';
    $codes[MFCS_REQUEST_CLASSIFICATION_EXTERNAL] .= 'The <dfn class="term">Request</dfn> is categorized as a <dfn class="term">' . $options[MFCS_REQUEST_CLASSIFICATION_EXTERNAL] . '</dfn>. ';
    $codes[MFCS_REQUEST_CLASSIFICATION_EXTERNAL] .= '<br>';
    $codes[MFCS_REQUEST_CLASSIFICATION_EXTERNAL] .= '</li>';
  }

  if (empty($codes)) {
    $markup .= '<br>';
    $markup .= 'The system currently has no codes available. ';
  }
  else {
    ksort($codes);

    $markup .= '<ul class="list">';
    $markup .= implode('', $codes);
    $markup .= '</ul>';
  }
  unset($codes);

  $markup .= '<br>';
  $markup .= '</div>';


  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: email - advanced - request type codes.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_email_section_advanced_sub_section_request_type_codes($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_url;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  $options =  mfcs_get_request_type_list_options(NULL, TRUE);

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-email-section-advanced-sub_section-request_type_codes', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';

  $codes = array();

  if (array_key_exists(MFCS_REQUEST_TYPE_MEETING, $options)) {
    $codes[MFCS_REQUEST_TYPE_MEETING] = '';
    $codes[MFCS_REQUEST_TYPE_MEETING] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_TYPE_MEETING] .= '<strong class="bold">Code ' . MFCS_REQUEST_TYPE_MEETING . ', ' . $options[MFCS_REQUEST_TYPE_MEETING] . '</strong>: ';
    $codes[MFCS_REQUEST_TYPE_MEETING] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_MEETING] .= 'The <dfn class="term">Request</dfn> is categorized as a ' . strtolower($options[MFCS_REQUEST_TYPE_MEETING]) . ' <dfn class="term">Request Type</dfn>. ';
    $codes[MFCS_REQUEST_TYPE_MEETING] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_MEETING] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_TYPE_PERFORMANCE, $options)) {
    $codes[MFCS_REQUEST_TYPE_PERFORMANCE] = '';
    $codes[MFCS_REQUEST_TYPE_PERFORMANCE] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_TYPE_PERFORMANCE] .= '<strong class="bold">Code ' . MFCS_REQUEST_TYPE_PERFORMANCE . ', ' . $options[MFCS_REQUEST_TYPE_PERFORMANCE] . '</strong>: ';
    $codes[MFCS_REQUEST_TYPE_PERFORMANCE] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_PERFORMANCE] .= 'The <dfn class="term">Request</dfn> is categorized as a ' . strtolower($options[MFCS_REQUEST_TYPE_PERFORMANCE]) . ' <dfn class="term">Request Type</dfn>. ';
    $codes[MFCS_REQUEST_TYPE_PERFORMANCE] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_PERFORMANCE] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_TYPE_LECTURE, $options)) {
    $codes[MFCS_REQUEST_TYPE_LECTURE] = '';
    $codes[MFCS_REQUEST_TYPE_LECTURE] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_TYPE_LECTURE] .= '<strong class="bold">Code ' . MFCS_REQUEST_TYPE_LECTURE . ', ' . $options[MFCS_REQUEST_TYPE_LECTURE] . '</strong>: ';
    $codes[MFCS_REQUEST_TYPE_LECTURE] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_LECTURE] .= 'The <dfn class="term">Request</dfn> is categorized as a ' . strtolower($options[MFCS_REQUEST_TYPE_LECTURE]) . ' <dfn class="term">Request Type</dfn>. ';
    $codes[MFCS_REQUEST_TYPE_LECTURE] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_LECTURE] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_TYPE_RECEPTION, $options)) {
    $codes[MFCS_REQUEST_TYPE_RECEPTION] = '';
    $codes[MFCS_REQUEST_TYPE_RECEPTION] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_TYPE_RECEPTION] .= '<strong class="bold">Code ' . MFCS_REQUEST_TYPE_RECEPTION . ', ' . $options[MFCS_REQUEST_TYPE_RECEPTION] . '</strong>: ';
    $codes[MFCS_REQUEST_TYPE_RECEPTION] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_RECEPTION] .= 'The <dfn class="term">Request</dfn> is categorized as a ' . strtolower($options[MFCS_REQUEST_TYPE_RECEPTION]) . ' <dfn class="term">Request Type</dfn>. ';
    $codes[MFCS_REQUEST_TYPE_RECEPTION] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_RECEPTION] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_TYPE_BANQUET, $options)) {
    $codes[MFCS_REQUEST_TYPE_BANQUET] = '';
    $codes[MFCS_REQUEST_TYPE_BANQUET] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_TYPE_BANQUET] .= '<strong class="bold">Code ' . MFCS_REQUEST_TYPE_BANQUET . ', ' . $options[MFCS_REQUEST_TYPE_BANQUET] . '</strong>: ';
    $codes[MFCS_REQUEST_TYPE_BANQUET] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_BANQUET] .= 'The <dfn class="term">Request</dfn> is categorized as a ' . strtolower($options[MFCS_REQUEST_TYPE_BANQUET]) . ' <dfn class="term">Request Type</dfn>. ';
    $codes[MFCS_REQUEST_TYPE_BANQUET] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_BANQUET] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_TYPE_WORKSHOP, $options)) {
    $codes[MFCS_REQUEST_TYPE_WORKSHOP] = '';
    $codes[MFCS_REQUEST_TYPE_WORKSHOP] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_TYPE_WORKSHOP] .= '<strong class="bold">Code ' . MFCS_REQUEST_TYPE_WORKSHOP . ', ' . $options[MFCS_REQUEST_TYPE_WORKSHOP] . '</strong>: ';
    $codes[MFCS_REQUEST_TYPE_WORKSHOP] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_WORKSHOP] .= 'The <dfn class="term">Request</dfn> is categorized as a ' . strtolower($options[MFCS_REQUEST_TYPE_WORKSHOP]) . ' <dfn class="term">Request Type</dfn>. ';
    $codes[MFCS_REQUEST_TYPE_WORKSHOP] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_WORKSHOP] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_TYPE_TALENT, $options)) {
    $codes[MFCS_REQUEST_TYPE_TALENT] = '';
    $codes[MFCS_REQUEST_TYPE_TALENT] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_TYPE_TALENT] .= '<strong class="bold">Code ' . MFCS_REQUEST_TYPE_TALENT . ', ' . $options[MFCS_REQUEST_TYPE_TALENT] . '</strong>: ';
    $codes[MFCS_REQUEST_TYPE_TALENT] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_TALENT] .= 'The <dfn class="term">Request</dfn> is categorized as a ' . strtolower($options[MFCS_REQUEST_TYPE_TALENT]) . ' <dfn class="term">Request Type</dfn>. ';
    $codes[MFCS_REQUEST_TYPE_TALENT] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_TALENT] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_TYPE_CEREMONY, $options)) {
    $codes[MFCS_REQUEST_TYPE_CEREMONY] = '';
    $codes[MFCS_REQUEST_TYPE_CEREMONY] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_TYPE_CEREMONY] .= '<strong class="bold">Code ' . MFCS_REQUEST_TYPE_CEREMONY . ', ' . $options[MFCS_REQUEST_TYPE_CEREMONY] . '</strong>: ';
    $codes[MFCS_REQUEST_TYPE_CEREMONY] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_CEREMONY] .= 'The <dfn class="term">Request</dfn> is categorized as a ' . strtolower($options[MFCS_REQUEST_TYPE_CEREMONY]) . ' <dfn class="term">Request Type</dfn>. ';
    $codes[MFCS_REQUEST_TYPE_CEREMONY] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_CEREMONY] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_TYPE_FAIR, $options)) {
    $codes[MFCS_REQUEST_TYPE_FAIR] = '';
    $codes[MFCS_REQUEST_TYPE_FAIR] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_TYPE_FAIR] .= '<strong class="bold">Code ' . MFCS_REQUEST_TYPE_FAIR . ', ' . $options[MFCS_REQUEST_TYPE_FAIR] . '</strong>: ';
    $codes[MFCS_REQUEST_TYPE_FAIR] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_FAIR] .= 'The <dfn class="term">Request</dfn> is categorized as a ' . strtolower($options[MFCS_REQUEST_TYPE_FAIR]) . ' <dfn class="term">Request Type</dfn>. ';
    $codes[MFCS_REQUEST_TYPE_FAIR] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_FAIR] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_TYPE_ATHLETIC, $options)) {
    $codes[MFCS_REQUEST_TYPE_ATHLETIC] = '';
    $codes[MFCS_REQUEST_TYPE_ATHLETIC] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_TYPE_ATHLETIC] .= '<strong class="bold">Code ' . MFCS_REQUEST_TYPE_ATHLETIC . ', ' . $options[MFCS_REQUEST_TYPE_ATHLETIC] . '</strong>: ';
    $codes[MFCS_REQUEST_TYPE_ATHLETIC] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_ATHLETIC] .= 'The <dfn class="term">Request</dfn> is categorized as an ' . strtolower($options[MFCS_REQUEST_TYPE_ATHLETIC]) . ' <dfn class="term">Request Type</dfn>. ';
    $codes[MFCS_REQUEST_TYPE_ATHLETIC] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_ATHLETIC] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_TYPE_OTHER, $options)) {
    $codes[MFCS_REQUEST_TYPE_OTHER] = '';
    $codes[MFCS_REQUEST_TYPE_OTHER] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_TYPE_OTHER] .= '<strong class="bold">Code ' . MFCS_REQUEST_TYPE_OTHER . ', ' . $options[MFCS_REQUEST_TYPE_OTHER] . '</strong>: ';
    $codes[MFCS_REQUEST_TYPE_OTHER] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_OTHER] .= 'The <dfn class="term">Request</dfn> is categorized as an ' . strtolower($options[MFCS_REQUEST_TYPE_OTHER]) . ' <dfn class="term">Request Type</dfn>. ';
    $codes[MFCS_REQUEST_TYPE_OTHER] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_OTHER] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_TYPE_QUICK_MEETING, $options)) {
    $codes[MFCS_REQUEST_TYPE_QUICK_MEETING] = '';
    $codes[MFCS_REQUEST_TYPE_QUICK_MEETING] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_TYPE_QUICK_MEETING] .= '<strong class="bold">Code ' . MFCS_REQUEST_TYPE_QUICK_MEETING . ', ' . $options[MFCS_REQUEST_TYPE_QUICK_MEETING] . '</strong>: ';
    $codes[MFCS_REQUEST_TYPE_QUICK_MEETING] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_QUICK_MEETING] .= 'The <dfn class="term">Request</dfn> is categorized as a ' . strtolower($options[MFCS_REQUEST_TYPE_QUICK_MEETING]) . ' <dfn class="term">Request Type</dfn>. ';
    $codes[MFCS_REQUEST_TYPE_QUICK_MEETING] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_QUICK_MEETING] .= '</li>';
  }

  if (array_key_exists(MFCS_REQUEST_TYPE_CLUB_MEETING, $options)) {
    $codes[MFCS_REQUEST_TYPE_CLUB_MEETING] = '';
    $codes[MFCS_REQUEST_TYPE_CLUB_MEETING] .= '<li class="list-item">';
    $codes[MFCS_REQUEST_TYPE_CLUB_MEETING] .= '<strong class="bold">Code ' . MFCS_REQUEST_TYPE_CLUB_MEETING . ', ' . $options[MFCS_REQUEST_TYPE_CLUB_MEETING] . '</strong>: ';
    $codes[MFCS_REQUEST_TYPE_CLUB_MEETING] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_CLUB_MEETING] .= 'The <dfn class="term">Request</dfn> is categorized as a ' . strtolower($options[MFCS_REQUEST_TYPE_CLUB_MEETING]) . ' <dfn class="term">Request Type</dfn>. ';
    $codes[MFCS_REQUEST_TYPE_CLUB_MEETING] .= '<br>';
    $codes[MFCS_REQUEST_TYPE_CLUB_MEETING] .= '</li>';
  }

  if (empty($codes)) {
    $markup .= '<br>';
    $markup .= 'The system currently has no codes available. ';
  }
  else {
    ksort($codes);

    $markup .= '<ul class="list">';
    $markup .= implode('', $codes);
    $markup .= '</ul>';
  }
  unset($codes);

  $markup .= '<br>';
  $markup .= '</div>';


  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: email - advanced - e-mail type codes.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_email_section_advanced_sub_section_email_type_codes($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_url;
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  mfcs_include(MFCS_INCLUDE_LIST_OPTIONS);
  $options =  mfcs_get_email_type_list_options(NULL, TRUE);

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-email-section-advanced-sub_section-email_type_codes', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';

  $codes = array();

  if (array_key_exists(MFCS_EMAIL_TYPE_AMENDED, $options)) {
    $codes[MFCS_EMAIL_TYPE_AMENDED] = '';
    $codes[MFCS_EMAIL_TYPE_AMENDED] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_AMENDED] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_AMENDED . ', ' . $options[MFCS_EMAIL_TYPE_AMENDED] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_AMENDED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_AMENDED] .= 'This e-mail represents an amendment to a specified <dfn class="term">Request</dfn>. ';
    $codes[MFCS_EMAIL_TYPE_AMENDED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_AMENDED] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_APPROVED, $options)) {
    $codes[MFCS_EMAIL_TYPE_APPROVED] = '';
    $codes[MFCS_EMAIL_TYPE_APPROVED] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_APPROVED] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_APPROVED . ', ' . $options[MFCS_EMAIL_TYPE_APPROVED] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_APPROVED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_APPROVED] .= 'This e-mail designates that the specified <dfn class="term">Request</dfn> has been <dfn class="term">Approved</dfn>. ';
    $codes[MFCS_EMAIL_TYPE_APPROVED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_APPROVED] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_DENIED, $options)) {
    $codes[MFCS_EMAIL_TYPE_DENIED] = '';
    $codes[MFCS_EMAIL_TYPE_DENIED] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_DENIED] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_DENIED . ', ' . $options[MFCS_EMAIL_TYPE_DENIED] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_DENIED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_DENIED] .= 'This e-mail designates that the specified <dfn class="term">Request</dfn> has been <dfn class="term">Denied</dfn>. ';
    $codes[MFCS_EMAIL_TYPE_DENIED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_DENIED] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_CANCELLED, $options)) {
    $codes[MFCS_EMAIL_TYPE_CANCELLED] = '';
    $codes[MFCS_EMAIL_TYPE_CANCELLED] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_CANCELLED] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_CANCELLED . ', ' . $options[MFCS_EMAIL_TYPE_CANCELLED] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_CANCELLED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_CANCELLED] .= 'This e-mail represents that a given <dfn class="term">Request</dfn> has been cancelled. ';
    $codes[MFCS_EMAIL_TYPE_CANCELLED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_CANCELLED] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_CANCELLING, $options)) {
    $codes[MFCS_EMAIL_TYPE_CANCELLING] = '';
    $codes[MFCS_EMAIL_TYPE_CANCELLING] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_CANCELLING] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_CANCELLING . ', ' . $options[MFCS_EMAIL_TYPE_CANCELLING] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_CANCELLING] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_CANCELLING] .= 'This e-mail represents that one or more dates and times for a given <dfn class="term">Request</dfn> have been cancelled. ';
    $codes[MFCS_EMAIL_TYPE_CANCELLING] .= 'The <dfn class="term">Request</dfn> itself has not been cancelled. ';
    $codes[MFCS_EMAIL_TYPE_CANCELLING] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_CANCELLING] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_COMMENT, $options)) {
    $codes[MFCS_EMAIL_TYPE_COMMENT] = '';
    $codes[MFCS_EMAIL_TYPE_COMMENT] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_COMMENT] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_COMMENT . ', ' . $options[MFCS_EMAIL_TYPE_COMMENT] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_COMMENT] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_COMMENT] .= 'A comment has been made on the specified <dfn class="term">Request</dfn>. ';
    $codes[MFCS_EMAIL_TYPE_COMMENT] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_COMMENT] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_DELETED, $options)) {
    $codes[MFCS_EMAIL_TYPE_DELETED] = '';
    $codes[MFCS_EMAIL_TYPE_DELETED] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_DELETED] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_DELETED . ', ' . $options[MFCS_EMAIL_TYPE_DELETED] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_DELETED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_DELETED] .= 'The specified <dfn class="term">Request</dfn> has been deleted. ';
    $codes[MFCS_EMAIL_TYPE_DELETED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_DELETED] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_FINAL_DECISION, $options)) {
    $codes[MFCS_EMAIL_TYPE_FINAL_DECISION] = '';
    $codes[MFCS_EMAIL_TYPE_FINAL_DECISION] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_FINAL_DECISION] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_FINAL_DECISION . ', ' . $options[MFCS_EMAIL_TYPE_FINAL_DECISION] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_FINAL_DECISION] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_FINAL_DECISION] .= 'The <dfn class="term">Request</dfn> has reached the <dfn class="term">Final Decision Step</dfn> of the <dfn class="term">Review</dfn> process. ';
    $codes[MFCS_EMAIL_TYPE_FINAL_DECISION] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_FINAL_DECISION] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_FORCED, $options)) {
    $codes[MFCS_EMAIL_TYPE_FORCED] = '';
    $codes[MFCS_EMAIL_TYPE_FORCED] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_FORCED] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_FORCED . ', ' . $options[MFCS_EMAIL_TYPE_FORCED] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_FORCED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_FORCED] .= 'The current <dfn class="term">Review Step</dfn> or <dfn class="term">Request Status</dfn> for a specified <dfn class="term">Request</dfn> has been administratively altered. ';
    $codes[MFCS_EMAIL_TYPE_FORCED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_FORCED] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_INSURANCE_PROVIDED, $options)) {
    $codes[MFCS_EMAIL_TYPE_INSURANCE_PROVIDED] = '';
    $codes[MFCS_EMAIL_TYPE_INSURANCE_PROVIDED] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_INSURANCE_PROVIDED] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_INSURANCE_PROVIDED . ', ' . $options[MFCS_EMAIL_TYPE_INSURANCE_PROVIDED] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_INSURANCE_PROVIDED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_INSURANCE_PROVIDED] .= 'Insurance for the specified <dfn class="term">Request</dfn> has been provided. ';
    $codes[MFCS_EMAIL_TYPE_INSURANCE_PROVIDED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_INSURANCE_PROVIDED] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_INSURANCE_REQUIRED, $options)) {
    $codes[MFCS_EMAIL_TYPE_INSURANCE_REQUIRED] = '';
    $codes[MFCS_EMAIL_TYPE_INSURANCE_REQUIRED] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_INSURANCE_REQUIRED] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_INSURANCE_REQUIRED . ', ' . $options[MFCS_EMAIL_TYPE_INSURANCE_REQUIRED] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_INSURANCE_REQUIRED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_INSURANCE_REQUIRED] .= 'These specified <dfn class="term">Request</dfn> requires insurance to be provided. ';
    $codes[MFCS_EMAIL_TYPE_INSURANCE_REQUIRED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_INSURANCE_REQUIRED] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_ISSUES, $options)) {
    $codes[MFCS_EMAIL_TYPE_ISSUES] = '';
    $codes[MFCS_EMAIL_TYPE_ISSUES] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_ISSUES] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_ISSUES . ', ' . $options[MFCS_EMAIL_TYPE_ISSUES] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_ISSUES] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_ISSUES] .= 'A <dfn class="term">Reviewer</dfn> or <dfn class="term">Venue Coordinator</dfn> has designated that there are issues with the <dfn class="term">Request</dfn>. ';
    $codes[MFCS_EMAIL_TYPE_ISSUES] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_ISSUES] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_ISSUES_NONE, $options)) {
    $codes[MFCS_EMAIL_TYPE_ISSUES_NONE] = '';
    $codes[MFCS_EMAIL_TYPE_ISSUES_NONE] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_ISSUES_NONE] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_ISSUES_NONE . ', ' . $options[MFCS_EMAIL_TYPE_ISSUES_NONE] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_ISSUES_NONE] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_ISSUES_NONE] .= 'A <dfn class="term">Reviewer</dfn> or <dfn class="term">Venue Coordinator</dfn> has designated that there are no issues with the <dfn class="term">Request</dfn>. ';
    $codes[MFCS_EMAIL_TYPE_ISSUES_NONE] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_ISSUES_NONE] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_MODIFIED, $options)) {
    $codes[MFCS_EMAIL_TYPE_MODIFIED] = '';
    $codes[MFCS_EMAIL_TYPE_MODIFIED] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_MODIFIED] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_MODIFIED . ', ' . $options[MFCS_EMAIL_TYPE_MODIFIED] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_MODIFIED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_MODIFIED] .= 'The <dfn class="term">Request</dfn> has been modified. ';
    $codes[MFCS_EMAIL_TYPE_MODIFIED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_MODIFIED] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_MOVED, $options)) {
    $codes[MFCS_EMAIL_TYPE_MOVED] = '';
    $codes[MFCS_EMAIL_TYPE_MOVED] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_MOVED] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_MOVED . ', ' . $options[MFCS_EMAIL_TYPE_MOVED] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_MOVED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_MOVED] .= 'The <dfn class="term">Request</dfn> has been moved to a new <dfn class="term">Room</dfn> or <dfn class="term">Building</dfn>. ';
    $codes[MFCS_EMAIL_TYPE_MOVED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_MOVED] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_NEEDS_REVIEW, $options)) {
    $codes[MFCS_EMAIL_TYPE_NEEDS_REVIEW] = '';
    $codes[MFCS_EMAIL_TYPE_NEEDS_REVIEW] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_NEEDS_REVIEW] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_NEEDS_REVIEW . ', ' . $options[MFCS_EMAIL_TYPE_NEEDS_REVIEW] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_NEEDS_REVIEW] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_NEEDS_REVIEW] .= 'The <dfn class="term">Request</dfn> is now ready to be <dfn class="term">Reviewed</dfn>. ';
    $codes[MFCS_EMAIL_TYPE_NEEDS_REVIEW] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_NEEDS_REVIEW] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_REASSIGN_COORDINATOR, $options)) {
    $codes[MFCS_EMAIL_TYPE_REASSIGN_COORDINATOR] = '';
    $codes[MFCS_EMAIL_TYPE_REASSIGN_COORDINATOR] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_REASSIGN_COORDINATOR] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_REASSIGN_COORDINATOR . ', ' . $options[MFCS_EMAIL_TYPE_REASSIGN_COORDINATOR] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_REASSIGN_COORDINATOR] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_REASSIGN_COORDINATOR] .= 'A new <dfn class="term">Venue Coordinator</dfn> has been assigned for the specified <dfn class="term">Request</dfn>. ';
    $codes[MFCS_EMAIL_TYPE_REASSIGN_COORDINATOR] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_REASSIGN_COORDINATOR] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_REASSIGN_REQUESTER, $options)) {
    $codes[MFCS_EMAIL_TYPE_REASSIGN_REQUESTER] = '';
    $codes[MFCS_EMAIL_TYPE_REASSIGN_REQUESTER] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_REASSIGN_REQUESTER] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_REASSIGN_REQUESTER . ', ' . $options[MFCS_EMAIL_TYPE_REASSIGN_REQUESTER] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_REASSIGN_REQUESTER] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_REASSIGN_REQUESTER] .= 'A new <dfn class="term">Requester</dfn> has been assigned to the specified <dfn class="term">Request</dfn>. ';
    $codes[MFCS_EMAIL_TYPE_REASSIGN_REQUESTER] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_REASSIGN_REQUESTER] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_UNCANCELLED, $options)) {
    $codes[MFCS_EMAIL_TYPE_UNCANCELLED] = '';
    $codes[MFCS_EMAIL_TYPE_UNCANCELLED] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_UNCANCELLED] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_UNCANCELLED . ', ' . $options[MFCS_EMAIL_TYPE_UNCANCELLED] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_UNCANCELLED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_UNCANCELLED] .= 'The specified <dfn class="term">Request</dfn> has been uncancelled. ';
    $codes[MFCS_EMAIL_TYPE_UNCANCELLED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_UNCANCELLED] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_UNREVIEWED_REQUEST, $options)) {
    $codes[MFCS_EMAIL_TYPE_UNREVIEWED_REQUEST] = '';
    $codes[MFCS_EMAIL_TYPE_UNREVIEWED_REQUEST] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_UNREVIEWED_REQUEST] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_UNREVIEWED_REQUEST . ', ' . $options[MFCS_EMAIL_TYPE_UNREVIEWED_REQUEST] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_UNREVIEWED_REQUEST] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_UNREVIEWED_REQUEST] .= 'This e-mail represents a reminder that a specified <dfn class="term">Request</dfn> has not yet been <dfn class="term">Reviewed</dfn>. ';
    $codes[MFCS_EMAIL_TYPE_UNREVIEWED_REQUEST] .= 'This is normally sent out a number of days before the <dfn class="term">Request\'s</dfn> first event date. ';
    $codes[MFCS_EMAIL_TYPE_UNREVIEWED_REQUEST] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_UNREVIEWED_REQUEST] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_REVIEWED, $options)) {
    $codes[MFCS_EMAIL_TYPE_REVIEWED] = '';
    $codes[MFCS_EMAIL_TYPE_REVIEWED] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_REVIEWED] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_REVIEWED . ', ' . $options[MFCS_EMAIL_TYPE_REVIEWED] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_REVIEWED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_REVIEWED] .= 'This e-mail designates that a <dfn class="term">Request</dfn> has been reviewed by all of the <dfn class="term">Reviewers</dfn> during the <dfn class="term">Review Step</dfn>. ';
    $codes[MFCS_EMAIL_TYPE_REVIEWED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_REVIEWED] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_UNAVAILABLE, $options)) {
    $codes[MFCS_EMAIL_TYPE_UNAVAILABLE] = '';
    $codes[MFCS_EMAIL_TYPE_UNAVAILABLE] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_UNAVAILABLE] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_UNAVAILABLE . ', ' . $options[MFCS_EMAIL_TYPE_UNAVAILABLE] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_UNAVAILABLE] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_UNAVAILABLE] .= 'This e-mail designates that the specified <dfn class="term">Request</dfn> has been <dfn class="term">Denied</dfn> because of some unavailability. ';
    $codes[MFCS_EMAIL_TYPE_UNAVAILABLE] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_UNAVAILABLE] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_UNAVAILABLE_LOCKED, $options)) {
    $codes[MFCS_EMAIL_TYPE_UNAVAILABLE_LOCKED] = '';
    $codes[MFCS_EMAIL_TYPE_UNAVAILABLE_LOCKED] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_UNAVAILABLE_LOCKED] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_UNAVAILABLE_LOCKED . ', ' . $options[MFCS_EMAIL_TYPE_UNAVAILABLE_LOCKED] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_UNAVAILABLE_LOCKED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_UNAVAILABLE_LOCKED] .= 'This e-mail designates that the specified <dfn class="term">Request</dfn> has been <dfn class="term">Denied</dfn> because another <dfn class="term">Request</dfn> currently has the <dfn class="term">Room</dfn> locked as one or more of the requested dates and times. ';
    $codes[MFCS_EMAIL_TYPE_UNAVAILABLE_LOCKED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_UNAVAILABLE_LOCKED] .= '</li>';
  }

  if (array_key_exists(MFCS_EMAIL_TYPE_REQUESTED, $options)) {
    $codes[MFCS_EMAIL_TYPE_REQUESTED] = '';
    $codes[MFCS_EMAIL_TYPE_REQUESTED] .= '<li class="list-item">';
    $codes[MFCS_EMAIL_TYPE_REQUESTED] .= '<strong class="bold">Code ' . MFCS_EMAIL_TYPE_REQUESTED . ', ' . $options[MFCS_EMAIL_TYPE_REQUESTED] . '</strong>: ';
    $codes[MFCS_EMAIL_TYPE_REQUESTED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_REQUESTED] .= 'This e-mail designates that a new <dfn class="term">Request</dfn> has been made. ';
    $codes[MFCS_EMAIL_TYPE_REQUESTED] .= '<br>';
    $codes[MFCS_EMAIL_TYPE_REQUESTED] .= '</li>';
  }

  if (empty($codes)) {
    $markup .= '<br>';
    $markup .= 'The system currently has no codes available. ';
  }
  else {
    ksort($codes);

    $markup .= '<ul class="list">';
    $markup .= implode('', $codes);
    $markup .= '</ul>';
  }
  unset($codes);

  $markup .= '<br>';
  $markup .= '</div>';


  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: calendar.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_calendar($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-calendar', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div class="paragraph">';
  $markup .= 'The <dfn class="term"><abbr class="abbreviation" title="Facilities Use System">FCS</abbr></dfn> provides a calendar containing only <dfn class="term">Accepted</dfn> <dfn class="term">Requests</dfn> and holidays. ';
  $markup .= 'No other <dfn class="term">Requests</dfn> or <dfn class="term">Room</dfn> unavailabilities are displayed. ';
  $markup .= 'The calendars are designed to be printer-friendly with a minimum amount of inconsistencies. ';
  $markup .= 'There are two main types of calendars on this system: monthly calendars and daily calendars. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'Monthly calendars present only the <dfn class="term">Request</dfn> name for the sake of space. ';
  $markup .= 'All other information must be view on the <dfn class="term">Request</dfn> itself. ';
  $markup .= 'Holidays are also presented in name only but are not presented as links. ';
  $markup .= 'Holidays can be viewed in detail using the holidays link usually found on the top of the screen in the navigation panel. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'Daily calendars present the most common information associated with a given <dfn class="term">Request</dfn>, such as the name, <dfn class="term">Building</dfn>, <dfn class="term">Room</dfn>, time, <dfn class="term">Request ID</dfn>, and the <dfn class="term">Request Type</dfn>. ';
  $markup .= 'For the sake of space when view and when printing, only four <dfn class="term">Requests</dfn> are presented per hour slot per day. ';
  $markup .= 'There is a <em class="emphasis">next</em> link on each hour slot for when there are more than four <dfn class="term">Requests</dfn> for that given slot. ';
  $markup .= 'Holidays are also presented by the holiday name only but are not presented as links. ';
  $markup .= 'Holidays can be viewed in detail using the holidays link usually found on the top of the screen in the navigation panel. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'At the bottom of each calendar page, there should be a collection of links for filtering the calendar. ';
  $markup .= 'This type of filter alters the <dfn class="term"><abbr class="abbreviation" title="Universal Resource Locator">URL</abbr></dfn> with the options so that you can customize the filter and then bookmark that filter. ';
  $markup .= 'The filter settings are preserved so long as the information is still present in the <dfn class="term"><abbr class="abbreviation" title="Universal Resource Locator">URL</abbr></dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'Many browsers hide or obscure the additional information in the <dfn class="term"><abbr class="abbreviation" title="Universal Resource Locator">URL</abbr></dfn> making it difficult to view and use. ';
  $markup .= 'If you think the calendar is not showing enough information or showing too much, be sure to scroll to the bottom of the page to determine if any filter is currently in use. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'In the same area as the filters are the <dfn class="term">iCalendar</dfn> download options. ';
  $markup .= 'These options provide an easy way to export the <dfn class="term">Requests</dfn> for the given calendar (including any active filters) and then import the <dfn class="term">Requests</dfn> into your own personal calendar. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'When exporting <dfn class="term">Requests</dfn> (and subsequently importing into your own calendar), the exported <dfn class="term">Requests</dfn> are a copy. ';
  $markup .= 'Any changes made to the <dfn class="term">Requests</dfn> on the <dfn class="term">Facilities Use System</dfn> cannot be reflected on your own calendar. ';
  $markup .= 'The <dfn class="term">Requests</dfn> in your calendar could <em class="emphasis">potentially</em> be out of date. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/faq' . $url_arguments . '" class="item-link">Documentation: Frequently Asked Questions</a>. ';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: faq.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_faq($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-faq', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<div id="faq-1" class="paragraph faq-item">';
  $markup .= '<div class="faq-item-question question"> ';
  $markup .= '<strong class="bold faq-item-number">1.</strong> Why doesn\'t anybody receive my e-mail replies?';
  $markup .= '</div>';
  $markup .= '<div class="faq-item-answer answer"> ';
  $markup .= 'E-mails sent by this sent should not be replied to. There is no one to answer or see them. Instead, replies should be enterred in as <dfn class="term">Comments</dfn> on the <dfn class="term">Request</dfn> form in the <dfn class="term">Facilities Use System</dfn>. ';
  $markup .= '</div>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/terminology' . $url_arguments . '" class="item-link">Documentation: Terminology</a>. ';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: terminology.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_terminology($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-terminology', $section_information['header_attributes'], $section_information['headers'], $start_count);

  if (!$as_inline) {
    $section_information_child = $section_information;
    $section_information_child['id'] = 'help-documentation-topic-terminology-section-facilities_use';
    $section_information_child['class'] = array('section-help', 'section-help-break', 'section-help-terminology-facilities_use');
    $section_information_child['headers'] = array(
      0 => array('markup' => 'Facilities Use Terminology', 'attributes' => array('class' => array('header-3', 'section'))),
    );
    $section_information_child['navigation'] = NULL;
    $section_information_child['start_count'] = 2;

    $markup .= mfcs_help_topic_terminology_section_facilities_use($allowed_topics, $section_information_child, TRUE);

    $section_information_child['id'] = 'help-documentation-topic-terminology-section-system';
    $section_information_child['class'] = array('section-help', 'section-help-break', 'section-help-terminology-system');
    $section_information_child['headers'][0]['markup'] = 'System Terminology';

    $markup .= mfcs_help_topic_terminology_section_system($allowed_topics, $section_information_child, TRUE);

    $section_information_child['id'] = 'help-documentation-topic-terminology-section-documentation';
    $section_information_child['class'] = array('section-help', 'section-help-break', 'section-help-terminology-documentation');
    $section_information_child['headers'][0]['markup'] = 'Documentation Terminology';

    $markup .= mfcs_help_topic_terminology_section_documentation($allowed_topics, $section_information_child, TRUE);
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: terminology - facilities use.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_terminology_section_facilities_use($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-terminology-section-facilities_use', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<dl class="definition_list">';

  $markup .= '<dt id="terminology-facilities_use-approved" class="definition_list-title">Accepted</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A Request State in which the Request has been Approved by the Reviewers and is now reserved for the requested dates, times, and place. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-request_state" class="definition_list-see_other no_print_links_as_link">Request State</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-approved_cancelled" class="definition_list-title">Accepted (Cancelled)</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A Request State in which the Request is removed from the Review process or whose reservation is otherwise made unavailable after the Request has already been Approved. ';
  $markup .= 'All other Requests that have not been Approved should be assigned the Request State of Cancelled. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-request_state" class="definition_list-see_other no_print_links_as_link">Request State</a>.</span> ';
  $markup .= '<br>';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-cancelled" class="definition_list-see_other no_print_links_as_link">Cancelled</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-approved" class="definition_list-title">Approve</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A decision made by the Reviewer during the Final Decision Step to designate that the Request has been Accepted. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-review" class="definition_list-see_other no_print_links_as_link">Review</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-available" class="definition_list-title">Available</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A decision made during the Venue Available Step by the Venue Coordinator designating that the venue is available. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-review_step" class="definition_list-see_other no_print_links_as_link">Review Step</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-cancelled" class="definition_list-title">Cancelled</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A Request State in which the Request is removed from the Review process or whose reservation is otherwise made unavailable. ';
  $markup .= 'This does not include Requests that have already been Approved and then Cancelled. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-request_state" class="definition_list-see_other no_print_links_as_link">Request State</a>.</span> ';
  $markup .= '<br>';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#approved_cancelled" class="definition_list-see_other no_print_links_as_link">Approved (Cancelled)</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-classification" class="definition_list-title">Classification</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-request_classification" class="definition_list-see_other no_print_links_as_link">Request Classification</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-closed" class="definition_list-title">Closed</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'Any Request State in which the Request is no longer under Review. ';
  $markup .= 'This includes States such as: Accepted, Accepted (Cancelled), Cancelled, Room Unavailable, Room Unavailable (Due to Lock), and Denied. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-request_state" class="definition_list-see_other no_print_links_as_link">Request State</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-completed" class="definition_list-title">Completed</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A Review Step in which a Request is has been completed (as accepted or denied) or closed (as room unavailable, cancelled, etc..). ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-review_step" class="definition_list-see_other no_print_links_as_link">Review Step</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-coordinator" class="definition_list-title">Coordinator</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see">See: <a href="' . $url_arguments . '#terminology-facilities_use-venue_coordinator" class="definition_list-see_other no_print_links_as_link">Venue Coordinator</a>.</span> ';
  $markup .= '<br>';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-room_coordinator" class="definition_list-see_other no_print_links_as_link">Room Coordinator</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-approved" class="definition_list-title">Deny</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A decision made by the Reviewer during the Final Decision Step to designate that the Request has been Denied. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-review" class="definition_list-see_other no_print_links_as_link">Review</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-denied" class="definition_list-title">Denied</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A Request State in which the Request is refused. ';
  $markup .= '<br>';
  $markup .= '<br>';
  $markup .= 'Alternatively, this could be used to reference the Deny decision made by a Reviewer during the Final Decision Review Step. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-request_state" class="definition_list-see_other no_print_links_as_link">Request State</a>.</span> ';
  $markup .= '<br>';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-review_step" class="definition_list-see_other no_print_links_as_link">Review Step</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-external" class="definition_list-title">External</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= '<dfn class="term">External</dfn> refers to a classification assigned to Requesters or as a Request Classification. ';
  $markup .= 'This classification represents that the user or request is associated with a non-McNeese affiliated or otherwise 3rd-party. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-request_classification" class="definition_list-see_other no_print_links_as_link">Request Classification</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-fcs" class="definition_list-title">FCS</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see">See: <a href="' . $url_arguments . '#terminology-facilities_use-facilities_use_system" class="definition_list-see_other no_print_links_as_link">Facilities Use System</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-final_approval" class="definition_list-title">Final Approval</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A Review Step in which a Request is waiting to have the final decision made by the Reviewers to be Approved or Denied. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-review_step" class="definition_list-see_other no_print_links_as_link">Review Step</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-financer" class="definition_list-title">Financer</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A <dfn class="term">Financer</dfn> is a Reviewer who is allowed to add, modify, or delete financial related information during the Review process for a given Request. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-reviewer" class="definition_list-see_other no_print_links_as_link">Reviewer</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-facilities_use_system" class="definition_list-title">Facilities Use System</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'The <dfn class="term">Facilities Use System</dfn>, or <abbr title="Facilities Use System" class="abbreviation">FCS</abbr>, is the digital system or service used to provide <em class="emphasis">McNeese State University</em> with a facilities reservation process and tools. ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-insurer" class="definition_list-title">Insurer</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'An <dfn class="term">Insurer</dfn> is a Reviewer who is allowed to add, modify, or delete insurance related information during the Review process for a given Request. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-reviewer" class="definition_list-see_other no_print_links_as_link">Reviewer</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-issues" class="definition_list-title">Issues</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A decision made by the Reviewer during the Review Step or the Final Decision Step to designate that the Reviewer has issues with the Request. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-review" class="definition_list-see_other no_print_links_as_link">Review</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-location" class="definition_list-title">Location</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A physical or virtual place in which events may be held. ';
  $markup .= 'This is not restricted to buildings and can be something such as a baseball field. ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-manager" class="definition_list-title">Manager</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A <dfn class="term">Manager</dfn> is responsible most administrative tasks on the system. ';
  $markup .= 'This user can perform Venue Coordinator duties, Review Requests, and make modifications not normally allowed to Requests. ';
  $markup .= '<dfn class="term">Managers</dfn> should be considered power users for the <abbr title="Facilities Use System" class="abbreviation">FCS</abbr>. ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-no_issues" class="definition_list-title">No Issues</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A decision made by the Reviewer during the Review Step or the Final Decision Step to designate that the Reviewer does not have any issues with the Request. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-review" class="definition_list-see_other no_print_links_as_link">Review</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-request" class="definition_list-title">Request</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A web form entry for reserving a particular place at some specified time. ';
  $markup .= 'This is a central part of the <abbr title="Facilities Use System" class="abbreviation">FCS</abbr> such that most operations, functionality, features, and searches are centered around requests. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $base_path . 'help-0/requests' . $url_arguments . '" class="definition_list-see_other no_print_links_as_link">Documentation: Requests</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-request_id" class="definition_list-title">Request ID</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A unique ID assigned to every Request found on this system. ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-requester" class="definition_list-title">Requester</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-request_coordinator" class="definition_list-see_other no_print_links_as_link">Request Coordinator</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-request_classification" class="definition_list-title">Request Classification</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A category, or classification, that a Request belongs to or a Requester is associated with. ';
  $markup .= 'This directly determines how Requests are created, reviewed, and presented. ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-request_coordinator" class="definition_list-title">Request Coordinator</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'Sometimes called Requester for short, the <dfn class="term">Request Coordinator</dfn> is the user who is filling out the request form or is the point of contact for the request. ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-request_state" class="definition_list-title">Request State</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A status assigned to a Request. ';
  $markup .= 'There are 9 types of statuses: Accepted, Accepted (Cancelled), Cancelled, Denied, Queued (Locked), Queued (Unlocked), Room Unavailable, Room Unavailable (Due to Lock), and Deleted. ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-review_step" class="definition_list-title">Review Step</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A step within the Review process in which a Request currently belongs to. ';
  $markup .= 'There are 4 types of steps: Venue Available, Review, Final Approval, and Completed. ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-request_type" class="definition_list-title">Request Type</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A <dfn class="term">Request Type</dfn> defines a purpose or intent for the requested reservation. ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-requirement" class="definition_list-title">Requirement</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A decision made by the Reviewer, generally during the Review Step or Final Decision Step, to designate that the Request has been additional restrictions. ';
  $markup .= 'Such restrictions could be paying facilities use fees or providing insurance. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-review" class="definition_list-see_other no_print_links_as_link">Review</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-review" class="definition_list-title">Review</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A Review Step in which a Request is being reviewed by Reviewers, Financers, and Insurers. ';
  $markup .= '<br>';
  $markup .= '<br>';
  $markup .= 'Alteratively, this is referred to as the process in which Requests undergo to have their reservation approved or denied. ';
  $markup .= '<br>';
  $markup .= 'This entails multiple users (Reviewers, Venue Coordinators, Managers, etc..) specifying whether or not there are issues, assign any financial or insurance related requirements, and checking the availability of the requested room. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-review_step" class="definition_list-see_other no_print_links_as_link">Review Step</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-reviewer" class="definition_list-title">Reviewer</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'These users are allowed to designate whether or not a Request has issues or does not have issues. ';
  $markup .= 'They operate on Requests during the Review process. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-financer" class="definition_list-see_other no_print_links_as_link">Financer</a>.</span> ';
  $markup .= '<br>';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-insurer" class="definition_list-see_other no_print_links_as_link">Insurerer</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-reviewer_classification" class="definition_list-title">Reviewer Classification</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A category or classification assigned to a Reviewer for the purposes of Reviewing. ';
  $markup .= 'Reviewers do not Review as themselves but instead Review as this classification. ';
  $markup .= '<br>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-room" class="definition_list-title">Room</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'This generally represents a specific room within some building. ';
  $markup .= 'However, because some Buildings do not always represent an actual building (such as a parking lot), a <dfn class="term">Room</dfn> could instead represent a sub-section or smaller area within the designated Building (such as a zone within a parking lot). ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-room_coordinator" class="definition_list-title">Room Coordinator</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'Venue Coordinators who may only coordinate specific rooms within a Building are called <dfn class="term">Room Coordinators</dfn>. ';
  $markup .= '<dfn class="term">Room Coordinators</dfn> are otherwise functionally identical to Venue Coordinators. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see">See: <a href="' . $url_arguments . '#terminology-facilities_use-venue_coordinator" class="definition_list-see_other no_print_links_as_link">Venue Coordinator</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-room_unavailable" class="definition_list-title">Room Unavailable</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A Request State in which the Request has been designated as Unavailable. ';
  $markup .= 'This Request has been effectively Denied. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-request_state" class="definition_list-see_other no_print_links_as_link">Request State</a>.</span> ';
  $markup .= '<br>';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-room_unavailable_due_to_lock" class="definition_list-see_other no_print_links_as_link">Room Unavailable (Due to Lock)</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-room_unavailable_due_to_lock" class="definition_list-title">Room Unavailable (Due to Lock)</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A Request State in which the Request has been designated as Unavailable, but only because another Request is currently undergoing the Review process. ';
  $markup .= 'Only one Request may be in the Review process at any time and the system generally automatically places conflicting Requests within this State. ';
  $markup .= 'You may want to try again later in case the conflicting Request has been Denied. ';
  $markup .= 'This Request has been effectively Denied. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-request_state" class="definition_list-see_other no_print_links_as_link">Request State</a>.</span> ';
  $markup .= '<br>';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-room_unavailable" class="definition_list-see_other no_print_links_as_link">Room Unavailable</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-state" class="definition_list-title">State</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see">See: <a href="' . $url_arguments . '#terminology-facilities_use-request_state" class="definition_list-see_other no_print_links_as_link">Request State</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-step" class="definition_list-title">Step</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see">See: <a href="' . $url_arguments . '#terminology-facilities_use-review_step" class="definition_list-see_other no_print_links_as_link">Review Step</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-student_organization" class="definition_list-title">Student Organization</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= '<dfn class="term">Student Organization</dfn> refers to a classification assigned to Requesters or Requests. ';
  $markup .= 'This classification represents that the user or request is associated with a <em class="emphasis">McNeese State University</em> student organization. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-request_classification" class="definition_list-see_other no_print_links_as_link">Request Classification</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-queued_locked" class="definition_list-title">Queued (Locked)</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A Request State in which the Request has been places in the Review process and the specified dates and times has been locked for the requested Room. ';
  $markup .= 'Any other Requests for the given Room at the specified dates and times will be automatically Closed as Room Unavailable (Due to Lock). ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-request_state" class="definition_list-see_other no_print_links_as_link">Request State</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-queued_locked" class="definition_list-title">Queued (Unlocked)</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A Request State in which the Request has been places in the Review process but the specified dates and times for the given Room have yet to be locked. ';
  $markup .= 'Any other Requests for the given Room at the specified dates and times will be also be placed in the Queued (Unlocked) State. ';
  $markup .= 'The first Request to be Approved should cause all other Queued Requests for the specified dates and times for the given Room to be closed as Room Unavailable. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-request_state" class="definition_list-see_other no_print_links_as_link">Request State</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-unavailable" class="definition_list-title">Unavailable</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A decision made during the Venue Available Step by the Venue Coordinator designating that the venue is unavailable. ';
  $markup .= 'The Request is subsequentally Closed as Room Unavailable. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-review_step" class="definition_list-see_other no_print_links_as_link">Review Step</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-venue_available" class="definition_list-title">Venue Available</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A Review Step in which a Request is waiting to be designated as available or unavailable by the Venue Coordinator. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-review_step" class="definition_list-see_other no_print_links_as_link">Review Step</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-venue_coordinator" class="definition_list-title">Venue Coordinator</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'This represents the person responsible for managing a specific building or room. ';
  $markup .= 'In many cases, this user will fill out requests in lieu of of the Requester. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="' . $url_arguments . '#terminology-facilities_use-room_coordinator" class="definition_list-see_other no_print_links_as_link">Room Coordinator</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '</dl>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/terminology/system' . $url_arguments . '" class="item-link">Documentation: Terminology - System</a>. ';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: terminology - system.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_terminology_section_system($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-terminology-section-system', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<dl class="definition_list">';

  $markup .= '<dt id="terminology-system-banner" class="definition_list-title">Banner</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'McNeese State University provides a service called Banner Self Service, or <dfn class="term">Banner</dfn> for short, for many of its operations. ';
  $markup .= 'User and class related information provided by <dfn class="term">Banner</dfn> are directly used by this system. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="//www.mcneese.edu/schedule/banner" class="definition_list-see_other" target="_blank">https://www.mcneese.edu/schedule/banner</a>. ';
  $markup .= '</span>';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-system-banner_id" class="definition_list-title">Banner ID</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A unique ID assigned to each person involved with McNeese State University, generally as a student, faculty, or staff. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="//www.mcneese.edu/schedule/banner" class="definition_list-see_other" target="_blank">https://www.mcneese.edu/schedule/banner</a>. ';
  $markup .= '</span>';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-system-fcs" class="definition_list-title">HTML5</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= '<dfn class="term">HTML5</dfn> is an internet standard that can very loosely be described as the <em class="emphasis">"web language"</em> or the <em class="emphasis">"internet"</em>. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="//en.wikipedia.org/wiki/HTML5" class="definition_list-see_other" target="_blank">https://en.wikipedia.org/wiki/HTML5</a>. ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-system-fcs" class="definition_list-title">iCalendar</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= '<dfn class="term">iCalendar</dfn> is an internet standard used to electronically communicate event information. ';
  $markup .= 'Files generally have the excetions <em>.ics</em>. ';
  $markup .= 'Files in this format (With a file extension of <em>.ics</em>) may be used to import an event or a set of events into your own personal calendar or even transfer the event details to a third party. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="//en.wikipedia.org/wiki/ICalendar" class="definition_list-see_other" target="_blank"><span class="no_print_links_as_link"></span>https://en.wikipedia.org/wiki/ICalendar</a>. ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-system-fcs" class="definition_list-title">OpenPGP</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= '<dfn class="term">OpenPGP</dfn> stands for <strong class="bold">Open Pretty Good Privacy</strong>. ';
  $markup .= 'This is a standard used as a digital signature and to encrypt files and e-mails. ';
  $markup .= 'E-mails sent by this system are digitally signed using this standard to help ensure that the e-mails are not spam or otherwise falsified. ';
  $markup .= '<br>';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see_also">See Also: <a href="//en.wikipedia.org/wiki/Pretty_Good_Privacy" class="definition_list-see_other" target="_blank">https://en.wikipedia.org/wiki/Pretty_Good_Privacy</a>. ';
  $markup .= '<br>';
  $markup .= '<span class="see_also">See Also: <a href="//en.wikipedia.org/wiki/Pretty_Good_Privacy#OpenPGP" class="definition_list-see_other" target="_blank">https://en.wikipedia.org/wiki/Pretty_Good_Privacy#OpenPGP</a>. ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-system-proxy" class="definition_list-title">Proxy</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= '<span class="see_section">';
  $markup .= '<span class="see">See: <a href="' . $url_arguments . '#terminology-system-venue_coordinator_proxy" class="definition_list-see_other no_print_links_as_link">Venue Coordinator Proxy</a>.</span> ';
  $markup .= '</span>';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-system-request_history" class="definition_list-title">Request History</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'The <dfn class="term">Request History</dfn> contains a record of all changes performed on a Request. ';
  $markup .= 'This log is restricted to the Venue Coordinator and Managers. ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-system-review_log" class="definition_list-title">Review Log</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'The <dfn class="term">Review Log</dfn> contains a record of all Review decisions made for a given Request. ';
  $markup .= 'This log is restricted to the Requester, Venue Coordinator, Reviewers, and Managers. ';
  $markup .= 'Some log entries may have further restrictions. ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-system-venue_coordinator_proxy" class="definition_list-title">Venue Coordinator Proxy</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A <dfn class="term">Venue Coordinator Proxy</dfn> is some user who may perform tasks as if they were a specified Venue Coordinator and is granted all appropriate priveleges necessary to perform such duties. ';
  $markup .= 'Such granted priveleges are restricted only to Venue Coordinator tasks. ';
  $markup .= '</span>';
  $markup .= '</dd>';

  $markup .= '</dl>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/terminology/documentation' . $url_arguments . '" class="item-link">Documentation: Terminology - Documentation</a>. ';
    $markup .= '</div>';
  }

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: terminology - documentation.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param array $section_information
 *   Information needed for populating and customizing the generated section tag.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $closing_tag
 *   (optional) When TRUE, the closing section tag is appended.
 *   When FALSE, no closing section tag is appended and the caller is expected to properly close.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_terminology_section_documentation($allowed_topics, $section_information, $as_inline = FALSE, $closing_tag = TRUE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_array($section_information)) {
    cf_error::invalid_array('section_information');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    $as_inline = FALSE;
  }

  if (!is_bool($closing_tag)) {
    cf_error::invalid_bool('closing_tag');
    $closing_tag = TRUE;
  }

  if ($as_inline) {
    $section_information['navigation'] = NULL;
  }

  $start_count = 0;
  if (isset($section_information['start_count']) && is_numeric($section_information['start_count']) && $section_information['start_count'] >= 0) {
    $start_count = (int) $section_information['start_count'];
  }

  $markup = '<section id="' . $section_information['id'] . '" class="' . implode(' ', $section_information['class']) . '">';
  $markup .= $section_information['navigation'];
  $markup .= mfcs_help_render_headers('topic-terminology-section-documentation', $section_information['header_attributes'], $section_information['headers'], $start_count);

  $markup .= '<dl class="definition_list">';

  $markup .= '<dt id="terminology-documentation-documentation" class="definition_list-title">Documentation</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'The <dfn class="term">Documentation</dfn> (within the context of this facilities use documentation) refers to a descriptive explanation of the system and its functions. ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-documentation-user_guides" class="definition_list-title">User Guides</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A <dfn class="term">User Guide</dfn> refers to a step-by-step explanation on how to perform a particular task within the system (often times including screenshots). ';
  $markup .= '</dd>';

  $markup .= '</dl>';

  if ($closing_tag) {
    $markup .= '</section>';
  }

  return $markup;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
