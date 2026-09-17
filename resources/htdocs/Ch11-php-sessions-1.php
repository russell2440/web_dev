<?php
// Start the session
session_start();
?>

<!DOCTYPE html>
<html>
<head>
	<title>Ch 11 PHP Sessions</title>
</head>

<body>
<?php
// Set session variables
$_SESSION["favcolor"] = "red";
$_SESSION["favanimal"] = "frogs";
echo "Session variables are set.";
?>
	
<h2>PHP sessions allow you to track people</h2>

<a href="http://localhost/Ch11-php-sessions-2.php">View Session</a>
	


</body>
</html>