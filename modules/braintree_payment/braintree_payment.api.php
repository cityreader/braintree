<?php

/**
 * @file
 * API documentation for braintree payment form.
 */

/**
 * Alter styles of Braintree Hosted Fields.
 *
 * @param $styles
 *
 * @see braintree_hostedfields_styles()
 * @see https://developers.braintreepayments.com/javascript+php/guides/hosted-fields/styling
 */
function hook_braintree_hostedfields_styles_alter(&$styles) {
  $styles = array(
    // Styling a specific field
    ".number" => array(
      "font-family" =>  "sans-serif"
    ),
  );
}
