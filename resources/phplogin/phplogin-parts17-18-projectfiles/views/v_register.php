<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
	<head>	
		<title>Register Member</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
		<link href="views/style.css" media="screen" rel="stylesheet" type="text/css">
	</head>
	<body>
		
		<h1>Register Member</h1>
		<div id="content">
			<form action="" method="post">
			<div>
				<?php if ($error['alert'] != '') { 
					echo "<div class='alert'>".$error['alert']."</div>"; } ?>
			
				<label for="username">Username: *</label>
				<input  type="text" name="username" value="<?php echo $input['user']; ?>"><div class="error"><?php echo $error['user']; ?></div>
				
				<label for="email">Email: *</label>
				<input  type="text" name="email" value="<?php echo $input['email']; ?>"><div class="error"><?php echo $error['email']; ?></div>
				
				<label for="type">Member Type: *</label>
				<select name="type">
					<?php echo $select; ?>
				</select>
				<div class="error"><?php echo $error['type']; ?></div>
								
				<label for="password">Password: *</label>
				<input type="password" name="password" value="<?php echo $input['pass']; ?>"><div class="error"><?php echo $error['pass']; ?></div>
				
				<label for="password2">Password (again): *</label>
				<input type="password" name="password2" value="<?php echo $input['pass2']; ?>"><div class="error"><?php echo $error['pass2']; ?></div>
								
				<p class="required">* required fields</p>
				
				<input type="submit" name="submit" class="submit" value="Submit">
			</div>
			</form>
			
			<p><a href="members.php">Back to member's page</a> | 
				<a href="change_password.php">Change Password</a> |
				<a href="logout.php">Log Out</a></p>
		</div>
		
	</body>
</html>