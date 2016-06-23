<?php

/**
 * @file
 * Drupal core implementations of MailSystemInterface.
 */

/**
 * The default Drupal mail backend using PHP's mail function.
 */
class McNeeseFacilitiesUseMail implements MailSystemInterface {
  /**
   * Concatenate and wrap the e-mail body for plain-text mails.
   *
   * @param $message
   *   A message array, as described in hook_mail_alter().
   *
   * @return
   *   The formatted $message.
   */
  public function format(array $message) {
    $boundary = NULL;
    $final_string = '';

    $instance = mfcs_instance();

    $fcs_name = variable_get('fcs_name', 'McNeese Facilities Use System');
    $fcs_site_url = variable_get('fcs_site_url', 'fcs.mcneese.edu');
    $reply_to = variable_get('reply_to', 'no_reply@fcs.mcneese.edu');

    // override any pre-defined headers.
    $message['headers']['MIME-Version'] = '1.0';
    $message['headers']['X-Mailer'] = $fcs_name;
    $message['headers']['Date'] = date('r', $instance);

    $from_address = filter_var($message['from_address'], FILTER_SANITIZE_SPECIAL_CHARS, FILTER_FLAG_STRIP_LOW);
    $message['headers']['Return-Path'] = $from_address;

    if (!empty($reply_to) && is_string($reply_to)) {
      $message['headers']['Reply-To'] = $reply_to;
    }

    $message['headers']['Content-Transfer-Encoding'] = '8Bit';
    #$message['headers']['Content-Transfer-Encoding'] = '7Bit';
    #unset($message['headers']['Content-Transfer-Encoding']);

    #$message['headers']['Sender'] = $message['from_address'];
    unset($message['headers']['Sender']);

    $message['headers']['Message-ID'] = '<';
    $message['headers']['Message-ID'] .= drupal_random_key(12);
    $message['headers']['Message-ID'] .= '.' . $instance;
    $message['headers']['Message-ID'] .= '@';
    $message['headers']['Message-ID'] .= 'mfcs';
    $message['headers']['Message-ID'] .= '>';

    $message['headers']['Keywords'] = 'McNeese, Facilities Use';

    $boundary = hash('sha1', $message['headers']['Message-ID']);
    $message['headers']['Content-Type'] = 'multipart/related; boundary="' . $boundary . '"';

    $final_string .= '--' . $boundary . "\n";
    $final_string .= 'Content-Type: multipart/alternative;' . "\n";
    $final_string .= '  boundary="-alt_' . $boundary . '"' . "\n";

    $final_string .= "\n" . '---alt_' . $boundary . "\n";
    $final_string .= 'Content-Type: text/plain; charset=UTF-8' . "\n";
    $final_string .= 'Content-Transfer-Encoding: quoted-printable' . "\n";
    $final_string .= 'Content-Description: ' . $fcs_name . ' - Message - Plain Text' . "\n\n";

    if (!empty($message['body'])) {
      $final_string .= quoted_printable_encode(implode('', $message['body']));
    }

    $final_string .= "\n" . '---alt_' . $boundary . "\n";
    $final_string .= 'Content-Type: text/html; charset=UTF-8' . "\n";
    $final_string .= 'Content-Transfer-Encoding: quoted-printable' . "\n";
    $final_string .= 'Content-Description: ' . $fcs_name . ' - Message - HTML Text' . "\n\n";

    if (!empty($message['markup'])) {
      $final_string .= quoted_printable_encode(implode('', $message['markup']));
    }

    $final_string .= "\n" . '---alt_' . $boundary . "--\n";

    if (array_key_exists('files', $message) && is_array($message['files'])) {
      foreach ($message['files'] as $file) {
        if (!is_array($file) || !array_key_exists('mime', $file) || !array_key_exists('name', $file) || !array_key_exists('data', $file)) {
          continue;
        }

        if (!isset($file['data'])) {
          watchdog(MFCS_WATCHDOG_ID, "Error detected in file process of e-mail generation. Not sending file. File Information = %file.", array('%file' => print_r($file, TRUE)), WATCHDOG_ERROR);
          continue;
        }

        $mime = filter_var($file['mime'], FILTER_SANITIZE_STRING, FILTER_FLAG_STRIP_LOW);
        $name = filter_var($file['name'], FILTER_SANITIZE_STRING, FILTER_FLAG_STRIP_LOW);
        $disposition = filter_var($file['disposition'], FILTER_SANITIZE_STRING, FILTER_FLAG_STRIP_LOW);

        $mime = preg_replace('/(;|"|\')/', '', $mime);
        $name = preg_replace('/(;|"|\')/', '', $name);
        $disposition = preg_replace('/(;|"|\')/', '', $disposition);

        $final_string .= "\n" . '--' . $boundary . "\n";
        $final_string .= 'Content-Type: ' . $mime . '; name=' . $name . "\n";
        $final_string .= 'Content-Disposition: ' . $disposition . '; filename=' . $name . "\n";
        $final_string .= 'Content-Transfer-Encoding: base64' . "\n\n";
        $final_string .=  chunk_split(base64_encode($file['data']));

        unset($mime);
        unset($name);
        unset($disposition);
      }
    }

    $final_string .= "\n" . '--' . $boundary . "--\n";
    $message['body'] = $final_string;
    unset($final_string);


    // process GNU PGP keys.
    $this->gpg_sign($message, $boundary);


    // save the e-mail to the log.
    $this->log_email($message);


    return $message;
  }

  /**
   * Send an e-mail message, using Drupal variables and default settings.
   *
   * @see http://php.net/manual/function.mail.php
   * @see drupal_mail()
   *
   * @param $message
   *   A message array, as described in hook_mail_alter().
   * @return
   *   TRUE if the mail was successfully accepted, otherwise FALSE.
   */
  public function mail(array $message) {
    // If 'Return-Path' isn't already set in php.ini, we pass it separately
    // as an additional parameter instead of in the header.
    // However, if PHP's 'safe_mode' is on, this is not allowed.
    if (isset($message['headers']['Return-Path']) && !ini_get('safe_mode')) {
      $return_path_set = strpos(ini_get('sendmail_path'), ' -f');
      if (!$return_path_set) {
        $message['Return-Path'] = $message['headers']['Return-Path'];
        unset($message['headers']['Return-Path']);
      }
    }

    $mimeheaders = array();
    foreach ($message['headers'] as $name => $value) {
      $mimeheaders[] = $name . ': ' . mime_header_encode($value);
    }


    // Prepare mail commands.
    $mail_subject = mime_header_encode(substr($message['subject'], 0, MFCS_EMAIL_MAX_SUBJECT_LENGTH));

    // For headers, PHP's API suggests that we use CRLF normally,
    // but some MTAs incorrectly replace LF with CRLF. See #234403.
    $mail_headers = implode("\n", $mimeheaders);

    if (isset($message['Return-Path']) && !ini_get('safe_mode')) {
      // On most non-Windows systems, the "-f" option to the sendmail command
      // is used to set the Return-Path. There is no space between -f and
      // the value of the return path.
      $mail_result = @mail(
        substr($message['to'], 0, MFCS_EMAIL_MAX_DESTINATION_LENGTH),
        $mail_subject,
        $message['body'],
        $mail_headers,
        '-f' . $message['Return-Path']
      );
    }
    else {
      // The optional $additional_parameters argument to mail() is not
      // allowed if safe_mode is enabled. Passing any value throws a PHP
      // warning and makes mail() return FALSE.
      $mail_result = @mail(
        substr($message['to'], 0, MFCS_EMAIL_MAX_DESTINATION_LENGTH),
        $mail_subject,
        $message['body'],
        $mail_headers
      );
    }


    return $mail_result;
  }

  /**
   * Sign the message us GNU PG.
   *
   * @param $message
   *   A message array, as described in hook_mail_alter().
   * @param $content_type
   *   (optional) The content type to use, such as: 'multipart/mixed'.
   *
   * @param bool
   *   TRUE if signed, FALSE otherwise.
   */
  private function gpg_sign(&$message, $boundary, $content_type = 'multipart/related') {
    global $conf;

    if (!class_exists('gnupg') || !isset($conf['gpg-mail'][$message['from_address']]['fingerprint'])) {
      return FALSE;
    }

    if (!is_string($conf['gpg-mail'][$message['from_address']]['fingerprint'])) {
      return FALSE;
    }

    $gpg = new gnupg();
    if (!is_object($gpg)) {
      return FALSE;
    }

    try {
      $sig_boundary = 'sig_' . $boundary;
      $fingerprint = $conf['gpg-mail'][$message['from_address']]['fingerprint'];

      $gpg->seterrormode(gnupg::ERROR_EXCEPTION);
      $gpg->addsignkey($fingerprint);
      $gpg->setsignmode(gnupg::SIG_MODE_DETACH);
      $gpg->setarmor(1);

      $prepend = 'Content-Type: ' . $content_type . ";\n";
      $prepend .= '  boundary="' . $boundary . '"' . "\n";
      $prepend .= "\n";

      // e-mail clients/transports will clobber the line endings, so validate one with "\r\n".
      $body = preg_replace("/\r/i", '', $prepend . $message['body']);
      $body = preg_replace("/\n/i", "\r\n", $body);

      $signature = $gpg->sign($body);
      #$valid = $gpg->verify($body, $signature);

      $prepend = '--' . $sig_boundary . "\n";

      $append = "\n" . '--' . $sig_boundary . "\n";
      $append .= 'Content-Type: application/pgp-signature; name="Digital Signature"' . "\n";
      $append .= 'Content-Disposition: attachment; filename=signature.sig' . "\n";
      $append .= 'Content-Description: OpenPG Digital Signature' . "\n\n";
      $append .= $signature . "\n";
      $append .= '--' . $sig_boundary . '--';
    }
    catch (Error $e) {
      watchdog(MFCS_WATCHDOG_ID, "Failed to sign PGP key: @key, reason: @reason", array('@key' => $fingerprint, '@reason' => $e->getMessage()), WATCHDOG_ERROR);
      return FALSE;
    }
    catch (Exception $e) {
      watchdog(MFCS_WATCHDOG_ID, "Failed to sign PGP key: @key, reason: @reason", array('@key' => $fingerprint, '@reason' => $e->getMessage()), WATCHDOG_ERROR);
      return FALSE;
    }

    // e-mail clients/transports will clobber the line endings, so reset.
    $body = preg_replace("/\r\n/i", "\n", $body);

    $message['headers']['Content-Type'] = 'multipart/signed; boundary="' . $sig_boundary . '"; protocol="application/pgp-signature"; micalg=pgp-sha256';
    $message['body'] = $prepend . $body . $append;

    return TRUE;
  }

  /**
   * Logs the e-mail in the database.
   *
   * @param array $message
   *   A message array, as described in hook_mail_alter().
   */
  private function log_email($message) {
    $instance = mfcs_instance();

    $user = cf_current_user();
    $user_id = 0;
    if (isset($user->uid)) {
      $user_id = $user->uid;
    }

    // When using anoynmous account, check to see if effective system id is enabled.
    if ($user_id == 0) {
      $system_id_enabled = mfcs_using_effective_system_id();
      if ($system_id_enabled) {
        $user_id = 1;
      }
    }

    $fields = array(
      'destination' => substr($message['to'], 0, MFCS_EMAIL_MAX_DESTINATION_LENGTH),
      'title' => substr($message['subject'], 0, MFCS_EMAIL_MAX_SUBJECT_LENGTH),
      'headers' => serialize($message['headers']),
      'body' => serialize($message['body']),
      'code' => MFCS_EMAIL_CODE_NORMAL,
      'user_id' => $user_id,
      'date' => $instance,
    );

    $transaction = db_transaction();

    try {
      $query = db_insert('mfcs_log_emails');
      $query->fields($fields);
      $query->execute();

      // enforce transaction execution
      unset($transaction);
    }
    catch (Error $e) {
      $transaction->rollback();
      cf_error::on_exception($e);

      return FALSE;
    }
    catch (Exception $e) {
      $transaction->rollback();
      cf_error::on_exception($e);

      return FALSE;
    }
  }
}
