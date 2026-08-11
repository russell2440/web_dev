<?php
    function render_edit_form($firstname = '', $lastname = '', $error = '', $id = '')
    {
?>
        <!DOCTYPE HTML >
        <html>
            <head>
                <title>
                    <?php echo 'Edit Record'; ?>
                </title>
                <meta http-equiv="Content-Type" content="text/html; charset=utf8-8" />
            </head>
            <body>
                <h1><?php echo 'Edit Record'; ?></h1>
                <?php
                    if ($error != '') {
                        echo "<div style='padding: 4px; border: 1px solid red; color: red'>" . $error . "</div>";
                    }
                ?>
                <form action='' method='post'>
                <div>
                    <input type="hidden" name="id" value="<?php echo $id; ?>" />
                    <p>ID: <?php echo $id; ?></p>
                    <strong>First Name: *</strong>
                    <input type='text' name='firstname' value="<?php echo $firstname ?>" />
                    <br>
                    <strong>Last Name: *</strong>
                    <input type='text' name='lastname' value="<?php echo $lastname ?>" />
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
    include('connect_db.php');

    if (isset($_GET['id'])) {
        if (isset($_POST['submit'])) {
            if (is_numeric($_POST['id'])) {
                // 1. Session must start before any HTML or output.
                session_start();

                $id = $_POST['id'];
                $firstname = htmlentities($_POST['firstname'], ENT_QUOTES);
                $lastname = htmlentities($_POST['lastname'], ENT_QUOTES);

                if ($firstname == '' ||  $lastname == '') {
                    $error = 'ERROR: Please fill in all required fields!';
                    render_edit_form($firstname, $lastname, $error, $id);
                } else {
                    if ($stmt = $mysqli->prepare("UPDATE players SET firstname = ?, lastname = ? WHERE id=?")) {
                        $stmt->bind_param('ssi', $firstname, $lastname, $id);
                        $stmt->execute();
                        $stmt->close();

                        // Set the flash message in the session.
                        $_SESSION['flash_message'] = 'SUCESS: '.$firstname.' '.$lastname.'(ID: '.$id.')';
                    } else {
                        $_SESSION['flash_error'] = 'ERROR: Could not prepare SQL statement.';
                    }

                    header("Location: view.php");
                    exit();
                }
            } else {
                echo "Error: posted id";
            }
        } else {
            // edit existing record
            if (is_numeric($_GET['id']) && $_GET['id'] > 0) {
                // Query database
                $id = $_GET['id'];

                if ($stmt = $mysqli->prepare("SELECT * FROM players WHERE id=?")) {
                    $stmt->bind_param('i', $id);
                    $stmt->execute();

                    $stmt->bind_result($id, $firstname, $lastname);
                    $stmt->fetch();

                    render_edit_form($firstname, $lastname, NULL, $id);

                    $stmt->close();
                } else {
                    echo 'ERROR: Could not prepare SQL statement.';
                }
            } else {
                header("Location: view.php");
            }
        }
    } else {
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
                } else {
                    echo 'ERROR: Could not prepare SQL statement.';
                }

                header("Location: view.php");
            }
        } else {
            render_add_form();
        }
    }

?>
