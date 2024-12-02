<?php
require_once dirname(__FILE__)."/../Common/util.php";

/*********************************************************************************
 * Function Name:   GetAllUsers
 * Parameters:      Override message
 * Returns:         Nothing
 * Description:     Echos a full list of user records
 *********************************************************************************/
function GetAllUsers($message = null)
{
    //Verify Permission
    if (!HasPermission("Manage_Users"))
        Respond("Action Prohibited");

    $q = "SELECT "
        . "u.user_id as 'User ID', "
        . "u.username as 'Username', "
        . "u.password as 'Hashed Password', "
        . "r.name as 'Role' "
        . "FROM users u "
        . "JOIN user_roles ur "
        . "ON ur.user_id = u.user_id "
        . "JOIN roles r "
        . "ON r.role_id = ur.role_id "
        . "ORDER BY u.user_id;";

    $result = QueryDatabase_Serializable($q);

    if ($message == null)
        $message = "Retrieved: " . $result['affectedRows'] . " user records";
    Respond($message, true, $result);
}

/*********************************************************************************
 * Function Name:   TryDeleteUser
 * Parameters:      boolean to indicate whether to echo back to client on success
 * Returns:         Nothing
 * Description:     Deletes a user
 *********************************************************************************/
function TryDeleteUser($respond = true)
{
    global $safePost, $db_connection;

    //Verify validity and authorization
    $victim = GetUser_ByID($safePost['user_id']);
    if ($victim == null) {
        Respond("User ID invalid.");
    }
    if (!HasPermission("Manage_Users") || $victim['rank'] <= $_SESSION['currentUser']['rank'])
        Respond("Action Prohibited");

    //Delete Operation
    $db_connection->begin_transaction();
    try {
        //Delete all roles
        $q = "DELETE "
            . "FROM user_roles "
            . "where user_id = " . $victim['user_id'] . "; ";
        NonQueryDatabase($q);

        //Delete user
        $q = "DELETE "
            . "FROM users "
            . "where user_id = " . $victim['user_id'] . "; ";
        NonQueryDatabase($q);

        $db_connection->commit();
    } catch (mysqli_sql_exception $err) {
        HandleSQLError($err);
    }

    if ($respond)
        GetAllUsers("User '" . $victim['username'] . "' deleted.");
}

/*********************************************************************************
 * Function Name:   TryEditUserRole
 * Parameters:      boolean to indicate whether to echo back to client on success
 * Returns:         Nothing
 * Description:     Edits a user's role
 *********************************************************************************/
function TryEditUserRole($respond = true)
{
    global $safePost, $db_connection;

    //Verify validity and authorization of user
    $user = GetUser_ByID($safePost['user_id']);
    if ($user == null) {
        Respond("User ID invalid.");
    }
    if (!HasPermission("Manage_Users") || $user['rank'] <= $_SESSION['currentUser']['rank'])
        Respond("Action Prohibited");

    //Ensure authorization over new role
    $newRole = GetRoleInfo_ByName($safePost['role']);
    if ($newRole['rank'] <= $_SESSION['currentUser']['rank'])
        Respond("Action Prohibited");

    //Edit Operation
    $db_connection->begin_transaction();
    try {

        $q = "DELETE "
            . "FROM user_roles "
            . "where user_id = " . $user['user_id'] . "; ";
        NonQueryDatabase($q);

        $q = "INSERT INTO user_roles (user_id, role_id)"
            . "VALUES (" . $user['user_id'] . "," . $newRole['role_id'] . ")";
        NonQueryDatabase($q);

        $db_connection->commit();
    } catch (mysqli_sql_exception $err) {
        HandleSQLError($err);
    }

    if ($respond)
        GetAllUsers("Role of user '" . $user['username'] . "' changed!");
}
