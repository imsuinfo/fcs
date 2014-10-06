<?php

/**
 * @file
 * Provides base stream wrapper class for mcneese_file_db.
 *
 * Requires file_db_local_name variable to be defined in settings.php.
 * Requires file_db to be defined in $databases array of settings.php.
 */

/**
 * @addtogroup mcneese_file_db
 * @{
 */

abstract class mcneese_file_db_stream_wrapper implements DrupalStreamWrapperInterface {
  static protected $directoryless = TRUE;

  protected $file_meta = NULL;
  protected $user = 0;
  protected $uri = NULL;
  protected $sri = NULL;

  protected $tmp_filename = NULL;
  protected $tmp_checksum = NULL;
  protected $tmp_id = NULL;
  protected $tmp_fid = NULL;
  protected $tmp_by = NULL;

  protected $stream = NULL;
  protected $stream_blocks = 0;
  protected $stream_position = 0;
  protected $stream_last_read = 0;
  protected $stream_last_write = 0;
  protected $stream_write_buffer = NULL;
  protected $stream_fhandle = NULL;
  protected $stream_fname = NULL;
  protected $stream_fid = 0;

  protected $image_style = NULL;
  protected $image_style_uri = NULL;
  protected $image_style_by = NULL;
  protected $image_style_id = NULL;

  protected $read_only = FALSE;
  protected $slow_build = TRUE;


  /**
   * Implements getUri().
   */
  public function getUri() {
    if (empty($this->uri)) {
      return $this->get_scheme() . '://';
    }

    return $this->uri;
  }

  /**
   * Implements getExternalUrl().
   *
   * Return the HTML URI of a dynamic file.
   */
  public function getExternalUrl() {
    $built = $this->build_uri();

    if ($built === FALSE) {
      return FALSE;
    }

    $parts = explode('://', $built['sri'], 2);

    if (empty($parts[1])) {
      return FALSE;
    }

    if (empty($this->image_style_uri)) {
      global $base_path;

      return $base_path . $this->get_external() . '/' . $parts[1];
    }

    $wrapper = file_stream_wrapper_get_instance_by_uri($this->image_style_uri);

    if (is_object($wrapper)) {
      return $wrapper->getExternalUrl();
    }

    return FALSE;
  }

  /**
   * Implements getLocalPath().
   *
   * Return the HTML URI of a dynamic file.
   */
  public function getLocalPath($uri = NULL) {
    if (!is_null($uri)) {
      $this->setUri($uri);
      $parsed = $this->parse_uri();

      if ($parsed === FALSE) {
        return FALSE;
      }
    }

    $built = $this->build_uri();

    if ($built === FALSE) {
      return FALSE;
    }

    $parts = explode('://', $built['uri'], 2);

    if (empty($parts[1])) {
      return FALSE;
    }

    return $this->get_external() . '/' . $parts[1];
  }

  /**
   * Implements chmod().
   */
  public function chmod($mode) {
    // unsupported
    return TRUE;
  }

  /**
   * Implements getTarget().
   */
  public function getTarget($uri = NULL) {
    if (!is_null($uri)) {
      $this->setUri($uri);
      $parsed = $this->parse_uri();

      if ($parsed === FALSE) return FALSE;
    }

    $built = $this->build_uri();

    if (empty($this->image_style) || empty($this->image_style_uri)) {
      if ($built === FALSE) {
        return FALSE;
      }

      $parts = explode('://', $built['uri'], 2);

      if (empty($parts[1])) {
        return FALSE;
      }

      return $this->get_external() . '/' . $parts[1];
    }
    else {
      return $this->image_style_uri;
    }

    return FALSE;
  }

  /**
   * Implements realpath().
   */
  public function realpath() {
    $built = $this->build_uri();

    if ($built === FALSE) {
      return FALSE;
    }

    $parts = explode('://', $built['uri'], 2);

    if (empty($parts[1])) {
      return FALSE;
    }

    global $base_path;
    global $base_url;

    return $base_url . $base_path . $this->get_external() . '/' . $parts[1];
  }

  /**
   * Implements stream_lock().
   */
  public function stream_lock($operation) {
    // unsupported
    return FALSE;
  }

  /**
   * Implements stream_eof().
   */
  public function stream_eof() {
    if (empty($this->stream) || empty($this->stream_fid)) {
      return FALSE;
    }

    if ($this->stream_position < $this->stream_blocks) {
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Implements stream_seek().
   */
  public function stream_seek($offset, $whence) {
    if (empty($this->stream) || empty($this->stream_fid)) {
      return FALSE;
    }

    $target_block = floor(abs($offset) / MCNEESE_FILE_DB_BLOCK_SIZE);
    $underflow = (($target_block + 1) * MCNEESE_FILE_DB_BLOCK_SIZE) - $offset;

    if ($whence == SEEK_SET) {
      if ($offset == 0) {
        return TRUE;
      }

      if ($offset < 0) {
        return FALSE;
      }

      if ($target_block >= $this->stream_blocks) {
        $target_block = $this->stream_blocks;
        $underflow = 0;
      }

      $this->stream_position = $target_block;
      $this->stream_last_read = $underflow;
      return TRUE;
    }
    else if ($whence == SEEK_CUR || $whence == SEEK_END) {
      if ($offset == 0) {
        if ($whence == SEEK_END) {
          $this->stream_position = $this->stream_blocks;
          $this->stream_last_read = 0;
        }

        return TRUE;
      }

      if ($offset > 0) {
        if ($whence == SEEK_END) {
          return FALSE;
        }

        $difference = $this->stream_last_read + $underflow;

        if ($difference >= MCNEESE_FILE_DB_BLOCK_SIZE) {
          $target_block++;
          $underflow = MCNEESE_FILE_DB_BLOCK_SIZE - $difference;
        }

        $new_position = $this->stream_position + $target_block;

        if ($new_position > $this->stream_blocks) {
          return FALSE;
        }

        if ($new_position == $this->stream_blocks && $underflow > 0) {
          return FALSE;
        }

        $this->stream_position = $new_position;
        $this->stream_last_read = $underflow;
        return TRUE;
      }
      else {
        $difference = $this->stream_last_read - $underflow;

        if ($difference < 0) {
          $target_block++;
          $underflow = MCNEESE_FILE_DB_BLOCK_SIZE - $difference;
        }
        else {
          $underflow = $difference;
        }

        $difference = $this->stream_position - $target_block;

        if ($difference < 0) {
          return FALSE;
        }

        $this->stream_position = $difference;
        $this->stream_last_read = $underflow;
        return TRUE;
      }
    }

    return FALSE;
  }

  /**
   * Implements stream_flush().
   */
  public function stream_flush() {
    // unsupported
    return FALSE;
  }

  /**
   * Implements stream_tell().
   */
  public function stream_tell() {
    if (empty($this->stream) || empty($this->stream_fid)) {
      return FALSE;
    }

    return ($this->stream_position * $this->stream_blocks) + $this->stream_last_read;
  }

  /**
   * Implements stream_metadata().
   */
  public function stream_metadata($path, $option, $var) {
    // @todo
    return FALSE;
  }

  /**
   * Implements mkdir().
   */
  public function mkdir($uri, $mode, $options) {
    // unsupported, but return TRUE and pretend.
    return TRUE;
  }

  /**
   * Implements rmdir().
   */
  public function rmdir($uri, $options) {
    // unsupported, but return TRUE and pretend.
    return TRUE;
  }

  /**
   * Implements dirname().
   */
  public function dirname($uri = NULL) {
    if (!is_null($uri)) {
      $this->setUri($uri);
      $parsed = $this->parse_uri();

      if ($parsed === FALSE) return '';
    }

    $built = $this->build_uri();

    if ($built == FALSE) {
      return FALSE;
    }

    return $built['sri'];
  }

  /**
   * Implements dir_opendir().
   */
  public function dir_opendir($uri, $options) {
    // unsupported
    return FALSE;
  }

  /**
   * Implements dir_readdir().
   */
  public function dir_readdir() {
    // unsupported
    return FALSE;
  }

  /**
   * Implements dir_rewinddir().
   */
  public function dir_rewinddir() {
    // unsupported
    return FALSE;
  }

  /**
   * Implements dir_closedir().
   */
  public function dir_closedir() {
    // unsupported
    return FALSE;
  }

  /**
   * Implements getDirectoryPath().
   */
  public function getDirectoryPath() {
    // unsupported
    return '';
  }

  /**
   * Wrapper function to load the current scheme string.
   *
   * @return string
   *   The scheme string.
   */
  abstract public function get_scheme();

  /**
   * Wrapper function to load the current external string.
   *
   * @return string
   *   The external string.
   */
  abstract public function get_external();

  /**
   * Wrapper function to load the current path by hash string.
   *
   * @return string
   *   The path by hash string.
   */
  abstract public function get_path_by_hash();

  /**
   * Wrapper function to load the current path by its numeric id.
   *
   * @return string
   *   The path by id string.
   */
  abstract public function get_path_by_id();

  /**
   * Wrapper function to load the current path by its drupal file id.
   *
   * @return string
   *   The path by id string.
   */
  abstract public function get_path_by_fid();

  /**
   * Wrapper function to load the associated drupal core scheme.
   *
   * @return string
   *   The drupal core scheme, such as 'public' or 'private'.
   */
  abstract public function get_core();

  /**
   * Wrapper function to load the association table name.
   *
   * @return string
   *   The name of a table such as: mcneese_file_db_unrestricted.
   */
  abstract public function get_association();

  /**
   * Loads the file meta if a valid checksum is supplied.
   *
   * @param string $checksum
   *   use the specified checksum to load the file meta.
   * @param bool $assign
   *   (optional) When TRUE, save the file meta object in $this->file_meta.
   *
   * @return object|FALSE
   *   A file_meta object as returned by the database.
   */
  protected function load_file_meta($checksum, $assign = TRUE) {
    if (empty($checksum)) {
      return FALSE;
    }

    // use existing data if possible
    if (!empty($this->file_meta) && $this->file_meta->checksum == $checksum) {
      return $this->file_meta;
    }

    $file_meta = FALSE;
    $scheme = $this->get_scheme();
    $cached_file_meta = &drupal_static('load_file_meta_' . $scheme, array());

    $shortsum = substr($checksum, 0, MCNEESE_FILE_DB_SHORT_SUM_SIZE);

    if (isset($cached_file_meta['checksum'][$shortsum])) {
      $id = $cached_file_meta['checksum'][$shortsum];

      if (isset($cached_file_meta['id'][$id])) {
        $file_meta = $cached_file_meta['id'][$id];
      }
    }

    if ($file_meta === FALSE) {
      $files = (array) mcneese_file_db_load_available_files(array('checksum' => $checksum, 'association' => $this->get_association()));

      if (!empty($files)) {
        $file_meta = array_pop($files);
      }
    }

    if ($assign) {
      unset($this->file_meta);
      $this->file_meta = NULL;

      if ($file_meta !== FALSE) {
        $this->file_meta = $file_meta;
      }
    }

    if ($file_meta !== FALSE) {
      if (!isset($cached_file_meta['id'][$file_meta->id])) {
        $cached_file_meta['id'][$file_meta->id] = $file_meta;
      }

      if (!isset($cached_file_meta['checksum'][$shortsum])) {
        $cached_file_meta['checksum'][$shortsum] = $file_meta->id;
      }
    }

    return $file_meta;
  }

  /**
   * Loads the file meta if a valid id is supplied.
   *
   * @param string $id
   *   use the specified id to load the file meta.
   * @param bool $assign
   *   (optional) When TRUE, save the file meta object in $this->file_meta.
   *
   * @return object|FALSE
   *   A file_meta object as returned by the database.
   */
  protected function load_file_meta_by_id($id, $assign = TRUE) {
    if (empty($id)) {
      return FALSE;
    }

    // use existing data if possible
    if (!empty($this->file_meta) && $this->file_meta->id == $id) {
      return $this->file_meta;
    }

    $file_meta = FALSE;
    $scheme = $this->get_scheme();
    $cached_file_meta = &drupal_static('load_file_meta_' . $scheme, array());

    if (isset($cached_file_meta['id'][$id])) {
      $file_meta = $cached_file_meta['id'][$id];
    }

    if ($file_meta === FALSE) {
      $files = (array) mcneese_file_db_load_available_files(array('id' => $id, 'association' => $this->get_association()));

      if (!empty($files)) {
        $file_meta = array_pop($files);
      }
    }

    if ($assign) {
      unset($this->file_meta);
      $this->file_meta = NULL;

      if ($file_meta !== FALSE) {
        $this->file_meta = $file_meta;
      }
    }

    if ($file_meta !== FALSE) {
      if (!isset($cached_file_meta['id'][$id])) {
        $cached_file_meta['id'][$id] = $file_meta;
      }
    }

    return $file_meta;
  }

  /**
   * Loads the file meta if a valid drupal file id is supplied.
   *
   * @param string $fid
   *   use the specified drupal file id to load the file meta.
   * @param bool $assign
   *   (optional) When TRUE, save the file meta object in $this->file_meta.
   *
   * @return object|FALSE
   *   A file_meta object as returned by the database.
   */
  protected function load_file_meta_by_fid($fid, $assign = TRUE) {
    if (empty($fid) || !is_numeric($fid)) {
      return FALSE;
    }

    $file_meta = FALSE;
    $scheme = $this->get_scheme();
    $cached_file_meta = &drupal_static('load_file_meta_' . $scheme, array());

    if (isset($cached_file_meta['fid'][$fid])) {
      $id = $cached_file_meta['fid'][$fid];

      if (isset($cached_file_meta['id'][$id])) {
        $file_meta = $cached_file_meta['id'][$id];
      }
    }

    // load the drupal file database to determine the meta names.
    if ($file_meta === FALSE && is_numeric($fid)) {
      $query = db_select('file_managed', 'fm');
      $query->fields('fm');
      $query->condition('fm.fid', $fid);

      $found = (array) $query->execute()->fetchAssoc();
      if (!empty($found['uri'])) {
        $outer_parts = explode('://', $found['uri'], 2);

        if ($outer_parts[0] == $this->get_scheme()) {
          $filepath = $found['uri'];
          $inner_parts = explode('/', $outer_parts[1]);

          if (isset($inner_parts[0]) && isset($inner_parts[1])) {
            if ($inner_parts[0] == $this->get_path_by_hash()) {
              $file_meta = $this->load_file_meta($inner_parts[1], $assign);
            }
          }
        }
      }
    }

    if ($file_meta !== FALSE) {
      if (!isset($cached_file_meta[$scheme]['id'][$file_meta->id])) {
        $cached_file_meta[$scheme]['id'][$file_meta->id] = $file_meta;
      }

      if (!isset($cached_file_meta[$scheme]['fid'][$fid])) {
        $cached_file_meta[$scheme]['fid'][$fid] = $file_meta->id;
      }
    }

    return $file_meta;
  }

  /**
   * Checks to see if a file by the given checksum exists.
   *
   * @param string $checksum
   *   Checksum to validate against
   *
   * @param bool|string $associated
   *   (optional) If TRUE, then limit query to associated files.
   *   If 'unrestricted', then limit to association with unrestricted files.
   *   If 'restricted', then limit to association with restricted files.
   *   Otherwise do not limit query to associated files.
   *
   * @return bool
   *   TRUE if file exists, FALSE otherwise.
   */
  protected function checksum_exists($checksum, $associated = TRUE) {
    try {
      $file_db = mcneese_file_db_prepare_connection();
      $options = mcneese_file_db_prepare_options();

      if ($file_db === FALSE) {
        return FALSE;
      }

      $built = mcneese_file_db_build_query_by_checksum($checksum, $associated);
      $results = $file_db->queryRange($built['query'], 0, 1, $built['arguments'], $options);

      return $results->rowCount() != 0;
    }
    catch (Exception $e) {
      cf_error::on_query_execution($e);

      throw $e;
    }

    return FALSE;
  }

  /**
   * Returns the file id for a given checksum.
   *
   * @param string $checksum
   *   Checksum to validate against
   *
   * @param bool|string $associated
   *   (optional) If TRUE, then limit query to associated files.
   *   If 'unrestricted', then limit to association with unrestricted files.
   *   If 'restricted', then limit to association with restricted files.
   *   Otherwise do not limit query to associated files.
   *
   * @return int|bool
   *   File ID if it exists, FALSE otherwise.
   */
  protected function get_checksum_file_id($checksum, $associated = TRUE) {
    try {
      $file_db = mcneese_file_db_prepare_connection();
      $options = mcneese_file_db_prepare_options();

      if ($file_db === FALSE) {
        return FALSE;
      }

      $built = mcneese_file_db_build_query_by_checksum($checksum, $associated);
      $results = $file_db->queryRange($built['query'], 0, 1, $built['arguments'], $options);

      $row = (array) $results->fetchAll();

      if (empty($row)) {
        return FALSE;
      }

      return $row[0]->id;
    }
    catch (Exception $e) {
      cf_error::on_query_execution($e);

      throw $e;
    }

    return FALSE;
  }

  /**
   * Loads the current user id if one is not defined.
   */
  protected function load_user() {
    if ($this->user > 0) {
      return;
    }

    $user = cf_current_user();
    $this->user = $user->uid;
  }

  /***
   * Process a given uri, loading & storing its information.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  protected function parse_uri() {
    if (is_null($this->uri)) {
      return FALSE;
    }

    $outer_parts = explode('://', $this->uri, 2);

    if ($outer_parts[0] != $this->get_scheme()) {
      return FALSE;
    }

    if (count($outer_parts) > 1) {
      // first eliminate all repeated '/', such as 'dbu://a/b/////c/d' which should be read as 'dbu://a/b/c/d'.
      $fixed_uri = preg_replace('@/+@i', '/', $outer_parts[1]);

      $parts = explode('/', $fixed_uri);

      if (count($parts) < 2) {
        return FALSE;
      }

      // A uri may also contain a link to a style path, such as dbu://styles/square_thumbnail/dbu/f/c/01234567/gksu.png .
      if ($parts[0] == MCNEESE_FILE_DB_STYLES) {
        if (empty($parts[1])) {
          return FALSE;
        }

        $this->image_style = $parts[1];
        $this->image_style_uri = NULL;
        $this->image_style_by = NULL;
        $this->image_style_id = NULL;

        if (!isset($parts[2]) || $parts[2] != $this->get_scheme()) {
          return FALSE;
        }

        if (!isset($parts[3])) {
          return FALSE;
        }
        elseif ($parts[3] == $this->get_external()) {
          $parts = array_slice($parts, 4);
        }
        elseif ($parts[3] == $this->get_path_by_hash() || $parts[3] == $this->get_path_by_id || $parts[3] == $this->get_path_by_fid()) {
          $parts = array_slice($parts, 3);
        }
        else {
          return FALSE;
        }

        $this->tmp_by = $parts[0];
        $this->image_style_by = $parts[0];
        $this->image_style_id = $parts[1];
      }
      else {
        $this->tmp_by = $parts[0];
      }

      // allow being passed a path beginning with external path such as: /MCNEESE_FILE_DB_FILE_PATH .
      // doing this allows for modules to find the scheme using only the final url.
      if (empty($parts[0]) && $parts[1] == $this->get_external()) {
        array_shift($parts);
        array_shift($parts);

        if (count($parts) < 2) {
          return FALSE;
        }
      }

      if ($parts[0] == $this->get_path_by_hash() && !empty($parts[1])) {
        if (is_null($this->file_meta)) {
          $this->tmp_checksum = $parts[1];

          if (!empty($parts[2])) {
            $this->tmp_filename = $parts[2];
          }

          return TRUE;
        }

        $this->tmp_checksum = $this->file_meta->checksum;
        $this->tmp_id = $this->file_meta->id;

        // look for the custom filename that overrides the one saved in the database if provided.
        if (empty($parts[2])) {
          $this->tmp_filename = $this->file_meta->filename;

          if (!empty($this->file_meta->extension)) {
            $this->tmp_filename .= '.' . $this->file_meta->extension;
          }
        }
        else {
          $this->tmp_filename = $parts[2];
        }

        return TRUE;
      }

      if ($parts[0] == $this->get_path_by_id() && !empty($parts[1])) {
        if (is_null($this->file_meta)) {
          $this->tmp_id = $parts[1];

          if (!empty($parts[2])) {
            $this->tmp_filename = $parts[2];
          }

          return TRUE;
        }

        $this->tmp_checksum = $this->file_meta->checksum;
        $this->tmp_id = $this->file_meta->id;

        // look for the custom filename that overrides the one saved in the database if provided.
        if (empty($parts[2])) {
          $this->tmp_filename = $this->file_meta->filename;

          if (!empty($this->file_meta->extension)) {
            $this->tmp_filename .= '.' . $this->file_meta->extension;
          }
        }
        else {
          $this->tmp_filename = $parts[2];
        }

        return TRUE;
      }

      if ($parts[0] == $this->get_path_by_fid() && !empty($parts[1])) {
        if (is_null($this->file_meta)) {
          $this->tmp_fid = $parts[1];

          return TRUE;
        }

        $this->tmp_checksum = $this->file_meta->checksum;
        $this->tmp_id = $this->file_meta->id;
        $this->tmp_fid = $parts[1];

        // look for the custom filename that overrides the one saved in the database if provided.
        if (empty($parts[2])) {
          $this->tmp_filename = $this->file_meta->filename;

          if (!empty($this->file_meta->extension)) {
            $this->tmp_filename .= '.' . $this->file_meta->extension;
          }
        }
        else {
          $this->tmp_filename = $parts[2];
        }

        return TRUE;
      }
    }

    return FALSE;
  }

  /**
   * Uses the file data to build the appropriate uri.
   *
   * @param string $method
   *   (optional) Designates the method followed when building the uri.
   *
   * @return array|bool
   *   And array with the following keys:
   *   - uri: The full uri, containing the filename.
   *   - sri: The short uri, without the filename.
   *   FALSE is returned on error.
   */
  protected function build_uri($method = NULL) {
    $built = array();
    $built['uri'] = '';
    $built['sri'] = '';

    if (empty($this->file_meta) && empty($this->uri)) {
      return FALSE;
    }

    // determine method from uri
    if (!empty($this->tmp_by)) {
      $method = $this->tmp_by;
    }
    elseif (is_null($method)) {
      // Give $method a default argument that is determed by a function call.
      $method = $this->get_path_by_hash();
    }

    if (empty($this->file_meta) && $this->slow_build) {
      if (!empty($this->stream_fid)) {
        $this->load_file_meta_by_id($this->stream_fid);
      }
      else if (!empty($this->tmp_id)) {
        $this->load_file_meta_by_id($this->tmp_id);
      }
      else if (!empty($this->tmp_checksum)) {
        $this->load_file_meta($this->tmp_checksum);
      }
    }

    if ($method == $this->get_path_by_hash()) {
      if (is_null($this->file_meta)) {
        if (empty($this->tmp_checksum)) {
          if (!empty($this->tmp_id)) {
            $this->load_file_meta_by_id($this->tmp_id);
          }
          elseif (!empty($this->tmp_fid)) {
            $this->load_file_meta_by_fid($this->tmp_fid);
          }
          else {
            return FALSE;
          }

          if (is_null($this->file_meta)) return FALSE;

          $method_value = substr($this->file_meta->checksum, 0, MCNEESE_FILE_DB_SHORT_SUM_SIZE);
        }
        else {
          $method_value = substr($this->tmp_checksum, 0, MCNEESE_FILE_DB_SHORT_SUM_SIZE);
        }
      }
      else {
        $method_value = substr($this->file_meta->checksum, 0, MCNEESE_FILE_DB_SHORT_SUM_SIZE);
      }
    }
    elseif ($method == $this->get_path_by_id()) {
      if (is_null($this->file_meta)) {
        if (empty($this->tmp_id)) {
          if (!empty($this->tmp_checksum)) {
            $this->load_file_meta_by_checksum($this->tmp_checksum);
          }
          elseif (!empty($this->tmp_fid)) {
            $this->load_file_meta_by_fid($this->tmp_fid);
          }
          else {
            return FALSE;
          }

          if (is_null($this->file_meta)) return FALSE;

          $method_value = $this->file_meta->id;
        }
        else {
          $method_value = $this->tmp_id;
        }
      }
      else {
        $method_value = $this->file_meta->id;
      }
    }
    elseif ($method == $this->get_path_by_fid()) {
      if (empty($this->tmp_fid)) {
        // the drupal file id is not stored in the file_meta, so if it is not already available, it cannot be used.
        return FALSE;
      }
      else {
        $method_value = $this->tmp_fid;
      }
    }
    else {
      return FALSE;
    }

    $built['sri'] = $this->get_scheme() . '://';
    $built['sri'] .= $method . '/';
    $built['sri'] .= $method_value;
    $filename = '';

    if (empty($built['sri'])) return FALSE;

    if (!empty($this->image_style)) {
      $image_source_uri = $this->get_scheme() . '://';
      $image_source_uri .= $method . '/';
      $image_source_uri .= $method_value;

      $this->image_style_uri = $this->get_core() . '://styles/';
      $this->image_style_uri .= $this->image_style . '/';
      $this->image_style_uri .= $this->get_scheme() . '/';
      $this->image_style_uri .= $this->get_external() . '/';

      if (empty($this->image_style_by) || $this->image_style_by == $method) {
        $this->image_style_uri .= $method . '/';
        $this->image_style_uri .= $method_value;
      }
      else {
        $this->image_style_uri .= $this->image_style_by . '/';
        $this->image_style_uri .= $this->image_style_id;
      }
    }

    $extension = '';
    if (!is_null($this->file_meta) && !empty($this->file_meta->extension)) {
      $extension .= '.' . $this->file_meta->extension;
    }

    if (empty($this->tmp_filename)) {
      if (!is_null($this->file_meta) && !empty($this->file_meta->filename)) {
        $filename .= '/' . $this->file_meta->filename . $extension;
      }
    }
    else {
      $filename .= '/' . $this->tmp_filename;
    }

    $built['uri'] = $built['sri'] . $filename;

    if (!empty($this->image_style_uri)) {
      $this->image_style_uri .= $filename;
    }

    return $built;
  }

  /***
   * Creates the image style for images.
   *
   * @param string $image_source_uri
   *   (optional) Source uri of the image.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   */
  public function create_image_style($image_source_uri = NULL) {
    if (empty($this->uri) && !empty($image_source_uri)) {
      $this->setUri($image_source_uri);
    }

    if (empty($this->image_style) || empty($this->image_style_uri) || empty($this->uri)) {
      return FALSE;
    }

    if (empty($this->file_meta) && (empty($this->uri) || empty($this->tmp_checksum))) {
      return FALSE;
    }

    // custom variant of image_style_create_derivative() to properly create the image style.
    $dirname = file_stream_wrapper_get_instance_by_scheme($this->get_core())->dirname($this->image_style_uri);

    if (!file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
      watchdog('image', "Failed to create style directory: %directory", array('%directory' => $dirname), WATCHDOG_ERROR);
      return FALSE;
    }

    if (!$image = image_load($this->uri)) {
      return FALSE;
    }

    $style = image_style_load($this->image_style);

    if (isset($style['effects']) && is_array($style['effects'])) {
      foreach ($style['effects'] as $effect) {
        image_effect_apply($image, $effect);
      }
    }

    if (!image_save($image, $this->image_style_uri)) {
      if (file_exists($this->image_style_uri)) {
        watchdog('image', "Cached image file %destination already exists. There may be an issue with your rewrite configuration.", array('%destination' => $this->image_style_uri), WATCHDOG_ERROR);
      }

      return FALSE;
    }

    return TRUE;
  }

  /**
   * Loads the fast build settings.
   *
   * @return bool
   *   TRUE/FALSE for current fast_build setting.
   */
  public function get_fast_build() {
    return !$this->slow_build;
  }

  /**
   * Sets the fast build settings.
   *
   * This allows for fast processing of URLs.
   * This should be done before any other operation to properly function.
   *
   * @param bool $fast_build
   *   Set to TRUE to prevent database from being loaded.
   */
  public function set_fast_build($fast_build) {
    if (!is_bool($fast_build)) {
      cf_error::invalid_bool('fast_build');
      return;
    }

    $this->slow_build = !$fast_build;
  }
}

/**
 * @} End of '@addtogroup mcneese_file_db'.
 */
