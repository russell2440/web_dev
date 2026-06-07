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

$conn->close();
?>

</body>
</html>
