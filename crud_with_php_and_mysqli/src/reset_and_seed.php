<?php
require_once 'connect_db.php';
require_once 'flash_helper.php';

// 1. Truncate table
$mysqli->query("TRUNCATE TABLE players");

// 2. Insert 100 players
$players = [
    ['Michael', 'Smith'], ['Lisa', 'Johnson'], ['David', 'Williams'], ['Mary', 'Brown'],
    ['John', 'Jones'], ['Karen', 'Garcia'], ['James', 'Miller'], ['Susan', 'Davis'],
    ['Robert', 'Rodriguez'], ['Patricia', 'Martinez'], ['Mark', 'Hernandez'], ['Linda', 'Lopez'],
    ['William', 'Gonzalez'], ['Donna', 'Wilson'], ['Richard', 'Anderson'], ['Michelle', 'Thomas'],
    ['Thomas', 'Taylor'], ['Cynthia', 'Moore'], ['Jeffrey', 'Jackson'], ['Angela', 'Martin'],
    ['Steven', 'Lee'], ['Sandra', 'Perez'], ['Joseph', 'Thompson'], ['Pamela', 'White'],
    ['Timothy', 'Harris'], ['Brenda', 'Sanchez'], ['Kevin', 'Clark'], ['Deborah', 'Ramirez'],
    ['Charles', 'Lewis'], ['Stephanie', 'Robinson'], ['Daniel', 'Walker'], ['Laura', 'Young'],
    ['Paul', 'Allen'], ['Christine', 'King'], ['Kenneth', 'Wright'], ['Sharon', 'Scott'],
    ['Brian', 'Torres'], ['Carol', 'Nguyen'], ['Scott', 'Hill'], ['Tammy', 'Flores'],
    ['Gregory', 'Green'], ['Nancy', 'Adams'], ['Anthony', 'Nelson'], ['Teresa', 'Baker'],
    ['Edward', 'Hall'], ['Barbara', 'Rivera'], ['Christopher', 'Campbell'], ['Denise', 'Mitchell'],
    ['Donald', 'Carter'], ['Cheryl', 'Roberts'], ['Ronald', 'Gomez'], ['Kimberly', 'Phillips'],
    ['Gary', 'Evans'], ['Lori', 'Turner'], ['Stephen', 'Diaz'], ['Debra', 'Parker'],
    ['Eric', 'Cruz'], ['Julie', 'Edwards'], ['Larry', 'Collins'], ['Paula', 'Reyes'],
    ['George', 'Stewart'], ['Tracy', 'Morris'], ['Douglas', 'Morales'], ['Kathy', 'Murphy'],
    ['Todd', 'Cook'], ['Diane', 'Rogers'], ['Frank', 'Gutierrez'], ['Janet', 'Ortiz'],
    ['Raymond', 'Morgan'], ['Robin', 'Cooper'], ['Dennis', 'Peterson'], ['Rhonda', 'Bailey'],
    ['Jerry', 'Reed'], ['Dawn', 'Kelly'], ['Craig', 'Howard'], ['Connie', 'Ramos'],
    ['Patrick', 'Kim'], ['Wanda', 'Cox'], ['Peter', 'Ward'], ['Anita', 'Richardson'],
    ['Walter', 'Watson'], ['Jill', 'Brooks'], ['Alan', 'Chavez'], ['Sheila', 'Wood'],
    ['Philip', 'James'], ['Tina', 'Bennett'], ['Terry', 'Gray'], ['Sherry', 'Mendoza'],
    ['Randy', 'Ruiz'], ['Valerie', 'Hughes'], ['Russell', 'Price'], ['Vicki', 'Alvarez'],
    ['Johnny', 'Castillo'], ['Becky', 'Sanders'], ['Carl', 'Patel'], ['Terri', 'Myers'],
    ['Arthur', 'Long'], ['Judy', 'Ross'], ['Lawrence', 'Foster'], ['Peggy', 'Jimenez']
];

$stmt = $mysqli->prepare("INSERT INTO players (firstname, lastname) VALUES (?, ?)");
foreach ($players as $player) {
    $stmt->bind_param("ss", $player[0], $player[1]);
    $stmt->execute();
}

$stmt->close();
$mysqli->close();

// Set a success message before redirecting
flash_set('success', 'Database successfully reset and seeded with 100 players!');

// Redirect straight to view page
header("Location: view.php");
exit();
?>
