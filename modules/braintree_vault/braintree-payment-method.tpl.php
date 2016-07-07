<?php

/**
 * @file
 * Default theme implementation for payment method item.
 */
?>
<div class="<?php print $classes; ?>">
  <div class="payment-method-summary">
    <?php if ($type == 'credit_card'): ?>
      <span class="icon"></span><span class="label"><?php print $card_type; ?></span> <span class="masked-number"><?php print $masked_number; ?> <span class="expiration-date"><?php print $expiration_date; ?></span>
    <?php else: ?>
      <span class="icon"></span><span class="label">PayPal balance</span> <span class="paypal-account"><?php print $email; ?></span>
    <?php endif; ?>
  </div>

  <?php if (isset($action_links)): ?>
    <?php print render($action_links); ?>
  <?php endif; ?>
</div>
