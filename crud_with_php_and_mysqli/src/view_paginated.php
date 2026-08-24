
<?php
require_once 'flash_helper.php';
?>

<!DOCTYPE HTML>
<html>
    <head>
        <title>View Records</title>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <style>
            .alert { padding: 12px; margin-bottom: 15px; border-radius: 4px; font-family: sans-serif; }
            .alert-success { background-color: #d4edda; color: #155724; border: 1px solid #c3e6cb; }
            .alert-danger  { background-color: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; }
            .alert-info    { background-color: #d1ecf1; color: #0c5460; border: 1px solid #bee5eb; }
        </style>
    </head>
    <body>
        <?php flash(); ?>

        <h1>View Records</h1>

        <?php
            include("connect_db.php");

            $per_page = 10;

            if ($result = $mysqli->query("SELECT * FROM players ORDER BY id")) {
//print_r($result);echo '<br>';
                if ($result->num_rows != 0) {
                    $total_results = $result->num_rows;
                    $total_pages = ceil($total_results / $per_page);
//echo 'total_results '; print_r($total_results);echo '<br>';
//echo 'total_pages '; print_r($total_pages);echo '<br>';

                    if (isset($_GET['page']) && is_numeric($_GET['page'])) {
                        $show_page = $_GET['page'];
//echo 'show_page '; print_r($show_page);echo '<br>';

                        if ($show_page > 0 && $show_page <= $total_pages) {
                            $start = ($show_page - 1) * $per_page;
                            $end = $start + $per_page;
                        } else {
                            $start = 0;
                            $end = $per_page;
                        }
                    } else {
                        $start = 0;
                        $end = $per_page;
                    }
//echo 'start '; print_r($start);echo '<br>';
//echo 'end '; print_r($end);echo '<br>';

                    // display pagination
                    echo "<p><a href='view.php'>View All</a> | <b>View Page: </b>";
                    for($i = 1; $i <= $total_pages; $i++) {
                        if (isset($_GET['page']) && $_GET['page'] == $i) {
                            echo $i . " ";
                        } else {
                            echo "<a href='view_paginated.php?page=$i'>" . $i . "</a> ";
                        }
                    }

                    // display records in table
                    echo "<table border='1' cellpadding='10'>";
                    echo "<tr><th>ID</th><th>First Name</th><th>Last Name</th><th></th><th></th>";

                    for($i = $start; $i < $end; $i++) {
                        if ($i == $total_results)
                            break;
                        $result->data_seek($i);
                        $row = $result->fetch_object();


                        echo "<tr>";
                        echo "<td>" . $row->id . "</td>";
                        echo "<td>" . $row->firstname . "</td>";
                        echo "<td>" . $row->lastname . "</td>";
                        echo "<td><a href='edit.php?id=" . $row->id . "'>Edit</a></td>";
                        echo "<td><a href='delete.php?id=" . $row->id . "'>Delete</a></td>";
                        echo "</tr>";

//echo "row $i: "; print_r($row); echo '<br>';
                    }

                    echo "</table>";

                    echo "</p>";
                    echo "<a href='add.php'>Add New Record</a> | ";
                    echo "<a href='../index.php'>Back to Main Menu</a>";
                    echo "<br>";
                    echo "<br>";
                } else {
                    echo "No results to display!";
                }
            } else {
                echo "Error :" . $mysqli->error;
            }

            // Closde DB connection
            $mysqli->close();








            /**
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
            **/
        ?>

    </body>
</html>
