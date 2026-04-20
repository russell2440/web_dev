<?php
    // start the session
    session_start();
?>
<DOCTYPE html>
<html>
	<head>
		<title>PHP Session Helper</title>
	</head>
	<body>
        <h1>Ch11 - PHP Sessions</h1>
        <h2>PHP Sessions allow you to keep track of users</h2>
        <?php
            echo "The session id is " . session_id() . "<br>";
            // Echo out the session variables
            echo "The session variables are: <br>";
            echo "First name is " . $_SESSION['first_name'] . "<br>";
            echo "Second name is " . $_SESSION['last_name'] . "<br>";
            ?>
    </body>
</html>
