<?php
dirname(__FILE__) . "/../Common/util.php";


/*********************************************************************************
 * Function Name:   TryLogin
 * Parameters:      None
 * Returns:         Nothing
 * Description:     Validates Credentials and logs in if valid
 *********************************************************************************/
function TryLogin()
{
    global $safePost;
    Val_Credentials_Soft();
    $q = QueryDatabase_Serializable("Call Get_Credentials_By_User('" . $safePost['username'] . "')");

    if ($q['affectedRows'] == 0) {
        Respond("Account Not Found.");
    } else if (!password_verify($safePost['password'], $q['results'][0]["password"])) {
        Respond("Incorrect Password");
    } else {
        $_SESSION['currentUser'] = $q['results'][0];
        $_SESSION['currentUser']['plainPassword'] = $safePost['password'];
        error_log($_SESSION['currentUser']['plainPassword']);
        error_log($safePost['password']);
        Respond("Logged in!", true);
    }
}

/*********************************************************************************
 * Function Name:   Logout
 * Parameters:      None
 * Returns:         Nothing
 * Description:     Ends session and logs out.
 *********************************************************************************/
function Logout()
{
    session_unset();
    session_destroy();
    Respond("Logged Out!", true);
}

/*********************************************************************************
 * Function Name:   TryRegister
 * Parameters:      None
 * Returns:         Nothing
 * Description:     Validates Credentials and registers a user if valid
 *********************************************************************************/
function TryRegister()
{
    global $safePost, $db_connection;

    Val_Credentials_Soft();

    $db_connection->begin_transaction();
    try {
        //Check if user exists
        $q = QueryDatabase_Serializable("Call Get_Credentials_By_User('" . $safePost['username'] . "')");
        if ($q['affectedRows'] != 0) {
            Respond("Account already exists. Please login or choose another username.");
        }

        //Add Credentials
        $hash = password_hash($safePost['password'], PASSWORD_DEFAULT);
        NonQueryDatabase("Call Add_Credentials('" . $safePost['username'] . "','" . $hash . "')");

        if (isset($safePost['role'])) {
            //Update Role
            $safePost['user_id'] = QueryDatabase_Serializable("Call Get_Credentials_By_User('" . $safePost['username'] . "')")['results'][0]['user_id'];
            TryEditUserRole(false);

            //Respond with different information if role information is provided
            $db_connection->commit();
            GetAllUsers("User added successfully!");
        }

        //Respond with message
        $db_connection->commit();
        Respond("User added successfully!", true);
    } catch (mysqli_sql_exception $err) {
        HandleSQLError($err);
    }
}

/*********************************************************************************
 * Function Name:   Val_Credentials_Soft
 * Parameters:      None
 * Returns:         Nothing
 * Description:     Checks if credentials meet base requirements, does NOT check
 *                   if they match existing records
 *********************************************************************************/
function Val_Credentials_Soft()
{
    global $safePost;

    //attempted username used for persistance on login page
    $_SESSION['attemptedUsername'] = $safePost['username'];

    $username = $_SESSION['attemptedUsername'];
    $password = $safePost['password'];

    if (is_null($username) || strlen($username) == 0) {
        Respond("Please provide a username.");
    }
    if (is_null($password) || strlen($password) == 0) {
        Respond("Please provide a password.");
    }
    if (strlen($username) > 15) {
        Respond("Username max length: 15 characters.");
    }
}

