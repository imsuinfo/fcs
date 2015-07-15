<?php

/**
 * @file
 * Class for managing RSS Feeds.
 */

/**
 * Class for managing RSS Feed Channels.
 * This is a base class that contains fields shared between RSS channel and RSS
 * item.
 */
class McNeeseFeedsRssBase {
  protected $title = NULL;
  protected $link = NULL;
  protected $description = NULL;
  protected $content = NULL;
  protected $publish_date = NULL;
  protected $copyright = NULL;
  protected $categories = array();
  protected $tab = '';

  /**
   * Assign values for the required base fields.
   *
   * @param string $title
   *   Title value.
   * @param string $link
   *   Link URL
   * @param string $description
   *   Description text or markup.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function assign_common_required($title, $link, $description) {
    if (!$this->set_title($title)) {
      return FALSE;
    }

    if (!$this->set_link($link)) {
      return FALSE;
    }

    if (!$this->set_description($description)) {
      return FALSE;
    }

    return TRUE;
  }

  /**
   * assign values for the optional base fields.
   *
   * @param string|null $content
   *   (optional) Specify non-plain text description. This gets appended to the
   *   description as 'content:encoded'.
   * @param int $publish_date
   *   (optional) The publish date timestamp.
   * @param string $copyright
   *   (optional) The copyright string.
   * @param array $categories
   *   (optional) An array of category strings.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function assign_common_optional($content = NULL, $publish_date = NULL, $copyright = NULL, $categories = NULL) {
    if (!is_null($content)) {
      if (!$this->set_content($content)) {
        return FALSE;
      }
    }

    if (!is_null($publish_date) && $publish_date > 0) {
      if (!$this->set_publish_date($publish_date)) {
        return FALSE;
      }
    }

    if (!is_null($copyright)) {
      if (!$this->set_copyright($copyright)) {
        return FALSE;
      }
    }

    if (!is_null($categories)) {
      if (!$this->set_categories($categories)) {
        return FALSE;
      }
    }

    return TRUE;
  }

  /**
   * Assign a title to the RSS channel.
   *
   * This is the title of the RSS feed.
   *
   * @param string $title
   *   The RSS feed channel title.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_title($title) {
    if (!is_string($title)) {
      cf_error::invalid_string('title');
      return FALSE;
    }

    $checked = strip_tags($title);
    $this->title = htmlspecialchars($checked, ENT_XML1, 'UTF-8');
    return TRUE;
  }

  /**
   * Unassign a title from the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_title() {
    $this->title = NULL;
    return TRUE;
  }

  /**
   * Get the value of the RSS feed title.
   *
   * @return null|string
   *   The value of the RSS feed title.
   */
  public function get_title() {
    return $this->title;
  }

  /**
   * Assign a link to the RSS channel.
   *
   * This is a link to the website that provides this feed.
   * This should not be a link to the feed itself.
   *
   * @param string $link
   *   The RSS feed channel link.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_link($link) {
    if (!is_string($link)) {
      cf_error::invalid_string('link');
      return FALSE;
    }

    $checked = filter_var($link, FILTER_SANITIZE_URL);
    if ($checked === FALSE) {
      cf_error::invalid_variable('link', "Not a valid URL.");
      return FALSE;
    }

    $this->link = $checked;
    return TRUE;
  }

  /**
   * Unassign a link from the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_link() {
    $this->link = NULL;
    return TRUE;
  }

  /**
   * Get the value of the RSS feed link.
   *
   * @return null|string
   *   The value of the RSS feed link.
   */
  public function get_link() {
    return $this->link;
  }

  /**
   * Assign a description to the RSS channel.
   *
   * This is the description of the RSS feed.
   *
   * @param string $description
   *   The RSS feed channel description.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_description($description) {
    if (!is_string($description)) {
      cf_error::invalid_string('description');
      return FALSE;
    }

    $this->description = $description;
    return TRUE;
  }

  /**
   * Unassign a description from the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_description() {
    $this->description = NULL;
    return TRUE;
  }

  /**
   * Get the value of the RSS feed description.
   *
   * @return null|string
   *   The value of the RSS feed description.
   */
  public function get_description() {
    return $this->description;
  }

  /**
   * Assign a content to the RSS channel.
   *
   * This is the content of the RSS feed.
   *
   * @param string $content
   *   The RSS feed channel content.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_content($content) {
    if (!is_string($content)) {
      cf_error::invalid_string('content');
      return FALSE;
    }

    $this->content = $content;
    return TRUE;
  }

  /**
   * Unassign a content from the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_content() {
    $this->content = NULL;
    return TRUE;
  }

  /**
   * Get the value of the RSS feed content.
   *
   * @return null|string
   *   The value of the RSS feed content.
   */
  public function get_content() {
    return $this->content;
  }

  /**
   * Assign a last build date to the RSS channel.
   *
   * This is the publish date of the RSS feed.
   *
   * @param string $publish_date
   *   The RSS feed channel publish date.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_publish_date($publish_date) {
    if (!is_numeric($publish_date)) {
      cf_error::invalid_numeric('publish_date');
      return FALSE;
    }

    // consider anything that matches date(0) to be invalid at the cost of not being able to use a date of "1969/12/31 06:00:00 pm".
    if (date("r", 0) == date("r", $publish_date)) {
      cf_error::invalid_variable('publish_date', "Not a valid date.");
      return FALSE;
    }

    $this->publish_date = (int) $publish_date;
    return TRUE;
  }

  /**
   * Unassign a publish date from the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_publish_date() {
    $this->publish_date = NULL;
    return TRUE;
  }

  /**
   * Get the value of the RSS feed publish date.
   *
   * @return null|int
   *   The value of the RSS feed publish_date.
   */
  public function get_publish_date() {
    return $this->publish_date;
  }

  /**
   * Assign a copyright to the RSS channel.
   *
   * This is the copyright of the RSS feed.
   *
   * @param string $copyright
   *   The RSS feed channel copyright.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_copyright($copyright) {
    if (!is_string($copyright)) {
      cf_error::invalid_string('copyright');
      return FALSE;
    }

    $checked = strip_tags($copyright);
    $this->copyright = htmlspecialchars($checked, ENT_XML1, 'UTF-8');
    return TRUE;
  }

  /**
   * Unassign a copyright from the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_copyright() {
    $this->copyright = NULL;
    return TRUE;
  }

  /**
   * Get the value of the RSS feed copyright.
   *
   * @return null|string
   *   The value of the RSS feed copyright.
   */
  public function get_copyright() {
    return $this->copyright;
  }

  /**
   * Assign categories to the RSS channel.
   *
   * This will assign categories to the RSS feed.
   *
   * @param array $categories
   *   The RSS feed channel categories.
   * @param string|null $domain
   *   (optional) The domain is a group in which the category belongs to.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_categories($categories, $domain = NULL) {
    if (!is_array($categories)) {
      cf_error::invalid_array('categories');
      return FALSE;
    }

    if (!is_null($domain) && !is_string($domain)) {
      cf_error::invalid_string('domain');
      return FALSE;
    }

    if (!isset($this->categories[$domain])) {
      $this->categories[$domain] = array();
    }

    $previous_categories = $this->categories[$domain];

    $this->categories[$domain] = array();
    foreach ($categories as $category_name) {
      if ($this->append_category($category_name, $domain) === FALSE) {
        $this->categories[$domain] = $previous_categories;
        return FALSE;
      }
    }

    return TRUE;
  }

  /**
   * Append a category to the RSS channel.
   *
   * This will append a single category to the RSS feed.
   *
   * @param string $category
   *   The RSS feed channel category.
   * @param string|null $domain
   *   (optional) The domain is a group in which the category belongs to.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function append_category($category, $domain = NULL) {
    if (!is_string($category)) {
      cf_error::invalid_string('category');
      return FALSE;
    }

    if (!is_null($domain) && !is_string($domain)) {
      cf_error::invalid_string('domain');
      return FALSE;
    }

    $checked = strip_tags($category);
    $checked = htmlspecialchars($category, ENT_XML1, 'UTF-8');

    if (!isset($this->categories[$domain])) {
      $this->categories[$domain] = array();
    }

    $this->categories[$domain][] = $checked;
    return TRUE;
  }

  /**
   * Unassign categories from the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_categories() {
    $this->categories = array();
    return TRUE;
  }

  /**
   * Get the value of the RSS feed categories.
   *
   * @return array
   *   The value of the RSS feed categories.
   */
  public function get_categories() {
    return $this->categories;
  }

  /**
   * Assign a tab spacing to be applied to all generated elements.
   *
   * @param string $tab
   *   The tab spacing, such as '  '. This may be empty to assign no tab.
   *   Only spaces are allowed (not any whitespace, specifically spaces only).
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_tab($tab) {
    if (!is_string($tab)) {
      cf_error::invalid_string('tab');
      return FALSE;
    }

    if ($tab != "" && !preg_match('/^[ ]+$/', $tab)) {
      cf_error::invalid_variable('tab', "Must only be an empty string or contain spaces.");
      return FALSE;
    }

    $this->tab = $tab;
    return TRUE;
  }

  /**
   * Get the current tab value.
   *
   * @return FALSE|string
   *   The tab value on success, FALSE otherwise.
   */
  public function get_tab() {
    return $this->tab;
  }

  /**
   * Check to see if all minimally required fields are setup.
   *
   * @return bool
   *   TRUE when base, title, link, and description are supplied, FALSE
   *   otherwise.
   */
  public function is_ready() {
    if (is_null($this->title)) {
      return FALSE;
    }

    if (is_null($this->link)) {
      return FALSE;
    }

    if (is_null($this->description)) {
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Get the base markup.
   *
   * This is an internal function and does not validate $tab.
   *
   * @param string $tab
   *   (optional) The tab spacing, such as '  '. This may be empty to assign no tab.
   *   Only spaces are allowed (not any whitespace, specifically spaces only).
   *
   * @return string
   *   The generated markup or FALSE on error.
   */
  protected function pr_get_base($tab = '  ') {
    $markup = $this->tab . $tab . '<title>' . $this->title . '</title>' . "\n";
    $markup .= $this->tab . $tab . '<link>' . $this->link . '</link>' . "\n";
    $markup .= $this->tab . $tab . '<description><![CDATA[' . $this->description . ']]></description>' . "\n";

    if (!is_null($this->content)) {
      $markup .= $this->tab . $tab . '<content:encoded><![CDATA[' . $this->content . ']]></content:encoded>' . "\n";
    }

    if (!is_null($this->publish_date) && $this->publish_date > 0) {
      $markup .= $this->tab . $tab . '<pubDate>' . gmdate(DATE_RFC7231, $this->publish_date) . '</pubDate>' . "\n";
    }

    if (!empty($this->categories)) {
      foreach ($this->categories as $domain => $categories) {
        if (is_null($domain) || strlen($domain) == 0) {
          $domain_attribute = '';
        }
        else {
          $domain_attribute = ' domain="' . $domain . '"';
        }

        foreach ($categories as $category) {
          $markup .= $this->tab . $tab . '<category' . $domain_attribute . '>' . $category . '</category>' . "\n";
        }
      }
    }

    return $markup;
  }
}

/**
 * Class for managing RSS Feed Channels.
 */
class McNeeseFeedsRssChannel extends McNeeseFeedsRssBase {
  private $base = NULL;
  private $source = NULL;
  private $language = NULL;
  private $last_build_date = NULL;
  private $image = array();
  private $items = array();


  /**
   * assign values for the optional base fields.
   *
   * @param string $base
   *   The base URL for relative links.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function assign_required($base = NULL) {
    if (!is_null($base)) {
      if (!$this->set_base($base)) {
        return FALSE;
      }
    }

    return TRUE;
  }

  /**
   * assign values for the optional base fields.
   *
   * @param string $language
   *   The language string, such as 'en-US'.
   * @param string $source
   *   The URL that this feed is derived from.
   * @param bool $link_atom
   *   Enable or disable the atom:link tag.
   * @param int $last_build_date
   *   The last build date timestamp.
   * @param array $image
   *   An array containing the following fields:
   *   - (required) 'url': image url.
   *   - (optional) 'title': the alt text for the image (usually the feed title).
   *   - (optional) 'link': link to the feed.
   *   - (optional) 'width': width of the image (in pixels).
   *   - (optional) 'height': height of the image (in pixels).
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function assign_optional($language = NULL, $source = NULL, $link_atom = NULL, $last_build_date = NULL, $image = NULL, $items = NULL) {
    if (!is_null($language)) {
      if (!$this->set_language($language)) {
        return FALSE;
      }
    }

    if (!is_null($source)) {
      if (!$this->set_source($source)) {
        return FALSE;
      }
    }

    if (!is_null($link_atom)) {
      if (!$this->set_link_atom($link_atom)) {
        return FALSE;
      }
    }

    if (!is_null($last_build_date) && $last_build_date > 0) {
      if (!$this->set_last_build_date($last_build_date)) {
        return FALSE;
      }
    }

    if (!is_null($image)) {
      if (!is_array($image) || empty($image['url'])) {
        return FALSE;
      }

      if (empty($image['title'])) {
        $image['title'] = NULL;
      }

      if (empty($image['link'])) {
        $image['link'] = NULL;
      }

      if (empty($image['width'])) {
        $image['width'] = NULL;
      }

      if (empty($image['height'])) {
        $image['height'] = NULL;
      }

      if (!$this->set_image($image['url'], $image['title'], $image['link'], $image['width'], $image['height'])) {
        return FALSE;
      }
    }

    if (!is_null($items)) {
      if (!$this->set_items($items)) {
        return FALSE;
      }
    }

    return TRUE;
  }

  /**
   * Assign a base url of the RSS channel.
   *
   * This is the url to the feed website to be used as the base for relative
   * links.
   *
   * @param string $base
   *   The RSS feed channel base.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_base($base) {
    if (!is_string($base)) {
      cf_error::invalid_string('base');
      return FALSE;
    }

    $checked = filter_var($base, FILTER_SANITIZE_URL);
    if ($checked === FALSE) {
      cf_error::invalid_variable('base', "Not a valid URL.");
      return FALSE;
    }

    $this->base = $checked;
    return TRUE;
  }

  /**
   * Unassign a base from the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_base() {
    $this->base = NULL;
    return TRUE;
  }

  /**
   * Get the value of the RSS feed base.
   *
   * @return null|string
   *   The value of the RSS feed base.
   */
  public function get_base() {
    return $this->base;
  }

  /**
   * Assign a language to the RSS channel.
   *
   * This is the language of the RSS feed.
   *
   * @param string $language
   *   The RSS feed channel language.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_language($language) {
    if (!is_string($language)) {
      cf_error::invalid_string('language');
      return FALSE;
    }

    $checked = strip_tags($language);
    $this->language = htmlspecialchars($checked, ENT_XML1, 'UTF-8');
    return TRUE;
  }

  /**
   * Unassign a language from the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_language() {
    $this->language = NULL;
    return TRUE;
  }

  /**
   * Get the value of the RSS feed language.
   *
   * @return null|string
   *   The value of the RSS feed language.
   */
  public function get_language() {
    return $this->language;
  }

  /**
   * Assign a source url of the RSS channel.
   *
   * This is the url to the feed website to be used as the source for relative
   * links.
   *
   * @param string $source
   *   The RSS feed channel source.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_source($source) {
    if (!is_string($source)) {
      cf_error::invalid_string('source');
      return FALSE;
    }

    $checked = filter_var($source, FILTER_SANITIZE_URL);
    if ($checked === FALSE) {
      cf_error::invalid_variable('source', "Not a valid URL.");
      return FALSE;
    }

    $this->source = $checked;
    return TRUE;
  }

  /**
   * Unassign a source from the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_source() {
    $this->source = NULL;
    return TRUE;
  }

  /**
   * Get the value of the RSS feed source.
   *
   * @return null|string
   *   The value of the RSS feed source.
   */
  public function get_source() {
    return $this->source;
  }

  /**
   * Assign a link to the RSS channel.
   *
   * This is an 'atom' link to the url that provides this feed.
   * This requires the source field to be defined.
   *
   * @param bool $link_atom
   *   TRUE to enable use of atom:link, FALSE to ignore.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   *
   * @see: set_source()
   */
  public function set_link_atom($link_atom) {
    if (!is_bool($link_atom)) {
      cf_error::invalid_bool('link_atom');
      return FALSE;
    }

    $this->link_atom = $link_atom;
    return TRUE;
  }

  /**
   * Unassign a link_atom from the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_link_atom() {
    $this->link_atom = FALSE;
    return TRUE;
  }

  /**
   * Get the value of the RSS feed link_atom.
   *
   * @return null|string
   *   The value of the RSS feed link_atom.
   */
  public function get_link_atom() {
    return $this->link_atom;
  }

  /**
   * Assign a last build date to the RSS channel.
   *
   * This is the last build date of the RSS feed.
   *
   * @param int $build_date
   *   The RSS feed channel build_date.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_last_build_date($last_build_date) {
    if (!is_int($last_build_date)) {
      cf_error::invalid_int('last_build_date');
      return FALSE;
    }

    // consider anything that matches date(0) to be invalid at the cost of not being able to use a date of "1969/12/31 06:00:00 pm".
    if (date("r", 0) == date("r", $last_build_date)) {
      cf_error::invalid_variable('last_build_date', "Not a valid date.");
      return FALSE;
    }

    $this->last_build_date = $last_build_date;
    return TRUE;
  }

  /**
   * Unassign a last build date from the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_last_build_date() {
    $this->last_build_date = NULL;
    return TRUE;
  }

  /**
   * Get the value of the RSS feed last_build_date.
   *
   * @return null|int
   *   The value of the RSS feed last_build_date.
   */
  public function get_last_build_date() {
    return $this->last_build_date;
  }

  /**
   * Assign a image to the RSS channel.
   *
   * This is the image of the RSS feed.
   *
   * @param string $url
   *   The url to the RSS feed image.
   * @param null|string $title
   *   The title of the image. This is required. If NULL, then the title of the
   *   RSS feed will be used.
   * @param null|string $link
   *   The link associated with the image. This is required. If NULL, then the
   *   link of the RSS feed will be used. This should be a link pointing to the
   *   feed website.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_image($url, $title = NULL, $link = NULL, $width = NULL, $height = NULL) {
    if (!is_string($url)) {
      cf_error::invalid_string('url');
      return FALSE;
    }

    $checked = filter_var($url, FILTER_SANITIZE_URL);
    if ($checked === FALSE) {
      cf_error::invalid_variable('url', "Not a valid URL.");
      return FALSE;
    }

    $image = array();
    $image['url'] = $checked;

    if (is_null($title)) {
      $image['title'] = NULL;
    }
    else {
      $checked = strip_tags($title);
      $image['title'] = htmlspecialchars($title, ENT_XML1, 'UTF-8');
    }

    if (is_null($link)) {
      $image['link'] = NULL;
    }
    else {
      $checked = filter_var($link, FILTER_SANITIZE_URL);
      if ($checked === FALSE) {
        cf_error::invalid_variable('link', "Not a valid URL.");
        return FALSE;
      }

      $image['link'] = $checked;
    }

    if (!is_null($width)) {
      if (!is_int($width)) {
        cf_error::invalid_int('width');
        return FALSE;
      }

      $image['width'] = $width;
    }

    if (!is_null($height)) {
      if (!is_int($height)) {
        cf_error::invalid_int('height');
        return FALSE;
      }

      $image['height'] = $height;
    }

    $this->image = $image;
    return TRUE;
  }

  /**
   * Unassign a image from the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_image() {
    $this->image = array();
    return TRUE;
  }

  /**
   * Get the value of the RSS feed image.
   *
   * @return array
   *   The value of the RSS feed image.
   */
  public function get_image() {
    return $this->image;
  }

  /**
   * Assign items to the RSS channel.
   *
   * This will assign items to the RSS feed.
   *
   * @param array $items
   *   The RSS feed channel items.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_items($items) {
    if (!is_array($items)) {
      cf_error::invalid_array('items');
      return FALSE;
    }

    $previous_items = $this->items;

    $this->unset_items();
    foreach ($items as $item) {
      if ($this->append_item($item) === FALSE) {
        $this->items = $previous_items;
        return FALSE;
      }
    }

    return TRUE;
  }

  /**
   * Unassign a items from the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_items() {
    $this->items = array();
    return TRUE;
  }

  /**
   * Get the value of the RSS feed items.
   *
   * @return array
   *   The value of the RSS feed items.
   */
  public function get_items() {
    return $this->items;
  }

  /**
   * Assign an item to the RSS channel.
   *
   * This will append a single item to the RSS feed.
   *
   * @param McNeeseFeedsRssItem $item
   *   An RSS feed item.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function append_item($item) {
    if (!is_object($item) || get_class($item) != 'McNeeseFeedsRssItem') {
      cf_error::invalid_object('item');
      return FALSE;
    }

    $this->items[] = $item;
    return TRUE;
  }

  /**
   * Check to see if all minimally required fields are setup.
   *
   * @return bool
   *   TRUE when base, title, link, and description are supplied, FALSE
   *   otherwise.
   */
  public function is_ready() {
    if (is_null($this->base)) {
      return FALSE;
    }

    return parent::is_ready();
  }

  /**
   * Returns the generated RSS feed string.
   *
   * @param bool|string
   *   Generated RSS feed string on success, FALSE otherwise.
   */
  public function get_feed() {
    if (!$this->is_ready()) {
      return FALSE;
    }

    $tab = $this->tab;

    $feed = $tab . '<?xml version="1.0" encoding="utf-8" ?>' . "\n";
    $feed .= $tab . '<rss version="2.0" xml:base="' . $this->base . '" xmlns:atom="http://www.w3.org/2005/Atom" xmlns:content="http://purl.org/rss/1.0/modules/content/" xmlns:dc="http://purl.org/dc/elements/1.1/">' . "\n";

    $tab .= '  ';
    $feed .= $tab . '<channel>' . "\n";

    $tab = $this->tab . '    ';

    $feed .= $this->pr_get_base($tab);

    if (!is_null($this->link_atom) && !is_null($this->source)) {
      $feed .= $tab . '<atom:link href="' . $this->source . '" rel="self" type="application/rss+xml" />' . "\n";
    }

    if (!is_null($this->last_build_date) && $this->last_build_date > 0) {
      $feed .= $tab . '<lastBuildDate>' . gmdate(DATE_RFC7231, $this->last_build_date) . '</lastBuildDate>' . "\n";
    }

    if (!empty($this->image)) {
      $sub_tab = $tab . '  ';

      $feed .= $tab . '<image>' . "\n";
      $feed .= $sub_tab . '<url>' . $this->image['url'] . '</url>' . "\n";
      $feed .= $sub_tab . '<title>' . $this->image['title'] . '</title>' . "\n";
      $feed .= $sub_tab . '<link>' . $this->image['link'] . '</link>' . "\n";

      if (!empty($this->image['width'])) {
        $feed .= $sub_tab . '<width>' . $this->image['width'] . '</width>' . "\n";
      }

      if (!empty($this->image['height'])) {
        $feed .= $sub_tab . '<height>' . $this->image['height'] . '</height>' . "\n";
      }

      $feed .= $tab . '</image>' . "\n";
    }

    if (!is_null($this->language)) {
      $feed .= $this->tab . $tab . '<language>' . $this->language . '</language>' . "\n";
    }

    if (!empty($this->items)) {
      $sub_tab = $tab . '  ';

      $source = NULL;
      if (!empty($this->source)) {
        $source = $this->source;
      }

      foreach ($this->items as $item) {
        $item->set_tab($sub_tab);

        if (!is_null($source)) {
          $item->set_source($source);
        }

        $built_item = $item->get_item();
        if ($built_item !== FALSE) {
          $feed .= $built_item;
        }
      }
    }

    $tab = $this->tab;
    $tab .= '  ';
    $feed .= $tab . '</channel>' . "\n";

    $tab = $this->tab;
    $feed .= $tab . '</rss>' . "\n";

    return $feed;
  }
}

/**
 * Class for managing RSS Feed Items.
 */
class McNeeseFeedsRssItem extends McNeeseFeedsRssBase {
  private $unique_id = NULL;
  private $permanent_link = NULL;
  private $creator = NULL;
  private $enclosures = array();
  private $source = NULL;


  /**
   * assign values for the optional base fields.
   *
   * @param string $unique_id
   *   A unique identifier for the item.
   * @param bool $permanent_link
   *   TRUE when the $unique_id is a URL. FALSE when the id is not a URL.
   * @param string $creator
   *   A name or e-mail address of the creator.
   * @param array $enclosures
   *   An array of files with the following structure for each file entry:
   *     'url': URL of the file.
   *     'length': File size.
   *     'type': Mime type of the file.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function assign_optional($unique_id = NULL, $permanent_link = NULL, $creator = NULL, $enclosures = NULL) {
    if (!is_null($unique_id)) {
      if (is_null($permanent_link)) {
        if (!$this->set_unique_id($unique_id)) {
          return FALSE;
        }
      }
      else {
        if (!$this->set_unique_id($unique_id, $permanent_link)) {
          return FALSE;
        }
      }
    }

    if (!is_null($creator)) {
      if (!$this->set_creator($creator)) {
        return FALSE;
      }
    }

    if (!is_null($enclosures)) {
      if (!$this->set_enclosures($enclosures)) {
        return FALSE;
      }
    }

    return TRUE;
  }

  /**
   * Assign a unique_id to the RSS item.
   *
   * This is the url to the feed website to be used as the base for relative
   * links.
   *
   * @param string $unique_id
   *   The RSS feed channel unique_id.
   * @param bool $permanent_link
   *   Set to TRUE if the unique id is a parmenent link URL, FALSE otherwise.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_unique_id($unique_id, $permanent_link = FALSE) {
    if (!is_string($unique_id)) {
      cf_error::invalid_string('unique_id');
      return FALSE;
    }

    if (!is_bool($permanent_link)) {
      cf_error::invalid_bool('permanent_link');
      return FALSE;
    }

    $checked = strip_tags($unique_id);
    $checked = htmlspecialchars($checked, ENT_XML1, 'UTF-8');
    if ($permanent_link) {
      $checked = filter_var($unique_id, FILTER_SANITIZE_URL);
      if ($checked === FALSE) {
        cf_error::invalid_variable('unique_id', "Not a valid URL.");
        return FALSE;
      }
    }

    $this->unique_id = $checked;
    $this->permanent_link = $permanent_link;
    return TRUE;
  }

  /**
   * Unassign a unique id for the RSS feed item.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_unique_id() {
    $this->unique_id = NULL;
    $this->permanent_link = FALSE;
    return TRUE;
  }

  /**
   * Get the unique id value for the RSS feed item.
   *
   * @return null|string
   *   The unique id of the RSS feed item.
   */
  public function get_unique_id() {
    return $this->unique_id;
  }

  /**
   * Get the permanent link value for the RSS feed item.
   *
   * @return null|bool
   *   The permanent link status of the unique id of the RSS feed item.
   */
  public function get_permanent_link() {
    return $this->permanent_link;
  }

  /**
   * Assign a creator to the RSS item.
   *
   * Do not set this if the author/creator is intended to be private.
   *
   * @param string $creator
   *   Name or e-mail of the creator associated with the RSS Item.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_creator($creator) {
    if (!is_string($creator)) {
      cf_error::invalid_string('creator');
      return FALSE;
    }

    $this->creator = $creator;
    return TRUE;
  }

  /**
   * Unassign a creator from the RSS item.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_creator() {
    $this->creator = NULL;
    return TRUE;
  }

  /**
   * Get the value of the RSS feed creator.
   *
   * @return null|string
   *   The value of the RSS feed creator.
   */
  public function get_creator() {
    return $this->creator;
  }

  /**
   * Assign enclosures to the RSS item.
   *
   * This will assign enclosures to the RSS feed item.
   *
   * @param array $enclosures
   *   The RSS feed item enclosures.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_enclosures($enclosures) {
    if (!is_array($enclosures)) {
      cf_error::invalid_array('enclosures');
      return FALSE;
    }

    $previous_enclosures = $this->enclosures;

    $this->unset_enclosures();
    foreach ($enclosures as $enclosure) {
      if (empty($enclosure['url']) || empty($enclosure['length']) || empty($enclosure['type'])) {
        $this->enclosures = $previous_enclosures;
        return FALSE;
      }

      if ($this->append_enclosure($enclosure['url'], $enclosure['length'], $enclosure['type']) === FALSE) {
        $this->enclosures = $previous_enclosures;
        return FALSE;
      }
    }

    return TRUE;
  }

  /**
   * Append a enclosure to the RSS channel.
   *
   * This will append a single enclosure to the RSS feed.
   *
   * @param string $url
   *   The URL string that contains the file.
   * @param int $length
   *   The file size.
   * @param string $type
   *   The file mime type.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function append_enclosure($url, $length, $type) {
    if (!is_string($url)) {
      cf_error::invalid_string('url');
      return FALSE;
    }

    if (!is_int($length)) {
      cf_error::invalid_int('length');
      return FALSE;
    }

    if (!is_string($type)) {
      cf_error::invalid_string('type');
      return FALSE;
    }

    $enclosure = array();

    $checked = filter_var($url, FILTER_SANITIZE_URL);
    if ($checked === FALSE) {
      cf_error::invalid_variable('url', "Not a valid URL.");
      return FALSE;
    }

    $enclosure['url'] = $checked;
    $enclosure['size'] = $length;

    $checked = strip_tags($type);
    $enclosure['type'] = htmlspecialchars($checked, ENT_XML1, 'UTF-8');

    $this->enclosures[] = $enclosure;
    return TRUE;
  }

  /**
   * Unassign enclosures from the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_enclosures() {
    $this->enclosures = array();
    return TRUE;
  }

  /**
   * Get the value of the RSS feed enclosures.
   *
   * @return array
   *   The value of the RSS feed enclosures.
   */
  public function get_enclosures() {
    return $this->enclosures;
  }

  /**
   * Assign the source url of the RSS channel.
   *
   * This is the url to the feed website to be used as the source for relative
   * links.
   *
   * This is intended to be called directly by the McNeeseFeedsRssChannel object.
   *
   * @param string $source
   *   The RSS feed channel source.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function set_source($source) {
    if (!is_string($source)) {
      cf_error::invalid_string('source');
      return FALSE;
    }

    $checked = filter_var($source, FILTER_SANITIZE_URL);
    if ($checked === FALSE) {
      cf_error::invalid_variable('source', "Not a valid URL.");
      return FALSE;
    }

    $this->source = $checked;
    return TRUE;
  }

  /**
   * Unassign the source URL of the RSS channel.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function unset_source() {
    $this->source = NULL;
    return TRUE;
  }

  /**
   * Get the value of the RSS feed source.
   *
   * @return null|string
   *   The value of the RSS feed source.
   */
  public function get_source() {
    return $this->source;
  }

  /**
   * Check to see if all minimally required fields are setup.
   *
   * @return bool
   *   TRUE when base, title, link, and description are supplied, FALSE
   *   otherwise.
   */
  public function is_ready() {
    if (is_null($this->title)) {
      return FALSE;
    }

    if (is_null($this->link)) {
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Returns the generated RSS feed item string.
   *
   * @param bool|string
   *   Generated RSS feed item string on success, FALSE otherwise.
   */
  public function get_item() {
    if (!$this->is_ready()) {
      return FALSE;
    }

    $tab = $this->tab;

    $item = $tab . '<item>' . "\n";

    $tab = $this->tab . '  ';

    $item .= $this->pr_get_base();

    if (!empty($this->unique_id)) {
      $item .= $tab . '<guid isPermaLink=';

      if ($this->permanent_link === TRUE) {
        $item .= '"true">';
      }
      else {
        $item .= '"false">';
      }

      $item .= $this->unique_id;

      if ($this->permanent_link === FALSE) {
        global $base_url;
        global $base_path;
        $item .= " on " . htmlspecialchars($base_url . $base_path, ENT_XML1, 'UTF-8');
      }

      $item .= '</guid>' . "\n";
    }

    if (!is_null($this->creator)) {
      $item .= $tab . '<dc:creator>' . $this->creator . '</dc:creator>' . "\n";
    }

    if (!empty($this->enclosures)) {
      foreach ($this->enclosures as $enclosure) {
        $item .= $tab . '<enclosure url="' . $enclosure['url'] . '" length="' . $enclosure['length'] . '" type="' . $enclosure['type'] . ' />' . "\n";
      }
    }

    $tab = $this->tab;
    $item .= $tab . '</item>' . "\n";

    return $item;
  }
}
