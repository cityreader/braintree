<?php

/**
 * @file
 * API documentation for braintree subscription.
 */

/**
 * Check if subscription purchase is inside an order.
 *
 * @param $order
 * @param $use_subscription
 */
function hook_braintree_subscription_in_order_alter($order, &$use_subscription) {
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $line_item_wrapper = $order_wrapper->commerce_line_items->getIterator()->current();

  if ($line_item_wrapper->commerce_product->type->value() == variable_get('braintree_subscription_product_type')) {
    $field = variable_get('braintree_subscription_field');

    if ($field && $line_item_wrapper->commerce_product->{$field}->value()) {
      $use_subscription = TRUE;
    }
  }
}

/**
 * Alter subscription data before creating or updating a subscription.
 *
 * @param $subscription_data
 * @param $context
 */
function hook_braintree_subscription_data_alter(&$subscription_data, &$context) {
  if ($context['op'] == 'create' && isset($context['order'])) {
    $product_wrapper = commerce_order_get_braintree_product_wrapper($context['order']);

    if ($product_wrapper->field_product_billing_period->value() == 'billed_monthly') {
      $subscription_data['planId'] = 'monthly-plan';
    }
    else {
      $subscription_data['planId'] = 'annual-plan';
    }

    $subscription_data['price'] = $context['price'];
    $subscription_data['merchantAccountId'] = $context['merchant_account_id'];
    $subscription_data['options']['startImmediately'] = TRUE;
  }
}

/**
 * Do custom logic when a subscription is saved successfully.
 *
 * @param $result
 * @param $context
 */
function hook_braintree_subscription_saved($result, &$context) {
  if ($context['op'] == 'create') {
    // Save to session and do some logic later.
    $_SESSION['braintree_subscription_id'] = $result->subscription->id;
  }
}

/**
 * Do custom logic after a subscription is cancelled successfully.
 *
 * @param $result
 * @param $context
 */
function hook_braintree_subscription_cancel($result, &$context) {

}
