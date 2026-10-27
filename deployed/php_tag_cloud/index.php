<?php

// load database helper and model
require_once __DIR__ . "/models/database.php";
require_once __DIR__ . "/models/tagcloud.model.php";

// create database instance and inject the initial sql table file path[cite: 5]
$database = new database(__DIR__ . "/models/database.sql");

// create tagcloud here with strict database type injection
$tagcloud = new tagcloud_model($database);

$data['tag_list'] = $tagcloud->get_tag_list();
$data['tag_cloud'] = $tagcloud->get_tag_cloud();

// load view
require_once __DIR__ . "/views/tagcloud.view.php";

?>
