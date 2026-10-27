<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
	<head>	
		<title>Reset Password</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
		<link href="views/style.css" media="screen" rel="stylesheet" type="text/css">
	</head>
	<body>
		
		<h1>Reset Password</h1>
		<div id="content">
			<form action="" method="post">
			<div>
				<?php if ($error['alert'] != '') { 
					echo "<div class='alert'>".$error['alert']."</div>"; } ?>
			
				<p>Forgot your password? Enter your email below, and we'll email you a link to reset your password.</p>
			
				<label for="email">Email: *</label>
				<input  type="text" name="email" value="<?php echo $input['email']; ?>"><div class="error"><?php echo $error['email']; ?></div>
								
				<p class="required">* required fields</p>
				
				<input type="submit" name="submit" class="submit" value="Submit">
			</div>
			</form>
			
			<p><a href="login.php">Login</a></p>
		</div>
		
	</body>
</html>