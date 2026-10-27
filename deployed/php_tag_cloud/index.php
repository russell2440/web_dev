<?php

// load database helper and model
require_once __DIR__ . "/models/database.php";
require_once __DIR__ . "/models/tagcloud.model.php";

// 1. Here is where the $database variable is created:
$database = new database();

// 2. Here is where that $database variable is passed into the model:
$tagcloud = new tagcloud_model($database);

$data['tag_list'] = $tagcloud->get_tag_list();
$data['tag_cloud'] = $tagcloud->get_tag_cloud();

// load view
require_once __DIR__ . "/views/tagcloud.view.php";

?>
