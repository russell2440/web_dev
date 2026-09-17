<?php

		$name =  $_POST["name"];
		$favcar = $_POST["fav-car"];
		

		switch ($favcar) {
		    case "Audi":
		        $message = "Your favorite car is Audi!";
		        break;
		    case "Ford":
		        $message = "Your favorite car is Ford!";
		        break;
		    case "Jeep":
		        $message = "Your favorite car is Jeep!";
		        break;
		    default:
		        $message = "We were too lazy to list all the options!";
		    }

?>
<!DOCTYPE html>
<html>
	<head>
		<title>Ch7 PHP</title>
	
	</head>
	<body>

		<h1>PHP Conditionals and Form Processing - part  3</h1>

		<?php echo $message;?>
		
	</body>

</html>