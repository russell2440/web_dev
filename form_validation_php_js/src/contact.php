<?php
// Pull in validation and email functions.
require_once 'helper_functions.php';

// variables
$error_open = "<label class='error'>";
$error_close = "</label>";
$valid_form = TRUE;
$redirect = "success.php";

$form_elements = array('name', 'phone', 'fax', 'email', 'comments');
$required = array('name', 'phone', 'email');


foreach ($required as $require)
{
    $error[$require] = '';
}


if (isset($_POST['submit']))
{
    // process form

    // get form data
    foreach ($form_elements as $element)
    {
        $form[$element] = htmlspecialchars($_POST[$element]);
    }

    // check form elements
    {
        // check required fields
        if ($form['name'] == '') {
            $error['name'] = $error_open . "Please fill in all required fields!" . $error_close;
            $valid_form = FALSE;
        }
        if ($form['phone'] == '') {
            $error['phone'] = $error_open . "Please fill in all required fields!" . $error_close;
            $valid_form = FALSE;
        }
        if ($form['email'] == '') {
            $error['email'] = $error_open . "Please fill in all required fields!" . $error_close;
            $valid_form = FALSE;
        }

        // check formatting
        $phone_re = '/^(\+?1-?)?(\([2-9]([02-9]\d|1[02-9])\)|[2-9]([02-9]\d|1[02-9]))-?[2-9]\d{2}-?\d{4}$/';
        if ($error['phone'] == '' && !preg_match($phone_re, $form['phone'])) {
            $error['phone'] = $error_open . "Please enter a valid phone number!" . $error_close;
            $valid_form = FALSE;
        }
        $email_re = '/^([0-9a-zA-Z]([-.\w]*[0-9a-zA-Z])*@([0-9a-zA-Z][-\w]*[0-9a-zA-Z]\.)+[a-zA-Z]{2,9})$/';
        if ($error['email'] == '' && !preg_match($email_re, $form['email'])) {
            $error['email'] = $error_open . "Please enter a valid email!" . $error_close;
            $valid_form = FALSE;
        }

    }

    // Check form for bad data
    {
        if (contains_bad_str($form['name']) ||
            contains_bad_str($form['phone']) ||
            contains_bad_str($form['email']) ||
            contains_bad_str($form['fax']) ||
            contains_bad_str($form['comments'])) {
            $valid_form = false;
        }
        if (contains_newline($form['name']) ||
            contains_newline($form['phone']) ||
            contains_newline($form['email']) ||
            contains_newline($form['fax'])) {
            $valid_form = false;
        }
    }

//print_r($form);

    // Process valid form
    // or display form again
    if ($valid_form) {
        // Setup email routing parameters.

        $to = "russ85226@gmail.com";

        $infinity_free_domain = "merlin.gt.tc";
        $subject = "Message from your Merlin website " . $infinity_free_domain;

        $message =   'Name: ' . $form['name'] . "\n";
        $message .=  'Email: ' . $form['email'] . "\n";
        $message .=  'Phone: ' . $form['phone'] . "\n";
        $message .=  'Fax: ' . $form['fax'] . "\n\n";
        $message .=  'Message: ' . $form['comments'];

        $reply_to = $form['email'];

        // Drop-in wrapper function executing our SMTP logic.
        if (send_smtp_mail($to, $subject, $message, $reply_to)) {
            // Redirect on successful transmisison.
            header("Location: " . $redirect);
            exit();
        } else {
            // Default back to rendering the form on
            // SMTP authentication or connection fail.
            $error['email'] = $error_open . "Mail transmission failed. Please try again later." . $error_close;
            include('form.php');
        }
    }
    else {
        // Display form with validation errors.
        include('form.php');
    }
}
else
{
    foreach ($form_elements as $element)
    {
        $form[$element] = '';
    }

    // display form
    include('form.php');
}
