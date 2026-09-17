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
// Echo session variables that were set on previous page
echo "Favorite color is " . $_SESSION["favcolor"] . ".<br>";
echo "Favorite animal is " . $_SESSION["favanimal"] . ".";

?>

<h2>View session with print_r</h2>
<?php
print_r($_SESSION);

$_SESSION["favcolor"] = "blue";
echo "New session variable value:";
print_r($_SESSION);
?>


<?php
// remove all session variables
session_unset(); 

// destroy the session 
session_destroy(); 

echo "<hr>Session destroyed<br>";

print_r($_SESSION);
?>

	

	


</body>
</html>