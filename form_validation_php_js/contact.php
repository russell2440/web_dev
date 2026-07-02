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
        // Create message and email it.
        $to = "example@example.com";
        $subject = "Message from your website domain.com";

        $message =   'Name: ' . $form['name'] . "\n";
        $message .=  'Email: ' . $form['email'] . "\n";
        $message .=  'Phone: ' . $form['phone'] . "\n";
        $message .=  'Fax: ' . $form['fax'] . "\n\n";
        $message .=  'Message: ' . $form['comments'];

        $headers = "From: www.example.com <admin@example.com>\r\n";
        $headers .= "X-Sender: <admin@example.com>\r\n";
        $headers .= "X-Mailer: PHP/" . phpversion() . "\r\n";
        $headers .= "Reply-To: " . $form['email'];

        //echo "Calling mail() in 3";
        //sleep(3);

        mail($to, $subject, $message, $headers);

        //echo "Calling header() in 3";
        //sleep(3);

        // redirect
        header("Location: " . $redirect);

        exit();
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

function contains_bad_str($str_to_test) {
    $bad_strings = array(
        "content-type:",
        "mime-version:",
        "multipart/mixed",
        "Content-Transfer-Encoding:",
        "bcc:",
        "cc:",
        "to:");

    foreach($bad_strings as $bad_string) {
        if (stristr(strtolower($str_to_test), $bad_string))
            return true;
    }
    return false;
}

function contains_newline($str_to_test) {
    if (preg_match("/(%0A|%0D|\\n+|\\r+)/i", $str_to_test) != 0)
            return true;
    return false;
}

?>
