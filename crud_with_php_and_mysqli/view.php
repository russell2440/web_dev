<!DOCTYPE HTML PUBLIC>
<html>
    <head>
        <title>View Records</title>
        <meta http-equiv="Content-Type" content="text/html; charset=utf8-8" />
    </head>
    <body>
        <h1>View Records</h1>

        <?php
            include("connect_db.php");

            if ($result = $mysqli->query("SELECT * FROM players ORDER BY id")) {
                if ($result->num_rows > 0) {
                    echo "<table border='1' cellpadding='10'>";
                    echo "<tr><th>ID</th><th>First Name</th><th>Last Name</th></tr>";

                    while ($row = $result->fetch_object()) {
                        echo "<tr>";
                        echo "<td>" . $row->id . "</td>";
                        echo "<td>" . $row->firstname . "</td>";
                        echo "<td>" . $row->lastname . "</td>";
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
    </body>
</html>

