<?php
// Start the session
session_start();


// set session vars
$_SESSION["favcolor"] = "purple";
$_SESSION["favanimal"] = "kittens";
?>

<!DOCTYPE html>
<html>
<head>
	<title>Ch 11 PHP Sessions - lesson 3</title>
</head>

<body>


<h2>Session variables created, now let's look at them</h2>
<?php
// Echo session variables that were set on previous page
echo "Favorite color is " . $_SESSION["favcolor"] . ".<br>";
echo "Favorite animal is " . $_SESSION["favanimal"] . ".";

?>

<h2>View session with print_r</h2>
<?php
print_r($_SESSION);
?>

<h2>Reset session variables</h2>
<?php
$_SESSION["favcolor"] = "blue";
$_SESSION["favanimal"] = "lizards";
echo "New session variable value:";
print_r($_SESSION);
?>


<h2>Destroy session variables</h2>
<?php
// remove all session variables
session_unset(); 

// destroy the session 
session_destroy(); 

echo "Session destroyed<br>";

print_r($_SESSION);
?>


<h2>Use sessions in a variable in a conditional statement</h2>
<?php
// create a new session variable
$_SESSION["favcar"] = "audi";

//nset($_SESSION["favcar"]);



if(isset($_SESSION['favcar'])) {

	echo "It has been set! It is: " . $_SESSION['favcar'];

} else {

	echo "It has NOT been set!";
	$_SESSION["isAnswered"] = "No";

}


?>


<h2>Finally, let's check what is in session with print_r</h2>
<?php
// Display what is in session
print_r($_SESSION);
?>

?>
	


</body>
</html>