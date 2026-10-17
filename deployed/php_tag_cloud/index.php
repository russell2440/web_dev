<?php

// open db connection
include_once __DIR__ . "models/open_db.php";

// load models
include_once __DIR__ . "models/tagcloud.model.php";

// create tagcloud here
$tagcloud = new tagcloud_model($mysqli);
$data['tag_list'] = $tagcloud->get_tag_list();
$data['tag_cloud'] = $tagcloud->get_tag_cloud();


// load view
include_once __DIR__ . "models/tagcloud.view.php";

// close db connection
include_once __DIR__ . "models/close_db.php";

?>
