<?php

/**
 * Implements HOOK_get_braintree_product_wrapper().
 *
 * @param $order_wrapper
 * @param bool|TRUE $use_cache
 * @return mixed
 *
 * @deprecated 
 */
function commerce_order_get_braintree_product_wrapper($order_wrapper, $use_cache = TRUE) {
  static $cache;

  $order_id = $order_wrapper->order_id->value();
  if (!$use_cache || is_null($cache[$order_id])) {
    $cache[$order_id] = FALSE;

    foreach ($line_item_wrapper = $order_wrapper->commerce_line_items as $line_item_wrapper) {
      if ($line_item_wrapper->commerce_product->type->value() == variable_get('braintree_subscription_product_type')) {
        $cache[$order_id] = $line_item_wrapper->commerce_product;
        break;
      }
    }
  }

  return $cache[$order_id];
}
