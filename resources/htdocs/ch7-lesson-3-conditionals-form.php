<!DOCTYPE html>
<html>
	<head>
		<title>Ch7 The form</title>
	
	<style type="text/css">

		input {display: block; margin-bottom: 10px;}

	</style>

	</head>
	<body>

	<h1>PHP Form Processing - part 3</h1>

	<form method="POST" action="ch7-lesson-3-conditionals.php">

		Name: <input type="text" name="name">

		Favorite car: 
		<select name="fav-car">
		  <option value="volvo">Volvo</option>
		  <option value="saab">Saab</option>
		  <option value="mercedes">Mercedes</option>
		  <option value="Audi">Audi</option>
		  <option value="Ford">Ford</option>
		  <option value="Jeep">Jeep</option>
		  <option value="Some crappy card brand">Some crappy card brand</option>
		</select>

		<input type="submit">


	</form>	

		
	</body>
</html>