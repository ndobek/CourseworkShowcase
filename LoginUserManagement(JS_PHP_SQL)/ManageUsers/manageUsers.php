<?php
session_start();
require_once __DIR__ . '/../Common/util.php';

if (!HasPermission("Manage_Users"))
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
    <script src="manageUsers.js"></script>
    <script src="../Common/util.js"></script>
</head>

<body>
    <div id="site">
        <header>
            <h1>ND - Lab 3 - User Management</h1>
        </header>
        <div id="content">
            <div class="section">
                <form class="loginInfo">
                    <label for="username">Username:</label><input id="username" type="text"
                        value="">
                    <label for="password">Password:</label><input id="password" type="password">
                    <label for="role">Role:</label>
                    <select id = "role"></select>
                    <button id="addUserButton" type="button" class="fullGridWidth">Add User</button>
                </form>
            </div>
            <div class="section" id="status"></div>
            <div class="section" id="usersTableDiv"></div>
            <nav class="section">
                    <div class="fullGridWidth"><a href="../index.php" >Main Page</a></div>
                </nav>
            </div>
            <footer>
                &copy; Nathaniel Dobek<br>
                <script>document.write('Last Modified:' + document.lastModified);</script>
            </footer>
        </div>
    </body>

    </html>