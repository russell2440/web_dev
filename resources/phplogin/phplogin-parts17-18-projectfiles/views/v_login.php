<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
	<head>	
		<title>Log In</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
		<link href="views/style.css" media="screen" rel="stylesheet" type="text/css">
	</head>
	<body>
		
		<h1>Log In</h1>
		<div id="content">
			<form action="" method="post">
			<div>
				<?php if ($error['alert'] != '') { 
					echo "<div class='alert'>".$error['alert']."</div>"; } ?>
			
				<label for="username">Username: *</label>
				<input  type="text" name="username" value="<?php echo $input['user']; ?>"><div class="error"><?php echo $error['user']; ?></div>
								
				<label for="password">Password: *</label>
				<input type="password" name="password" value="<?php echo $input['pass']; ?>"><div class="error"><?php echo $error['pass']; ?></div>
								
				<p class="required">* required fields</p>
				
				<input type="submit" name="submit" class="submit" value="Submit">
			</div>
			</form>
			
			<p><a href="reset_password.php">Forgot Password?</a></p>
		</div>
		
	</body>
</html>