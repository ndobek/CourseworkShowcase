<?php


require_once dirname(__FILE__) . '/database.php';
require_once dirname(__FILE__) . "/../ManageRoles/manageRoles_util.php";
require_once dirname(__FILE__) . '/../ManageUsers/manageUsers_util.php';
require_once dirname(__FILE__) . '/../Login/login_util.php';

/*********************************************************************************
 * Function Name:   Sanitize
 * Parameters:      Array
 * Returns:         Sanitized Array
 * Description:     Recursively sanitizes an array
 *********************************************************************************/
function Sanitize($array)
{
    global $db_connection;

    if (!isset($array))
        return;

    $result = [];

    foreach ($array as $key => $value) {
        $safeKey = $db_connection->real_escape_string(strip_tags(trim($key)));
        if (is_array($value))
            $safeValue = Sanitize($value);
        else
            $safeValue = $db_connection->real_escape_string(strip_tags(trim($value)));
        $result[$safeKey] = $safeValue;
    }
    return $result;
}

/*********************************************************************************
 * Function Name:   Respond
 * Parameters:      Status Message, Success Boolean, Data Object
 * Returns:         Nothing
 * Description:     Echos to client and ends script
 *********************************************************************************/
function Respond($status, $success = false, $data = null)
{
    $response['status'] = $status;
    $response['success'] = $success;

    if (!is_null($data))
        $response['data'] = $data;

    echo json_encode($response);
    die();
}

/*********************************************************************************
 * Function Name:   Persist
 * Parameters:      Name of session variable
 * Returns:         Nothing
 * Description:     Helper function to insert session variables if they exist
 *********************************************************************************/
function Persist($name)
{
    if (isset($_SESSION[$name]))
        echo $_SESSION[$name];
}

/*********************************************************************************
 * Function Name:   IsLoggedIn
 * Parameters:      None
 * Returns:         boolean indicating user is logged in
 * Description:     Determines if user is logged in
 *********************************************************************************/
function IsLoggedIn()
{
    return isset($_SESSION['currentUser']);
}

/*********************************************************************************
 * Function Name:   HasPermission
 * Parameters:      Permission to check
 * Returns:         boolean indicating id user has permission
 * Description:     Determines if currently logged in user has a given permission
 *********************************************************************************/
function HasPermission($permissionName)
{
    if (!IsLoggedIn())
        return false;
    $q = QueryDatabase_Serializable("Call Get_Permissions_By_ID('" . $_SESSION['currentUser']['user_id'] . "')");
    foreach ($q['results'] as $permission) {
        if ($permission['permission'] == $permissionName)
            return true;
    }
    return false;
}

/*********************************************************************************
 * Function Name:   GetSubordinateRoles
 * Parameters:      boolean indicating whether to return to client
 * Returns:         object containing roles
 * Description:     Finds roles that currently logged in user is able to modify
 *********************************************************************************/
function GetSubordinateRoles($respond = true)
{
    $q = "SELECT DISTINCT "
        . "r.name as 'role', "
        . "r.role_id as 'role_id' "
        . "FROM roles r "
        . "WHERE r.rank > " . $_SESSION['currentUser']['rank'];

    $result = QueryDatabase_Serializable($q);
    if ($respond)
        Respond("", true, $result);
    return $result;
}

/*********************************************************************************
 * Function Name:   GetUser_ByID
 * Parameters:      User ID
 * Returns:         Object containing user information
 * Description:     Gets user information
 *********************************************************************************/
function GetUser_ByID($userID)
{
    $q = QueryDatabase_Serializable("Call Get_Credentials_By_ID('" . $userID . "')");
    if ($q['affectedRows'] == 0)
        return null;
    return $q['results'][0];
}

/*********************************************************************************
 * Function Name:   GetRoleInfo_ByName
 * Parameters:      Role Name
 * Returns:         Object containing role information
 * Description:     Gets role information
 *********************************************************************************/
function GetRoleInfo_ByName($roleName)
{
    $q = "SELECT * FROM roles r WHERE r.name LIKE '" . $roleName . "'";
    $result = QueryDatabase_Serializable($q)['results'][0];

    $q = "SELECT permission FROM role_permissions rp WHERE rp.role_id LIKE '" . $result['role_id'] . "'";
    $result['permissions'] = QueryDatabase_Serializable($q)['results'];
    return $result;
}
/*********************************************************************************
 * Function Name:   GetRoleInfo_ByID
 * Parameters:      Role ID
 * Returns:         Object containing role information
 * Description:     Gets role information
 *********************************************************************************/
function GetRoleInfo_ByID($role_id)
{
    $q = "SELECT * FROM roles r WHERE r.role_id LIKE '" . $role_id . "'";
    $result = QueryDatabase_Serializable($q)['results'][0];

    $q = "SELECT permission FROM role_permissions rp WHERE rp.role_id LIKE '" . $result['role_id'] . "'";
    $result['permissions'] = QueryDatabase_Serializable($q)['results'];
    return $result;
}

/*********************************************************************************
 * Function Name:   HandleSQLError
 * Parameters:      SQL Error
 * Returns:         Nothing
 * Description:     Sends error to log, rolls back transaction, and echos to user.
 *********************************************************************************/
function HandleSQLError(mysqli_sql_exception $err)
{
    global $db_connection;
    error_log($err->getMessage());
    $db_connection->rollback();
    Respond("An unknown error occured");
}
?>