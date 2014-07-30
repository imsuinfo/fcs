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
    #$message['headers']['Content-Transfer-Encoding'] = '7Bit';
    $message['headers']['Return-Path'] = $message['from_address'];

    #$message['headers']['Sender'] = $message['from_address'];
    unset($message['headers']['Sender']);

    $message['headers']['Message-ID'] = '<';
    $message['headers']['Message-ID'] .= drupal_random_key(12);
    $message['headers']['Message-ID'] .= '.' . REQUEST_TIME;
    $message['headers']['Message-ID'] .= '@';
    $message['headers']['Message-ID'] .= 'mcneese_event_request';
    $message['headers']['Message-ID'] .= '>';

    $message['headers']['Keywords'] = 'McNeese, Event Workflow, Review';

    $boundary = hash('sha256', $message['headers']['Message-ID']);
    $message['headers']['Content-Type'] = 'multipart/mixed; boundary="' . $boundary . '"';

    if (empty($message['body'])) {
      $final_string .= '--' . $boundary . "\n";
    }
    else {
      foreach ($message['body'] as $body) {
        $final_string .= '--' . $boundary . "\n";
        $final_string .= 'Content-Type: text/plain; charset=UTF-8' . "\n";
        $final_string .= 'Content-Transfer-Encoding: quoted-printable' . "\n\n";
        $final_string .= quoted_printable_encode($body) . "\n\n";
      }
    }

    foreach ($message['files'] as $file) {
      $final_string .= '--' . $boundary . "\n";
      $final_string .= 'Content-Type: ' . $file['type'] . '; name=' . $file['name'] . "\n";
      $final_string .= 'Content-Disposition: ' . $file['disposition'] . '; filename=' . $file['name'] . "\n";
      $final_string .= 'Content-Transfer-Encoding: ' . $file['encoding'] . "\n\n";
      $final_string .=  chunk_split(base64_encode(file_get_contents($file['uri']))) . "\n";
    }

    $final_string .= '--' . $boundary . "--\n";
    $message['body'] = $final_string;
    unset($final_string);


    // process GNU PGP keys.
    $this->gpg_sign($message, $boundary);


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
      $fingerprint = $conf['gpg-mail'][$message['from_address']]['fingerprint'];
      $gpg->seterrormode(gnupg::ERROR_EXCEPTION);
      $gpg->addsignkey($fingerprint);
      $gpg->setsignmode(gnupg::SIG_MODE_DETACH);
      $gpg->setarmor(1);

      $prepend = 'Content-Type: ' . $content_type . ";\n";
      $prepend .= '  boundary="' . $boundary . '"' . ";\n";
      $prepend .= 'Content-Transfer-Encoding: 7bit' . "\n\n";

      $message['body'] = $prepend . $message['body'];

      $signature = $gpg->sign($message['body']);

      $sig_boundary = 'sig_' . $boundary;

      $prepend = '--' . $sig_boundary . "\n";

      $append = "\n" . '--' . $sig_boundary . "\n";
      $append .= 'Content-Type: application/pgp-signature; name=signature.sig' . "\n";
      $append .= 'Content-Disposition: attachment; filename=signature.sig' . "\n";
      $append .= 'Content-Description: OpenPG Digital Signature' . "\n\n";
      $append .= $signature . "\n";
      $append .= '--' . $sig_boundary . "--\n";
    }
    catch (Exception $ex) {
      watchdog('error', "Failed to sign PGP key: @key, reason: @reason", array('@key' => $fingerprint, '@reason' => $ex->getMessage()), WATCHDOG_ERROR);
      return FALSE;
    }

    $message['headers']['Content-Type'] = 'multipart/signed; boundary="' . $sig_boundary . '"; protocol="application/pgp-signature"; micalg=pgp-sha256;';
    $message['body'] = $prepend . $message['body'] . $append;

    return TRUE;
  }
}
