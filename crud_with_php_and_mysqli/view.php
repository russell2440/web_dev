<?php
require_once 'flash_helper.php';
?>

<!DOCTYPE HTML PUBLIC>
<html>
    <head>
        <title>View Records</title>
        <meta http-equiv="Content-Type" content="text/html; charset=utf8-8" />
                <style>
                    .alert { padding: 12px; margin-bottom: 15px; border-radius: 4px; font-family: sans-serif; }
                    .alert-success { background-color: #d4edda; color: #155724; border: 1px solid #c3e6cb; }
                    .alert-danger  { background-color: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; }
                    .alert-info    { background-color: #d1ecf1; color: #0c5460; border: 1px solid #bee5eb; }
                </style>
    </head>
    <body>
        <!-- Option A: Render ALL queued flash messages automatically -->
        <?php flash(); ?>

        <!-- Option B: Render only a specific message key explicitly -->
        <!-- <?php flash('success'); ?> -->


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
                        echo "<td><a href='edit.php?id=" . $row->id . "'>Edit</a></td>";
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

        <a href="add.php">Add New Record</a>

    </body>
</html>

