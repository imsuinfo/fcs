<?php

/**
 * @file
 * Defines McNeese facilities use event agreement page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use event agreement page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $request_id
 *   The unique identifier for an event request.
 * @param bool $pdf
 *   (optional) When TRUE, the page is presented as a PDF.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_event_agreement_0_form($form, &$form_state, $request_id, $pdf = FALSE) {
  $event = event_workflow_load_event_by_id($request_id);

  if (empty($event)) {
    watchdog(MFCS_WATCHDOG_ID, "Unable to find event with id of @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
    return array();
  }

  global $base_path;

  $page_title = "Facilities Use Agreement";
  if (!empty($event['information']['title'][0]->value)) {
    $page_title = "Facilities Use Agreement: " . $event['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  $module_path = drupal_get_path('module', 'mfcs');

  $form = array();

  $form['form'] = array();

  $form['form']['request_id'] = array(
    '#type' => 'value',
    '#value' => $request_id,
  );

  $form['form']['event'] = array(
    '#type' => 'value',
    '#value' => &$event,
  );

  $form['form']['pdf'] = array(
    '#type' => 'value',
    '#value' => $pdf,
  );

  $form['agreement'] = array(
    '#markup' => '<div id="mfcs-event_request-agreement" class="agreement-wrapper">',
  );

  $markup = &$form['agreement']['#markup'];

  // agreement header
  $markup .= '<div class="agreement-header">';

  $markup .= '<div class="agreement-header-address">';
  $markup .= '<div class="agreement-header-address-line agreement-header-address-line-0">McNeese State University</div>';
  $markup .= '<div class="agreement-header-address-line agreement-header-address-line-1">University Events</div>';
  $markup .= '<div class="agreement-header-address-line agreement-header-address-line-2">Lake Charles, LA 70609</div>';
  $markup .= '<div class="agreement-header-address-line agreement-header-address-line-3">Tel: (337) 562-4MSU Fax: (337) 475-5202</div>';
  $markup .= '</div>'; // agreement-header-address

  $markup .= '<div class="agreement-header-information">';

  // determine the last review by date.
  reset($event['review']['review']);
  $review_date = NULL;
  foreach ($event['review']['review'] as $review) {
    if ($review->step != MFCS_REVIEW_STEP_REVIEW) {
      continue;
    }

    if ($review_date < $review->date) {
      $review_date = $review->date;
    }
  }

  // failsafe to current date.
  if (is_null($review_date)) {
    $review_date = REQUEST_TIME;
  }

  $markup .= '<div class="agreement-header-date">';
  $markup .= '<div class="agreement-header-date-label">' . "Date:" . '</div> ';
  $markup .= '<div class="agreement-header-date-value">' . date("Y / m / d", $review_date) . '</div>';
  $markup .= '</div>'; // agreement-header-date

  $markup .= '<div class="agreement-header-request_id">';
  $markup .= '<div class="agreement-header-request_id-label">' . "Request ID:" . '</div> ';
  $markup .= '<div class="agreement-header-request_id-value">' . $request_id . '</div>';
  $markup .= '</div>'; // agreement-header-request_id

  $markup .= '</div>'; // agreement-header-information
  $markup .= '</div>'; // agreement-header


  // agreement table
  $markup .= '<div class="agreement-table">';
  $markup .= '<div class="agreement-table-header">';

  $markup .= '<div class="agreement-table-header-row">';

  $markup .= '<div class="agreement-table-header-cell column-0">' . "Description" . '</div>';
  $markup .= '<div class="agreement-table-header-cell column-1">' . "Cost" . '</div>';
  $markup .= '<div class="agreement-table-header-cell column-2">' . "Total" . '</div>';

  $markup .= '</div>'; // agreement-table-header-row
  $markup .= '</div>'; // agreement-table-header

  $markup .= '<div class="agreement-table-body">';

  $grand_total = (float) 0;

  $requirements = array();
  $requirements['facilities'] = "Facilities";
  $requirements['equipment'] = "Equipment";
  $requirements['custodial'] = "Custodial";
  $requirements['security'] = "Security";

  $requirements_found = FALSE;
  foreach ($requirements as $requirement => $requirement_name) {
    if (!empty($event['requirements'][$requirement])) {
      $requirements_found = TRUE;
      $markup .= '<div class="agreement-table-body-row">';

      $value = reset($event['requirements'][$requirement]);

      $amount = array();
      $amount['quantity'] = (int) $value->quantity;
      $amount['hours'] = (float) event_workflow_convert_value_from_database_format($value->hours, 'currency');
      $amount['days'] = (float) event_workflow_convert_value_from_database_format($value->days, 'currency');
      $amount['amount'] = (float) event_workflow_convert_value_from_database_format($value->amount, 'currency');

      $amount['total'] = $amount['quantity'] * $amount['hours'] * $amount['days'] * $amount['amount'];
      $grand_total += $amount['total'];

      // column 0
      $markup .= '<div class="agreement-table-body-row-cell column-0">';
      $markup .= '<div class="item-label">' . $requirement_name . ' Fees:</div> ';

      $markup .= '<div class="item-value">';

      $markup .= $amount['quantity'] . ' personnel ';
      if (floor($amount['hours']) == $amount['hours']) {
        $markup .= $amount['hours'];
      }
      else {
        $markup .= sprintf('%01.2f', $amount['hours']);
      }
      $markup .= ' hours ';

      if (floor($amount['days']) == $amount['days']) {
        $markup .= $amount['days'];
      }
      else {
        $markup .= sprintf('%01.2f', $amount['days']);
      }
      $markup .= ' days at $' . sprintf('%01.2f', $amount['amount']);
      $markup .= '</div>'; // item-value
      $markup .= '</div>'; // column-0

      // column 1
      $markup .= '<div class="agreement-table-body-row-cell column-1">';
      $markup .= '$' . sprintf('%01.2f', $amount['total']);
      $markup .= '</div>'; // column-1

      // column 2
      $markup .= '<div class="agreement-table-body-row-cell column-2">';
      $markup .= '$' . sprintf('%01.2f', $grand_total);
      $markup .= '</div>'; // column-2

      $markup .= '</div>'; // agreement-table-body-row
    }
  }

  if (!$requirements_found) {
    $markup .= '<div class="agreement-table-body-row">';
    $markup .= '<div class="agreement-table-body-row-cell column-0">There are no requirements.</div>';
    $markup .= '<div class="agreement-table-body-row-cell column-1">$0.00</div>';
    $markup .= '<div class="agreement-table-body-row-cell column-2">$0.00</div>';
    $markup .= '</div>'; // agreement-table-body-row
  }

  $markup .= '</div>'; // agreement-table-body

  $markup .= '</div>'; // agreement-table

  // amount due
  $markup .= '<div class="agreement-amount_due">';
  $markup .= '<div class="agreement-amount_due-label">' . "Amount Due" . '</div>';
  $markup .= '<div class="agreement-amount_due-value">' . '$' . sprintf('%01.2f', $grand_total) . '</div>';
  $markup .= '</div>';


  // signature section
  $markup .= '<div class="agreement-signature">';

  $markup .= '<div class="agreement-signature-message">';
  $markup .= "Please sign this document to agree to the above stated charges and to confirm your reservation for McNeese facility use.";
  $markup .= '</div>'; // agreement-signature-message

  $markup .= '<div class="agreement-signature-name agreement-signature-field">';
  $markup .= '<label class="agreement-signature-field-label agreement-signature-field-name-label" for="agreement-signature-name-field-' . $request_id . '">' . "Name:" . '</label>';
  $markup .= '<input id="agreement-signature-name-field-' . $request_id . '" class="agreement-signature-field-input agreement-signature-field-name-input" type="textfield" value="">';
  $markup .= '</div>'; // agreement-signature-name

  $markup .= '<div class="agreement-signature-signature agreement-signature-field">';
  $markup .= '<label class="agreement-signature-field-label agreement-signature-field-signature-label" for="agreement-signature-signature-' . $request_id . '">' . "Signature:" . '</label>';
  $markup .= '<input id="agreement-signature-signature-field-' . $request_id . '" class="agreement-signature-field-input agreement-signature-field-signature-input" type="textfield" readonly="readonly" value="">';
  $markup .= '</div>'; // agreement-signature-signature

  $markup .= '<div class="agreement-signature-date agreement-signature-field">';
  $markup .= '<label class="agreement-signature-field-label agreement-signature-field-date-label" for="agreement-signature-date-field-' . $request_id . '">' . "Date:" . '</label>';
  $markup .= '<input id="agreement-signature-date-field-' . $request_id . '" class="agreement-signature-field-input agreement-signature-field-date-input" type="textfield" value="">';
  $markup .= '</div>'; // agreement-signature-date

  $markup .= '</div>'; // agreement-signature


  // download button.
  if (!$pdf) {
    $form['submit'] = array(
      '#id' => 'agreement-submit',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'no-print',
          'event-agreement-submit',
        ),
      ),
      '#weight' => 1000,
    );

    $form['submit']['download'] = array(
      '#id' => 'agreement-submit-download',
      '#name' => 'agreement-submit-download',
      '#type' => 'submit',
      '#default_value' => t("Download"),
      '#attributes' => array(
        'class' => array(
          'no-print',
          'agreement-submit-download',
        ),
      ),
    );
  }

  return $form;
}


/**
 * Custom submit process for mfcs_event_view_0_page().
 *
 * Used for generating a print PDF.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_event_view_0_page()
 */
function mfcs_event_agreement_0_form_submit($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $request_id = (int) $form['form']['request_id']['#value'];

  if (($clicked_id != 'agreement-submit-download') || empty($request_id)) {
    if (empty($request_id)) {
      $form_state['redirect'] = 'events';
    }
    else {
      $form_state['redirect'] = 'events/view-0/' . $request_id;
    }

    return;
  }

  global $base_path;

  $form_state['redirect'] = '/events/agreement-0/' . $request_id . '/pdf';
}

/**
 * Renders a PDF of a given event request agreement.
 *
 * @param int $request_id
 *   The unique identifier for an event request.
 */
function mfcs_event_agreement_0_pdf($request_id) {
  $event = event_workflow_load_event_by_id($request_id);

  if (empty($event)) {
    watchdog(MFCS_WATCHDOG_ID, "Unable to find event with id of @request_id.", array('@request_id' => $request_id), WATCHDOG_ERROR);
    drupal_not_found();
    drupal_exit();
    return array();
  }

  global $base_path;

  $parameters = array();

  $page_title = "Facilities Use Agreement";
  if (!empty($event['information']['title'][0]->value)) {
    $page_title = "Facilities Use Agreement: " . $event['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  $parameters['page_title'] = $page_title;

  $form = array();
  $form_state = array();

  $page = array();
  $page['html'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => 'mfcs-event-agreement-0-form',
      'class' => array(
        'mfcs-event-agreement-0-pdf',
      ),
    ),
  );

  // turn off devel shutdown functionality.
  $GLOBALS['devel_shutdown'] = FALSE;

  $page['html']['content'] = array(mfcs_event_agreement_0_form($form, $form_state, $request_id, TRUE));
  $renderred = drupal_render($page);
  $html = drupal_render_page($renderred);

  $result = mfcs_event_pdf_0_page_using_wkhtmltopdf($request_id, $html, FALSE,