<?php

/**
 * @file
 * Implement theme functions for payment method.
 */

/**
 * Processes variables for braintree-payment-methods.tpl.php.
 *
 * @see braintree-payment-methods.tpl.php
 */
function template_preprocess_braintree_payment_methods(&$variables) {
  $customer = $variables['customer'];

  $variables['default_method'] = '';
  $variables['other_methods'] = array();

  foreach (braintree_payment_methods_sorted($customer->paymentMethods) as $delta => $payment_method) {
    if (braintree_payment_method_is_default($payment_method)) {
      $variables['default_method'] = theme('braintree_payment_method', array(
        'payment_method' => $payment_method,
        'is_default' => TRUE,
        'delta' => $delta,
        'has_links' => TRUE,
      ));
    }
    else {
      $variables['other_methods'][] = theme('braintree_payment_method', array(
        'payment_method' => $payment_method,
        'delta' => $delta,
        'has_links' => TRUE,
      ));
    }
  }

  $variables['add_url'] = url(braintree_payment_method_url('add'));

}

/**
 * Processes variables for braintree-payment-method.tpl.php.
 *
 * @see braintree-payment-method.tpl.php
 */
function template_preprocess_braintree_payment_method(&$variables) {
  $payment_method = $variables['payment_method'];
  $delta = $variables['delta'];

  if (braintree_payment_method_is_credit_card($payment_method)) {
    $variables['type'] = 'credit_card';
    $variables['card_type'] = $payment_method->cardType;
    // Get last four numbers of masked number.
    $variables['masked_number'] = 'x-' . substr($payment_method->maskedNumber, -4);
    $variables['expiration_date'] = $payment_method->expirationDate;
    $variables['classes_array'][] = 'braintree-payment-method--' . drupal_strtolower($variables['card_type']);
  }
  elseif (braintree_payment_method_is_paypal($payment_method)) {
    $variables['type'] = 'paypal';
    $variables['email'] = $payment_method->email;
    $variables['classes_array'][] = 'braintree-payment-method--paypal';
  }

  if ($variables['is_default']) {
    $variables['classes_array'][] = 'braintree-payment-method--default';

//    if (!empty($payment_method->subscriptions)) {
//      $subscription = braintree_find_default_subscription(array($payment_method));
//
//      $variables['next_bill'] = t('Your next auto-payment will be: $%amount on %date', array(
//        '%amount' => $subscription->nextBillAmount,
//        '%date' => $subscription->nextBillingDate->format(variable_get('date_format_short')),
//      ));
//    }
  }
  else {
    $links = array();
    $links['make-default'] = array(
      'href' => url(braintree_payment_method_url('make_default', $delta)),
      'title' => t('Make primary payment'),
      'weight' => 0,
    );

    $links['remove'] = array(
      'href' => url(braintree_payment_method_url('remove', $delta)),
      'title' => t('Remove'),
      'weight' => 20,
    );
  }

  if (braintree_payment_method_is_credit_card($payment_method)) {
    $links['edit'] = array(
      'href' => url(braintree_payment_method_url('edit', $delta)),
      'title' => t('Edit'),
      'weight' => 10,
    );
  }

  if(isset($variables['is_default']) && $variables['is_default']) {
    $variables['has_links'] = TRUE;
    $links[] = array(
      'title' => 'This is your primary payment method',
      'weight' => -1,
    );
  }

  if ($variables['has_links'] && !empty($links)) {
    // Sort links
    if (count($links) > 1) {
      uasort($links, function($a, $b) { return ($a['weight'] < $b['weight']) ? -1 : 1; });
    }

    $variables['action_links'] = array(
      '#theme' => 'links',
      '#links' => $links,
      '#attributes' => array('class' => array('links', 'inline')),
    );
  }

  $variables['classes_array'][] = 'clearfix';

}

/**
 * Processes variables for theme_braintree_payment_method_label().
 *
 * @see theme_braintree_payment_method_label().
 */
function template_preprocess_braintree_payment_method_label(&$variables) {
  $payment_method = $variables['payment_method'];

  if (braintree_payment_method_is_credit_card($payment_method)) {
    $variables['type'] = 'credit_card';
    $variables['card_type'] = $payment_method->cardType;
    // Get last four numbers of masked number.
    $variables['masked_number'] = 'x-' . substr($payment_method->maskedNumber, -4);
  }
  elseif (braintree_payment_method_is_paypal($payment_method)) {
    $variables['type'] = 'paypal';
    $variables['email'] = $payment_method->email;
  }
}

/**
 * Return HTML for Payment method label.
 *
 * @see template_preprocess_braintree_payment_method_label()
 *
 * @ingroup themeable
 */
function theme_braintree_payment_method_label(&$variables) {
  $payment_method = $variables['payment_method'];

  if ($variables['type'] == 'credit_card') {
    $output = $payment_method->cardType . ' ' . $variables['masked_number'];
  }
  else {
    $output = 'PayPal balance ' . $variables['email'];
  }

  return $output;
}
