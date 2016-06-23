<?php

/**
 * @file
 * Provides the stream wrapper: dbu://
 *
 * This class will handle upload & download of unrestricted files.
 * The following paths are supports:
 * - upload: dbu://[filename]
 * - download: dbu://c/[short_checksum]/[filename (optional)]
 *
 * The external url for these files are:
 * - unique download path: http(s)://[website]/f/c/[short_checksum]/[filename]
 *
 * The sha1 checksums of the files can be downloaded via:
 * - unique download path: http(s)://[website]/f/s/[short_checksum]/[filename]
 */

/**
 * @addtogroup mcneese_file_db
 * @{
 */

class mcneese_file_db_unrestricted_stream_wrapper extends mcneese_file_db_stream_wrapper {
  const SCHEME = 'dbu';
  const CORE = 'public';
  const ASSOCIATION= 'mcneese_file_db_unrestricted';


  /**
   * Implements rename().
   */
  public function rename($from_uri, $to_uri) {
    return FALSE;
  }

  /**
   * Implements setUri().
   */
  public function setUri($uri) {
    $this->file_meta = NULL;
    $this->tmp_filename = NULL;
    $this->tmp_checksum = NULL;
    $this->tmp_id = NULL;
    $this->tmp_fid = NULL;
    $this->tmp_by = NULL;
    $this->image_style = NULL;
    $this->image_style_uri = NULL;
    $this->image_style_by = NULL;
    $this->image_style_id = NULL;

    if (empty($uri)) {
      return FALSE;
    }

    $this->uri = urldecode($uri);
    $parsed = $this->parse_uri();

    if ($parsed === FALSE) {
      $this->uri = NULL;
      return FALSE;
    }

    $built = $this->build_uri();

    if ($built === FALSE || empty($built['uri'])) {
      $this->uri = $uri;
    }
    else {
      $this->uri = $built['uri'];
    }

    return TRUE;
  }

  /**
   * Implements stream_close().
   */
  public function stream_close() {
    global $conf;

    if (empty($conf[MCNEESE_FILE_DB_CONF_SERVER_ID])) {
      return FALSE;
    }

    if (!is_null($this->stream_fhandle) && $this->stream_fid != 0) {
      $stat = fstat($this->stream_fhandle);
      fclose($this->stream_fhandle);

      $file_meta = $this->load_file_meta_by_id($this->stream_fid, FALSE);

      // if the checksum does not exist, then the file has not yet been saved.
      if (empty($file_meta->checksum)) {
        $fields = array();
        $fields['size'] = $stat['size'];

        if (!empty($this->tmp_filename)) {
          $filename_parts = explode('.', $this->tmp_filename);

          if (count($filename_parts) > 1) {
            $fields['extension'] = strtolower(array_pop($filename_parts));
          }
          else {
            $fields['extension'] = '';
          }

          $fields['filename'] = implode('.', $filename_parts);
        }

        include_once DRUPAL_ROOT . '/includes/file.mimetypes.inc';
        $mapping = file_mimetype_mapping();

        $fields['mimetype'] = file_get_mimetype($this->stream_fname, $mapping);

        if (empty($fields['extension'])) {
          $mime_id = array_search($fields['mimetype'], $mapping['mimetypes']);

          if ($mime_id === FALSE) {
            $fields['extension'] = '';
          }
          else {
            $fields['extension'] = strtolower($mapping['extensions'][$mime_id]);
          }
        }
        else if ($fields['mimetype'] == 'application/octet-stream') {
          // try to avoid using octet-stream and instead use the extension to derive the mime type.
          if (!empty($mapping['extensions'][$fields['extension']])) {
            $fields['mimetype'] = $mapping['mimetypes'][$mapping['extensions'][$fields['extension']]];
          }
        }

        $fields['checksum'] = hash_file(MCNEESE_FILE_DB_PATH_BY_HASH_ALGORITHM, $this->stream_fname);
        $this->tmp_checksum = $fields['checksum'];

        // if the checksum already exists in the database, then delete the current newly created file.
        if ($this->checksum_exists($this->tmp_checksum, 'unrestricted')) {
          $transaction = mcneese_file_db_transaction();

          try {
            $file_db = mcneese_file_db_prepare_connection();
            $options = mcneese_file_db_prepare_options();

            if ($file_db === FALSE) {
              return FALSE;
            }

            $query = $file_db->delete('mcneese_file_db_files', $options);
            $query->condition('id', $this->stream_fid);
            $query->execute();
          }
          catch (Error $e) {
            $transaction->rollback();
            cf_error::on_query_execution($e);

            throw $e;
          }
          catch (Exception $e) {
            $transaction->rollback();
            cf_error::on_query_execution($e);

            throw $e;
          }

          // flush the transaction
          unset($transaction);
        }
        else {
          $associated_id = $this->get_checksum_file_id($this->tmp_checksum, FALSE);
          $transaction = mcneese_file_db_transaction();

          try {
            $file_db = mcneese_file_db_prepare_connection();
            $options = mcneese_file_db_prepare_options();

            if ($file_db === FALSE) {
              return FALSE;
            }

            // if the file is in the database, but is unassociated, then associated it.
            if ($associated_id === FALSE) {
              $query = $file_db->update('mcneese_file_db_files', $options);
              $query->fields($fields);
              $query->condition('id', $this->stream_fid);

              $write_result = $query->execute();
              $associated_id = $this->stream_fid;
            }

            $query = $file_db->select($this->get_association(), 'assoc', $options);
            $query->fields('assoc', array('file_id'));
            $query->condition('file_id', $associated_id);
            $query->condition('server_id', $conf[MCNEESE_FILE_DB_CONF_SERVER_ID]);
            $query->countQuery();

            $read_executed = $query->execute();
            $read_result = 0;
            if (is_object($read_executed)) {
              $read_result = $read_executed->fetchField();
            }

            if ($read_result == 0) {
              $query = $file_db->insert($this->get_association(), $options);
              $fields = array('file_id' => $associated_id, 'server_id' => $conf[MCNEESE_FILE_DB_CONF_SERVER_ID]);
              $query->fields($fields);

              $write_result = $query->execute();
            }

            $this->load_user();


            $query = $file_db->select('mcneese_file_db_users', 'mfdu', $options);
            $query->fields('mfdu', array('file_id'));
            $query->condition('file_id', $associated_id);
            $query->condition('user_id', $this->user);
            $query->condition('server_id', $conf[MCNEESE_FILE_DB_CONF_SERVER_ID]);
            $query->countQuery();

            $read_executed = $query->execute();
            $read_result = 0;
            if (is_object($read_executed)) {
              $read_result = $read_executed->fetchField();
            }

            if ($read_result == 0) {
              $query = $file_db->insert('mcneese_file_db_users', $options);
              $fields = array('file_id' => $associated_id, 'user_id' => $this->user, 'server_id' => $conf[MCNEESE_FILE_DB_CONF_SERVER_ID]);
              $query->fields($fields);

              $write_result = $query->execute();
            }

            // @todo: add group relations
          }
          catch (Error $e) {
            $transaction->rollback();
            cf_error::on_query_execution($e);

            throw $e;
          }
          catch (Exception $e) {
            $transaction->rollback();
            cf_error::on_query_execution($e);

            throw $e;
          }

          // flush the transaction
          unset($transaction);
        }
      }

        // @todo: step 5: the database should perform integrity checks for file data that has been on the system for, say at least 24 hours, then if the file is still NULL for things like checksum, then

      unset($this->stream_fhandle);
      unlink($this->stream_fname);

      $this->stream_fhandle = NULL;
      $this->stream_fname = NULL;

      $built = $this->build_uri();

      if ($built !== FALSE && !empty($built['uri'])) {
        $this->uri = $built['uri'];
      }

      $user = cf_current_user();
      watchdog('File DB', "The user %name (%uid) has added the file %uri (%fid)", array('%name' => $user->name, '%uid' => $user->uid, '%uri' => $this->uri, '%fid' => $associated_id), WATCHDOG_INFO);
    }

    unset($this->stream);

    $this->stream = NULL;
    $this->stream_blocks = 0;
    $this->stream_position = 0;
    $this->stream_last_read = 0;
    $this->stream_fid = 0;
    $this->user = 0;

    return TRUE;
  }

  /**
   * Implements stream_open().
   */
  public function stream_open($uri, $mode, $options, &$opened_path) {
    // prevent opening of a new stream until the existing one is closed
    // @todo: alternatively, should I close an open stream instead of returning FALSE?
    if (!empty($this->stream)) {
      return FALSE;
    }

    if (empty($this->uri)) {
      $slow_build = $this->slow_build;
      $this->slow_build = TRUE;
      $this->setUri($uri);
      $this->slow_build = $slow_build;
    }

    if (empty($this->file_meta)) {
      if (!empty($this->tmp_id)) {
        $this->load_file_meta_by_id($this->tmp_id);
      }
      elseif (!empty($this->tmp_checksum)) {
        $this->load_file_meta($this->tmp_checksum);
      }
      elseif (!empty($this->tmp_fid)) {
        $this->load_file_meta_by_fid($this->tmp_fid);
      }
    }

    if (empty($this->file_meta)) {
      return TRUE;
    }
    else {
      global $conf;

      if (empty($conf[MCNEESE_FILE_DB_CONF_SERVER_ID])) {
        $this->stream = FALSE;
        return FALSE;
      }

      try {
        $file_db = mcneese_file_db_prepare_connection();
        $options = mcneese_file_db_prepare_options();

        if ($file_db === FALSE) {
          return FALSE;
        }

        $built = array();
        $built['query'] = '';
        $built['arguments'] = array();
        $built['arguments'][':server_id'] = $conf[MCNEESE_FILE_DB_CONF_SERVER_ID];
        $built['arguments'][':file_id'] = $this->file_meta->id;

        $built['query'] = 'SELECT mfdfd.file_id, mfdfd.block FROM {mcneese_file_db_file_data} mfdfd';
        $built['query'] .= ' INNER JOIN {' . check_plain($this->get_association()) . '} mfda ON (mfdfd.file_id = mfda.file_id)';
        $built['query'] .= ' WHERE mfda.server_id = :server_id';
        $built['query'] .= ' AND mfda.file_id = :file_id';

        $this->stream = $file_db->query($built['query'], $built['arguments'], $options);

        if ($this->stream === FALSE) {
          $this->stream_close(); // @todo: is this necessary?
          return FALSE;
        }

        $this->stream_fid = $this->file_meta->id;
        $this->stream_position = 0;
        $this->stream_last_read = 0;
        $this->stream_blocks = $this->stream->rowCount();
      }
      catch (Error $e) {
        cf_error::on_query_execution($e);

        throw $e;
      }
      catch (Exception $e) {
        cf_error::on_query_execution($e);

        throw $e;
      }
    }

    return TRUE;
  }

  /**
   * Implements stream_read().
   */
  public function stream_read($bytes) {
    if (empty($this->stream) || empty($this->stream_fid)) {
      return FALSE;
    }

    $output = '';

    if ($bytes == 0) {
      return $output;
    }

    global $conf;

    if (empty($conf[MCNEESE_FILE_DB_CONF_SERVER_ID])) {
      return FALSE;
    }

    // determine the number of blocks to read.
    $offset = $bytes + $this->stream_last_read;
    $blocks = ceil($offset / MCNEESE_FILE_DB_BLOCK_SIZE);
    $underflow = ($blocks * MCNEESE_FILE_DB_BLOCK_SIZE) - $offset;
    $max_blocks = $this->stream_position + $blocks;

    if ($max_blocks > $this->stream_blocks) {
      $max_blocks = $this->stream_blocks;
    }

    $file_db = mcneese_file_db_prepare_connection();
    $options = mcneese_file_db_prepare_options();

    if ($file_db === FALSE) {
      return FALSE;
    }

    $query = 'SELECT mfdfd.data, mfdfd.block FROM {mcneese_file_db_file_data} mfdfd';
    $query .= ' INNER JOIN {' . check_plain($this->get_association()) . '} mfda ON (mfdfd.file_id = mfda.file_id)';
    $query .= ' WHERE mfda.server_id = :server_id';
    $query .= ' AND mfda.file_id = :file_id';
    $query .= ' AND mfdfd.block >= :stream_position';
    $query .= ' AND mfdfd.block < :max_blocks';
    $query .= ' ORDER BY mfdfd.block';

    $arguments = array(':server_id' => $conf[MCNEESE_FILE_DB_CONF_SERVER_ID]);
    $arguments[':file_id'] = $this->stream_fid;
    $arguments[':stream_position'] = $this->stream_position;
    $arguments[':max_blocks'] = $max_blocks;

    try {
      $results = (array) $file_db->query($query, $arguments, $options)->fetchAll();
      $raw_bytes = '';

      // @fixme/@todo: if the results array is empty, there there is a problem. handle this error here and now!
      foreach ($results as $key => &$result) {
        $raw_bytes .= $result->data;
      }

      $output = substr($raw_bytes, $this->stream_last_read, $bytes);

      if ($underflow > 0) {
        $this->stream_last_read += $bytes;
      }
      else {
        $this->stream_last_read = 0;
        $this->stream_position = $max_blocks;
      }

    }
    catch (Error $e) {
      cf_error::on_query_execution($e);

      throw $e;
    }
    catch (Exception $e) {
      cf_error::on_query_execution($e);

      throw $e;
    }

    return $output;
  }

  /**
   * Implements stream_stat().
   */
  public function stream_stat() {
    if (empty($this->file_meta)) {
      if (!empty($this->stream_fid)) {
        $this->load_file_meta_by_id($this->stream_fid);
      }
      else if (!empty($this->tmp_id)) {
        $this->load_file_meta_by_id($this->tmp_id);
      }
      else if (!empty($this->tmp_checksum)) {
        $this->load_file_meta($this->tmp_checksum);
      }
      elseif (!empty($this->tmp_fid)) {
        $this->load_file_meta_by_fid($this->tmp_fid);
      }

      if (is_null($this->file_meta)) {
        return FALSE;
      }
    }

    $timestamp = strtotime($this->file_meta->timestamp);

    $stat = array();
    $stat['mode'] = 100000 | 661; // 100000 = file, 661 = rw-,rw-,r--
    $stat['ino'] = 0;
    $stat['nlink'] = 1;
    $stat['uid'] = 0;
    $stat['gid'] = 0;
    $stat['rdev'] = 0;
    $stat['size'] = $this->file_meta->size;
    $stat['atime'] = $timestamp;
    $stat['mtime'] = $timestamp;
    $stat['ctime'] = $timestamp;
    $stat['blksize'] = MCNEESE_FILE_DB_BLOCK_SIZE;
    $stat['blocks'] = $this->stream_blocks;

    return $stat;
  }

  /**
   * Implements stream_write().
   */
  public function stream_write($data) {
    // if checksum is already populated, then the file is already in the database.
    if (!empty($this->file_meta)) {
      if (!empty($this->file_meta->checksum)) {
        // @todo: finish this, I should be performing a check to see if the specified checksum actually exists.
        return strlen($data);
      }
    }

    // initialize the raw file so that it can be stat'd on stream_close().
    if (is_null($this->stream_fhandle) || empty($this->stream_fid)) {
      $this->stream_fname = drupal_tempnam('temporary://', MCNEESE_FILE_DB_KEY . '_');
      $this->stream_position = 0;
      $this->stream_fhandle = fopen($this->stream_fname, 'w+b');
      $this->stream_last_write = 0;
    }

    // ensure that PHP uses the proper buffer sizes.
    stream_set_timeout($this->stream_fhandle, 3);
    stream_set_read_buffer($this->stream_fhandle, MCNEESE_FILE_DB_BLOCK_SIZE);
    stream_set_write_buffer($this->stream_fhandle, MCNEESE_FILE_DB_BLOCK_SIZE);
    stream_set_chunk_size($this->stream_fhandle, MCNEESE_FILE_DB_BLOCK_SIZE);

    $block_length = fwrite($this->stream_fhandle, $data, MCNEESE_FILE_DB_BLOCK_SIZE);
    $block = substr($data, 0, $block_length);

    try {
      $file_db = mcneese_file_db_prepare_connection();
      $options = mcneese_file_db_prepare_options();

      if ($file_db === FALSE) {
        return FALSE;
      }

      $transaction = $file_db->startTransaction("mcneese_file_db_stream_write");

      if (empty($this->stream_fid)) {
        try {
          $query = $file_db->insert('mcneese_file_db_files', $options);
          $query->useDefaults(array('timestamp')); // @todo: use php timestamp or leave it alone to postgresql?
          $this->stream_fid = $query->execute();
        }
        catch (Error $e) {
          $transaction->rollback();
          cf_error::on_query_execution($e);

          throw $e;
        }
        catch (Exception $e) {
          $transaction->rollback();
          cf_error::on_query_execution($e);

          throw $e;
        }
      }

      if ($this->stream_last_write > 0) {
        $query = $file_db->delete('mcneese_file_db_file_data', $options);
        $query->condition('file_id', $this->stream_fid);
        $query->condition('block', $this->stream_position);

        $delete_result = $query->execute();

        $this->stream_write_buffer .= $block;
      }
      else {
        $this->stream_write_buffer = $block;
      }

      $fields = array('file_id' => $this->stream_fid, 'block' => $this->stream_position, 'data' => $this->stream_write_buffer);

      $query = $file_db->insert('mcneese_file_db_file_data', $options);
      $query->fields(array_keys($fields));
      $query->values($fields);
      $write_result = $query->execute();

      if (($this->stream_last_write + $block_length) < MCNEESE_FILE_DB_BLOCK_SIZE) {
        $this->stream_last_write += $block_length;
      }
      else {
        $this->stream_position++;
        $this->stream_last_write = 0;
        $this->stream_write_buffer = NULL;
      }

      return $block_length;
    }
    catch (Error $e) {
      if (isset($transaction)) $transaction->rollback();
      cf_error::on_query_execution($e);

      throw $e;
    }
    catch (Exception $e) {
      if (isset($transaction)) $transaction->rollback();
      cf_error::on_query_execution($e);

      throw $e;
    }
  }

  /**
   * Implements unlink().
   */
  public function unlink($uri) {
    if (empty($this->uri)) {
      // set read_only to ensure the image style does to get generated while being deleted.
      $this->read_only = TRUE;
      $this->setUri($uri);
      $this->read_only = FALSE;
    }

    if (empty($this->image_style_uri)) {
      // file does not exist?
      if (empty($this->file_meta)) {
        return FALSE;
      }

      $file_db = mcneese_file_db_prepare_connection();
      $options = mcneese_file_db_prepare_options();

      if ($file_db === FALSE) {
        return FALSE;
      }

      try {
        // only dis-associate files if all references have been deleted.
        $shortsum = substr($this->file_meta->checksum, 0, MCNEESE_FILE_DB_SHORT_SUM_SIZE);

        $check_uri = $this->get_scheme() . '://' . $this->get_path_by_hash() . '/' . $shortsum;
        $query = db_select('file_managed', 'fm');
        $query->condition('fm.uri', db_like($check_uri) . '%', 'LIKE');
        $query->addExpression('count(fid)');

        $total = $query->execute()->fetchField();

        if ($total < 2) {
          $query = $file_db->delete(check_plain($this->get_association()), $options);
          $query->condition('server_id', mcneese_file_db_get_server_id());
          $query->condition('file_id', $this->file_meta->id);
          $query->execute();
        }
      }
      catch (Error $e) {
        cf_error::on_query_execution($e);

        throw $e;
      }
      catch (Exception $e) {
        cf_error::on_query_execution($e);

        throw $e;
      }

      try {
        $query = $file_db->delete('mcneese_file_db_users', $options);
        $query->condition('server_id', mcneese_file_db_get_server_id());
        $query->condition('file_id', $this->file_meta->id);
        $query->execute();
      }
      catch (Error $e) {
        cf_error::on_query_execution($e);

        throw $e;
      }
      catch (Exception $e) {
        cf_error::on_query_execution($e);

        throw $e;
      }

      $fid = '';
      $user = cf_current_user();

      if (is_object($this->file_meta)) {
        $fid = $this->file_meta->id;
      }

      watchdog('File DB', "The user %name (%uid) has deleted the file %uri (%fid)", array('%name' => $user->name, '%uid' => $user->uid, '%uri' => $uri, '%fid' => $fid), WATCHDOG_INFO);
      return TRUE;
    }

    $wrapper = file_stream_wrapper_get_instance_by_uri($this->image_style_uri);

    if (is_object($wrapper)) {
      return $wrapper->unlink($this->image_style_uri);
    }

    return FALSE;
  }

  /**
   * Implements url_stat().
   */
  public function url_stat($uri, $flags) {
    if (empty($this->uri)) {
      $this->setUri($uri);
    }

    $stat = FALSE;

    if (empty($this->image_style_uri)) {
      $stat = $this->stream_stat($uri);

      // return FALSE when the stream cannot be stat'd.
      if ($stat === FALSE) {
        return FALSE;
      }

      // 'mode' is required for functions like file_exists() (this is in octal).
      // calculated from 0100000 (octal for file) + 1204 (octal for 644)
      $stat['mode'] = 0101204;
    }
    else {
      $wrapper = file_stream_wrapper_get_instance_by_uri($this->image_style_uri);

      if (is_object($wrapper)) {
        $stat =  $wrapper->url_stat($this->image_style_uri, $flags);
      }
    }

    return $stat;
  }

  /**
   * Implements get_mimetype().
   */
  public function get_mimetype() {
    $guess_mimetype = FALSE;

    if (!empty($this->image_style_uri)) {
      $wrapper = file_stream_wrapper_get_instance_by_uri($this->image_style_uri);

      if (is_object($wrapper)) {
        return $wrapper->getMimeType($this->image_style_uri);
      }

      $guess_mimetype = TRUE;
    }
    elseif (empty($this->file_meta) || empty($this->file_meta->mimetype)) {
      $guess_mimetype = TRUE;
    }

    if ($guess_mimetype) {
      if (!empty($this->tmp_filename)) {
        $filename_parts = explode('.', $this->tmp_filename);

        if (count($filename_parts) > 1) {
          include_once DRUPAL_ROOT . '/includes/file.mimetypes.inc';
          return file_get_mimetype($this->stream_fname);
        }
      }

      return 'application/octet-stream';
    }

    // attempt to avoid '$this->file_meta->mimetype' when possible.
    if ($this->file_meta->mimetype == 'application/octet-stream') {
      include_once DRUPAL_ROOT . '/includes/file.mimetypes.inc';
      return file_get_mimetype($this->file_meta->filename . '.' . $this->file_meta->extension);
    }

    return $this->file_meta->mimetype;
  }

  /**
   * Implements getMimeType().
   */
  public static function getMimeType($uri, $mapping = NULL) {
    // Drupal made getMimeType() static, so getting the mimetype requires creating another object and then calling a custom get_mimetype() function.
    $wrapper = new self();
    $wrapper->setUri($uri);
    return $wrapper->get_mimetype();
  }

  /**
   * Implements get_scheme().
   */
  public function get_scheme() {
    return self::SCHEME;
  }

  /**
   * Implements get_external().
   */
  public function get_external() {
    return MCNEESE_FILE_DB_FILE_PATH;
  }

  /**
   * Implements get_path_by_hash().
   */
  public function get_path_by_hash() {
    return MCNEESE_FILE_DB_PATH_BY_HASH;
  }

  /**
   * Implements get_path_by_hash().
   */
  public function get_path_by_id() {
    return MCNEESE_FILE_DB_PATH_BY_ID;
  }

  /**
   * Implements get_path_by_fid().
   */
  public function get_path_by_fid() {
    return MCNEESE_FILE_DB_PATH_BY_FID;
  }

  /**
   * Implements get_core().
   */
  public function get_core() {
    return self::CORE;
  }

  /**
   * Implements get_association().
   */
  public function get_association() {
    return self::ASSOCIATION;
  }
}

/**
 * @} End of '@addtogroup mcneese_file_db'.
 */
