<?php
require_once dirname(__FILE__)."/../Common/util.php";

/*********************************************************************************
 * Function Name:   GetAllRoles
 * Parameters:      Override message
 * Returns:         Nothing
 * Description:     Echos a full list of role records
 *********************************************************************************/
function GetAllRoles($message = null)
{
    if (!HasPermission("Manage_Roles"))
        Respond("Action Prohibited");

    $q = "SELECT "
        . "r.role_id as 'Role ID', "
        . "r.name as 'Title', "
        . "r.description as 'Description', "
        . "r.rank as 'Rank' "
        . "FROM roles r "
        . "ORDER BY r.rank ";

    $result = QueryDatabase_Serializable($q);
    $result['subordinateRoles'] = GetSubordinateRoles(false)['results'];

    $q = "SELECT * FROM permissions";
    $permissions = QueryDatabase_Serializable($q);
    foreach ($result['results'] as $key => $role) {
        foreach ($permissions['results'] as $permission) {
            $q = "SELECT * FROM role_permissions WHERE role_id = " . $role['Role ID'] . " AND permission LIKE '" . $permission['name'] . "'";
            $hasPer = "false";
            if (QueryDatabase($q)->num_rows > 0)
                $hasPer = "true";
            $role[$permission['name']] = $hasPer;
        }
        $result['results'][$key] = $role;
    }

    if ($message == null)
        $message = "Retrieved: " . $result['affectedRows'] . " role records";
    Respond($message, true, $result);
}


/*********************************************************************************
 * Function Name:   Val_Role_Name
 * Parameters:      None
 * Returns:         Nothing
 * Description:     Ensures role name meets minimum requirements
 *********************************************************************************/
function Val_Role_Name()
{
    global $safePost;

    $name = $safePost['name'];

    if (is_null($name) || strlen($name) == 0) {
        Respond("Please provide a role name.");
    }
    if (strlen($name) > 10) {
        Respond("Role name max length: 10 characters.");
    }
}

/*********************************************************************************
 * Function Name:   TryAddRole
 * Parameters:      None
 * Returns:         Nothing
 * Description:     Adds a role and echos back a full list of roles
 *********************************************************************************/
function TryAddRole()
{
    global $safePost;

    //Validate Role Name
    $name = $safePost['name'];
    Val_Role_Name();

    //Ensure authorization
    if (!HasPermission("Manage_Roles"))
        Respond("Action Prohibited");

    //Check for pre-existing Role
    $q = "SELECT * FROM roles WHERE name LIKE '" . $name . "'";
    $qty = QueryDatabase($q)->num_rows;
    if ($qty > 0) {
        Respond("Cannot add role: Role already exists!");
    }

    //Insert operation
    $q = "INSERT into roles (name, description, rank)"
        . "VALUES ('" . $name . "','',100)";
    NonQueryDatabase($q);

    GetAllRoles("Role '" . $name . "' added!");
}

/*********************************************************************************
 * Function Name:   TryEditRole
 * Parameters:      None
 * Returns:         Nothing
 * Description:     Edits a role's information and authorizations
 *********************************************************************************/
function TryEditRole()
{
    global $safePost, $db_connection;

    //Package role for easy echo later on
    $role['role_id'] = $safePost['role_id'];
    $role['name'] = $safePost['name'];
    $role['description'] = $safePost['description'];
    $role['rank'] = $safePost['rank'];
    $role['permissions'] = $safePost['permissions'];

    //Find info on user's old role
    $oldRoleData = GetRoleInfo_ByID($role['role_id']);

    //Ensure authorization over user's old roles
    if (!HasPermission("Manage_Roles"))
        Respond("Action Prohibited");
    if ($oldRoleData['rank'] <= $_SESSION['currentUser']['rank'])
        Respond("Action Prohibited");

    //Validate and ensure authority over new rank
    if (!is_numeric($role['rank'])) {
        Respond("Rank must be a number.");
    }
    if ($role['rank'] <= $_SESSION['currentUser']['rank'])
        Respond("Action Prohibited: Rank must be higher than your own.");


    //Validate role name
    Val_Role_Name();

    //Validate Description
    if (strlen($role['description']) > 30) {
        Respond("Role description max length: 30 characters.");
    }

    //Update Operation
    $changesMade = false;
    $db_connection->begin_transaction();
    try {

        //Update operation (except permissions)
        $q = "UPDATE roles "
            . "SET name = '" . $role['name'] . "',"
            . "description = '" . $role['description'] . "',"
            . "rank = " . $role['rank'] . " "
            . "WHERE role_id = " . $role['role_id'] . "";
        $qty = NonQueryDatabase($q);
        if ($qty != 0) {
            $changesMade = true;
        }

        //Update Permissions
        foreach ($role['permissions'] as $permission => $hasPer) {

            $changedThisPermission = false;
            //Add Permissions
            if ($hasPer == 'true') {
                $q = "SELECT * FROM role_permissions "
                    . "WHERE permission LIKE '" . $permission . "'"
                    . "AND role_id LIKE '" . $role['role_id'] . "'";
                $qty = QueryDatabase($q)->num_rows;
                if ($qty == 0) {
                    $q = "INSERT into role_permissions (role_id, permission) "
                        . "VALUES (" . $role['role_id'] . ",'" . $permission . "')";
                    NonQueryDatabase($q);
                    $changedThisPermission = true;
                }
            //Remove Permissions
            } else {
                $q = "DELETE from role_permissions "
                    . "WHERE permission LIKE '" . $permission . "'"
                    . "AND role_id LIKE '" . $role['role_id'] . "'";
                $qty = NonQueryDatabase($q);
                if ($qty != 0) {
                    $changedThisPermission = true;
                }
            }

            //If a change was actually made, ensure user was allowed to do that
            //  and rollback if not.
            if ($changedThisPermission) {
                if (!HasPermission($permission)) {
                    $db_connection->rollback();
                    Respond("Action Prohibited: You need the permission " . $permission . " in order to change access to it");
                }
                $changesMade = true;
            }

        }
        $db_connection->commit();
    } catch (mysqli_sql_exception $err) {
        HandleSQLError($err);
    }

    if (!$changesMade)
        Respond("No Roles Updated");
    Respond("Role #" . $role['role_id'] . " updated!", true, $role);
}

/*********************************************************************************
 * Function Name:   TryDeleteRole
 * Parameters:      Boolean indicating whether to echo to client afterwords
 * Returns:         Nothing
 * Description:     Delete's a role
 *********************************************************************************/
function TryDeleteRole($respond = true)
{
    global $safePost, $db_connection;

    //Validate role to delete
    $roleData = GetRoleInfo_ByID($safePost['role_id']);
    if ($roleData == null) {
        Respond("Role ID invalid.");
    }
    //Validate Authority over role
    if (!HasPermission("Manage_Roles") || $roleData['rank'] <= $_SESSION['currentUser']['rank'])
        Respond("Action Prohibited");
    if ($roleData['role_id'] <= 2) { //Explicit protection for the root and guest roles
        Respond("Action Prohibited: Protected Role");
    }

    //Ensure no users are still assigned to the role
    $q = "SELECT * "
        . "FROM user_roles "
        . "where role_id = " . $roleData['role_id'] . "; ";
    $qty = QueryDatabase($q)->num_rows;
    if ($qty > 0) {
        Respond("Cannot remove role: " . $qty . " users still assigned to role");
    }

    //Delete Operation
    $db_connection->begin_transaction();
    try {
        //Delete Permissions
        $q = "DELETE "
            . "FROM role_permissions "
            . "where role_id = " . $roleData['role_id'] . "; ";
        NonQueryDatabase($q);
        //Delete Roles
        $q = "DELETE "
            . "FROM roles "
            . "where role_id = " . $roleData['role_id'] . "; ";
        NonQueryDatabase($q);

        $db_connection->commit();
    } catch (mysqli_sql_exception $err) {
        HandleSQLError($err);
    }

    //Echo list of Roles if requested
    if ($respond)
        Respond("Role '" . $roleData['name'] . "' deleted.", true, $roleData);
}
