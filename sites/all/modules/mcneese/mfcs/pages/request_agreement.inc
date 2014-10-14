<?php

/**
 * @file
 * Defines McNeese facilities use request agreement page functions.
 */

/**
 * @addtogroup mfcs
 * @{
 */

/**
 * Provides the facilities use request agreement page.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 * @param int $request_id
 *   The unique identifier for an request.
 * @param bool $contained
 *   (optional) When TRUE, the page is self-contained, asuch as with a PDF.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_agreement_0_form($form, &$form_state, $request_id, $contained = FALSE) {
  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  mfcs_add_canonical_headers('requests/agreement-0/' . $request_id);

  global $base_path;

  $page_title = "Facilities Use Agreement";
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = "Facilities Use Agreement: " . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  $module_path = drupal_get_path('module', 'mfcs');

  $form = array();

  $form['form'] = array();

  $form['form']['request_id'] = array(
    '#type' => 'value',
    '#value' => $request_id,
  );

  $form['form']['revision_id'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );

  $form['form']['request'] = array(
    '#type' => 'value',
    '#value' => &$request,
  );

  $form['form']['contained'] = array(
    '#type' => 'value',
    '#value' => $contained,
  );

  $form['agreement'] = array(
    '#markup' => '<div id="mfcs-request-agreement" class="agreement-wrapper">',
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
  reset($request['review']['review']);
  $review_date = NULL;
  foreach ($request['review']['review'] as $review) {
    if ($review->step != MFCS_REVIEW_STEP_REVIEW) {
      continue;
    }

    if ($review_date < $review->date) {
      $review_date = $review->date;
    }
  }

  // failsafe to current date.
  if (is_null($review_date)) {
    $instance = mfcs_instance();
    $review_date = $instance;
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
  $requirements['facilities'] = array(
    'calculate' => array(
      'days' => 'days',
      'amount' => 'amount',
    ),
    'name' => "Facilities",
  );

  $requirements['equipment'] = array(
    'calculate' => array(
      'days' => 'days',
      'amount' => 'amount',
    ),
    'name' => "Equipment",
  );

  $requirements['custodial'] = array(
    'calculate' => array(
      'quantity' => 'quantity',
      'hours' => 'hours',
      'days' => 'days',
      'amount' => 'amount',
    ),
    'name' => "Custodial",
  );

  $requirements['security'] = array(
    'calculate' => array(
      'quantity' => 'quantity',
      'hours' => 'hours',
      'days' => 'days',
      'amount' => 'amount',
    ),
    'name' => "Security",
  );

  $requirements_found = FALSE;
  foreach ($requirements as $requirement => $requirement_data) {
    if (!empty($request['requirements'][$requirement])) {
      $requirements_found = TRUE;

      $value = reset($request['requirements'][$requirement]);

      $amount = array();
      $amount['total'] = NULL;
      $amount['quantity'] = NULL;
      $amount['hours'] = NULL;
      $amount['days'] = NULL;
      $amount['amount'] = NULL;

      if (isset($requirement_data['calculate']['quantity'])) {
        if (is_null($amount['total'])) {
          $amount['total'] = 1;
        }

        $amount['quantity'] = (int) $value->quantity;
        $amount['total'] *= $amount['quantity'];
      }

      if (isset($requirement_data['calculate']['hours'])) {
        if (is_null($amount['total'])) {
          $amount['total'] = 1;
        }

        $amount['hours'] = (float) mfcs_convert_value_from_database_format($value->hours, 'currency');
        $amount['total'] *= $amount['hours'];
      }

      if (isset($requirement_data['calculate']['days'])) {
        if (is_null($amount['total'])) {
          $amount['total'] = 1;
        }

        $amount['days'] = (float) mfcs_convert_value_from_database_format($value->days, 'currency');
        $amount['total'] *= $amount['days'];
      }

      if (isset($requirement_data['calculate']['amount'])) {
        if (is_null($amount['total'])) {
          $amount['total'] = 1;
        }

        $amount['amount'] = (float) mfcs_convert_value_from_database_format($value->amount, 'currency');
        $amount['total'] *= $amount['amount'];
      }

      if (is_null($amount['total'])) {
        continue;
      }

      $markup .= '<div class="agreement-table-body-row">';

      $grand_total += $amount['total'];

      // column 0
      $markup .= '<div class="agreement-table-body-row-cell column-0">';
      $markup .= '<div class="item-label">' . $requirement_data['name'] . ' Fees:</div> ';

      $markup .= '<div class="item-value">';

      if (isset($requirement_data['calculate']['quantity'])) {
        $markup .= $amount['quantity'] . ' personnel ';
      }

      if (isset($requirement_data['calculate']['hours'])) {
        if (floor($amount['hours']) == $amount['hours']) {
          $markup .= $amount['hours'];
        }
        else {
          $markup .= sprintf('%01.2f', $amount['hours']);
        }
        $markup .= ' hours ';
      }

      if (isset($requirement_data['calculate']['days'])) {
        if (floor($amount['days']) == $amount['days']) {
          $markup .= $amount['days'];
        }
        else {
          $markup .= sprintf('%01.2f', $amount['days']);
        }

        $markup .= ' days ';
      }

      if (isset($requirement_data['calculate']['amount'])) {
        $markup .= 'at $' . sprintf('%01.2f', $amount['amount']);
      }

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
  if (!$contained) {
    $form['submit'] = array(
      '#id' => 'agreement-submit',
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array(
          'no-print',
          'request-agreement-submit',
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
 * Custom submit process for mfcs_request_view_0_form().
 *
 * Used for generating a print PDF.
 *
 * @param array $form
 *   The form array as defined by drupals form api.
 * @param array $form_state
 *   The form state array as defined by drupals form api
 *
 * @see: mfcs_request_view_0_form()
 */
function mfcs_request_agreement_0_form_submit($form, &$form_state) {
  $clicked_id = '';
  if (isset($form_state['triggering_element']['#id'])) {
    $clicked_id = $form_state['triggering_element']['#id'];
  }

  $request_id = (int) $form['form']['request_id']['#value'];

  if (($clicked_id != 'agreement-submit-download') || empty($request_id)) {
    if (empty($request_id)) {
      $form_state['redirect'] = mfcs_build_redirect_array('requests');
    }
    else {
      $form_state['redirect'] = mfcs_build_redirect_array('requests/view-0/' . $request_id);
    }

    return;
  }

  global $base_path;

  $form_state['redirect'] = mfcs_build_redirect_array('/requests/agreement-0/' . $request_id . '/pdf');
}

/**
 * Renders a PDF of a given request agreement.
 *
 * @param int $request_id
 *   The unique identifier for an request.
 *
 * @return exit
 *   This function should not return.
 */
function mfcs_request_agreement_0_pdf($request_id) {
  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  mfcs_add_canonical_headers('requests/agreement-0/' . $request_id . '/pdf');

  $page_title = "Facilities Use Agreement";
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = "Facilities Use Agreement: " . $request['information']['title'][0]->value;
  }
  drupal_set_title($page_title);

  $form = array();
  $form_state = array();

  $page = array();
  $page['html'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => 'mfcs-request-agreement-0-form',
      'class' => array(
        'mfcs-request-agreement-0-pdf',
      ),
    ),
  );

  // turn off devel shutdown functionality.
  $GLOBALS['devel_shutdown'] = FALSE;

  $page['html']['content'] = array(mfcs_request_agreement_0_form($form, $form_state, $request_id, TRUE));
  $renderred = drupal_render($page);
  $html = drupal_render_page($renderred);

  mfcs_include(MFCS_INCLUDE_OUTPUT);

  $parameters = array(
    'page_title' => $page_title,
    'title' => $page_title,
  );

  $contained = mfcs_output_print_page_view($html, $parameters);

  if (!is_string($contained)) {
    drupal_not_found();
    drupal_exit();
  }

  $result = mfcs_request_pdf_0_page_using_wkhtmltopdf($request_id, NULL, $contained, FALSE, MFCS_PDF_TYPE_AGREEMENT);

  if ($result) {
    exit();
  }

  drupal_not_found();
  drupal_exit();
}

/**
 * @} End of '@addtogroup mfcs'.
 */
