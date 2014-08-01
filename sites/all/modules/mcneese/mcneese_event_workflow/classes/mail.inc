<?php

/**
 * @file
 * Drupal core implementations of MailSystemInterface.
 */

/**
 * The default Drupal mail backend using PHP's mail function.
 */
class McNeeseEventWorkflowMail implements MailSystemInterface {
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


    // override any pre-defined headers.
    $message['headers']['MIME-Version'] = '1.0';
    $message['headers']['X-Mailer'] = 'McNeese Event Workflow System';
    $message['headers']['Date'] = date('r', REQUEST_TIME);
    $message['headers']['Return-Path'] = $message['from_address'];

    $message['headers']['Content-Transfer-Encoding'] = '8Bit';
    #$message['headers']['Content-Transfer-Encoding'] = '7Bit';
    #unset($message['headers']['Content-Transfer-Encoding']);

    #$message['headers']['Sender'] = $message['from_address'];
    unset($message['headers']['Sender']);

    $message['headers']['Message-ID'] = '<';
    $message['headers']['Message-ID'] .= drupal_random_key(12);
    $message['headers']['Message-ID'] .= '.' . REQUEST_TIME;
    $message['headers']['Message-ID'] .= '@';
    $message['headers']['Message-ID'] .= 'mcneese_event_request';
    $message['headers']['Message-ID'] .= '>';

    $message['headers']['Keywords'] = 'McNeese, Event Workflow, Review';

    $boundary = hash('sha1', $message['headers']['Message-ID']);
    $message['headers']['Content-Type'] = 'multipart/related; boundary="' . $boundary . '"';

    // @fixme: change body and markup to be multipart/alternate, using separate boundary.
    if (empty($message['body'])) {
      if (empty($message['markup']) && empty($message['files'])) {
        $final_string .= '--' . $boundary . "\n";
      }
    }
    else {
      $final_string .= '--' . $boundary . "\n";

      foreach ($message['body'] as $body) {
        $final_string .= 'Content-Type: text/plain; charset=UTF-8' . "\n";
        $final_string .= 'Content-Transfer-Encoding: quoted-printable' . "\n";
        $final_string .= 'Content-Description: McNeese Event Workflow System - Message - Plain Text' . "\n\n";
        $final_string .= quoted_printable_encode($body);
      }
    }

    if (!empty($message['markup'])) {
      $final_string .= "\n" . '--' . $boundary . "\n";

      foreach ($message['markup'] as $markup) {
        $final_string .= 'Content-Type: text/html; charset=UTF-8' . "\n";
        $final_string .= 'Content-Transfer-Encoding: quoted-printable' . "\n";
        $final_string .= 'Content-Description: McNeese Event Workflow System - Message - HTML Text' . "\n\n";
        $final_string .= quoted_printable_encode($markup);
      }
    }

    foreach ($message['files'] as $file) {
      $final_string .= "\n" . '--' . $boundary . "\n";
      $final_string .= 'Content-Type: ' . $file['type'] . '; name=' . $file['name'] . "\n";
      $final_string .= 'Content-Disposition: ' . $file['disposition'] . '; filename=' . $file['name'] . "\n";
      $final_string .= 'Content-Transfer-Encoding: ' . $file['encoding'] . "\n\n";
      $final_string .=  chunk_split(base64_encode(file_get_contents($file['uri'])));
    }

    $final_string .= "\n" . '--' . $boundary . "--\n";
    $message['body'] = $final_string;
    unset($final_string);


    // process GNU PGP keys.
    $this->gpg_sign($message, $boundary);
    #$this->gpg_sign_manually($message, $boundary);


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
    $mail_subject = mime_header_encode($message['subject']);

    // For headers, PHP's API suggests that we use CRLF normally,
    // but some MTAs incorrectly replace LF with CRLF. See #234403.
    $mail_headers = implode("\n", $mimeheaders);

    if (isset($message['Return-Path']) && !ini_get('safe_mode')) {
      // On most non-Windows systems, the "-f" option to the sendmail command
      // is used to set the Return-Path. There is no space between -f and
      // the value of the return path.
      $mail_result = @mail(
        $message['to'],
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
        $message['to'],
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
      $prepend .= '  boundary="' . $boundary . '"' . ";\n";
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
    catch (Exception $ex) {
      watchdog('error', "Failed to sign PGP key: @key, reason: @reason", array('@key' => $fingerprint, '@reason' => $ex->getMessage()), WATCHDOG_ERROR);
      return FALSE;
    }

    // e-mail clients/transports will clobber the line endings, so reset.
    $body = preg_replace("/\r\n/i", "\n", $body);

    $message['headers']['Content-Type'] = 'multipart/signed; boundary="' . $sig_boundary . '"; protocol="application/pgp-signature"; micalg=pgp-sha256;';
    $message['body'] = $prepend . $body . $append;

    return TRUE;
  }

  /**
   * Sign the message us GNU PG.
   *
   * This variant was written to test and debug problems in signature
   * generation. Use this for testing or as a last resort.
   *
   * @param $message
   *   A message array, as described in hook_mail_alter().
   * @param $content_type
   *   (optional) The content type to use, such as: 'multipart/mixed'.
   *
   * @param bool
   *   TRUE if signed, FALSE otherwise.
   *
   * @see gpg_sign()
   */
  private function gpg_sign_manually(&$message, $boundary, $content_type = 'multipart/related') {
    global $conf;

    if (!is_string($conf['gpg-mail'][$message['from_address']]['short_id'])) {
      return FALSE;
    }

    $sig_boundary = 'sig_' . $boundary;
    $short_id = $conf['gpg-mail'][$message['from_address']]['short_id'];

    $prepend = 'Content-Type: ' . $content_type . ";\n";
    $prepend .= '  boundary="' . $boundary . '"' . ";\n";
    $prepend .= "\n";

    // e-mail clients/transports will clobber the line endings, so validate one with "\r\n".
    $body = preg_replace("/\r/i", '', $prepend . $message['body']);
    $body = preg_replace("/\n/i", "\r\n", $body);

    $temp_name = drupal_tempnam('temporary://', 'gpg_signature-');
    $file_name = drupal_realpath($temp_name);
    $file_signature = $file_name . '.asc';

    // make sure there is not existing .asc file.
    if (file_exists($file_signature)) {
      unlink($file_signature);
    }

    $result = file_put_contents($temp_name, $body);
    if ($result === FALSE || !file_exists($file_name)) {
      if (file_exists($file_name)) {
        unlink($file_name);
      }
      return FALSE;
    }

    $gpg_path = '/usr/bin/gpg';
    $gpg_command = 'gpg --detach-sign --armor --default-key ' . escapeshellarg($short_id) . ' --batch ' . escapeshellarg($file_name);
    $result = exec($gpg_command);

    $signature = file_get_contents($file_signature);

    if (file_exists($file_name)) {
      unlink($file_name);
    }

    if (file_exists($file_signature)) {
      unlink($file_signature);
    }

    if ($signature === FALSE) {
      return FALSE;
    }

    $prepend = '--' . $sig_boundary . "\n";

    $append = "\n" . '--' . $sig_boundary . "\n";
    $append .= 'Content-Type: application/pgp-signature; name="Digital Signature"' . "\n";
    $append .= 'Content-Disposition: attachment; filename=signature.sig' . "\n";
    $append .= 'Content-Description: OpenPG Digital Signature' . "\n\n";
    $append .= $signature . "\n";
    $append .= '--' . $sig_boundary . '--';

    // e-mail clients/transports will clobber the line endings, so reset.
    $body = preg_replace("/\r\n/i", "\n", $body);

    $message['headers']['Content-Type'] = 'multipart/signed; boundary="' . $sig_boundary . '"; protocol="application/pgp-signature"; micalg=pgp-sha256;';
    $message['body'] = $prepend . $body . $append;

    return TRUE;
  }
}
