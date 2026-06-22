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
