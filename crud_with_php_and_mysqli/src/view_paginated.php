<?php
require_once 'flash_helper.php';
require_once 'connect_db.php';

$per_page = 10;

// Validate page input
$page = filter_input(INPUT_GET, 'page', FILTER_VALIDATE_INT, [
    'options' => ['default' => 1, 'min_range' => 1]
]);

// 1. Fetch total count via prepared statement
$count_stmt = $mysqli->prepare("SELECT COUNT(*) FROM players");
$count_stmt->execute();
$count_stmt->bind_result($total_results);
$count_stmt->fetch();
$count_stmt->close();

$total_pages = (int) ceil($total_results / $per_page);

// Clamp requested page to max available pages
if ($page > $total_pages && $total_pages > 0) {
    $page = $total_pages;
}

$offset = ($page - 1) * $per_page;

// 2. Fetch target page slice directly from database
$stmt = $mysqli->prepare("SELECT id, firstname, lastname FROM players ORDER BY id LIMIT ? OFFSET ?");
$stmt->bind_param("ii", $per_page, $offset);
$stmt->execute();
$result = $stmt->get_result();
?>
<!DOCTYPE HTML>
<html>
    <head>
        <title>View Records</title>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <style>
            body { font-family: sans-serif; }
            .alert { padding: 12px; margin-bottom: 15px; border-radius: 4px; }
            .alert-success { background-color: #d4edda; color: #155724; border: 1px solid #c3e6cb; }
            .alert-danger  { background-color: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; }
            .alert-info    { background-color: #d1ecf1; color: #0c5460; border: 1px solid #bee5eb; }
            .pagination { margin: 15px 0; }
            .pagination a, .pagination span { padding: 4px 8px; margin-right: 2px; text-decoration: none; border: 1px solid #ccc; }
            .pagination .active { font-weight: bold; background-color: #eee; border-color: #999; }
        </style>
    </head>
    <body>
        <?php flash(); ?>

        <h1>View Records</h1>

        <?php if ($total_results > 0): ?>
            
            <!-- Direct Page Number List -->
            <div class="pagination">
                <a href="view_all.php">View All</a> |
                <b>Page: </b>

                <?php for ($i = 1; $i <= $total_pages; $i++): ?>
                    <?php if ($i == $page): ?>
                        <span class="active"><?= $i ?></span>
                    <?php else: ?>
                        <a href="view_paginated.php?page=<?= $i ?>"><?= $i ?></a>
                    <?php endif; ?>
                <?php endfor; ?>
            </div>

            <!-- Records Table -->
            <table border="1" cellpadding="10">
                <thead>
                    <tr>
                        <th>ID</th>
                        <th>First Name</th>
                        <th>Last Name</th>
                        <th></th>
                        <th></th>
                    </tr>
                </thead>
                <tbody>
                    <?php while ($row = $result->fetch_object()): ?>
                        <tr>
                            <td><?= htmlspecialchars($row->id, ENT_QUOTES, 'UTF-8') ?></td>
                            <td><?= htmlspecialchars($row->firstname, ENT_QUOTES, 'UTF-8') ?></td>
                            <td><?= htmlspecialchars($row->lastname, ENT_QUOTES, 'UTF-8') ?></td>
                            <td><a href="edit.php?id=<?= urlencode($row->id) ?>">Edit</a></td>
                            <td><a href="delete.php?id=<?= urlencode($row->id) ?>">Delete</a></td>
                        </tr>
                    <?php endwhile; ?>
                </tbody>
            </table>

            <br>
            <a href="add.php">Add New Record</a> |
            <a href="../index.php">Back to Main Menu</a>

        <?php else: ?>
            <p>No results to display!</p>
        <?php endif; ?>

        <?php
        $stmt->close();
        $mysqli->close();
        ?>
    </body>
</html>
