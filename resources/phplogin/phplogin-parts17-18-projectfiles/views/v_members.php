<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
	<head>	
		<title>Members Only!</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
		<link href="views/style.css" media="screen" rel="stylesheet" type="text/css" />
	</head>
	<body>
		
		<?php if (is_admin()) { ?>
			<h1>Admin Area</h1>
			<div id="content">
				<p>You have successfully logged in to the admin's area.</p>
				<p><a href="register.php">Register Member</a> | 
					<a href="change_password.php">Change Password</a> |
					<a href="logout.php">Log Out</a></p>
			</div>
		<?php } else { ?>			
			<h1>Members Area</h1>
			<div id="content">
				<p>You have successfully logged in to the member's area.</p>
				<p><a href="change_password.php">Change Password</a> |
					<a href="logout.php">Log Out</a></p>
			</div>	
		<?php } ?>
		
	</body>
</html>