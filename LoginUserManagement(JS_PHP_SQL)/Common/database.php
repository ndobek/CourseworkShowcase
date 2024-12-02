<?php

$db_connection = null;
$db_response = "";

ConnectToDatabase();
/*********************************************************************************
 * Function Name:   ConnectToDatabase
 * Parameters:      None
 * Returns:         Nothing
 * Description:     Connects to Database
 *********************************************************************************/
function ConnectToDatabase(){
    global $db_connection, $db_response;

    $db_connection = new mysqli("localhost","ndobek10_user","testeroniAndCheese","ndobek10_lab_DB");

    if($db_connection->connect_error){
        $db_response = "Connect Error (" . $db_connection->connect_errno . ") " . $db_connection->connect_error;
        error_log(json_encode($db_response));
        echo json_encode($db_response);
        die();
    } else
    {
        $db_response = "Connected to database.";
    }
}
/*********************************************************************************
 * Function Name:   QueryDatabase
 * Parameters:      Query
 * Returns:         Query Results
 * Description:     Queries Database
 *********************************************************************************/
function QueryDatabase($query){
    global $db_connection, $db_response;

    $result = false;

    if ($db_connection == null)
    {
        $db_response = "Not connected to database.";
        return $result;
    }

    if(!$result = $db_connection->query($query)){
        $db_response = "Query Error : {$db_connection->errno} : {$db_connection->error}";
    }

    return $result;
}

/*********************************************************************************
 * Function Name:   NonQueryDatabase
 * Parameters:      Query
 * Returns:         Affected rows
 * Description:     Sends request to database but doesn't expect table response
 *********************************************************************************/
function NonQueryDatabase($nonQuery){
    global $db_connection;
    $qResult = QueryDatabase($nonQuery);
    if(!$qResult) return 0;
    else return $db_connection->affected_rows;
}

/*********************************************************************************
 * Function Name:   QueryDatabase_Serializable
 * Parameters:      Query
 * Returns:         Query Results
 * Description:     Queries Database and returns results as an object that can be json encoded
 *********************************************************************************/
function QueryDatabase_Serializable($query){
    global $db_connection;
    $queryResult = QueryDatabase($query);
    $output["results"] = $queryResult->fetch_all(MYSQLI_ASSOC);
    $output["affectedRows"] = $queryResult->num_rows;
    mysqli_next_result($db_connection);
    return $output;
}

