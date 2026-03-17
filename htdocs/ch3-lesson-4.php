<!DOCTYPE html>
<html>
	<head>
		<title>Ch3 PHP</title>
	
	</head>
	<body>

		<h1>PHP variables - lesson 4</h1>

		<?php
		
			/*
				http://php.net/manual/en/language.types.intro.php

				Rules for PHP variables:

			    A variable starts with the $ sign, followed by the name of the variable
			    A variable name must start with a letter or the underscore character
			    A variable name cannot start with a number
			    A variable name can only contain alpha-numeric characters and underscores (A-z, 0-9, and _ )
			    Variable names are case-sensitive ($age and $AGE are two different variables)

			*/

			

				// PHP has booleans too! Case-insensitive	
				$beatlesIsCool = true;
				$cultureClubIsCool = false;

				/*PHP conditionals with true and false*/
				
				if($beatlesIsCool) {

					echo "Is it true!";

				} else {

					echo "Sadly, it is false.";
				}

				



		?>

		
	</body>
</html>