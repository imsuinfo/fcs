<?php

/**
 * @file
 * Defines templates page functions.
 */

/**
 * @addtogroup mcneese_management
 * @{
 */

/**
 * Provides the templates overview page.
 */
function mcneese_management_templates_page() {
  $use = user_access('mcneese management templates use');
  $manage = user_access('mcneese management templates manage');
  $template_url = 'admin/content/management/template';

  $output = '';
  $output .= '<div>' . t("Templates provide a way for users to quickly generate content without having to do as much work. Currently, templates are a new feature and are under active development. The interface is subject to change.") . '</div>';
  $output .= '<br>';

  if ($use) {
    $output .= '<h3><a href="' . url($template_url . '/use') . '">' . t("Using Templates") . '</a></h3>';
    $output .= '<div>' . t("You are allowed to used already created templates to assist in creating content.") . '</div>';
    $output .= '<br>';
  }

  if ($manage) {
    $output .= '<h3><a href="' . url($template_url . '/manage') . '">' . t("Managing Templates") . '</a></h3>';
    $output .= '<div>' . t("You are allowed to build custom templates and grant users access to the custom template.") . '</div>';
    $output .= '<br>';
  }

  if (!$use && !$manage) {
    $output .= '<div>' . t("You lack sufficient permissions to perform any template operations.") . '</div>';
  }

  return $output;
}

/**
 * @} End of '@addtogroup mcneese_management'.
 */
