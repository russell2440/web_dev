<?php
// Start the session
session_start();

// Remove all session variables.
session_unset();

// Destroy the session
session_destroy();
?>


<!DOCTYPE html>
<html>

    <title>ch12 - form processing - logout page.</title>
    <link href='http://fonts.googleapis.com/css?family=Calligraffiti' rel='stylesheet' type='text/css'>
    <link href='ch12_styles.css' rel='stylesheet' type='text/css'>
    
    <body>
        <nav>
            <ul>
                <li><a href="ch12_login.php">Home</a></li>
                <li><a href="ch12_protected_page.php">Protected Page</a></li>
                <li><a href="#">Contact</a></li>
                <li><a href="ch12_logout.php">Logout</a></li>
            </ul>
        </nav>

        <main>
            <p>You have been logged out!</p>
        </main>
    </body>
</html>
