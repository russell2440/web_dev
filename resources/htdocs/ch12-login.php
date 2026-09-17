<?php
// Start the session
session_start();

?>


<!DOCTYPE html>
<html>
	<head>
	   	<title>ch12 - form processing.</title>
      <link href='http://fonts.googleapis.com/css?family=Calligraffitti' rel='stylesheet' type='text/css'>
      <link rel="stylesheet" type="text/css" href="ch12-styles.css">
  </head>

<body>

<nav>
    <ul>
    <li><a href="ch12-login.php">Home</a></li>
    <li><a href="ch12-protected-page.php">Protected page</a></li>
    <li><a href="#">Contact</a></li>
    <li><a href="ch12-logout.php">Log Out</a></li>
  </ul>
</nav>



<main>


<?php

if(isset($_SESSION['isLoggedIn'])) {

    //The user logged in - don't show form and confuse the poor guy!

  echo "<p>You are already logged in buddy!</p>";

} else {


// Using Heredoc, to echo out the form. 
$theForm = <<<THEFORM


  <p>Welcome to WheatBook!</p>
  <h2>Please enter your user name and password to log in:</h2>

  <form method='post' action='ch12-login-response.php'>

      <input type='text' name='userName' id='username'>
      <input type='password' name='password'>
      <input type='submit'>

  </form>


THEFORM;

echo $theForm;
}

?>



  <?php

    // using the querystring to send messages back to this login page.

    $isBlock =  $_GET["isBlock"];
    $badUserCredentials = $_GET["badUserCredentials"];

    if(isset($isBlock)) {

      echo "<h2>Ah, ah, aaaaah ... you need to log in buddy!</h2>";
      echo "<script>document.getElementById('username').focus();</script>";

    } else if($badUserCredentials) {

      echo "<h2>User name OR password is wrong buddy!</h2>";
      echo "<script>document.getElementById('username').focus();</script>";

    }


  ?>


</main>

	</body>
</html>