<!DOCTYPE html>
<html>
	<head>
		<title>Ch3 PHP</title>
	
	</head>
	<body>

		<h1>PHP variables - and more!</h1>

		<?php
			/*

			In PHP, all keywords (e.g. if, else, while, echo, etc.), classes, functions, and user-defined functions are NOT case-sensitive.
			
			*/
			
			ECHO "Hello World!<br>";
			echo "Hello World!<br>";
			EcHo "Hello World!<br>";



			// variables though, ARE case sensitive

			$dog = "red";
			echo "My dog is " . $dog . "<br>";
			echo "My cat is " . $DOG . "<br>";
			echo "My tiger is " . $dOg . "<br>";

		?>

		
	</body>
</html>