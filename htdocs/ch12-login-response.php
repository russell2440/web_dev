<?php
// Start the session
session_start();



/* Checking to see if they put in the right username.

In the real world, we would be checking both username and password and 
we would also be talking to a database that would be storing the usernames and
passwords. 

.. But alas, we are just noob nerds, so we are doing something simple. 

*/

$userName = $_POST["userName"];

if(trim($userName) == "Jimmy") {

    $_SESSION['isLoggedIn'] = true;
    header('Location: ch12-protected-page.php');

} else {

  header('Location: ch12-login.php?badUserCredentials=true');

}

?>

