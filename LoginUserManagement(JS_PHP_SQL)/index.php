<?php
session_start();
require_once __DIR__ . '/Common/util.php';

if (!IsLoggedIn())
    header('Location:  Login/login.php');

?>
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet"
        href="https://thor.cnt.sast.ca/~ndobek10/danger_breakfast/cmpe2550/commonFiles/V2/standardICALayout.css">
    <link rel="stylesheet" href="Common/style.css">
    <title>Lab 3 - Nathaniel Dobek</title>
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.7.1/jquery.min.js"></script>
    <script src="index.js"></script>
    <script src="Common/util.js"></script>
</head>

<body>
    <div id="site">
        <header>
            <h1>ND - Lab 3 - Index Page</h1>
        </header>
        <div id="content">
            <div id="status" class="section">Welcome, <?php echo $_SESSION['currentUser']['username']; ?></div>
            <nav class="section">

                <?php if (HasPermission('Manage_Users'))
                    echo '<div><a href="ManageUsers/manageUsers.php">User Management</a></div>' ?>
                <?php if (HasPermission('Manage_Roles'))
                    echo '<div><a href="ManageRoles/manageRoles.php">Role Management</a></div>' ?>
                <?php if (HasPermission('Messaging'))
                    echo '<div><a href="Messages/messages.php">Messages</a></div>' ?>
                    <button id="logoutButton" type="button" class="fullGridWidth">Logout</button>

                </nav>
            </div>
            <footer>
                &copy; Nathaniel Dobek<br>
                <script>document.write('Last Modified:' + document.lastModified);</script>
            </footer>
        </div>
    </body>

    </html>