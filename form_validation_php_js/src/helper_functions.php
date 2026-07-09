<?php
// Always put 'use' statements at the top of the file
use PHPMailer\PHPMailer\PHPMailer;
use PHPMailer\PHPMailer\Exception;

/**
 * Custom SMTP Mailer function using PHPMailer
 */
function send_smtp_mail($to, $subject, $message, $replyTo) {
    // __DIR__ ensures it looks inside the same folder (src/) for PHPMailer files
    require_once __DIR__ . '/php_mailer/Exception.php';
    require_once __DIR__ . '/php_mailer/PHPMailer.php';
    require_once __DIR__ . '/php_mailer/SMTP.php';

    $mail = new PHPMailer(true);

    try {
        // Server Settings
        $mail->isSMTP();
        $mail->Host       = 'smtp.gmail.com';
        $mail->SMTPAuth   = true;
        $mail->Username   = 'your_gmail_address@gmail.com'; // Replace with your real Gmail
        $mail->Password   = 'your_gmail_app_password';      // Replace with your 16-character App Password
        $mail->SMTPSecure = PHPMailer::ENCRYPTION_STARTTLS;
        $mail->Port       = 587;

        // Routing Configuration
        $mail->setFrom('your_gmail_address@gmail.com', 'Merlin Webmaster');
        $mail->addAddress($to);
        $mail->addReplyTo($replyTo);

        // Content
        $mail->isHTML(false); 
        $mail->Subject = $subject;
        $mail->Body    = $message;
        $mail->CharSet = 'UTF-8';

        $mail->send();
        return true;
    } catch (Exception $e) {
        // Returns false to seamlessly match the old native mail() behavior
        return false;
    }
}

/**
 * Check form for injected spam headers
 */
function contains_bad_str($str_to_test) {
    $bad_strings = array(
        "content-type:",
        "mime-version:",
        "multipart/mixed",
        "Content-Transfer-Encoding:",
        "bcc:",
        "cc:",
        "to:"
    );

    foreach($bad_strings as $bad_string) {
        if (stristr(strtolower($str_to_test), $bad_string))
            return true;
    }
    return false;
}

/**
 * Check form fields for malicious newline injections
 */
function contains_newline($str_to_test) {
    if (preg_match("/(%0A|%0D|\\n+|\\r+)/i", $str_to_test) != 0)
        return true;
    return false;
}
