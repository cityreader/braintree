<?php

/**
 * @file
 * Default theme implementation for payment method list.
 *
 */
?>
<div class="payment-methods">
  <h3>Payment methods</h3>
  <?php print $default_method; ?>
  <?php foreach ($other_methods as $payment_method): ?>
    <?php print $payment_method; ?>
  <?php endforeach; ?>
</div>
<div class="actions">
  <a class="green-button" href="<?php print $add_url ?>">Add new	payment	type</a>
</div>
