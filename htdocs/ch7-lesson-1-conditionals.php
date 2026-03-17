<?php

		$name =  $_POST["name"];
		$password = $_POST["password"];
		$message;

		if($password != "qwerty") {

			header("Location:  http://www.studioweb.com");
			//echo "<script> window.history.back(); </script>";
			//echo "<script> alert('You failed!'); </script>";

			$message = "Sorry, no show for you!";

		} else {

			echo "<script> alert('You got the password!'); </script>";
			$message = "Welcome to the show!";
		}


?>
<!DOCTYPE html>
<html>
	<head>
		<title>Ch7 PHP</title>
	
	</head>
	<body>

		<h1>PHP Conditionals and Form Processing</h1>

		<?php echo $message;?>
		
	</body>

</html>