
<?php
// Start the session
session_start();

    /* Checking that they put in the correct username...
     * In the real world we would be checking both the username and password
     * and talking to a database that would be storing the usernames and
     * passwords.
     */
    
    $username = $_POST["username"];
    
    if (trim($username == "Harcord")) {
        $_SESSION["is_loggined_in"] = true;
        header('Location: ch12_protected_page.php');
    } else {
        header('Location: ch12_login.php?bad_user_credentionals=true');
    }

?>
