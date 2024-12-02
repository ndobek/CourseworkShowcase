<?php
session_start();
require_once __DIR__ . '/../Common/util.php';

if (!HasPermission("Messaging"))
    header('Location:  ../index.php');
?>
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet"
        href="https://thor.cnt.sast.ca/~ndobek10/danger_breakfast/cmpe2550/commonFiles/V2/standardICALayout.css">
    <link rel="stylesheet" href="../Common/style.css">
    <title>Lab 3 - Nathaniel Dobek</title>
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.7.1/jquery.min.js"></script>
    <script src="messages.js">    </script>
    <script src="../Common/util.js"></script>
</head>

<body>
    <div id="site">
        <header>
            <h1>ND - Lab 3 - Messages</h1>
        </header>
        <div id="content">
            <div id="options" class="section loginInfo">
                <label for="filter">Filter:</label><input id="filter" type="text" value="">
                <button id="filterButton" type="button" class="fullGridWidth">Filter Messages</button>
                <label for="message">Message:</label><input id="message" type="text" value="">
                <button id="messageButton" type="button" class="fullGridWidth">Add Message</button>
            </div>
            <div class="section" id="status" style="text-align:center"></div>
            <div class="section" id="messages"></div>
            <nav class="section">
                <div class="fullGridWidth"><a href="../index.php">Main Page</a></div>
            </nav>
        </div>
        <footer>
            &copy; Nathaniel Dobek<br>
            <script>document.write('Last Modified:' + document.lastModified);</script>
        </footer>
    </div>
</body>

</html>