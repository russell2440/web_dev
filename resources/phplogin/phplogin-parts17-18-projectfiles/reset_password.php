<?php

/*
 *  RESET_PASSWORD.PHP
 *  Allows users to reset their password
*/

// start session / load configs
session_start();
include('includes/config.php');
include('includes/db.php');

// form defaults
$error['alert'] = '';
$error['email'] = '';
$error['pass'] = '';
$error['pass2'] = '';
$input['email'] = '';
$input['pass'] = '';
$input['pass2'] = '';

if (isset($_GET['key']))
{
	/* 
		User entering a new password
	*/
	if (isset($_POST['submit']))
	{
		// process form
		$input['email'] = htmlentities($_POST['email'], ENT_QUOTES);
		$input['pass'] = htmlentities($_POST['password'], ENT_QUOTES);
		$input['pass2'] = htmlentities($_POST['password2'], ENT_QUOTES);
		$key = htmlentities($_GET['key'], ENT_QUOTES);
		
		// check input
		if ($input['email'] == '' || $input['pass'] == '' || $input['pass2'] == '')
		{
			// all fields need to be filled in
			if ($input['email'] == '') { $error['email'] = 'required!'; }
			if ($input['pass'] == '') { $error['pass'] = 'required!'; }
			if ($input['pass2'] == '') { $error['pass2'] = 'required!'; }
			$error['alert'] = 'Please fill in required fields!';
			
			// show form
			include('views/v_reset_pw2.php');
		}
		else if (!preg_match('/^([0-9a-zA-Z]([-.\w]*[0-9a-zA-Z])*@([0-9a-zA-Z][-\w]*[0-9a-zA-Z]\.)+[a-zA-Z]{2,9})$/', $input['email']))
		{
			// email is invalid
			$error['email'] = 'Please enter a valid email!';
			
			// show form
			include('views/v_reset_pw2.php');
		}
		else
		{
			// make sure email and key match
			$check = $mysqli->prepare("SELECT email FROM members WHERE email = ? AND pw_reset = ?");
			$check->bind_param("ss", $input['email'], $key);
			$check->execute();
			$check->store_result();
			if($check->num_rows == 0)
			{
				// error
				$error['alert'] = "Unfortunately the reset key and the email you have entered do not match, or the password reset key is invalid. Please double check your email address, or try <a href='reset_password.php'>resetting your password again</a>.";
				include('views/v_reset_pw2.php');
			}
			else
			{
				// update password/clearing reset key
				$check = $mysqli->prepare("UPDATE members SET password = ?, pw_reset = '' WHERE email = ?");
				$check->bind_param("ss", md5($input['pass'] . $config['salt']), $input['email']);
				$check->execute();
				$check->close();
				
				// add alert and clear form values
				$error['alert'] = 'Password updated successfully. Please <a href="login.php">login</a>.';
				$input['email'] = '';
				$input['pass'] = '';
				$input['pass2'] = '';
				
				include('views/v_reset_pw2.php');
			}
		
		}
	}
	else
	{
		// show form
		include("views/v_reset_pw2.php");
	}
}
else
{
	/*
		User requesting password reset
	*/
	if (isset($_POST['submit']))
	{
		// process form
		$input['email'] = htmlentities($_POST['email'], ENT_QUOTES);
		
		if ($_POST['email'] == '')
		{
			// email is blank
			$error['email'] = 'required!';
			$error['alert'] = 'Please fill in required fields!';
			
			// show form
			include('views/v_reset_pw.php');
		}
		else if (!preg_match('/^([0-9a-zA-Z]([-.\w]*[0-9a-zA-Z])*@([0-9a-zA-Z][-\w]*[0-9a-zA-Z]\.)+[a-zA-Z]{2,9})$/', $input['email']))
		{
			// email is invalid
			$error['email'] = 'Please enter a valid email!';
			
			// show form
			include('views/v_reset_pw.php');
		}
		else
		{
			// check that email exists in the database
			$check = $mysqli->prepare("SELECT email FROM members WHERE email = ?");
			$check->bind_param("s", $input['email']);
			$check->execute();
			$check->store_result();
			if ($check->num_rows == 0)
			{
				$check->close();
				
				// display error - email isn't in database
				$error['alert'] = "Please check for typos. That email doesn't exist in the database.";
				include('views/v_reset_pw.php');
			}
			else
			{
				$check->close();
				
				// create key
				$key = randomString(16);
				
				// create email
				$subject = "Password reset request from " . $config['site_name'];
				
				$message = "<html><body>";
				$message .= "<p>Hello,</p>";
				$message .= "<p>You (or someone claiming to be you) recently asked that your " . $config['site_name'] . " password be reset. If so, please click the link below to reset your password. If you do not want to reset your password, or if the request was in error, please ignore this message.</p>";
				$message .= "<a href='" . $config['site_url'] . "/reset_password.php?key=" . $key . "'>" . $config['site_url'] . "/reset_password.php?key=" . $key . "</a></p>";
				$message .= "<p>Thanks, <br/>The Administrator, " . $config['site_name'] . "</p>";
				$message .= "</body></html>";
				
				// create email headers
				$headers = "MIME-Version: 1.0 \r\n";
				$headers .= "Content-type: text/html; charset=iso-8859-1 \r\n";
				$headers .= "From: " . $config['site_name'] . " <noreply@" . $config['site_domain'] . ">\r\n";
				$headers .= "X-Sender: <noreply@" . $config['site_domain'] . ">\r\n";
				$headers .= "Reply-To: <noreply@" . $config['site_domain'] . ">\r\n";
				
				// send email
				mail($input['email'], $subject, $message, $headers);
				
				// update database
				$stmt = $mysqli->prepare("UPDATE members SET pw_reset = ? WHERE email = ?");
				$stmt->bind_param("ss", $key, $input['email']);
				$stmt->execute();
				$stmt->close();
				
				// add alert and clear form values
				$error['alert'] = "Password reset sent successfully. Please check your email.";
				$input['email'] = '';
				include("views/v_reset_pw.php");
			}
		}
	}
	else
	{
		// show form
		include("views/v_reset_pw.php");
	}
	
	
}

// close db connection
$mysqli->close();

function randomString($length) {
    $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $string = '';    

    for ($p = 0; $p < $length; $p++) {
        $string .= $characters[mt_rand(0, strlen($characters)-1)];
    }
    return $string;
}

?>