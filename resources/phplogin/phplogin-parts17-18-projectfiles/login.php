<?php

/*
 *  LOGIN.PHP
 *  Log in members
*/

// start session / load configs
session_start();
include('includes/config.php');
include('includes/db.php');

// form defaults
$error['alert'] = '';
$error['user'] = '';
$error['pass'] = '';
$input['user'] = '';
$input['pass'] = '';

if (isset($_POST['submit']))
{
	// if form has been submitted, process form
	if ($_POST['username'] == '' || $_POST['password'] == '')
	{
		// both fields need to be filled in
		if ($_POST['username'] == '') { $error['user'] = 'required!'; }
		if ($_POST['password'] == '') { $error['pass'] = 'required!'; }
		$error['alert'] = 'Please fill in required fields!';
		
		// get data from form
		$input['user'] = htmlentities($_POST['username'], ENT_QUOTES);
		$input['pass'] = htmlentities($_POST['password'], ENT_QUOTES);
		
		// show form
		include('views/v_login.php');
	}
	else
	{
		// get and clean data from form
		$input['user'] = htmlentities($_POST['username'], ENT_QUOTES);
		$input['pass'] = htmlentities($_POST['password'], ENT_QUOTES);
		
		// create query
		if ($stmt = $mysqli->prepare("SELECT members.id, permissions.name FROM members, permissions WHERE username=? AND password = ? AND members.type = permissions.id"))
		{
			$stmt->bind_param("ss", $input['user'], md5($input['pass'] . $config['salt']));
			$stmt->execute();
			$stmt->bind_result($id, $type);
			$stmt->fetch();
			
			// check if there is a match in the database for the user/password combination
			if ($id)
			{
				// close statement
				$stmt->close();
				
				// set session variable
				$_SESSION['id'] = $id;
				$_SESSION['type'] = $type;
				$_SESSION['username'] = $input['user'];
				$_SESSION['last_active'] = time();
				
				// redirect to member's page
				header("Location: members.php");
			}
			else
			{
				// close statement
				$stmt->close();
				
				// username/password incorrect
				$error['alert'] = "Username or password incorrect!";
				
				// show form
				include('views/v_login.php');
			}
		}
		else
		{
			echo "ERROR: Could not prepare MySQLi statement.";
		}
	}
}
else
{
	// check for any variables within the URL
	if (isset($_GET['unauthorized']))
	{
		$error['alert'] = 'Please login to view that page!';
	}
	if (isset($_GET['timeout']))
	{
		$error['alert'] = 'Your session has expired. Please log in again.';
	}

	// if the form hasn't been submitted, show form
	include('views/v_login.php');
}

// close db connection
$mysqli->close();

?>