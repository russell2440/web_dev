<?php
    // Start the session
    session_start();

    // This could have code put into an include so you could password protect any page
    // you want.

    // Check session to see if they logged in:
    if (! isset($_SESSION['is_logged_in'])) {
        // The user is NOT logged in, redirect them to the login page immediately
        header('Location: ch12_login.php?is_blocked=true');
        exit;
    }
    // do nothing, the user logged in.
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
            <p>You are now viewing members only content! Aren't you special!</p>
        </main>
    </body>
</html>
