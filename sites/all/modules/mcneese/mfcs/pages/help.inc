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

  $title = 'Help';
  if (is_null($topic)) {
    mfcs_add_canonical_headers('help');
  }
  elseif (is_null($section)) {
    $topic = strtolower($topic);

    mfcs_add_canonical_headers('help/' . $topic);

    $title = 'Help - ' . ucfirst($topic);
  }
  elseif (is_null($sub_section)) {
    $topic = strtolower($topic);
    $section = strtolower($section);

    mfcs_add_canonical_headers('help/' . $topic . '/' . $section);

    $title = 'Help - ' . ucfirst($topic) . ' - ' . ucfirst($section);
  }
  else {
    $topic = strtolower($topic);
    $section = strtolower($section);
    $sub_section = strtolower($sub_section);

    mfcs_add_canonical_headers('help/' . $topic . '/' . $section . '/' . $sub_section);

    $title = 'Help - ' . ucfirst($topic) . ' - ' . ucfirst($section) . ' - ' . ucfirst($sub_section);
  }

  drupal_set_title($title);

  if (is_null($topic)) {
    return mfcs_help_topic();
  }

  $allowed_topics = mfcs_help_0_allowed_topics($user);

  if (!in_array($topic, $allowed_topics)) {
    return mfcs_help_topic_not_found($topic, $section, $sub_section);
  }

  // @todo: for each section and sub_section, provide and use an allowed_* array.

  // failsafe.
  return mfcs_help_topic_not_found($topic, $section, $sub_section);
}

/**
 * Provides the main/primary topic for the help page.
 *
 * This should be used as the start/home page for help/documentation.
 *
 * @return string|bool
 *   Return the HTML markup on success, otherwise FALSE is returned.
 *
 * @see: mfcs_help_0_page()
 */
function mfcs_help_topic() {
  global $base_path;
  global $mfcs_determined;
  $user = cf_current_user();

  $allowed_topics = mfcs_help_0_allowed_topics($user);

  $url_arguments = '';
  if (!empty($mfcs_determined['complete'])) {
    $url_arguments .= '?' . $mfcs_determined['complete'];
  }

  $base_class = array(
    'NULL-topic',
    'NULL-section',
    'NULL-sub_section',
  );

  $section_class = $base_class;
  $section_class[] = 'help-section';

  $markup = '<section class="' . implode(' ', $base_class) . '">';

  $headers = array(
    0 => array('markup' => 'Facilities Use Documentation'),
  );
  $markup .= mfcs_help_render_headers('section-topic_index', array(), $headers, 1);

  $markup .= '<div class="paragraph">';
  $markup .= '  The documentation is broken up into multiple <dfn class="term">Topic</dfn>s. ' . '<br>';
  $markup .= '  Each <dfn class="term">Topic</dfn> is may have multiple <dfn class="term">Section</dfn>s, each of which may contain their own <dfn class="term">Sub-Section</dfn>s. ' . '<br>';
  $markup .= '  <dfn class="term">Section</dfn>s and <dfn class="term">Sub-Section</dfn>s will provide specific information while the <dfn class="term">Topic</dfn> provides general information. ' . '<br>';
  $markup .= '  <br>';
  $markup .= '  If you are simply looking for a step-by-step guide, then use the <dfn class="term">Use Guide</dfn> index. ' . '<br>';
  $markup .= '  For more details on the how and the why or to gain a better understanding of the <dfn class="term">Facilities Use System</dfn>, use the <dfn class="term">Documentation</dfn> index. ' . '<br>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= '  <nav class="navigation navigation-help_index" role="navigation">' . '<br>';
  $headers = array(
    0 => array('markup' => 'Topic Index - User Guides'),
  );
  $markup .= mfcs_help_render_headers('menu-topic_index', array(), $headers, 1);
  $markup .= '    <ul class="navigation-list">';

  if (in_array('request-create', $allowed_topics)) {
    $markup .= '      <li class="list-item">';
    $markup .= '        <a href="' . $base_path . 'help-0/request-create' . $url_arguments . '" class="item-link" title="User Guide: Create a Request">' . 'Create a Request' . '</a><br>';
    $markup .= '        <ul class="item-navigation navigation-list">';
    $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/request-create/facilities_use' . $url_arguments . '" class="item-link" title="Section: Camps Requests">' . 'Camps' . '</a></li>';
    $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/request-create/facilities_use' . $url_arguments . '" class="item-link" title="Section: External Requests">' . 'External' . '</a></li>';
    $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/request-create/facilities_use' . $url_arguments . '" class="item-link" title="Section: Faculty, Staff, and Athletics Requests">' . 'Faculty, Staff, and Athletics' . '</a></li>';
    $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/request-create/facilities_use' . $url_arguments . '" class="item-link" title="Section: Student Organization Requests">' . 'Student Organization' . '</a></li>';
    $markup .= '        </ul>';
    $markup .= '      </li>';
  }

  if (in_array('request-edit', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/request-edit' . $url_arguments . '" class="item-link" title="User Guide: Edit a Request">' . 'Edit a Request' . '</a></li>';
  }

  if (in_array('request-cancel', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/request-cancel' . $url_arguments . '" class="item-link" title="User Guide: Cancel or Uncancel a Request">' . 'Cancel or Uncancel a Request' . '</a></li>';
  }

  if (in_array('request-copy', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/request-copy' . $url_arguments . '" class="item-link" title="User Guide: Copy a Request">' . 'Copy a Request' . '</a></li>';
  }

  if (in_array('request-find', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/request-find' . $url_arguments . '" class="item-link" title="User Guide: Find a Request">' . 'Find a Request' . '</a></li>';
  }

  if (in_array('request-review', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/request-review' . $url_arguments . '" class="item-link" title="User Guide: Review Requests">' . 'Review Requests' . '</a></li>';
  }

  if (in_array('request-manage', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/request-manage' . $url_arguments . '" class="item-link" title="User Guide: Manage System">' . 'Manage System' . '</a></li>';
  }

  if (in_array('calendar-use', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/calendar-use' . $url_arguments . '" class="item-link" title="User Guide: Using the Calendar">' . 'Using the Calendar' . '</a></li>';
  }

  if (in_array('calendar-export', $allowed_topics)) {
    $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/calendar-export' . $url_arguments . '" class="item-link" title="User Guide: Exporting the Calendar">' . 'Exporting the Calendar' . '</a></li>';
  }

  $markup .= '    </ul>';
  $markup .= '  </nav>';
  $markup .= '</div>';

  $markup .= '<div class="paragraph">';
  $markup .= '  <nav class="navigation navigation-help_index" role="navigation">' . '<br>';
  $headers = array(
    0 => array('markup' => 'Topic Index - Documentation'),
  );
  $markup .= mfcs_help_render_headers('menu-topic_index', array(), $headers, 1);
  $markup .= '    <ul class="navigation-list">';
  $markup .= '      <li class="list-item">';
  $markup .= '        <a href="' . $base_path . 'help-0/request-create' . $url_arguments . '" class="item-link" title="Documentation: Requests">' . 'Requests' . '</a><br>';
  $markup .= '        <ul class="item-navigation navigation-list">';
  $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/request/facilities_use' . $url_arguments . '" class="item-link" title="Section: Camps Requests">' . 'Camps' . '</a></li>';
  $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/request/facilities_use' . $url_arguments . '" class="item-link" title="Section: External Requests">' . 'External' . '</a></li>';
  $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/request/facilities_use' . $url_arguments . '" class="item-link" title="Section: Faculty, Staff, and Athletics Requests">' . 'Faculty, Staff, and Athletics' . '</a></li>';
  $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/request/facilities_use' . $url_arguments . '" class="item-link" title="Section: Student Organization Requests">' . 'Student Organization' . '</a></li>';
  $markup .= '        </ul>';
  $markup .= '      </li>';
  $markup .= '      <li class="list-item"><a href="' . $base_path . 'help-0/faq' . $url_arguments . '" class="item-link" title="Documentation: Frequently Asked Questions">' . 'Frequently Asked Questions' . '</a></li>';
  $markup .= '      <li class="list-item">';
  $markup .= '        <a href="' . $base_path . 'help-0/terminology' . $url_arguments . '" class="item-link" title="Documentation: Terminology">' . 'Terminology' . '</a><br>';
  $markup .= '        <ul class="item-navigation navigation-list">';
  $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/terminology/facilities_use' . $url_arguments . '" class="item-link" title="Section: Facilities Use">' . 'Facilities Use' . '</a></li>';
  $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/terminology/system' . $url_arguments . '" class="item-link" title="Section: System">' . 'System' . '</a></li>';
  $markup .= '          <li class="list-item"><a href="' . $base_path . 'help-0/terminology/documentation' . $url_arguments . '" class="item-link" title="Section: Documentation">' . 'Documentation' . '</a></li>';
  $markup .= '        </ul>';
  $markup .= '      </li>';
  $markup .= '    </ul>';
  $markup .= '  </nav>';
  $markup .= '</div>';

  $markup .= '</section>';

  return $markup;
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

  $base_class = array();
  if (is_null($topic)) {
    $base_class[] = 'NULL-topic';
    $base_class[] = 'NULL-section';
    $base_class[] = 'NULL-sub_section';

    $first_line = 'The help documentation you were looking for was not found. ' . '<br>';
  }
  elseif (is_null($section)) {
    $headers[0]['markup'] = 'No Valid Topic Found';

    $base_class[] = 'topic-' . cf_theme_safe_css_string_part($topic);
    $base_class[] = 'NULL-section';
    $base_class[] = 'NULL-sub_section';

    $first_line = 'The topic "' . $topic . '" was not found in the help documentation. ' . '<br>';
  }
  elseif (is_null($sub_section)) {
    $headers[0]['markup'] = 'No Valid Section Found';

    $base_class[] = 'topic-' . cf_theme_safe_css_string_part($topic);
    $base_class[] = 'section-' . cf_theme_safe_css_string_part($section);
    $base_class[] = 'NULL-sub_section';

    $first_line = 'The section "' . $section . '" was not found under topic "' . $topic . '" of the help documentation. ' . '<br>';
  }
  else {
    $headers[0]['markup'] = 'No Valid Sub-Section Found';

    $base_class[] = 'topic-' . cf_theme_safe_css_string_part($topic);
    $base_class[] = 'section-' . cf_theme_safe_css_string_part($section);
    $base_class[] = 'sub_section-' . cf_theme_safe_css_string_part($sub_section);

    $first_line = 'The sub-section "' . $sub_section . '" was not found under section "' . $section . '" of topic "' . $topic . '" of the help documentation. ' . '<br>';
  }

  $section_class = $base_class;
  $section_class[] = 'help-section';

  $markup = '<section class="' . implode(' ', $base_class) . '">';
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

  $header_class = array(
    'help-header',
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

  $markup = '<header' . $attributes_string . '">';
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
  $administer = user_access('mfcs administer', $user);
  $manager = user_access('mfcs manage', $user);
  $reviewer = user_access('mfcs review', $user);
  $requester = user_access('mfcs request', $user);

  $topics = array();
  $topics[] = 'request-create';
  $topics[] = 'request-edit';
  $topics[] = 'request-cancel';
  $topics[] = 'request-copy';
  $topics[] = 'request-find';
  $topics[] = 'calendar-use';
  $topics[] = 'calendar-export';

  // add reviewing-specific topics.
  if ($reviewer || $manager || $administer) {
    $topics[] = 'request-review';
  }

  // add managing-specific topics.
  if ($manager || $administer) {
    $topics[] = 'request-manage';
  }

  // add administer-specific topics.
  if ($administer) {
  }

  return $topics;
}

/**
 * @} End of '@addtogroup mfcs'.
 */
