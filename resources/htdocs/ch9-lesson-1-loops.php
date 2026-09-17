<!DOCTYPE html>
<html>
	<head>
		<title>Ch9 PHP Loops</title>
	
	</head>
	<body>

		<h1>PHP Loops</h1>
		<?php
			
			// for loop
			for ($i=0; $i <= 20; $i++) { 
				//echo "I promise to not eat in class again ... $i <br>";
			}



			// while loop
			$x = 0;
			while ($x < 1000) {
				//echo "the while ... $x <br>";
				$x++;
			}



			// foreach loop - just for arrays
			$records = array("Houses of the holy","Hathaway Live","Dark Side of the Moon","Revolver");
			foreach ($records as $value) {
				echo $value . "<br>";
			}

		?>

		

		
	</body>
</html>