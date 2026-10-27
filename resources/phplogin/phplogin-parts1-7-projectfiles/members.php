<?php

/*
 *  MEMBERS.PHP
 *  Password protected area for members only
*/

// start session
session_start();
include("includes/config.php");

/*
 * This section below checking if user is logged in/checking for inactivity 
 * may be best put in a reusable function so it is easily reused/updated
*/

// check that the user is logged in
if (!isset($_SESSION['username']))
{
	header("Location: login.php?unauthorized");
}

// check for inactivity
if (time() > $_SESSION['last_active'] + $config['session_timeout'])
{
	// log out user
	session_destroy();
	header("Location: login.php?timeout");
}
else
{
	// update the session variable
	$_SESSION['last_active'] = time();
}

// display view
include("views/v_members.php");

?>