<?php

/**
 * @file
 * Defines McNeese Catalog courses class.
 */

/**
 * @addtogroup mcneese_catalog
 * @{
 */

class mcneese_catalog_class_courses extends mcneese_catalog_class_base {
  const BASE_URL_COURSE = '/preview_course_nopop.php';

  /**
   * Class constuctor.
   */
  public function __construct($catalog_id, $legacy_id) {
    parent::__construct($catalog_id, $legacy_id);

    $this->table          = 'mcneese_catalog_courses';
    $this->path_name      = 'courses';
    $this->cache_name     = 'courses';
    $this->cache_lifetime = '+24 hours';

    $this->select_condition = ' where visible and active';

    $this->list_name = 'course-list';
  }

  /**
   * Process the loaded data.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  protected function process_data() {
    if (empty($this->loaded->{$this->list_name})) {
      $this->loaded = NULL;
      $this->processed = array();
      return TRUE;
    }

    $this->processed = array();
    foreach ($this->loaded->{$this->list_name} as $item) {
      // require the id.
      if (!property_exists($item, 'id') || !is_numeric($item->id)) {
        continue;
      }

      // require the catalog id.
      if (!property_exists($item, 'catalog-id') || !is_numeric($item->{'catalog-id'})) {
        continue;
      }

      // require the legacy id.
      if (!property_exists($item, 'legacy-id') || !is_numeric($item->{'legacy-id'}))  {
        continue;
      }

      // require the name string to exist and be non-empty.
      if (empty($item->name) || !is_string($item->name)) {
        continue;
      }

      // require the prefix to exist and be non-empty.
      if (empty($item->prefix) || !is_string($item->prefix)) {
        continue;
      }

      // require the code to exist and be non-empty.
      if (empty($item->code) || !is_string($item->code)) {
        continue;
      }

      // require the url to exist and be non-empty.
      if (empty($item->url) || !is_string($item->url)) {
        continue;
      }

      // the status object must exists
      if (!property_exists($item, 'status') || !is_object($item->status)) {
        continue;
      }

      // ignore invisible items.
      if (property_exists($item->status, 'visible') && !$item->status->visible) {
        continue;
      }

      // ignore inactive items.
      if (property_exists($item->status, 'active') && !$item->status->active) {
        continue;
      }

      $entry = new stdClass();
      $entry->id = (int) $item->id;
      $entry->catalog_id = $item->{'catalog-id'};
      $entry->legacy_id = (int) $item->{'legacy-id'};

      $entry->name = $item->name;
      $entry->prefix = substr($item->prefix, 0, 32);
      $entry->code = substr($item->code, 0, 64);
      $entry->url = substr($item->url, 0, 256);

      $entry->description = '';
      if (property_exists($item, 'body') && is_string($item->body)) {
        $entry->description = $this->sanitize_markup($item->body, array('hr'));
        $entry->description = $this->cleanup_description($entry->description);
      }

      if (!empty($item->created) && is_string($item->created)) {
        $sanitized_date = strtotime($item->created);
        $entry->created = date('r', $sanitized_date);
        unset($sanitized_date);
      }

      if (!empty($item->modified) && is_string($item->modified)) {
        $sanitized_date = strtotime($item->modified);
        $entry->modified = date('r', $sanitized_date);
        unset($sanitized_date);
      }

      $entry->active = TRUE;
      $entry->visible = TRUE;

      $entry->searchable = TRUE;
      if (property_exists($item->status, 'searchable') && !$item->status->searchable) {
        $entry->searchable = FALSE;
      }

      if (property_exists($item, 'fields') && is_array($item->fields)) {
        $entry->fields = json_encode($item->fields);
      }

      $this->processed[] = $entry;
      unset($entry);
    }

    $this->loaded = NULL;
    return TRUE;
  }

  /**
   * Renders the markup.
   *
   * @param int|null $page
   *   (optional) If NULL, then no pager is used.
   *   If an integer, then this represents the particular page in use for generating a pager.
   * @param int|null $per_page
   *   (optional) $page is defined and is an integer, then this represents the number of items per page.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  protected function render_markup($page = NULL, $per_page = NULL) {
    if (is_null($this->processed)) {
      return FALSE;
    }

    $markup = '';
    if ($this->list_as == self::AS_LIST) {
      $this->markup .= '<ul class="' . $this->get_class_base_name() . '-list list-as_list">';
      foreach ($this->processed as $item) {
        $this->markup .= '<li class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<a href="//' . self::BASE_URL . self::BASE_URL_COURSE . '?catoid=' . ((int) $this->legacy_id) . '&coid=' . ((int) $item->legacy_id) . '" class="' . $this->get_class_base_name() . '-name">' . check_plain($item->prefix) . ' ' . check_plain($item->code) . ' - ' . check_plain($item->name) . '</a>';

        if ($this->use_description) {
          $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->description, 'full_html') . '</div>';
        }
        $this->markup .= '</li>';
      }
      $this->markup .= '</ul>';
    }
    elseif ($this->list_as == self::AS_DIVIDER) {
      $this->markup .= '<div class="' . $this->get_class_base_name() . '-list list-as_divider">';
      foreach ($this->processed as $item) {
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<a href="//' . self::BASE_URL . self::BASE_URL_COURSE . '?catoid=' . ((int) $this->legacy_id) . '&coid=' . ((int) $item->legacy_id) . '" class="' . $this->get_class_base_name() . '-name">' . check_plain($item->prefix) . ' ' . check_plain($item->code) . ' - ' . check_plain($item->name) . '</a>';

        if ($this->use_description) {
          $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->description, 'full_html') . '</div>';
        }
        $this->markup .= '</div>';
      }
      $this->markup .= '</div>';
    }
    elseif ($this->list_as == self::AS_DIVIDER) {
      $this->markup .= '<div class="' . $this->get_class_base_name() . '-list list-as_span">';
      foreach ($this->processed as $item) {
        $this->markup .= '<span class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<a href="//' . self::BASE_URL . self::BASE_URL_COURSE . '?catoid=' . ((int) $this->legacy_id) . '&coid=' . ((int) $item->legacy_id) . '" class="' . $this->get_class_base_name() . '-name">' . check_plain($item->prefix) . ' ' . check_plain($item->code) . ' - ' . check_plain($item->name) . '</a>';

        if ($this->use_description) {
          $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->description, 'full_html') . '</div>';
        }
        $this->markup .= '</span>';
      }
      $this->markup .= '</div>';
    }
    elseif ($this->list_as == self::AS_ACCORDIAN) {
      $this->markup .= '<div class="' . $this->get_class_base_name() . '-list list-as_accordian">';
      foreach ($this->processed as $item) {
        $this->markup .= '<div class="' . $this->get_class_base_name() . '-list_item">';
        $this->markup .= '<a href="//' . self::BASE_URL . self::BASE_URL_COURSE . '?catoid=' . ((int) $this->legacy_id) . '&coid=' . ((int) $item->legacy_id) . '" class="' . $this->get_class_base_name() . '-name">' . check_plain($item->prefix) . ' ' . check_plain($item->code) . ' - ' . check_plain($item->name) . '</a>';

        if ($this->use_description) {
          $this->markup .= '<div class="' . $this->get_class_base_name() . '-description">' . check_markup($item->description, 'full_html') . '</div>';
        }

        $this->markup .= '</div>';
      }
      $this->markup .= '</div>';
    }

    $this->markup .= $this->build_pager($page, $per_page);

    return TRUE;
  }
}

/**
 * @} End of '@addtogroup mcneese_catalog'.
 */
