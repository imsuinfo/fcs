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
 *   The unique identifier for a request.
 * @param bool $contained
 *   (optional) When TRUE, the page is self-contained, asuch as with a PDF.
 *
 * @return string
 *   The HTML output representing the page.
 */
function mfcs_request_agreement_0_form($form, &$form_state, $request_id, $contained = FALSE) {
  if (!is_array($form)) {
    cf_error::invalid_array('form');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (!is_array($form_state)) {
    cf_error::invalid_array('form_state');

    drupal_not_found();
    drupal_exit();
    return array();
  }

  if (is_string($request_id)) {
    $resuest_id = (int) $request_id;
  }

  $request = mfcs_load_request_by_id($request_id);

  if (empty($request)) {
    drupal_not_found();
    drupal_exit();
    return array();
  }

  global $base_path;

  mfcs_add_canonical_headers('requests/agreement-0/' . $request_id);

  $page_title = 'Facilities Use Agreement';
  if (!empty($request['information']['title'][0]->value)) {
    $page_title = 'Facilities Use Agreement: ' . $request['information']['title'][0]->value;
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
    if (is_string($review->step)) {
      if (((int) $review->step) !== MFCS_REVIEW_STEP_REVIEW) {
        continue;
      }
    }
    else {
      if ($review->step !== MFCS_REVIEW_STEP_REVIEW) {
        continue;
      }
    }

    if ($review_date < $review->date) {
      $review_date = $review->date;
    }
  }

  $revision = 0;
  if (isset($request['mer']['revision'][0]->value)) {
    $revision = (int) $request['mer']['revision'][0]->value;
  }

  $requester = '';
  if (isset($request['mer']['user_id'][0]->value) && is_numeric($request['mer']['user_id'][0]->value)) {
    $requester_user = mfcs_get_user_information((int) $request['mer']['user_id'][0]->value);
    if (is_object($requester_user)) {
      $requester_user_name = $requester_user->name;
      $requester_user_email = $requester_user->email;
      if (!empty($requester_user->first_name) && !empty($requester_user->last_name)) {
        $requester_user_name = $requester_user->first_name . ' ' . $requester_user->last_name;
        $requester_user_email = '"' . $requester_user_name . '" <' . $requester_user->email . '>';
      }
      unset($requester_user_name);

      $requester = '<a href="mailto:' . str_replace('"', '%22', $requester_user_email) . '" class="agreement-header-requester-value-link no_print_links_as_link">' . check_plain($requester_user_email) . '</a>';
      unset($requester_user_email);
    }
    unset($requester_user);
  }

  // failsafe to current date.
  if (is_null($review_date)) {
    $instance = mfcs_instance();
    $review_date = $instance;
  }

  $markup .= '<div class="agreement-header-date">';
  $markup .= '<div class="agreement-header-date-label">' . 'Date:' . '</div> ';
  $markup .= '<div class="agreement-header-date-value">' . date(MFCS_DATE_FORMAT_DATE_SQUEEZE, $review_date) . '</div>';
  $markup .= '</div>'; // agreement-header-date

  $markup .= '<div class="agreement-header-request_id">';
  $markup .= '<div class="agreement-header-request_id-label">' . 'Request ID:' . '</div> ';
  $markup .= '<div class="agreement-header-request_id-value">' . $request_id . '</div>';
  $markup .= '</div>'; // agreement-header-request_id

  $markup .= '<div class="agreement-header-revision">';
  $markup .= '<div class="agreement-header-revision-label">' . 'Revision:' . '</div> ';
  $markup .= '<div class="agreement-header-revision-value">' . $revision . '</div>';
  $markup .= '</div>'; // agreement-header-revision

  $markup .= '<div class="agreement-header-requester">';
  $markup .= '<div class="agreement-header-requester-label">' . 'Requester:' . '</div> ';
  $markup .= '<div class="agreement-header-requester-value">' . $requester . '</div>';
  $markup .= '</div>'; // agreement-header-requester

  $markup .= '</div>'; // agreement-header-information
  $markup .= '</div>'; // agreement-header


  // agreement table
  $markup .= '<div class="agreement-table">';
  $markup .= '<div class="agreement-table-header">';

  $markup .= '<div class="agreement-table-header-row">';

  $markup .= '<div class="agreement-table-header-cell column-0">' . 'Description' . '</div>';
  $markup .= '<div class="agreement-table-header-cell column-1">' . 'Cost' . '</div>';
  $markup .= '<div class="agreement-table-header-cell column-2">' . 'Total' . '</div>';

  $markup .= '</div>'; // agreement-table-header-row
  $markup .= '</div>'; // agreement-table-header

  $markup .= '<div class="agreement-table-body">';

  $grand_total = (float) 0;

  $requirements = array();
  $requirements['facilities_fees'] = 'Facilities';
  $requirements['equipment_fees'] = 'Equipment';
  $requirements['custodial_fees'] = 'Custodial';
  $requirements['maintenance_fees'] = 'Maintenance';
  $requirements['grounds_fees'] = 'Grounds';
  $requirements['security_fees'] = 'Security';

  $requirements_found = FALSE;
  foreach ($requirements as $requirement => $requirement_name) {
    if (empty($request['requirements'][$requirement])) {
      continue;
    }

    $requirements_found = TRUE;
    $value = reset($request['requirements'][$requirement]);
    $amount = mfcs_calculate_use_fees($value);

    if ($amount === FALSE || (is_null($amount['total']) && is_null($amount['supplies']) && is_null($amount['post']))) {
      continue;
    }

    $markup .= '<div class="agreement-table-body-row">';

    $grand_total += $amount['total'];

    // column 0
    $markup .= '<div class="agreement-table-body-row-cell column-0">';
    $markup .= '<div class="item-label">' . $requirement_name . ' Fees:</div> ';

    $markup .= '<div class="item-value">';

    $markup_added = FALSE;
    if (!is_null($amount['rate'])) {
      if (!is_null($amount['personnel'])) {
        $markup .= $amount['personnel'] . ' personnel ';
      }

      if (!is_null($amount['hours'])) {
        if (floor($amount['hours']) == $amount['hours']) {
          $markup .= $amount['hours'];
        }
        else {
          $markup .= sprintf('%01.2f', $amount['hours']);
        }
        $markup .= ' hours ';
      }

      if (!is_null($amount['days'])) {
        if (floor($amount['days']) == $amount['days']) {
          $markup .= $amount['days'];
        }
        else {
          $markup .= sprintf('%01.2f', $amount['days']);
        }
        $markup .= ' days ';
      }

      $markup .= 'at $' . sprintf('%01.2f', $amount['rate']);
      $markup_added = TRUE;
    }

    if (!is_null($amount['supplies'])) {
      if ($markup_added) {
        $markup .= '<br>';
      }

      $markup .= 'Supplies at $' . sprintf('%01.2f', $amount['supplies']);

      $markup_added = TRUE;
    }

    if (!is_null($amount['post'])) {
      if ($markup_added) {
        $markup .= '<br>';
      }

      $markup .= 'Other Expenses at $' . sprintf('%01.2f', $amount['post']);

      if (!is_null($amount['description'])) {
        $markup .= '<div class="other_expenses-description">' . str_replace("\n", "<br>\n", $amount['description']) . '</div>';
      }

      $markup_added = TRUE;
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

  $form_state['redirect'] = mfcs_build_redirect_array('/requests/agreement-0/' . $request_id . '/pdf');
}

/**
 * Renders a PDF of a given request agreement.
 *
 * @param int $request_id
 *   The unique identifier for a request.
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
