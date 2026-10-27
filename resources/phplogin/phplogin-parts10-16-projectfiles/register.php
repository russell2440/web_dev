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
$error['email'] = '';
$error['pass'] = '';
$error['pass2'] = '';
$input['user'] = '';
$input['email'] = '';
$input['pass'] = '';
$input['pass2'] = '';

if (isset($_POST['submit']))
{
	$input['user'] = htmlentities($_POST['username'], ENT_QUOTES);
	$input['email'] = htmlentities($_POST['email'], ENT_QUOTES);
	$input['pass'] = htmlentities($_POST['password'], ENT_QUOTES);
	$input['pass2'] = htmlentities($_POST['password2'], ENT_QUOTES);
		
	// process form
	if ($_POST['username'] == '' || $_POST['password'] == '' || $_POST['password2'] == '' || $_POST['email'] == '')
	{
		// both fields need to be filled in
		if ($_POST['username'] == '') { $error['user'] = 'required!'; }
		if ($_POST['email'] == '') { $error['email'] = 'required!'; }
		if ($_POST['password'] == '') { $error['pass'] = 'required!'; }
		if ($_POST['password2'] == '') { $error['pass2'] = 'required!'; }
		$error['alert'] = 'Please fill in required fields!';
		
		// show form
		include('views/v_register.php');
	}
	else if ($_POST['password'] != $_POST['password2'])
	{
		// both password fields need to match
		$error['alert'] = 'Password fields must match!';
		
		// show form
		include('views/v_register.php'); 
	}
	else if (!preg_match('/^([0-9a-zA-Z]([-.\w]*[0-9a-zA-Z])*@([0-9a-zA-Z][-\w]*[0-9a-zA-Z]\.)+[a-zA-Z]{2,9})$/', $input['email']))
	{
		// email is invalid
		$error['email'] = "Please enter a valid email!";
		
		// display form
		include('views/v_register.php');
	}
	else
	{	
		// check if the email is taken
		$check = $mysqli->prepare("SELECT email FROM members WHERE email = ?");
		$check->bind_param("s", $input['email']);
		$check->execute();
		$check->store_result();
		if ($check->num_rows != 0)
		{
			// email is already in use
			$error['alert'] = "This email is already in use. Please choose a different email address.";
			$error['email'] = "Please choose a different email address.";
			
			// show form
			include('views/v_register.php');
			exit;
		}
		
		// check if the username is taken
		$check = $mysqli->prepare("SELECT username FROM members WHERE username = ?");
		$check->bind_param("s", $input['user']);
		$check->execute();
		$check->store_result();
		if ($check->num_rows != 0)
		{
			// username is already in use
			$error['alert'] = "This username is already in use. Please choose a different username.";
			$error['user'] = "Please choose a different username.";
			
			// show form
			include('views/v_register.php');
			exit;
		}
		
		// insert into database
		if ($stmt = $mysqli->prepare("INSERT members (username, email, password) VALUES (?,?,?)"))
		{
			$stmt->bind_param("sss", $input['user'], $input['email'], md5($input['pass'] . $config['salt']));
			$stmt->execute();
			$stmt->close();
			
			// add alert and clear form values
			$error['alert'] = 'Member added successfully!';
			$input['user'] = '';
			$input['email'] = '';
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