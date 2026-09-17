
		<?php
            /*
            There are a bunch of predifined veriables in PHP that are called
            superglobals. These are variables that are always available to
            any PHP script. 

            The PHP superglobals are:

            $_GET - The GET variables
            $_POST - The POST variables
            $_COOKIE - The COOKIE variables
            $_REQUEST - The REQUEST variables
            $_SERVER - The SERVER variables
            $_FILES - The FILES variables
            $_ENV - The ENV variables
            $_SESSION - The SESSION variables
            $_GLOBALS - The GLOBALS variables

            To process forms we use the $_POST and $_GET variables. 
            This is a superglobal that is only available when the form is submitted.

            reference: https://www.w3schools.com/php/php_superglobals.asp

            */

            var_dump($_POST);
            echo "<br>";
            var_dump($_GET);
            echo "<br>";
            if (array_key_exists('password', $_POST)) {
                $username = $_POST['username'];
                $password = $_POST['password'];
                if ($password != "1234") {
                    //header("Location: http://www.studioweb.com");
                    //echo "<script>window.history.back()</script>";
                    echo "<script>alert('The password is NOT CORRECT')</script>";
                    $message =  "The password is NOT CORRECT";
                }
                else {
                    echo "<script>alert('The password is correct')</script>";
                    $message =  "Welcome to the party Pal!";
                }
            }
            else {
                $username = $_POST['username'];
                $favcar = $_POST['favcar'];
                switch ($favcar) {
                    case "volvo":
                        $message = "You like Volvo";
                        break;
                    case "saab":
                        $message =  "You like Saab";
                        break;
                    case "mercedes":
                        $message =  "You like Mercedes";
                        break;
                    case "audi":
                        $message =  "You like Audi";
                        break;
                    case "chevy":
                        $message =  "You like Chevy";
                        break;
                    case "ford":
                        $message =  "You like Ford";
                        break;
                    default:
                        $message =  "You like something else";
                        break;
                }
            }
        ?>

<!DOCTYPE html>
<html>
	<head>
		<title>Form Action</title>
	</head>
	<body>

		<h1>Form Action</h1>

        <?php
            echo $message;
            echo "<br>";
        ?>

	</body>
</html>
