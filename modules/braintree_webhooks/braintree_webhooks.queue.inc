<?php

function braintree_webhooks_view_queue() {

  $items = db_query("SELECT q.item_id, q.expire, q.created, IFNULL(f.fail_count, 0) as fail_count FROM {queue} q
    LEFT OUTER JOIN braintree_webhooks_queue_fail_count f ON f.item_id = q.item_id
    WHERE name = :name
    ORDER BY q.created DESC", array(':name' => BRAINTREE_WEBHOOKS_QUEUE))->fetchAll();

  $header = array('ID', 'Expire', 'Created', 'Age', 'Failed attempts');
  $rows = array();

  $css = '.inner-table {border: none; width: auto; }';
  $css .= '.inner-table td {padding: 1px 4px; border: 1px solid #DDD !important;}';
  $css .= '.inner-table th {padding: 1px 4px; text-align: center; border: 1px solid #DDD; background-color: #EFEFEF}';
  drupal_add_css($css, 'inline');

  $build['title']['#markup'] = '<h2>' . t('Main queue') . '</h2>';
  $build['trigger_processing_form'] = drupal_get_form('braintree_webhooks_manually_process_queue_form');

  foreach($items as $item) {
    $rows[] = array(
      'ID'              => $item->item_id,
      'Expire'          => $item->expire == 0 ? '-' : date('d/m/Y H:i', $item->expire),
      'Created'         => date('d/m/Y H:i', $item->created),
      'Age'             => format_interval(time() - $item->created),
      'Failed attempts' => $item->fail_count,
    );
  }

  $build['table']['#markup'] = theme('table', array('header' => $header, 'rows' => $rows, 'empty' => 'There are currently no items in the queue'));

  return $build;

}

function braintree_webhooks_manually_process_queue_form($form, &$form_state) {
  $form['trigger_processing'] = array(
    '#type' => 'submit',
    '#value' => 'Trigger processing',
  );
  return $form;
}

function braintree_webhooks_manually_process_queue_form_submit($form, &$form_state) {
  braintree_webhooks_cron();
  drupal_set_message('Queue processed');
  $form_state['#redirect'] = BRAINTREE_ADMIN_PATH . '/webhooks/queue';
}
