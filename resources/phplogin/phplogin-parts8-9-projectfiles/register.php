<?php

/*
 *  REGISTER.PHP
 *  Register New Members
*/

// start session / load configs
session_start();
include('includes/config.php');
include('includes/db.php');

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

// form defaults
$error['alert'] = '';
$error['user'] = '';
$error['pass'] = '';
$error['pass2'] = '';
$input['user'] = '';
$input['pass'] = '';
$input['pass2'] = '';

if (isset($_POST['submit']))
{
	// process form
	if ($_POST['username'] == '' || $_POST['password'] == '' || $_POST['password2'] == '')
	{
		// both fields need to be filled in
		if ($_POST['username'] == '') { $error['user'] = 'required!'; }
		if ($_POST['password'] == '') { $error['pass'] = 'required!'; }
		if ($_POST['password2'] == '') { $error['pass2'] = 'required!'; }
		$error['alert'] = 'Please fill in required fields!';
		
		// get data from form
		$input['user'] = htmlentities($_POST['username'], ENT_QUOTES);
		$input['pass'] = htmlentities($_POST['password'], ENT_QUOTES);
		$input['pass2'] = htmlentities($_POST['password2'], ENT_QUOTES);
		
		// show form
		include('views/v_register.php');
	}
	else if ($_POST['password'] != $_POST['password2'])
	{
		// both password fields need to match
		$error['alert'] = 'Password fields must match!';
		
		// get data from form
		$input['user'] = htmlentities($_POST['username'], ENT_QUOTES);
		$input['pass'] = htmlentities($_POST['password'], ENT_QUOTES);
		$input['pass2'] = htmlentities($_POST['password2'], ENT_QUOTES);
		
		// show form
		include('views/v_register.php'); 
	}
	else
	{
		// get and clean data from form
		$input['user'] = $_POST['username'];
		$input['pass'] = $_POST['password'];
		$input['pass2'] = $_POST['password2'];
		
		// insert into database
		if ($stmt = $mysqli->prepare("INSERT members (username, password) VALUES (?,?)"))
		{
			$stmt->bind_param("ss", $input['user'], md5($input['pass'] . $config['salt']));
			$stmt->execute();
			$stmt->close();
			
			// add alert and clear form values
			$error['alert'] = 'Member added successfully!';
			$input['user'] = '';
			$input['pass'] = '';
			$input['pass2'] = '';
			
			// show form
			include('views/v_register.php');
		}
		else
		{
			echo "ERROR: Could not prepare MySQLi statement.";
		}
	}
}
else
{
	// show form
	include('views/v_register.php');
}

// close db connection
$mysqli->close();

?>