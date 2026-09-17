<?php
	
			// custom function - accepts an array as args
			function listBuilder($theArray) {

				$listItems = "";

				foreach ($theArray as $value) {

				$listItems =  $listItems ."<li>" . $value . "</li>";
				
				}

				return $listItems;

			}



			// Pretend we got these records from a database
			$records = array("Houses of the holy","Hathaway Live","Dark Side of the Moon","Revolver");	
			$newRecords = array("Album I don't know","Justin B","JZ","Adele");	

		?>


<!DOCTYPE html>
<html>
	<head>
		<title>Ch10</title>
	
	</head>
	<body>

		<h1>PHP Custom Functions - keeping your code clean</h1>



		<ol style="color: red">

			<?php echo listBuilder($newRecords); ?>

		</ol>


		
	</body>
</html>