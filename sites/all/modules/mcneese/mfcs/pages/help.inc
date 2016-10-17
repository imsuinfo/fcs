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

  $user = cf_current_user();

  $title = 'Help';
  if (is_null($topic)) {
    mfcs_add_canonical_headers('help');
  }
  elseif (is_null($section)) {
    $topic = strtolower($topic);

    mfcs_add_canonical_headers('help/' . $topic);

    $title = 'Help - ' . mfcs_help_url_cleanup_for_title($topic);
  }
  elseif (is_null($sub_section)) {
    $topic = strtolower($topic);
    $section = strtolower($section);

    mfcs_add_canonical_headers('help/' . $topic . '/' . $section);

    $title = 'Help - ' . mfcs_help_url_cleanup_for_title($topic) . ' - ' . mfcs_help_url_cleanup_for_title($section);
  }
  else {
    $topic = strtolower($topic);
    $section = strtolower($section);
    $sub_section = strtolower($sub_section);

    mfcs_add_canonical_headers('help/' . $topic . '/' . $section . '/' . $sub_section);

    $title = 'Help - ' . mfcs_help_url_cleanup_for_title($topic) . ' - ' . mfcs_help_url_cleanup_for_title($section) . ' - ' . mfcs_help_url_cleanup_for_title($sub_section);
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
  $navigation_urls = mfcs_help_determine_next_previous_top($topic, $section, $sub_section);
  $navigation_markup = '';

  if (!is_null($navigation_urls['top'])) {
    $navigation_markup = '<nav id="mfcs-help-navigation_links" class="help-navigation_links no-print">';

    if (!empty($navigation_urls['previous'])) {
      $navigation_markup .= '<a href="' . $navigation_urls['previous'] . '" title="' . $navigation_urls['tooltip-previous'] . '" class="help-navigation_link link-previous">Previous</a>';
    }

    $navigation_markup .= '<a href="' . $navigation_urls['top'] . '" title="' . $navigation_urls['tooltip-top'] . '" class="help-navigation_link link-top">Top</a>';

    if (!empty($navigation_urls['next'])) {
      $navigation_markup .= '<a href="' . $navigation_urls['next'] . '" title="' . $navigation_urls['tooltip-next'] . '" class="help-navigation_link link-next">Next</a>';
    }

    $navigation_markup .= '</nav>';
  }

  // prepare the header information.
  $section_id = 'mfcs-section-help-main';
  $section_class = mfcs_help_base_class($topic, $section, $sub_section);

  $headers = array(
    0 => array('markup' => 'Facilities Use Documentation', 'attributes' => array('class' => array('header-1', 'title'))),
  );

  $header_attributes = array();

  if (is_null($topic)) {
    $section_class[] = 'section-help-topic';
    $headers[1] = array('markup' => 'Glossary', 'attributes' => array('class' => array('header-2', 'topic')));

    $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
    $markup .= $navigation_markup;
    $markup .= mfcs_help_render_headers('topic-glossary', $header_attributes, $headers);

    $markup .= mfcs_help_topic($allowed_topics);
  }
  elseif ($topic == 'introduction') {
    $section_class[] = 'section-help-topic-introduction';
    $headers[1] = array('markup' => 'Introduction', 'attributes' => array('class' => array('header-2', 'topic')));

    $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
    $markup .= $navigation_markup;
    $markup .= mfcs_help_render_headers('topic-introduction', $header_attributes, $headers);
    $markup .= mfcs_help_topic_introduction($allowed_topics);
  }
  elseif ($topic == 'requests') {
    $section_class[] = 'section-help-topic-requests';
    $headers[1] = array('markup' => 'Requests', 'attributes' => array('class' => array('header-2', 'topic')));

    $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
    $markup .= $navigation_markup;
    $markup .= mfcs_help_render_headers('topic-requests', $header_attributes, $headers);
    $markup .= mfcs_help_topic_requests($allowed_topics);
  }
  elseif ($topic == 'faq') {
    $section_class[] = 'section-help-topic-faq';
    $headers[1] = array('markup' => 'Frequently Asked Questions', 'attributes' => array('class' => array('header-2', 'topic')));

    $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
    $markup .= $navigation_markup;
    $markup .= mfcs_help_render_headers('topic-faq', $header_attributes, $headers);
    $markup .= mfcs_help_topic_faq($allowed_topics);
  }
  elseif ($topic == 'terminology') {
    $headers[1] = array('markup' => 'Terminology', 'attributes' => array('class' => array('header-2', 'topic')));

    if (is_null($section)) {
      $section_class[] = 'section-help-topic-terminology';

      $markup = '<section id="' . $section_id . '" class="' . implode(' ', $section_class) . '">';
      $markup .= $navigation_markup;
      $markup .= mfcs_help_render_headers('topic-terminology', $header_attributes, $headers);
      $markup .= mfcs_help_topic_terminology($allowed_topics);
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
      $markup .= mfcs_help_render_headers('topic-terminology-section-documentation', array(), $headers);
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

  $base_class = array('section-help-main');
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
function mfcs_help_determine_next_previous_top($topic = NULL, $section = NULL, $sub_section = NULL) {
  $determined = array(
    'top' => NULL,
    'next' => NULL,
    'previous' => NULL,
    'tooltip-top' => NULL,
    'tooltip-next' => NULL,
    'tooltip-previous' => NULL,
  );

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

  // at the topmost path, such as index, so there is nowhere to go.
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

    if ($section == 'camps') {
      $determined['top'] = $base_path . 'help-0/requests' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Requests';

      $determined['next'] = $base_path . 'help-0/requests/external' . $url_arguments;
      $determined['tooltip-next'] = 'Section: External Requests';
    }
    elseif ($section == 'external') {
      $determined['top'] = $base_path . 'help-0/requests' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Requests';

      $determined['next'] = $base_path . 'help-0/requests/employee' . $url_arguments;
      $determined['tooltip-next'] = 'Section: Faculty, Staff, and Athletics Requests';

      $determined['previous'] = $base_path . 'help-0/requests/camps' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: Camps Requests';
    }
    elseif ($section == 'employee') {
      $determined['top'] = $base_path . 'help-0/requests' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Requests';

      $determined['next'] = $base_path . 'help-0/requests/student' . $url_arguments;
      $determined['tooltip-next'] = 'Section: Student Organization Requests';

      $determined['previous'] = $base_path . 'help-0/requests/external' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: External Requests';
    }
    elseif ($section == 'student') {
      $determined['top'] = $base_path . 'help-0/requests' . $url_arguments;
      $determined['tooltip-top'] = 'Documentation: Requests';

      $determined['previous'] = $base_path . 'help-0/requests/employee' . $url_arguments;
      $determined['tooltip-previous'] = 'Section: Faculty, Staff, and Athletics Requests';
    }
    else {
      $determined['next'] = $base_path . 'help-0/faq' . $url_arguments;
      $determined['tooltip-next'] = 'Documentation: Frequently Asked Questions';

      $determined['previous'] = $base_path . 'help-0/introduction' . $url_arguments;
      $determined['tooltip-previous'] = 'Documentation: Introduction';
    }
  }
  elseif ($topic == 'faq') {
    $determined['top'] = $base_path . 'help-0' . $url_arguments;
    $determined['tooltip-top'] = 'Glossary';

    $determined['next'] = $base_path . 'help-0/terminology' . $url_arguments;
    $determined['tooltip-next'] = 'Documentation: Terminology';

    $determined['previous'] = $base_path . 'help-0/requests' . $url_arguments;
    $determined['tooltip-previous'] = 'Documentation: Requests';
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
  $topics[] = 'introduction';
  $topics[] = 'requests';
  $topics[] = 'requests-create';
  $topics[] = 'requests-edit';
  $topics[] = 'requests-cancel';
  $topics[] = 'requests-copy';
  $topics[] = 'requests-find';
  $topics[] = 'calendar-use';
  $topics[] = 'calendar-export';
  $topics[] = 'faq';
  $topics[] = 'terminology';

  // add reviewing-specific topics.
  if ($is_reviewer || $is_manager || $is_administer) {
    $topics[] = 'request-review';
  }

  // add managing-specific topics.
  if ($is_manager || $is_administer) {
    $topics[] = 'request-manage';
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
 *
 * @return string|bool
 *   Return the HTML markup on success, otherwise FALSE is returned.
 *
 * @see: mfcs_help_0_page()
 */
function mfcs_help_topic($allowed_topics) {
  global $base_path;
  global $mfcs_determined;

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $section_class = array(
    'section',
  );

  $markup = '';
  $markup .= '<div class="paragraph">';
  $markup .= '  The documentation is broken up into multiple <dfn class="term">Topics</dfn>.<br>';
  $markup .= '  Each <dfn class="term">Topic</dfn> is may have multiple <dfn class="term">Sections</dfn>, each of which may contain their own <dfn class="term">Sub-Sections</dfn>.<br>';
  $markup .= '  <dfn class="term">Sections</dfn> and <dfn class="term">Sub-Sections</dfn> will provide specific information while the <dfn class="term">Topic</dfn> provides general information.<br>';
  $markup .= '  <br>';
  $markup .= '  If you are simply looking for a step-by-step guide, then use the <dfn class="term">User Guide</dfn> index.<br>';
  $markup .= '  For more details on the how and the why or to gain a better understanding of the <dfn class="term">Facilities Use System</dfn>, view the <dfn class="term">Documentation</dfn> topic index.<br>';
  $markup .= '</div>';

  $markup .= '<br>';

  $markup .= '<div class="paragraph">';
  $markup .= '  <nav class="navigation navigation-help_index" role="navigation">';

  $headers = array(
    0 => array('markup' => 'User Guides', 'attributes' => array('class' => array('header-2', 'section'))),
  );

  $markup .= mfcs_help_render_headers('topic-index-user_guides', array('class' => $section_class), $headers, 1);
  $markup .= '    <ul class="navigation-list">';

  if (in_array('requests-create', $allowed_topics)) {
    $markup .= '      <li class="list-item">';
    $markup .= '        <a href="' . $base_path . 'help-0/request-create' . $url_arguments . '" class="item-link" title="User Guide: Create a Request">Create a Request</a><br>';
    $markup .= '        <ul class="item-navigation navigation-list">';
    $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/request-create/facilities_use' . $url_arguments . '" class="item-link" title="Section: Camps Requests">Camps</a></li>';
    $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/request-create/facilities_use' . $url_arguments . '" class="item-link" title="Section: External Requests">External</a></li>';
    $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/request-create/facilities_use' . $url_arguments . '" class="item-link" title="Section: Faculty, Staff, and Athletics Requests">Faculty, Staff, and Athletics</a></li>';
    $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/request-create/facilities_use' . $url_arguments . '" class="item-link" title="Section: Student Organization Requests">Student Organization</a></li>';
    $markup .= '        </ul>';
    $markup .= '      </li>';
  }

  if (in_array('requests-edit', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/request-edit' . $url_arguments . '" class="item-link" title="User Guide: Edit a Request">Edit a Request</a></li>';
  }

  if (in_array('requests-cancel', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/request-cancel' . $url_arguments . '" class="item-link" title="User Guide: Cancel or Uncancel a Request">Cancel or Uncancel a Request</a></li>';
  }

  if (in_array('requests-copy', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/request-copy' . $url_arguments . '" class="item-link" title="User Guide: Copy a Request">Copy a Request</a></li>';
  }

  if (in_array('requests-find', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/request-find' . $url_arguments . '" class="item-link" title="User Guide: Find a Request">Find a Request</a></li>';
  }

  if (in_array('requests-review', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/request-review' . $url_arguments . '" class="item-link" title="User Guide: Review Requests">Review Requests</a></li>';
  }

  if (in_array('requests-manage', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/request-manage' . $url_arguments . '" class="item-link" title="User Guide: Manage System">Manage System</a></li>';
  }

  if (in_array('calendar-use', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/calendar-use' . $url_arguments . '" class="item-link" title="User Guide: Using the Calendar">Using the Calendar</a></li>';
  }

  if (in_array('calendar-export', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/calendar-export' . $url_arguments . '" class="item-link" title="User Guide: Exporting the Calendar">Exporting the Calendar</a></li>';
  }

  $markup .= '    </ul>';
  $markup .= '  </nav>';
  $markup .= '</div>';

  $markup .= '<br>';

  $markup .= '<div class="paragraph">';
  $markup .= '  <nav class="navigation navigation-help_index" role="navigation">';

  $headers = array(
    0 => array('markup' => 'Documentation', 'attributes' => array('class' => array('header-2', 'section'))),
  );

  $markup .= mfcs_help_render_headers('topic-index-documentation', array('class' => $section_class), $headers, 1);
  $markup .= '    <ul class="navigation-list">';
  $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/introduction' . $url_arguments . '" class="item-link" title="Documentation: Introduction">Introduction</a></li>';
  $markup .= '      <li class="list-item">';
  $markup .= '        <a href="' . $base_path . 'help-0/requests' . $url_arguments . '" class="item-link" title="Documentation: Requests">Requests</a><br>';
  $markup .= '        <ul class="item-navigation navigation-list">';
  $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/requests/facilities_use' . $url_arguments . '" class="item-link" title="Section: Camps Requests">Camps</a></li>';
  $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/requests/facilities_use' . $url_arguments . '" class="item-link" title="Section: External Requests">External</a></li>';
  $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/requests/facilities_use' . $url_arguments . '" class="item-link" title="Section: Faculty, Staff, and Athletics Requests">Faculty, Staff, and Athletics</a></li>';
  $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/requests/facilities_use' . $url_arguments . '" class="item-link" title="Section: Student Organization Requests">Student Organization</a></li>';
  $markup .= '        </ul>';
  $markup .= '      </li>';
  $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/faq' . $url_arguments . '" class="item-link" title="Documentation: Frequently Asked Questions">Frequently Asked Questions</a></li>';
  $markup .= '      <li class="list-item">';
  $markup .= '        <a href="' . $base_path . 'help-0/terminology' . $url_arguments . '" class="item-link" title="Documentation: Terminology">Terminology</a><br>';
  $markup .= '        <ul class="item-navigation navigation-list">';
  $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/terminology/facilities_use' . $url_arguments . '" class="item-link" title="Section: Facilities Use">Facilities Use</a></li>';
  $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/terminology/system' . $url_arguments . '" class="item-link" title="Section: System">System</a></li>';
  $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/terminology/documentation' . $url_arguments . '" class="item-link" title="Section: Documentation">Documentation</a></li>';
  $markup .= '        </ul>';
  $markup .= '      </li>';
  $markup .= '    </ul>';
  $markup .= '  </nav>';
  $markup .= '</div>';

  return $markup;
}

/**
 * Build the markup for the help topic: introduction.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_introduction($allowed_topics) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }


  $markup = '<div class="paragraph">';
  $markup .= 'The <dfn class="term">Facilities Use System</dfn>, or <abbr title="Facilities Use System" class="abbreviation">FCS</abbr>, is a product of <strong>McNeese State University\'s</strong> culture and needs in tracking special events. ';
  $markup .= 'Through this system, <strong>McNeese State University</strong> provides facilities and resources to our <strong>Students</strong>, <strong>Employees</strong>, and the <strong>Community</strong> to further educational, cultural, and recreational initiatives through events held on campus. ';
  $markup .= 'All activities, meetings, or requests, excluding scheduled academic classes, must be scheduled through the facilities use special event request form. ';
  $markup .= '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= 'The system itself provides methods for requesting, reviewing, and viewing such events through numerous technologies, such as e-mail, iCalendar, <abbr title="Pretty Good Privacy" class="abbreviation">OpenPGP</abbr>, and <abbr title="Hypertext Markup Language 5" class="abbreviation">HTML5</abbr> are used by the <dfn class="term">Facilities Use System</dfn>. ';
  $markup .= '<strong><abbr title="Pretty Good Privacy" class="abbreviation">OpenPGP</abbr></strong> signed e-mails are are sent out for many actions performed on or by this system. ';
  $markup .= '<strong>iCalendar</strong> is provided to allow users to download a copy of approved requests and add them to their own personal calendars.';
  $markup .= '<br>';
  $markup .= '</div>';

  return $markup;
}

/**
 * Build the markup for the help topic: requests.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_requests($allowed_topics) {
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

  $markup = '';

  $markup .= '<div class="paragraph">';
  $markup .= 'A <dfn class="term">Request</dfn> is a web form entry for reserving a particular place at some specified time. ';
  $markup .= '<br>';
  $markup .= '</div>';

  #$markup .= '<div class="paragraph">';
  #$markup .= ' ';
  #$markup .= '<br>';
  #$markup .= '</div>';

  return $markup;
}

/**
 * Build the markup for the help topic: faq.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_faq($allowed_topics) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  $markup = '';

  $markup .= '<div class="paragraph">';
  $markup .= '<br>';
  $markup .= '</div>';

  return $markup;
}

/**
 * Build the markup for the help topic: terminology.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_terminology($allowed_topics) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  $section_class = array(
    'section',
  );

  $headers = array(
    0 => array('markup' => 'Facilities Use Terminology', 'attributes' => array('class' => array('header-3', 'section'))),
  );

  $markup = mfcs_help_render_headers('topic-terminology-section-facilities_use', array('class' => $section_class), $headers);
  $markup .= mfcs_help_topic_terminology_section_facilities_use($allowed_topics);

  $headers = array(
    0 => array('markup' => 'System Terminology', 'attributes' => array('class' => array('header-3', 'section'))),
  );

  $markup .= mfcs_help_render_headers('topic-terminology-section-facilities_use', array('class' => $section_class), $headers);
  $markup .= mfcs_help_topic_terminology_section_system($allowed_topics);

  $headers = array(
    0 => array('markup' => 'Documentation Terminology', 'attributes' => array('class' => array('header-3', 'section'))),
  );

  $markup .= mfcs_help_render_headers('topic-terminology-section-documentation', array('class' => $section_class), $headers);
  $markup .= mfcs_help_topic_terminology_section_documentation($allowed_topics);

  return $markup;
}

/**
 * Build the markup for the help topic: terminology - facilities use.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_terminology_section_facilities_use($allowed_topics) {
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

  $markup = '<dl class="definition_list">';

  $markup .= '<dt id="terminology-facilities_use-fcs" class="definition_list-title">FCS</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'See: <a href="#terminology-facilities_use-facilities_use_system" class="definition_list-see_other no_print_as_link">Facilities Use System</a>. ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-facilities_use_system" class="definition_list-title">Facilities Use System</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'The <dfn class="term">Facilities Use System</dfn>, or FCS, is the digital system or service used to provide McNeese State University with a facilities reservation process and tools. ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-location" class="definition_list-title">Location</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A physical or virtual place in which events may be held. ';
  $markup .= 'This is not restricted to buildings and can be something such as a baseball field. ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-facilities_use-request" class="definition_list-title">Request</dt> ';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= 'A web form entry for reserving a particular place at some specified time. ';
  $markup .= 'This is a central part of the <abbr title="Facilities Use System" class="abbreviation">FCS</abbr> such that most operations, functionality, features, and searches are centered around requests. ';
  $markup .= '<br>';
  $markup .= '<br>See Also: <a href="' . $base_path . 'help-0/requests' . $url_arguments . '" class="definition_list-see_other no_print_as_link">Topic: Requests</a>. ';
  $markup .= '</dd>';

  $markup .= '</dl>';

  return $markup;
}

/**
 * Build the markup for the help topic: terminology - system.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_terminology_section_system($allowed_topics) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  $markup = '<section id="fcs-section-help-terminology-system" class="section-help-terminology-system">';

  $markup .= '<dl class="definition_list">';

  $markup .= '<dt id="terminology-system-fcs" class="definition_list-title">iCalendar</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= '<dfn class="term">iCal</dfn> is an internet standard used to electronically communicate event information. ';
  $markup .= 'Files generally have the excetions <em>.ics</em>. ';
  $markup .= 'Files in this format (With a file extension of <em>.ics</em>) may be used to import an event or a set of events into your own personal calendar or even transfer the event details to a third party. ';
  $markup .= '<br>';
  $markup .= '<br>See Also: <a href="//en.wikipedia.org/wiki/ICalendar" class="definition_list-see_other" target="_blank"><span class="no_print_as_link"></span>https://en.wikipedia.org/wiki/ICalendar</a>. ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-system-fcs" class="definition_list-title">OpenPGP</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= '<dfn class="term">OpenPGP</dfn> stands for <strong>Open Pretty Good Privacy</strong>. ';
  $markup .= 'This is a standard used as a digital signature and to encrypt files and e-mails. ';
  $markup .= 'E-mails sent by this system are digitally signed using this standard to help ensure that the e-mails are not spam or otherwise falsified. ';
  $markup .= '<br>';
  $markup .= '<br>See Also: <a href="//en.wikipedia.org/wiki/Pretty_Good_Privacy" class="definition_list-see_other" target="_blank">https://en.wikipedia.org/wiki/Pretty_Good_Privacy</a>. ';
  $markup .= '<br>See Also: <a href="//en.wikipedia.org/wiki/Pretty_Good_Privacy#OpenPGP" class="definition_list-see_other" target="_blank">https://en.wikipedia.org/wiki/Pretty_Good_Privacy#OpenPGP</a>. ';
  $markup .= '</dd>';

  $markup .= '<dt id="terminology-system-fcs" class="definition_list-title">HTML5</dt>';
  $markup .= '<dd class="definition_list-definition">';
  $markup .= '<dfn class="term">HTML5</dfn> is an internet standard that can be very loosely called as the <em class="emphasis">"web language"</em> or the <em class="emphasis">"internet"</em>. ';
  $markup .= '<br>';
  $markup .= '<br>See Also: <a href="//en.wikipedia.org/wiki/HTML5" class="definition_list-see_other" target="_blank">https://en.wikipedia.org/wiki/HTML5</a>. ';
  $markup .= '</dd>';

  $markup .= '</dl>';

  $markup .= '</section>';

  return $markup;
}

/**
 * Build the markup for the help topic: terminology - documentation.
 *
 * @param array $allowed_topics
 *   An array of topics currently allowed based on the users access.
 *
 * @return string
 *   The generated HTML markup.
 */
function mfcs_help_topic_terminology_section_documentation($allowed_topics) {
  if (!is_array($allowed_topics)) {
    cf_error::invalid_array('allowed_topics');
    return '';
  }

  $markup = '<section id="fcs-section-help-terminology-documentation" class="section-help-terminology-documentation">';

  $markup .= '<dl class="definition_list">';

  $markup .= '</dl>';

  $markup .= '</section>';

  return $markup;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
