<?php
    // Put this at the VERY top of add.php!

    function render_add_form($firstname = '', $lastname = '', $error = '')
    {
?>
        <!DOCTYPE HTML >
        <html>
            <head>
                <title>
                    <?php echo 'Add Record'; ?>
                </title>
                <meta http-equiv="Content-Type" content="text/html; charset=utf8-8" />
            </head>
            <body>
                <h1><?php echo 'Add Record'; ?></h1>
                <?php
                    if ($error != '') {
                        echo "<div style='padding: 4px; border: 1px solid red; color: red'>" . $error . "</div>";
                    }
                ?>
                <form action='' method='post'>
                <div>
                    <strong>First Name: *</strong>
                    <input type='text' name='firstname' value="<?php echo $firstname; ?>" />
                    <br>
                    <strong>Last Name: *</strong>
                    <input type='text' name='lastname' value="<?php echo $lastname; ?>" />
                    <p>* required</p>
                    <input type='submit' name='submit' value="Submit" />
                </div>
                </form>
            </body>
        </html>
<?php
    }
?>



<?php

/***************************************
// --- TEMPORARY DEBUG BLOCK ---
if (1) {
echo "<h3>Debug Info:</h3>";
echo "<strong>Request Method:</strong> " . $_SERVER['REQUEST_METHOD'] . "<br>";
echo "<strong>GET array count:</strong> " . count($_GET) . "<br>";
echo "<strong>POST array count:</strong> " . count($_POST) . "<br>";

echo "<pre>POST Data: ";
print_r($_POST);
echo "</pre>";

echo "<pre>GET Data: ";
print_r($_GET);
echo "</pre>";

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    echo "SUCCESS: Server received a POST request!<br>";
} else {
    echo "NOTICE: Server received a GET request.<br>";
}
}
//----------------------------------------
****************************************/

    require_once('connect_db.php');
    require_once('flash_helper.php');

    // create new record
    if (isset($_POST['submit'])) {
        $firstname = htmlentities($_POST['firstname'], ENT_QUOTES);
        $lastname = htmlentities($_POST['lastname'], ENT_QUOTES);

        if ($firstname == '' ||  $lastname == '') {
            $error = 'ERROR: Please fill in all required fields!';
            render_add_form($firstname, $lastname, $error);
        } else {
            if ($stmt = $mysqli->prepare("INSERT players (firstname, lastname) VALUES (?, ?)")) {
                $stmt->bind_param('ss', $firstname, $lastname);
                $stmt->execute();
                $stmt->close();

                // Set a success flash message
                flash_set('success', "Added new player: {$firstname} {$lastname}", 'success');
            } else {
                flash_set('error', 'Could not prepare INSERT SQL statement.', 'danger');
            }

            header("Location: view.php");
            exit();
        }
    } else {
        render_add_form();
    }

?>
