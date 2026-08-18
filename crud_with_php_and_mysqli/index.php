<!DOCTYPE html>
<html>
<head>
    <title>CRUD with PHP and MySQLi</title>
    <style>
        body { font-family: sans-serif; margin: 30px; line-height: 1.6; }
        .card { border: 1px solid #ccc; padding: 15px; margin-bottom: 15px; border-radius: 5px; max-width: 500px; }
        .btn { display: inline-block; padding: 10px 15px; color: #fff; background-color: #007bff; text-decoration: none; border-radius: 4px; }
        .btn-danger { background-color: #dc3545; }
    </style>
</head>
<body>
    <h1>Welcome to MAMP Server!</h1>
    <h2>CRUD with PHP and MySQLi</h2>
    <p>Please select an option below:</p>

    <!-- Option 1: Direct Link to View Page -->
    <div class="card">
        <h3>Option 1: View Database Directly</h3>
        <p>Go directly to the view page to see current database records.</p>
        <a href="src/view.php" class="btn">Proceed to View Page</a>
    </div>

    <!-- Option 2: Link to Reset Script (Which redirects to View) -->
    <div class="card">
        <h3>Option 2: Reset & Seed Database</h3>
        <p>Truncate the table, insert sample records, and then automatically proceed to the view page.</p>
        <a href="src/reset_and_seed.php" class="btn btn-danger">Reset, Seed & View</a>
    </div>
</body>
</html>
