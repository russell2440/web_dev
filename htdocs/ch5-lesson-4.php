<!DOCTYPE html>
<html>
	<head>
		<title>Ch5 PHP</title>
	
	</head>
	<body>

		<h1>PHP Arrays</h1>

		<?php

			/*
				
				1. Indexed array - numbered array.
				2. Associative array - uses name / value pairs. Named Keys
				3. Multidimensional array - arrays with arrays inside of it.

			*/


				$assocArray = array("Stefan" => "64", "Nick" => "74", "Mary" => "94",);

					

				echo "Stefan's math grade: " . $assocArray['Mary'] . "%";

				echo "Count: " . count($assocArray);


				

		

		?>

		
	</body>
</html>