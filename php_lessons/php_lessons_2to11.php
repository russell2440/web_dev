<?php
    // start the session
    session_start();


    function generate_greeting($first = "Russell", $last = "Shahenian") {
        return "Hello $first $last <br>";
    }
?>

<!DOCTYPE html>
<html>
	<head>
		<title>Ch1 PHP</title>
	
        <style>
            input {
                display: block;
                margin-bottom: 10px;
            }
        </style>
	</head>
	<body>

		<h1>Welcome to PHP!</h1>


		<?php
            /*
            phpinfo();
            echo "<br>";
            phpversion();
            echo "<br>";
            */

            // Ch2 - Your First PHP Code
            echo "Ch2 - Your First PHP Code <br>";

            $name = "Russell S";
            echo "Hello ".$name;
            echo "<br>";
            echo "Hello $name";
            echo "<br>";
			echo "Hello World";
            echo "<br>";
			echo "This is some php, russell";
            echo "<br>";
            echo "<br>";
            echo "<br>";

            // Ch3 - PHP Variables
            echo "Ch3 - PHP Variables <br>";
            // 1. PHP Vars - 1
            echo "1. PHP Vars - 1 <br>";

            /* Keywords, function names, class names and etc are NOT case sensitive */
            ECHO "Hello World"."<br>";
            echo "Hello World"."<br>";
            eCHO "Hello World"."<br>";

            // Variables though are case sensitive.
            $dog = "Bowser";
            echo "My dog is " . $dog . "<br>";
            echo "My cat is " . $DOG . "<br>";
            echo "My tiger is " . $dOg . "<br>";
            echo "<br>";
            echo "<br>";

            // 3. PHP Vars - 3 
            echo "3. PHP Vars - 3 <br>";
            $x = 4;
            $y = 5;
            $z = 6;

            $sum = $x + $y + $z;
            echo "The sum of $x, $y and $z is $sum";
            echo "<br>";

            echo "numerical sum: ".$x+$y+$z."<br>";
            echo "string sum: ".$x.$y.$z."<br>";
            echo "<br>";
            echo "<br>";
        ?>



       <h1>CH5 - PHP Arrays</h1>

        <?php
            // 1. PHP Arrays - 1
            // 2. PHP Arrays - 2
            // 3. PHP Arrays - 3

            //
            // Indexd Arrays - numerically indexed (one dim array)
            //

            $food = array("apple", "banana", "orange", 7);
            echo "The array -food is: <br>";
            var_dump($food);
            echo "<br>";
            $array_size = count($food);
            echo "There are " . $array_size . " items in the array<br>";
            echo "<br>";

            echo "The first item is " . $food[0] . "<br>";
            echo "The second item is " . $food[1] . "<br>";
            echo "The third item is " . $food[2] . "<br>";
            echo "The forth item is " . $food[3] . "<br>";
            echo "<br>";
            echo "<br>";

            for ($i = 0; $i < $array_size; $i++) {
                echo "The item at index $i is " . $food[$i] . "<br>";
            }
            echo "<br>";
            echo "<br>";


            //
            // 4. PHP Arrays - 4
            //

            // Associative Arrays - key/value pairs (maps)
            // Multidimensional Arrays - arrays of arrays (2d array)

            $colors = array("red" => 1, "green" => 2, "blue" => 3);
            echo "The array -colors is: <br>";
            var_dump($colors);
            echo "<br>";
            $array_size = count($colors);
            echo "There are " . $array_size . " items in the array<br>";
            echo "<br>";

            foreach ($colors as $key => $value) {
                echo "The key is $key and the value is $value<br>";
            }
            echo "<br>";
            echo "<br>";
        ?>



       <h1>Ch6 - Processing HTML Forms </h1>
           <!--
           // 1. Processing HTML Forms - 1
           // 2. Processing HTML Forms - 2
           // 3. Processing HTML Forms - 3
           // 4. Processing HTML Forms - 4
           // 5. Processing HTML Forms - 5
           -->
       <h1>Ch7 - Conditional Statements</h1>
           <!--
           // 1. Conditional Statements - 1
           // 2. Conditional Statements - 2
           -->
            <form method="post" action="form_action.php">
                username: <input type="text" name="username"><br>
                password: <input type="password" name="password"><br>
                <input type="submit" value="Submit">
            </form>
            <br>

           <!--
           // 3. Conditional Statements - 3
           -->
            <form method="post" action="form_action.php">
                username: <input type="text" name="username"><br>
                favorite car:
                <select name="favcar">
                    <option value="volvo">Volvo</option>
                    <option value="saab">Saab</option>
                    <option value="mercedes">Mercedes</option>
                    <option value="audi">Audi</option>
                    <option value="chevy">Chevy</option>
                    <option value="ford">Ford</option>
                </select>
                <input type="submit" value="Submit">
            </form>
       <br>


       <h1>Ch8 - PHP Functions</h1>
            <?php
            // 1. PHP Functions - 1
            $message = strlen("Welcome to the party pal!");
            echo $message;
            echo "<br>";

            // 2. PHP Functions - 2
            $message = date("y-m-d");
            echo $message;
            echo "<br>";

            $message = scandir("../php_lessons");
            echo print_r ($message);
            echo "<br>";
            ?>


       <h1>Ch9 - PHP Loops</h1>
            <?php
            // For loop
            for ($i = 0; $i < 10; $i++) {
                echo "The value of i is $i<br>";
            }
            echo "<br>";

            // While loop
            $x = 0;
            while ($x < 10) {
                echo "The value of x is $x<br>";
                $x++;
            }
            echo "<br>";

            // Foreach loop - just for arrays
            $albums = array("Houses of the Holy", "Dark Side Of the Moom", "2112");
            foreach ($albums as $album) {
                echo "The album is $album<br>";
            }
            echo "<br>";
            ?>


       <h1>Ch10 - Custom Functions</h1>
            <?php
            function generate_greeting_2($first = "Russell", $last = "Shahenian") {
                return "Hello $first $last <br>";
            }
            echo generate_greeting();
            echo generate_greeting("John", "Bigbooty");
            echo generate_greeting_2("Neil", "Armstrong");
            echo "<br>";
            ?>


        <h1>Ch11 - PHP Sessions</h1>
            <h2>PHP Sessions allow you to keep track of users</h2>
            <?php
            // 1. PHP Sessions - 1
            // 2. PHP Sessions - 2
            // 3. PHP Sessions - 3
            echo "The session id is " . session_id() . "<br>";
            // set session variables
            $_SESSION['first_name'] = "Russell";
            $_SESSION['last_name'] = "Shahenian";
            echo "The session variables are set<br>"
            ?>

            <h2>Session variables created, now let's see what we have</h2>
            <?php
                // Echo out the session variables
                echo "The session variables are: <br>";
                echo "First name is " . $_SESSION['first_name'] . "<br>";
                echo "Second name is " . $_SESSION['last_name'] . "<br>";
            ?>

            <h2>View session with print_r($_SESSION)</h2>
            <?php
                // print out all the session variables
                print_r($_SESSION);
                echo "<br>";
            ?>

            <h2>Reset session variables and view print_r($_SESSION)</h2>
            <?php
                // print out all the session variables
                $_SESSION['first_name'] = "Harvey";
                $_SESSION['last_name'] = "Mudd";
                print_r($_SESSION);
                echo "<br>";
            ?>
            <h2>View session within brower via another tab</h2>
            <a href="http://localhost:8888/php_session_helper.php">View Session</a>

            <h2>Destroy session and view print_r($_SESSION)</h2>
            <?php
                // remove all session variables
                //session_unset();
                echo "The session variables are unset<br>";
                // print out all the session variables
                print_r($_SESSION);
                echo "<br>";
                // destroy the session
                //session_destroy();
                echo "The session has been destroyed<br>";
                // print out all the session variables
                print_r($_SESSION);
            ?>
       <br>
       <br>



       <h1>Some Lorem Ipsum for your reading pleasure </h1>

		<p>Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum</p>
		
	</body>
</html>
