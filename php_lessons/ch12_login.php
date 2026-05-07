<?php
// Start the session
session_start();

?>

<!DOCTYPE html>
<html>
    <head>
        <title>ch12 - form processing.</title>
        <link href='http://fonts.googleapis.com/css?family=Calligraffiti' rel='stylesheet' type='text/css'>
        <link href='ch12_styles.css' rel='stylesheet' type='text/css'>
    </head>
    
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
            <?php
                if (isset($_SESSION["is_logged_in"])) {
                    // The user is already logged in, so don't show the form!
                    echo "You are already logged in buddy!</p>";
                } else {
                    // using Heredoc, to echo out the form.
                    $the_form = <<<THEFORM

                    <p>Welome to WheatBook!</p>
                    <h2>Please enter your username and password to log in:</h2>

                    <form method='post' action='ch12_login_response.php'>
                        <input type='text' name='username' id='username'>
                        <input type='password' name='password'>
                        <input type='submit'>
                    </form>

                    THEFORM;

                    echo $the_form;
                }
            ?>

            <?php
                // parse the query string for message sent to this page.
                $is_block = $_GET["is_block"];
                $bad_user_credentials = $_GET["bad_user_credentials"];

                if (isset($is_block)) {
                    echo "<h2>Ah, ah, ahhhhh ... you need to log in buddy!</h2>";
                    echo "<script>document.getElementById('username').focus();</script>";
                } else if ($bad_user_credentials) {
                    echo "<h2>user name OR password is wrong buddy!</h2>";
                    echo "<script>document.getElementById('username').focus();</script>";
                }
            ?>
        </main>
    </body>
</html>

