<!DOCTYPE html>
<html>
<head>
<title>PHP Crud Basics</title>
</head>

<body>
<h1>Connecting to Databases with PHP</h1>

<h2>MySQLi vs PDO</h2>

<?php
$servername = 'localhost';
$username = 'htmlnirv_russ';
$password = 'htmlnirv_russ';
$dbname = 'htmlnirvana_crud';

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connnection
if ($conn->connect_error) {
    die('Connection failed: ' . $conn->connect_error);
}

echo 'Success: a proper connection to MySQL was made.';
echo '<br>';
echo 'Host information: '.$conn->host_info;
echo '<br>';
echo 'Protocol version: '.$conn->protocol_version;
echo '<br>';

$sql = "select * from products where product_id = 344";
$sql = "select * from products where 1";
$result = $conn->query($sql);
if ($result->num_rows > 0) {
    echo "<h2 style='color: green'>$result->num_rows records found</h2>";
    while($row = $result->fetch_assoc()) {
        echo 'id: '.$row['id'].' - prod id: '.$row['product_id'].' - prod price: '.$row['product_price'];
        echo '<br>';
    }
    echo '<br>';
}
else {
    echo "<h2 style='color: red'>No records found</h2>";
    echo '<br>';
}

$conn->close();
?>

</body>
</html>
