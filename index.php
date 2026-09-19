
<?php
// Example of embedding index.html via an iframe inside PHP output
/*
header("Location: deployed/crud_with_php_and_mysqli/index.php");
exit();
header("Location: deployed/form_validation_php_js/index.php");
 */
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>PHP Wrapper</title>
</head>
<body>
    <h1>Main Dashboard</h1>
    <div>
        <a href="deployed/crud_with_php_and_mysqli/index.php">Crud with php and mySQL</a>
    </div>
    <div>
        <a href="deployed/form_validation_php_js/index.php">Form validation php and JS</a>
    </div>
    <div>
        <a href="deployed/my_website_v1/index.html">Static site</a>
    </div>
</body>
</html>
