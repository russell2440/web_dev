<?php
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

    // check for valid form
    if ($valid_form) {
        // redirect
        header("Location: " . $redirect);
        //exit();
    }
    else {
        // display form
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


?>
