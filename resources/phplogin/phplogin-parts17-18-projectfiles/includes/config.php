<?php

/*
 *  CONFIG.PHP
 *  Configuration Settings
*/


// user authentication
$config['salt'] = 'jK7d?3';
$config['session_timeout'] = 500; // seconds

// domain
$config['site_name'] = "Your Website";
$config['site_url'] = "http://www.yourdomain.com";
$config['site_domain'] = "yourdomain.com";

// error reporting
mysqli_report(MYSQLI_REPORT_ERROR);

/*
	Functions
*/
function is_admin()
{
	if ($_SESSION['type'] == 'admin')
	{
		return true;
	}
	else
	{
		return false;
	}
}


?>