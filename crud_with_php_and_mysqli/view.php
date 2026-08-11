<?php
// Always start the session at the top of the page.
session_start();
?>

<!DOCTYPE HTML PUBLIC>
<html>
    <head>
        <title>View Records</title>
        <meta http-equiv="Content-Type" content="text/html; charset=utf8-8" />
    </head>
    <body>

<?php
// 2. Check for and display success message.
if (isset($_SESSION['flash_message'])) {
    echo "<div class='alert_success'>".htmlentities($_SESSION['flash_message'])."</div>";
    // clear it so it only shows once
    unset($_SESSION['flash_message']);
}

// 3. Check for and display error message.
if (isset($_SESSION['flash_error'])) {
    echo "<div class='alert_error'>".htmlentities($_SESSION['flash_error'])."</div>";
    // clear it so it only shows once
    unset($_SESSION['flash_error']);
}
?>

        <h1>View Records</h1>

        <?php
            include("connect_db.php");

            if ($result = $mysqli->query("SELECT * FROM players ORDER BY id")) {
                if ($result->num_rows > 0) {
                    echo "<table border='1' cellpadding='10'>";
                    echo "<tr><th>ID</th><th>First Name</th><th>Last Name</th><th></th><th></th></tr>";

                    while ($row = $result->fetch_object()) {
                        echo "<tr>";
                        echo "<td>" . $row->id . "</td>";
                        echo "<td>" . $row->firstname . "</td>";
                        echo "<td>" . $row->lastname . "</td>";
                        echo "<td><a href='records.php?id=" . $row->id . "'>Edit</a></td>";
                        echo "<td><a href='delete.php?id=" . $row->id . "'>Delete</a></td>";
                        echo "</tr>";
                    }

                    echo "</table>";
                } else {
                    echo "No results to display!";
                }
            } else {
                echo "Error: " . $mysqli->error;
            }

            $mysqli->close();
        ?>

        <a href="records.php">Add New Record</a>;

    </body>
</html>

