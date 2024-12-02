<?php
session_start();
require_once __DIR__.'/../Common/util.php';
?>
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="https://thor.cnt.sast.ca/~ndobek10/danger_breakfast/cmpe2550/commonFiles/V2/standardICALayout.css">
    <link rel="stylesheet" href="../Common/style.css">
    <title>Lab 3 - Nathaniel Dobek</title>
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.7.1/jquery.min.js"></script>
    <script src="login.js"></script>
    <script src="../Common/util.js"></script>
</head>

<body>
    <div id="site">
        <header>
            <h1>ND - Lab 3 - Login/Register</h1>
        </header>
        <div id="content">
            <div class="section">
                <form class="loginInfo">
                    <label for="username">Username:</label><input id="username" type="text" value="<?php Persist("attemptedUsername")?>">
                    <label for="password">Password:</label><input id="password" type="password">
                    <button id="loginButton" type="button" class="fullGridWidth">Login</button>
                    <button id="registerButton" type="button" class="fullGridWidth">Register</button>
                </form>
                <div id="status"></div>
            </div>
        </div>
        <footer>
            &copy; Nathaniel Dobek<br>
            <script>document.write('Last Modified:' + document.lastModified);</script>
        </footer>
    </div>
</body>

</html>