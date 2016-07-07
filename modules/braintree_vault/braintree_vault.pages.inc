<?php

/**
 * @file
 * Implement Braintree payment method operation pages.
 */

/**
 * Braintree payment method form.
 *
 * @see braintree_payment_method_form_submit()
 */
function braintree_payment_method_form($form, &$form_state, $op = 'add', $delta = NULL) {
  global $user;

  if ($op == 'edit') {
    $customer = braintree_customer_load_by_account($user);
    $payment_methods = braintree_payment_methods_sorted($customer->paymentMethods);
    if (!isset($payment_methods[$delta])) {
      drupal_goto('dashboard/payment_methods');
    }

    $payment_method = $payment_methods[$delta];
    $form_state['payment_method'] = $payment_method;

    if (braintree_payment_method_is_paypal($payment_method)) {
      drupal_goto('dashboard/payment_methods');
    }

  }

  $values = isset($form_state['values']) ? $form_state['values'] : array();
  $form_id = 'braintree_payment_method_form';

  $form_state['braintree_customer'] = braintree_customer_load_by_account($user);

  $form_state['op'] = $op;

  $form['payment_method'] = array(
    '#title' => t('Payment type'),
    '#type' => 'radios',
    '#options' => array(
      'braintree_paypal' => t('PayPal'),
      'braintree_hostedfields' => t('Credit card or debt card'),
    ),
    '#default_value' => 'braintree_paypal',
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'braintree_payment_method_form_refresh_js',
      'wrapper' => 'payment-details',
      'method' => 'replace',
    ),
  );

  if ($op == 'edit') {
    $form['payment_method']['#default_value'] = braintree_payment_method_is_credit_card($payment_method) ? 'braintree_hostedfields' : 'braintree_paypal';
    foreach ($form['payment_method']['#options'] as $method_name=>$label) {
      if ($method_name != $form['payment_method']['#default_value']) {
        unset($form['payment_method']['#options'][$method_name]);
      }
    }

  }

  $payment_method = !empty($values['payment_method']) ? $values['payment_method'] : $form['payment_method']['#default_value'];

  $form['payment_details'] = array(
    '#type' => 'container',
    '#id' => 'payment-details',
  );
  _braintree_payment_form($form['payment_details'], $form_state, $payment_method);

  // Add form ID here for payment method change event.
  $js_settings = array();
  $js_settings['formId'] = str_replace('_', '-', $form_id);

  $form['#attached']['js'][] = array(
    'data' => array('braintree' => $js_settings),
    'type' => 'setting',
  );

  $back_url = braintree_payment_method_list_url(FALSE, $render = TRUE);
  $form['actions'] = array('#type' => 'actions');

  $form['actions']['back'] = array(
    '#type' => 'link',
    '#title' => t('Back'),
    '#href' => $back_url,
    '#attributes' => array(
      'class' => array('form-submit', 'faux-submit'),
      'id' => 'edit-prev',
    ),
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#attributes' => array('class' => array('button', 'button--arrow')),
    '#value' => t('Save payment details'),
  );

  $form['#submit'][] = 'braintree_payment_method_form_submit';

  $form['#attributes']['class'][] = 'clearfix';

  return $form;
}

/**
 * Ajax callback to refresh payment method form.
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function braintree_payment_method_form_refresh_js($form, $form_state) {
  return $form['payment_details'];
}

/**
 * Submit handler for Braintree payment method form.
 */
function braintree_payment_method_form_submit($form, &$form_state) {
  $nonce = braintree_get_nonce();

  $payment_method = array(
    'paymentMethodNonce' => $nonce,
    'customerId' => $form_state['braintree_customer']->id,
    'paymentMethodToken' => $form_state['payment_method']->token,
  );

  $context = array(
    'op' => $form_state['op'],
  );

  $result = braintree_vault_payment_method_save($payment_method, $context, $error_message);
  if ($result === FALSE) {
    form_set_error('braintree', $error_message);
  }
  elseif (empty($form_state['redirect'])) {
    $form_state['redirect'] = braintree_payment_method_list_url(TRUE);
  }
}

/**
 * Remove payment method form.
 *
 * @param $form
 * @param $form_state
 * @param null $delta
 * @return mixed
 * @throws \Exception
 *
 * @see braintree_payment_method_remove_form_submit()
 */
function braintree_payment_method_remove_form($form, &$form_state, $delta = NULL) {
  $payment_method = braintree_payment_method_load_by_delta($delta);

  if (!$payment_method || braintree_payment_method_is_default($payment_method)) {
    drupal_not_found();
    drupal_exit();
  }

  $form_state['payment_method'] = $payment_method;
  $form_state['delta'] = $delta;

  $form['details'] = array(
    '#markup' => theme('braintree_payment_method', array(
      'payment_method' => $payment_method,
      'delta' => $delta,
    )),
  );

  $title = $question = t('Are you sure you remove this payment method?');

  $form = confirm_form(
    $form,
    $title,
    braintree_payment_method_list_url(),
    $question,
    t('Remove'),
    t('Cancel')
  );

  return $form;
}

/**
 * Submit handler for removing payment method form.
 *
 * @param $form
 * @param $form_state
 * @throws \Exception
 *
 * @see braintree_payment_method_remove_form()
 */
function braintree_payment_method_remove_form_submit($form, &$form_state) {
  $payment_method = $form_state['payment_method'];
  $delta = $form_state['delta'];

  $payment_method_label = theme('braintree_payment_method_label', array(
    'payment_method' => $payment_method,
    'delta' => $delta,
  ));

  $context = array();
  $result = braintree_vault_payment_method_delete($payment_method, $context, $error_message);

  if ($result === FALSE) {
    form_set_error('braintree', $error_message);
  }
  else {
    drupal_set_message(t('@method has been removed.', array(
      '@method' => $payment_method_label,
    )));

    $form_state['redirect'] = braintree_payment_method_list_url(TRUE);
  }
}

/**
 * Make default payment method form.
 *
 * @param $form
 * @param $form_state
 * @param null $delta
 * @return mixed
 * @throws \Exception
 *
 * @see braintree_payment_method_make_default_form_submit()
 */
function braintree_payment_method_make_default_form($form, &$form_state, $delta = NULL) {
  $payment_method = braintree_payment_method_load_by_delta($delta);
  if (!$payment_method || braintree_payment_method_is_default($payment_method)) {
    drupal_not_found();
    drupal_exit();
  }

  $form_state['payment_method'] = $payment_method;
  $form_state['delta'] = $delta;

  $form['details'] = array(
    '#markup' => theme('braintree_payment_method', array(
      'payment_method' => $payment_method,
      'delta' => $delta,
    )),
  );

  $title = t('Dashboard');
  $question = t('Are you sure you would like to set this as the default payment method?');
  $question = '<div class="description">' . $question . '</div>';

  $form = confirm_form(
    $form,
    $title,
    braintree_payment_method_list_url(),
    $question,
    t('Yes'),
    t('Cancel')
  );

  $form['actions']['submit']['#attributes']['class'][] = 'button';
  $form['actions']['submit']['#attributes']['class'][] = 'button--arrow';
  $form['actions']['cancel']['#attributes']['class'][] = 'back-link';

  $form['actions']['submit']['#weight'] = 2;
  $form['actions']['cancel']['#weight'] = 1;

  return $form;
}

/**
 * Submit handler for making default payment method form.
 *
 * @param $form
 * @param $form_state
 * @throws \Exception
 *
 * @see braintree_payment_method_make_default_form()
 */
function braintree_payment_method_make_default_form_submit($form, &$form_state) {
  $payment_method = $form_state['payment_method'];
  $delta = $form_state['delta'];

  $context = array();
  
  $result = braintree_vault_payment_method_make_default($payment_method, $context, $error_message);
  if ($result === FALSE) {
    form_set_error('braintree', $error_message);
  }
  else {
    $payment_method_label = theme('braintree_payment_method_label', array(
      'payment_method' => $payment_method,
      'delta' => $delta,
    ));

    drupal_set_message(t('@method is set as default payment method.', array(
      '@method' => $payment_method_label,
    )));

    $form_state['redirect'] = braintree_payment_method_list_url(TRUE);
  }
}

function braintree_payment_methods_page() {
  global $user;
  $customer = braintree_customer_load_by_account($user);

  $output = theme('braintree_payment_methods', ['customer' => $customer]);
  return $output;
}
