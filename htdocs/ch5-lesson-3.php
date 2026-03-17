<!DOCTYPE html>
<html>
	<head>
		<title>Ch5 PHP</title>
	
	</head>
	<body>

		<h1>PHP Arrays</h1>

		<?php

			$foodCart = array("bananas","burgers","chicken", "butter");

			$arraySize = count($foodCart);

			for($x = 0; $x < $arraySize; $x++) {

				echo $foodCart[$x] . "<br>";	

			}

		

		?>

		
	</body>
</html>