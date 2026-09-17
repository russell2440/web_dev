<?php
	
			/* custom function 

			- A function name can start with a letter or underscore (not a number).
			- Function names are NOT case-sensitive.


			*/


			function greeting($name = "Some Person Greeting") {

				echo "Hey there buddy: $name <br>";
		

			}

			function greeting2($name = "Some Person") {

				echo "Before RETURN <br>";

				return "Hey there buddy: $name <br>";

				echo "AFTER RETURN";
		

			}

			function greeting3($nameFirst = "MyName", $nameLast = "Whatever") {

			
				
			return "Hey there buddy, your name is $nameFirst $nameLast <br>";

		

			}




			
		?>


<!DOCTYPE html>
<html>
	<head>
		<title>Ch10</title>
	
	</head>
	<body>

		<h1>PHP Custom Functions - keeping your code clean</h1>





			<?php 

				/*echo greeting2("Nick");
				echo greeting2("Marc");
				echo greeting2("Mary");
				echo greeting2();

				echo "<br>";*/

				echo greeting2(greeting3());


				

			?>




		
	</body>
</html>