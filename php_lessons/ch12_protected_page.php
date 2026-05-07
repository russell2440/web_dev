<?php
// Start the session
session_start();

// this could have code put into an include so you could password protect any page
// you want.

// Check session to see if they logged in:

if (isset($is_logged_in)) {
    // do nothing, the user logged in.
} else {
    header('Location: ch12_login.php?is_blocked=true');
}
?>

<!DOCTYPE html>
<html>

    <title>ch12 - form processing - protected page.</title>
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
            <p>You are new viewing members only content! Aren't you special!</p>
        </main>
    </body>
</html>
