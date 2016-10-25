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

  // build the navigation markup.
  $navigation_urls = mfcs_help_determine_next_previous_top($allowed_topics, $topic, $section, $sub_section);
  $navigation_markup = '<nav id="mfcs-help-navigation_links" class="help-navigation_links no-print">';

  if (!is_null($navigation_urls['top'])) {
    if (!empty($navigation_urls['previous'])) {
      $navigation_markup .= '<a href="' . $navigation_urls['previous'] . '" title="' . $navigation_urls['tooltip-previous'] . '" class="help-navigation_link link-previous">Previous</a>';
    }

    $navigation_markup .= '<a href="' . $navigation_urls['top'] . '" title="' . $navigation_urls['tooltip-top'] . '" class="help-navigation_link link-top">Top</a>';

    if (!empty($navigation_urls['next'])) {
      $navigation_markup .= '<a href="' . $navigation_urls['next'] . '" title="' . $navigation_urls['tooltip-next'] . '" class="help-navigation_link link-next">Next</a>';
    }
  }

  if (file_exists(DRUPAL_ROOT . '/sites/all/modules/mcneese/mfcs/images/pdf/32.png')) {
    $module_path = drupal_get_path('module', 'mfcs');

    $image_file_path = $base_path . $module_path . '/images/pdf/32.png';

    $navigation_markup .= '<a href="' . $base_path . 'help-0/presentation/pdf/' . $pdf_url . $url_arguments . '" title="Download as PDF" class="help-navigation_link link-pdf"><img src="' . $image_file_path . '" alt="PDF" width="32" height="32" class="help-navigation_image image-pdf"></a>';

    unset($image_file_path);
    unset($module_path);
  }

  $navigation_markup .= '</nav>';

  // prepare the header information.
  $section_id = 'mfcs-section-help-main';
  $section_class = mfcs_help_base_class($topic, $section, $sub_section);

  $headers = array(
    0 => array('markup' => 'Facilities Use Documentation', 'attributes' => array('class' => array('header-1', 'title'))),
  );

  $header_attributes = array();

  if (is_null($topic)) {
    $headers[1] = array('markup' => 'Glossary', 'attributes' => array('class' => array('header-2', 'topic')));
    $section_class[] = 'section-help-break';

    if (is_null($section)) {
      $section_class[] = 'section-help-topic';

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-glossary', $header_attributes, $headers);

      $markup .= mfcs_help_topic_glossary($allowed_topics);
    }
    else {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
  }
  elseif ($topic == 'introduction') {
    $headers[1] = array('markup' => 'Introduction', 'attributes' => array('class' => array('header-2', 'topic')));
    $section_class[] = 'section-help-break';

    if (is_null($section)) {
      $section_class[] = 'section-help-topic-introduction';

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-introduction', $header_attributes, $headers);
      $markup .= mfcs_help_topic_introduction($allowed_topics);
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
  elseif ($topic == 'requests') {
    $headers[1] = array('markup' => 'Requests', 'attributes' => array('class' => array('header-2', 'topic')));
    $section_class[] = 'section-help-break';

    if (is_null($section)) {
      $section_class[] = 'section-help-topic-requests';

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-requests', $header_attributes, $headers);
      $markup .= mfcs_help_topic_requests($allowed_topics);
    }
    elseif (!is_null($sub_section)) {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
    elseif ($section == 'employee') {
      $section_class[] = 'section-help-topic-requests-employee';
      $headers[2] = array('markup' => 'Faculty, Staff, and Athletics Requests', 'attributes' => array('class' => array('header-3', 'section')));

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-requests-section-employee', $header_attributes, $headers);
      $markup .= mfcs_help_topic_requests_section_employee($allowed_topics);
    }
    elseif ($section == 'student') {
      $section_class[] = 'section-help-topic-requests-student';
      $headers[2] = array('markup' => 'Student Organization Requests', 'attributes' => array('class' => array('header-3', 'section')));

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-requests-section-student', $header_attributes, $headers);
      $markup .= mfcs_help_topic_requests_section_student_organization($allowed_topics);
    }
    elseif ($section == 'camps') {
      $section_class[] = 'section-help-topic-requests-camps';
      $headers[2] = array('markup' => 'Camps Requests', 'attributes' => array('class' => array('header-3', 'section')));

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-requests-section-camps', $header_attributes, $headers);
      $markup .= mfcs_help_topic_requests_section_camps($allowed_topics);
    }
    elseif ($section == 'external') {
      $section_class[] = 'section-help-topic-requests-external';
      $headers[2] = array('markup' => 'External Requests', 'attributes' => array('class' => array('header-3', 'section')));

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-requests-section-external', $header_attributes, $headers);
      $markup .= mfcs_help_topic_requests_section_external($allowed_topics);
    }
    else {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
  }
  elseif ($topic == 'review' && in_array('review', $allowed_topics)) {
    $headers[1] = array('markup' => 'Review', 'attributes' => array('class' => array('header-2', 'topic')));
    $section_class[] = 'section-help-break';

    if (is_null($section)) {
      $section_class[] = 'section-help-topic-review';

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-review', $header_attributes, $headers);
      $markup .= mfcs_help_topic_review($allowed_topics);
    }
    elseif (!is_null($sub_section)) {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
    elseif ($section == 'venue_available') {
      $section_class[] = 'section-help-topic-review-venue_available';
      $headers[2] = array('markup' => 'Venue Available Step', 'attributes' => array('class' => array('header-3', 'section')));

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-review-section-venue_available', $header_attributes, $headers);
      $markup .= mfcs_help_topic_review_section_venue_available($allowed_topics);
    }
    elseif ($section == 'review') {
      $section_class[] = 'section-help-topic-review-review';
      $headers[2] = array('markup' => 'Review Step', 'attributes' => array('class' => array('header-3', 'section')));

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-review-section-review', $header_attributes, $headers);
      $markup .= mfcs_help_topic_review_section_review($allowed_topics);
    }
    elseif ($section == 'final_decision') {
      $section_class[] = 'section-help-topic-review-final_decision';
      $headers[2] = array('markup' => 'Final Decision Step', 'attributes' => array('class' => array('header-3', 'section')));

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-review-section-final_decision', $header_attributes, $headers);
      $markup .= mfcs_help_topic_review_section_final_decision($allowed_topics);
    }
    elseif ($section == 'completed') {
      $section_class[] = 'section-help-topic-review-completed';
      $headers[2] = array('markup' => 'Completed Step', 'attributes' => array('class' => array('header-3', 'section')));

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-review-section-completed', $header_attributes, $headers);
      $markup .= mfcs_help_topic_review_section_completed($allowed_topics);
    }
    else {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
  }
  elseif ($topic == 'manage' && in_array('manage', $allowed_topics)) {
    $headers[1] = array('markup' => 'Manage', 'attributes' => array('class' => array('header-2', 'topic')));
    $section_class[] = 'section-help-break';

    if (is_null($section)) {
      $section_class[] = 'section-help-topic-manage';

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-manage', $header_attributes, $headers);
      $markup .= mfcs_help_topic_manage($allowed_topics);
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
  elseif ($topic == 'faq') {
    $headers[1] = array('markup' => 'Frequently Asked Questions', 'attributes' => array('class' => array('header-2', 'topic')));

    if (is_null($section)) {
      $section_class[] = 'section-help-topic-faq';

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-faq', $header_attributes, $headers);
      $markup .= mfcs_help_topic_faq($allowed_topics);
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
  elseif ($topic == 'terminology') {
    $headers[1] = array('markup' => 'Terminology', 'attributes' => array('class' => array('header-2', 'topic')));
    $section_class[] = 'section-help-break';

    if (is_null($section)) {
      $section_class[] = 'section-help-topic-terminology';

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-terminology', $header_attributes, $headers);
      $markup .= mfcs_help_topic_terminology($allowed_topics);
    }
    elseif (!is_null($sub_section)) {
      // failsafe.
      return mfcs_help_topic_not_found($topic, $section, $sub_section);
    }
    elseif ($section == 'facilities_use') {
      $section_class[] = 'section-help-topic-terminology-facilities_use';
      $headers[2] = array('markup' => 'Facilities Use Terminology', 'attributes' => array('class' => array('header-3', 'section')));

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-terminology-section-facilities_use', $header_attributes, $headers);
      $markup .= mfcs_help_topic_terminology_section_facilities_use($allowed_topics);
    }
    elseif ($section == 'system') {
      $section_class[] = 'section-help-topic-terminology-system';
      $headers[2] = array('markup' => 'System Terminology', 'attributes' => array('class' => array('header-3', 'section')));

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-terminology-section-system', array(), $headers);
      $markup .= mfcs_help_topic_terminology_section_system($allowed_topics);
    }
    elseif ($section == 'documentation') {
      $section_class[] = 'section-help-topic-terminology-documentation';
      $headers[2] = array('markup' => 'Documentation Terminology', 'attributes' => array('class' => array('header-3', 'section')));

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-terminology-section-system', array(), $headers);
      $markup .= mfcs_help_topic_terminology_section_documentation($allowed_topics);
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

  $markup .= '</section>';
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
    0 => array('markup' => 'Facilities Use Documentation', 'attributes' => array('class' => array('header-1', 'title'))),
  );

  $header_attributes = array();

  $section_class = $base_class;
  $section_class[] = 'section-help-main';
  $section_class[] = 'section-help-presentation-single';

  $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
  $markup .= $navigation_markup;
  $markup .= mfcs_help_render_headers('topic', $header_attributes, $headers, 0);

  $section_id = 'mfcs-help-documentation';
  $base_class[] = 'section-help-child';
  $base_class[] = 'section-help-documentation';

  $headers = array(
    0 => array('markup' => '', 'attributes' => array('class' => array('header-2', 'section'))),
  );


  // glossary
  $headers[0]['markup'] = 'Glossary';
  $section_class = $base_class;
  $section_class[] = 'section-help-topic-glossary';

  $markup .= '<section id="' . $section_id . '-glossary" class="' . implode(' ', $section_class) . '">';
  $markup .= mfcs_help_render_headers('topic-glossary', $header_attributes, $headers, 1);
  $markup .= mfcs_help_topic_glossary($allowed_topics, TRUE);
  $markup .= '</section>';


  // introduction
  $headers[0]['markup'] = 'Introduction';
  $section_class = $base_class;
  $section_class[] = 'section-help-topic-introduction';

  $markup .= '<section id="' . $section_id . '-introduction" class="' . implode(' ', $section_class) . '">';
  $markup .= mfcs_help_render_headers('topic-introduction', $header_attributes, $headers, 1);
  $markup .= mfcs_help_topic_introduction($allowed_topics, TRUE);
  $markup .= '</section>';


  // requests
  $headers[0]['markup'] = 'Requests';
  $section_class = $base_class;
  $section_class[] = 'section-help-topic-requests';

  $markup .= '<section id="' . $section_id . '-requests" class="' . implode(' ', $section_class) . '">';
  $markup .= mfcs_help_render_headers('topic-requests', $header_attributes, $headers, 1);
  $markup .= mfcs_help_topic_requests($allowed_topics, TRUE);

  $headers[0]['markup'] = 'Faculty, Staff, and Athletics Requests';
  $section_class = $sub_base_class;
  $section_class[] = 'section-help-topic-requests-section-employee';
  $section_class[] = 'no_first_of_type';

  $markup .= '<section id="' . $section_id . '-requests-section-employee" class="' . implode(' ', $section_class) . '">';
  $markup .= mfcs_help_render_headers('topic-requests-section-employee', $header_attributes, $headers, 2);
  $markup .= mfcs_help_topic_requests_section_employee($allowed_topics, TRUE);
  $markup .= '</section>';

  $headers[0]['markup'] = 'Student Organization Requests';
  $section_class = $sub_base_class;
  $section_class[] = 'section-help-topic-requests-section-student';

  $markup .= '<section id="' . $section_id . '-requests-section-student" class="' . implode(' ', $section_class) . '">';
  $markup .= mfcs_help_render_headers('topic-requests-section-student', $header_attributes, $headers, 2);
  $markup .= mfcs_help_topic_requests_section_student_organization($allowed_topics, TRUE);
  $markup .= '</section>';

  $headers[0]['markup'] = 'Camps Requests';
  $section_class = $sub_base_class;
  $section_class[] = 'section-help-topic-requests-section-camps';

  $markup .= '<section id="' . $section_id . '-requests-section-camps" class="' . implode(' ', $section_class) . '">';
  $markup .= mfcs_help_render_headers('topic-requests-section-camps', $header_attributes, $headers, 2);
  $markup .= mfcs_help_topic_requests_section_camps($allowed_topics, TRUE);
  $markup .= '</section>';

  $headers[0]['markup'] = 'External Requests';
  $section_class = $sub_base_class;
  $section_class[] = 'section-help-topic-requests-section-external';

  $markup .= '<section id="' . $section_id . '-requests-section-external" class="' . implode(' ', $section_class) . '">';
  $markup .= mfcs_help_render_headers('topic-requests-section-external', $header_attributes, $headers, 2);
  $markup .= mfcs_help_topic_requests_section_external($allowed_topics, TRUE);
  $markup .= '</section>';

  $markup .= '</section>';


  // review
  if (in_array('review', $allowed_topics)) {
    $headers[0]['markup'] = 'Review';
    $section_class = $base_class;
    $section_class[] = 'section-help-topic-review';

    $markup .= '<section id="' . $section_id . '-review" class="' . implode(' ', $section_class) . '">';
    $markup .= mfcs_help_render_headers('topic-review', $header_attributes, $headers, 1);
    $markup .= mfcs_help_topic_review($allowed_topics, TRUE);
    $markup .= '</section>';
  }


  // manage
  if (in_array('manage', $allowed_topics)) {
    $headers[0]['markup'] = 'Manage';
    $section_class = $base_class;
    $section_class[] = 'section-help-topic-manage';

    $markup .= '<section id="' . $section_id . '-manage" class="' . implode(' ', $section_class) . '">';
    $markup .= mfcs_help_render_headers('topic-manage', $header_attributes, $headers, 1);
    $markup .= mfcs_help_topic_manage($allowed_topics, TRUE);
    $markup .= '</section>';
  }


  // frequently asked questions
  $headers[0]['markup'] = 'Frequently Asked Questions';
  $section_class = $base_class;
  $section_class[] = 'section-help-topic-faq';

  $markup .= '<section id="' . $section_id . '-faq" class="' . implode(' ', $section_class) . '">';
  $markup .= mfcs_help_render_headers('topic-faq', $header_attributes, $headers, 1);
  $markup .= mfcs_help_topic_faq($allowed_topics, TRUE);
  $markup .= '</section>';


  // terminology
  $headers[0]['markup'] = 'Terminology';
  $section_class = $base_class;
  $section_class[] = 'section-help-topic-terminology';

  $markup .= '<section id="' . $section_id . '-terminology" class="' . implode(' ', $section_class) . '">';
  $markup .= mfcs_help_render_headers('topic-terminology', $header_attributes, $headers, 1);
  $markup .= mfcs_help_topic_terminology($allowed_topics, TRUE);
  $markup .= '</section>';


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
        $determined['next'] = $base_path . 'help-0/faq' . $url_arguments;
        $determined['tooltip-next'] = 'Documentation: Frequently Asked Questions';
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
        $determined['next'] = $base_path . 'help-0/faq' . $url_arguments;
        $determined['tooltip-next'] = 'Documentation: Frequently Asked Questions';
      }

      $determined['previous'] = $base_path . 'help-0/requests' . $url_arguments;
      $determined['tooltip-previous'] = 'Documentation: Requests';
    }
  }
  elseif ($topic == 'manage' && in_array('manage', $allowed_topics)) {
    $determined['top'] = $base_path . 'help-0' . $url_arguments;
    $determined['tooltip-top'] = 'Glossary';

    $determined['next'] = $base_path . 'help-0/faq' . $url_arguments;
    $determined['tooltip-next'] = 'Documentation: Frequently Asked Questions';

    if (in_array('review', $allowed_topics)) {
      $determined['previous'] = $base_path . 'help-0/review' . $url_arguments;
      $determined['tooltip-previous'] = 'Documentation: Review';
    }
    else {
      $determined['previous'] = $base_path . 'help-0/requests' . $url_arguments;
      $determined['tooltip-previous'] = 'Documentation: Requests';
    }
  }
  elseif ($topic == 'faq') {
    $determined['top'] = $base_path . 'help-0' . $url_arguments;
    $determined['tooltip-top'] = 'Glossary';

    $determined['next'] = $base_path . 'help-0/terminology' . $url_arguments;
    $determined['tooltip-next'] = 'Documentation: Terminology';


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
  $topics[] = 'calendar-use';
  $topics[] = 'calendar-export';

  // documentation
  $topics[] = 'introduction';
  $topics[] = 'requests';
  $topics[] = 'requests-create';
  $topics[] = 'requests-edit';
  $topics[] = 'requests-cancel';
  $topics[] = 'requests-copy';
  $topics[] = 'requests-find';
  $topics[] = 'faq';
  $topics[] = 'terminology';

  // add reviewing-specific topics.
  if ($is_reviewer || $is_manager || $is_administer) {
    // user guides
    $topics[] = 'request-review';

    // documentation
    $topics[] = 'requests-review';
    $topics[] = 'review';
  }

  // add managing-specific topics.
  if ($is_manager || $is_administer) {
    // user guides
    $topics[] = 'request-manage';

    // documentation
    $topics[] = 'requests-manage';
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
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 * @param bool $user_guides
 *   (optional) When TRUE, user guides will be linked as inline when $as_inline is TRUE.
 *   When FALSE, documentation will be linked as inline when $as_inline is TRUE.
 *
 * @return string|bool
 *   Return the HTML markup on success, otherwise FALSE is returned.
 *
 * @see: mfcs_help_0_page()
 */
function mfcs_help_topic_glossary($allowed_topics, $as_inline = FALSE, $user_guides = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  if (!is_bool($user_guides)) {
    cf_error::invalid_bool('user_guides');
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $section_class = array(
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
      $links['introduction']['url'] = '#mfcs-help-documentation-introduction' . $url_arguments;
      $links['requests']['url'] = '#mfcs-help-documentation-requests' . $url_arguments;
      $links['requests/employee']['url'] = '#mfcs-help-documentation-requests-section-employee' . $url_arguments;
      $links['requests/student']['url'] = '#mfcs-help-documentation-requests-section-student' . $url_arguments;
      $links['requests/camps']['url'] = '#mfcs-help-documentation-requests-section-camps' . $url_arguments;
      $links['requests/external']['url'] = '#mfcs-help-documentation-requests-section-external' . $url_arguments;
      $links['review']['url'] = '#mfcs-help-documentation-review' . $url_arguments;
      $links['review/venue_available']['url'] = '#mfcs-help-documentation-review-section-venue_available' . $url_arguments;
      $links['review/review']['url'] = '#mfcs-help-documentation-review-section-review' . $url_arguments;
      $links['review/final_decision']['url'] = '#mfcs-help-documentation-review-section-final_decision' . $url_arguments;
      $links['review/completed']['url'] = '#mfcs-help-documentation-review-section-completed' . $url_arguments;
      $links['manage']['url'] = '#mfcs-help-documentation-manage' . $url_arguments;
      $links['manage/requests']['url'] = '#mfcs-help-documentation-manage-section-requests' . $url_arguments;
      $links['manage/users']['url'] = '#mfcs-help-documentation-manage-section-users' . $url_arguments;
      $links['manage/problems']['url'] = '#mfcs-help-documentation-manage-section-problems' . $url_arguments;
      $links['manage/availability']['url'] = '#mfcs-help-documentation-manage-section-availability' . $url_arguments;
      $links['manage/logs']['url'] = '#mfcs-help-documentation-manage-section-logs' . $url_arguments;
      $links['faq']['url'] = '#mfcs-help-documentation-faq' . $url_arguments;
      $links['terminology']['url'] = '#mfcs-help-documentation-terminology' . $url_arguments;
      $links['terminology/facilities_use']['url'] = '#mfcs-help-documentation-terminology-section-facilities_use' . $url_arguments;
      $links['terminology/system']['url'] = '#mfcs-help-documentation-terminology-section-system' . $url_arguments;
      $links['terminology/documentation']['url'] = '#mfcs-help-documentation-terminology-section-documentation' . $url_arguments;
    }
  }

  $markup = '';
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

  $markup .= mfcs_help_render_headers('topic-glossary-user_guides', array('class' => $section_class), $headers, 2);
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

  $markup .= mfcs_help_render_headers('topic-glossary-documentation', array('class' => $section_class), $headers, 2);
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

    $markup .= mfcs_help_render_headers('topic-glossary-presentation', array('class' => $section_class), $headers, 2);
    $markup .= '<ul class="navigation-list">';
    $markup .= '<li class="list-item"><a href="' . $base_path . 'help-0/presentation/single' . $url_arguments . '" class="item-link no_print_links_as_link">View Documentation as a Single Page</a></li>';
    $markup .= '<li class="list-item"><a href="' . $base_path . 'help-0/presentation/pdf' . $url_arguments . '" class="item-link no_print_links_as_link">Download Entire Documentation as a PDF</a></li>';
    $markup .= '</ul>';

    $markup .= '</nav>';
    $markup .= '</div>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: introduction.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_introduction($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }


  $markup = '<div class="paragraph">';
  $markup .= 'The <dfn class="term">Facilities Use System</dfn>, or <dfn class="term"><abbr title="Facilities Use System" class="abbreviation">FCS</abbr></dfn>, is a product of <em class="emphasis">McNeese State University\'s</em> culture and needs in tracking special events. ';
  $markup .= 'Through this system, <em class="emphasis">McNeese State University</em> provides facilities and resources to our <strong class="bold">Students</strong>, <strong class="bold">Employees</strong>, and the <strong class="bold">Community</strong> to further educational, cultural, and recreational initiatives through events held on campus. ';
  $markup .= 'All activities, meetings, or requests, excluding scheduled academic classes, must be scheduled through the facilities use special event request form. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'The system itself provides methods for requesting, reviewing, and viewing such events through numerous technologies, such as e-mail, iCalendar, <abbr title="Open Pretty Good Privacy" class="abbreviation">OpenPGP</abbr>, and <abbr title="Hypertext Markup Language 5" class="abbreviation">HTML5</abbr> are used by the <dfn class="term">Facilities Use System</dfn>. ';
  $markup .= '<strong class="bold">OpenPGP</strong> signed e-mails are are sent out for many actions performed on or by this system. ';
  $markup .= '<strong class="bold">iCalendar</strong> is provided to allow users to download a copy of approved requests and add them to their own personal calendars. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/requests' . $url_arguments . '" class="item-link no_print_links_as_link" title="Documentation: Requests">Documentation: Requests</a>. ';
    $markup .= '</div>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: requests.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_requests($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $markup = '';

  $markup .= '<div class="paragraph">';
  $markup .= 'A <dfn class="term">Request</dfn> is a web form entry for reserving a particular place owned or operated by <em class="emphasis">McNeese State University</em> at some specified date and time. ';
  $markup .= 'Each <dfn class="term">Request</dfn> goes through a <dfn class="term">Review</dfn> process for approval to help ensure that the <dfn class="term">Room</dfn> is available, all appropriate expenses are paid, and all conflicts are avoided. ';
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
  $markup .= 'When making a request it is these <dfn class="term">Coordinators</dfn> that are the primary point of contact for their respective venue. ';
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
  $markup .= 'Room conflicts represent other approved requests or other requests currently under review for a given <dfn class="term">Room</dfn> on the specified date and time. ';
  $markup .= 'Holidays are only an informative restrictions to let the <dfn class="term">Requester</dfn> know that a given <dfn class="term">Room</dfn> <em class="emphasis">may</em> be unavailable on the requested date and time. ';
  $markup .= 'Room unavailabilities include classes or class-related activities. ';
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
  $markup .= 'When viewing <dfn class="term">Approved</dfn> <dfn class="term">Requests</dfn> on the daily calendar, custom color codes are used to make the different <dfn class="term">Request Types</dfn> easier to spot. ';
  $markup .= 'For those less visually inclined, the <dfn class="term">Request Type</dfn> is also presented along side the <dfn class="term">Approved</dfn> <dfn class="term">Request</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'During the <dfn class="term">Review</dfn> process, e-mails may be sent by the <dfn class="term">Facilities Use System</dfn>. These e-mails should not be replied to. Your replies will not be recieved. Instead, all responses to received e-mails should be sent via <dfn class="term">Comments</dfn> in the <dfn class="term"><abbr class="abbreviation" title="Facilities Use System">FCS</abbr></dfn> system. ';
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
  $markup .= 'It is during this <dfn class="term">Step</dfn> that all <dfn class="term">Issues</dfn> should be resolved and in some cases additional <dfn class="term">Requirements</dfn> may be specified. ';
  $markup .= 'The <dfn class="term">Request</dfn> may either be <dfn class="term">Approved</dfn> or <dfn class="term">Denied</dfn>, but regardless of the <dfn class="term">Final Decision</dfn>, the <dfn class="term">Request</dfn> will be moved into the <dfn class="term">Completed</dfn> <dfn class="term">Step</dfn> where the <dfn class="term">Review</dfn> process is considered complete. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'The <dfn class="term">Completed</dfn> <dfn class="term">Step</dfn> identifies a <dfn class="term">Request</dfn> as having been fully processed and does <strong class="bold">not</strong> represent whether or not a given <dfn class="term">Request</dfn> is <dfn class="term">Approved</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= ' <dfn class="term">Completed</dfn> and <dfn class="term">Approved</dfn> <dfn class="term">Requests</dfn> appear on the calendars, the time slots are reserved, and an e-mail is sent to the <dfn class="term">Requester</dfn>. ';
  $markup .= ' These e-mails also contain additional files, such as an iCalendar file (a file with an ICS file extension), for importing the <dfn class="term">Approved</dfn> <dfn class="term">Request</dfn> into the persons own personal calendars. ';
  $markup .= ' Under certain conditions, <dfn class="term">Requests</dfn> may be altered and even individual dates may be cancelled or uncancelled. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'When a <dfn class="term">Request</dfn> (or one of its dates) is <dfn class="term">Cancelled</dfn>, the requested dates and times is subject to no longer being available for <dfn class="term">Uncancelling</dfn>. ';
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
      $markup .= 'Continue on to <a href="' . $base_path . 'help-0/faq' . $url_arguments . '" class="item-link">Documentation: Frequently Asked Questions</a>. ';
    }
    $markup .= '</div>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic - faculty, staff, and athletics.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_requests_section_employee($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $markup = '';

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

  return $markup;
}

/**
 * Build the markup for the help topic: requests - student organization.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_requests_section_student_organization($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $markup = '';

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

  return $markup;
}

/**
 * Build the markup for the help topic: requests - camps.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_requests_section_camps($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $markup = '';

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

  return $markup;
}

/**
 * Build the markup for the help topic: requests, sub-section: external.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_requests_section_external($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  $markup = '';

  $markup .= '<div class="paragraph">';
  $markup .= 'These <dfn class="term">Requests</dfn> generally represent people or organizations not directly affiliated with <em class="emphasis">McNeese State University</em> or are otherwise consider a 3rd-party. ';
  $markup .= 'While there are no modifications to these forms, the <dfn class="term">Requester</dfn> is expected to go through the appropriate point of contact for filling out the forms and does not directly create <dfn class="term">Requests</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  return $markup;
}

/**
 * Build the markup for the help topic: review.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_review($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $markup = '';

  $markup .= '<div class="paragraph">';
  $markup .= 'The <dfn class="term">Facilities Use System</dfn> is centerred around the <dfn class="term">Review</dfn> process to help ensure room availability, issue resolution, fee assignments, insurance confirmation, and any other situation including but not limited to closures due to emergencies. ';
  $markup .= 'This process is intended to increase communication between the different parties in a consistent and centralized manner. ';
  $markup .= 'Many actions, decisions, comments, and other changes to <dfn class="term">Requests</dfn> are stored on the system in the <dfn class="term">Review Log</dfn> (or in the <dfn class="term">Request History</dfn>) and may also be sent via e-mail to relevant parties. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="note"> ';
  $markup .= 'E-mails sent by the <dfn class="term">Facilities Use System</dfn> should not be replied to. Your replies will not be recieved. Instead, all responses to received e-mails should be sent via <dfn class="term">Comments</dfn> in the <dfn class="term"><abbr class="abbreviation" title="Facilities Use System">FCS</abbr></dfn> system. ';
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
  $markup .= 'There is an option to restrict messages such that the <dfn class="term">Requester</dfn> does not receive the <dfn class="term">Comment</dfn> (and also hiding the message in the <dfn class="term">Review Log</dfn>) for more private messages between <dfn class="term">Reviewers</dfn> and the <dfn class="term">Venue Coordinator</dfn>. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= mfcs_help_topic_review_section_venue_available($allowed_topics, TRUE);
  $markup .= mfcs_help_topic_review_section_review($allowed_topics, TRUE);
  $markup .= mfcs_help_topic_review_section_final_decision($allowed_topics, TRUE);
  $markup .= mfcs_help_topic_review_section_completed($allowed_topics, TRUE);

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    if (in_array('manage', $allowed_topics)) {
      $markup .= 'Continue on to <a href="' . $base_path . 'help-0/manage' . $url_arguments . '" class="item-link">Documentation: Manage</a>. ';
    }
    else {
      $markup .= 'Continue on to <a href="' . $base_path . 'help-0/faq' . $url_arguments . '" class="item-link">Documentation: Frequently Asked Questions</a>. ';
    }
    $markup .= '</div>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: review - venue available step.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_review_section_venue_available($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $section_class = array(
    'section',
    'section-help',
    'section-help-break',
  );

  $markup = '<section id="mfcs-help-documentation-review-section-venue_available" class="section-help section-help-review-venue_available">';

  if ($as_inline) {
    $headers = array(
      0 => array('markup' => 'Venue Available Step', 'attributes' => array('class' => array('header-3', 'section'))),
    );

    $markup .= mfcs_help_render_headers('topic-review-section-venue_available', array('class' => $section_class), $headers, 2);
  }

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

  $markup .= '</section>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/review/review' . $url_arguments . '" class="item-link">Documentation: Review - Review</a>. ';
    $markup .= '</div>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: review - review step.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_review_section_review($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $section_class = array(
    'section',
    'section-help',
    'section-help-break',
  );

  $markup = '<section id="mfcs-help-documentation-review-section-review" class="section-help section-help-review-review">';

  if ($as_inline) {
    $headers = array(
      0 => array('markup' => 'Review Step', 'attributes' => array('class' => array('header-3', 'section'))),
    );

    $markup .= mfcs_help_render_headers('topic-review-section-review', array('class' => $section_class), $headers, 2);
  }

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

  $markup .= '</section>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/review/final_decision' . $url_arguments . '" class="item-link">Documentation: Review - Final Decision</a>. ';
    $markup .= '</div>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: review - final decision step.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_review_section_final_decision($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $section_class = array(
    'section',
    'section-help',
    'section-help-break',
  );

  $markup = '<section id="mfcs-help-documentation-review-section-final_decision" class="section-help section-help-review-final_decision">';

  if ($as_inline) {
    $headers = array(
      0 => array('markup' => 'Final Decision Step', 'attributes' => array('class' => array('header-3', 'section'))),
    );

    $markup .= mfcs_help_render_headers('topic-review-section-final_decision', array('class' => $section_class), $headers, 2);
  }

  $markup .= '<div class="paragraph">';
  $markup .= 'The <dfn class="term">Final Decision</dfn> <dfn class="term">Review Step</dfn> is where <dfn class="term">Reviewers</dfn> the decision for <dfn class="term">Approving</dfn> or <dfn class="term">Denying</dfn> the <dfn class="term">Request</dfn> is made. ';
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

  $markup .= '</section>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/review/completed' . $url_arguments . '" class="item-link">Documentation: Review - Completed</a>. ';
    $markup .= '</div>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: review - completed step.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_review_section_completed($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  $section_class = array(
    'section',
    'section-help',
    'section-help-break',
  );

  $markup = '<section id="mfcs-help-documentation-review-section-completed" class="section-help section-help-review-completed">';

  if ($as_inline) {
    $headers = array(
      0 => array('markup' => 'Completed Step', 'attributes' => array('class' => array('header-3', 'section'))),
    );

    $markup .= mfcs_help_render_headers('topic-review-section-completed', array('class' => $section_class), $headers, 2);
  }

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

  $markup .= '</section>';

  return $markup;
}

/**
 * Build the markup for the help topic: manage.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_manage($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $markup = '';

  $markup .= '<div class="paragraph">';
  $markup .= '<dfn class="term">Managers</dfn> are essentially less-technical administrators. ';
  $markup .= 'They can perform various administrative tasks such as forcibly changing the <dfn class="term">Review Step</dfn>, adding and modifying users, altering content, creating <dfn class="term">Room</dfn> <dfn class="term">Unavailabilities</dfn>, and troubleshooting. ';
  $markup .= 'Most of the additional management functionality is presented on the management page. ';
  $markup .= '<br>';
  $markup .= '</div>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/faq' . $url_arguments . '" class="item-link">Documentation: Frequently Asked Questions</a>. ';
    $markup .= '</div>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: faq.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_faq($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $markup = '';

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

  return $markup;
}

/**
 * Build the markup for the help topic: terminology.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_terminology($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  // @todo: navigation markup? no section seems to be defined here.

  $markup = '';
  $markup .= mfcs_help_topic_terminology_section_facilities_use($allowed_topics, TRUE);
  $markup .= mfcs_help_topic_terminology_section_system($allowed_topics, TRUE);
  $markup .= mfcs_help_topic_terminology_section_documentation($allowed_topics, TRUE);

  return $markup;
}

/**
 * Build the markup for the help topic: terminology - facilities use.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_terminology_section_facilities_use($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $section_class = array(
    'section',
    'section-help',
    'section-help-break',
  );

  $markup = '<section id="mfcs-help-documentation-terminology-section-facilities_use" class="section-help section-help-break section-help-terminology-system">';

  if ($as_inline) {
    $headers = array(
      0 => array('markup' => 'Facilities Use Terminology', 'attributes' => array('class' => array('header-3', 'section'))),
    );

    $markup .= mfcs_help_render_headers('topic-terminology-section-facilities_use', array('class' => $section_class), $headers, 2);
  }

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

  $markup .= '</section>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/terminology/system' . $url_arguments . '" class="item-link">Documentation: Terminology - System</a>. ';
    $markup .= '</div>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: terminology - system.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_terminology_section_system($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $section_class = array(
    'section',
    'section-help',
    'section-help-break',
  );

  $markup = '<section id="mfcs-help-documentation-terminology-section-system" class="section-help section-help-break section-help-terminology-system">';

  if ($as_inline) {
    $headers = array(
      0 => array('markup' => 'System Terminology', 'attributes' => array('class' => array('header-3', 'section'))),
    );

    $markup .= mfcs_help_render_headers('topic-terminology-section-system', array('class' => $section_class), $headers, 2);
  }

  $markup .= '<dl class="definition_list">';

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
  $markup .= '<dfn class="term">iCal</dfn> is an internet standard used to electronically communicate event information. ';
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

  $markup .= '</section>';

  if (!$as_inline) {
    $markup .= '<div class="continue_on no-print">';
    $markup .= 'Continue on to <a href="' . $base_path . 'help-0/terminology/documentation' . $url_arguments . '" class="item-link">Documentation: Terminology - Documentation</a>. ';
    $markup .= '</div>';
  }

  return $markup;
}

/**
 * Build the markup for the help topic: terminology - documentation.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 * @param bool $as_inline
 *   (optional) When TRUE, certain links and formatting are omitted.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_terminology_section_documentation($allowed_topics, $as_inline = FALSE) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  if (!is_bool($as_inline)) {
    cf_error::invalid_bool('as_inline');
    return '';
  }

  $section_class = array(
    'section',
    'section-help',
    'section-help-break',
  );

  $markup = '<section id="mfcs-help-documentation-terminology-section-documentation" class="section-help section-help-break section-help-terminology-documentation">';

  if ($as_inline) {
    $headers = array(
      0 => array('markup' => 'Documentation Terminology', 'attributes' => array('class' => array('header-3', 'section'))),
    );

    $markup .= mfcs_help_render_headers('topic-terminology-section-documentation', array('class' => $section_class), $headers, 2);
  }

  $markup .= '<dl class="definition_list">';

  $markup .= '<dt id="terminology-documentation-documentation" class="definition_list-title">Documentation</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'The <dfn class="term">Documentation</dfn> (within the context of this facilities use documentation) refers to a descriptive explanation of the system and its functions. ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-documentation-documentation" class="definition_list-title">User Guides</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A <dfn class="term">User Guide</dfn> refers to a step-by-step explanation on how to perform a particular task within the system (often times including screenshots). ';
  $markup .= '</dd>';

  $markup .= '</dl>';

  $markup .= '</section>';

  return $markup;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
