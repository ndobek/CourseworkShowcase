<?php
session_start();

require_once dirname(__FILE__).'/util.php';

//Pre sanitize POST data
$safePost = Sanitize($_POST);

//Perform request specific actions
if (isset($safePost["action"])) {
    $action = $safePost["action"];
    if ($action == "login")
        TryLogin();
    else if ($action == "register")
        TryRegister();
    else if ($action == "logout")
        Logout();
    else if ($action == "getAllUsers")
        GetAllUsers();
    else if ($action == "deleteUser")
        TryDeleteUser();
    else if ($action == "getSubordinateRoles")
        GetSubordinateRoles();
    else if ($action == "editUserRole")
        TryEditUserRole();
    else if ($action == "getAllRoles")
        GetAllRoles();
    else if ($action == "deleteRole")
        TryDeleteRole();
    else if ($action == "addRole")
        TryAddRole();
    else if ($action == "editRole")
        TryEditRole();
}

Respond("Action not recognized.");





















