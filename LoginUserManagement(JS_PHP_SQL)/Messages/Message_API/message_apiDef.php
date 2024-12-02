<?php
require_once dirname(__FILE__) . "/../../Common/util.php";
session_start();

class MessageAPI
{
    private $method = '';
    private $endpoint = '';
    private $verb = '';
    private $args = array();
    private $file = Null;
    private $safeData;

    public function __construct($request)
    {
        header("Content-Type: application/json");


        //Populate request details
        $this->args = Sanitize(explode('/', rtrim($request, '/')));
        $this->endpoint = array_shift($this->args);
        if (array_key_exists(0, $this->args) && !is_numeric($this->args[0])) {
            $this->verb = array_shift($this->args);
        }
        $this->method = $_SERVER['REQUEST_METHOD'];
        switch ($this->method) {
            case 'DELETE':
                parse_str(file_get_contents("php://input"), $this->file);
                $this->safeData = Sanitize($this->file);
                break;
            case 'POST':
                $this->safeData = Sanitize($_POST);
                break;
            case 'GET':
                $this->safeData = Sanitize($_GET);
                break;
            case 'PUT':
                parse_str(file_get_contents("php://input"), $this->file);
                $this->safeData = Sanitize($this->file);
                break;
            default:
                $this->StatusResponse('Invalid Method', 405);
                break;
        }
    }
    /*********************************************************************************
     * Function Name:   processAPI
     * Parameters:      Nothing
     * Returns:         Reponse
     * Description:     Directs to correct function
     *********************************************************************************/
    public function processAPI()
    {
        if (method_exists($this, $this->endpoint)) {
            return $this->{$this->endpoint}($this->args);
        }
        return $this->StatusResponse("No Endpoint: $this->endpoint", 404);
    }
    /*********************************************************************************
     * Function Name:   Response
     * Parameters:      data, status code
     * Returns:         Reponse
     * Description:     Builds response to send back to client with a given message
     *********************************************************************************/
    private function Response($data, $status = 200)
    {
        header("HTTP/1.1 " . $status . " " . $this->LookupStatus($status));
        return json_encode($data);
    }
    /*********************************************************************************
     * Function Name:   StatusResponse
     * Parameters:      Message, status code, data
     * Returns:         Reponse
     * Description:     Builds response to send back to client with a given message
     *********************************************************************************/
    private function StatusResponse($message, $status = 200, $data = array())
    {
        $data['status'] = $message;
        return $this->Response($data, $status);
    }
    /*********************************************************************************
     * Function Name:   LookupStatus
     * Parameters:      Code
     * Returns:         String message
     * Description:     Gets messages that match code
     *********************************************************************************/
    private function LookupStatus($code)
    {
        $status = array(
            200 => 'OK',
            400 => 'Invalid Argument',
            403 => 'Forbidden',
            404 => 'Not Found',
            405 => 'Method Not Allowed',
            500 => 'Internal Server Error',
        );
        return ($status[$code]) ? $status[$code] : $status[500];
    }
    /*********************************************************************************
     * Function Name:   GetMessages
     * Parameters:      Filter
     * Returns:         Reponse
     * Description:     Gets messages that match filter
     *********************************************************************************/
    private function GetMessages($filter)
    {

        $q = "SELECT "
            . "m.message_id as 'Message ID', "
            . "u.username as 'User', "
            . "m.message as 'Message', "
            . "m.timestamp as 'Timestamp' "
            . "FROM messages m "
            . "LEFT JOIN users u "
            . "ON m.user_id = u.user_id "
            . "WHERE m.message LIKE '%" . $filter . "%' "
            . "OR u.username LIKE '%" . $filter . "%' "
            . "ORDER BY m.timestamp ";


        $result = QueryDatabase_Serializable($q);

        //Get list of messages user can delete
        $q = "SELECT DISTINCT "
            . "m.message_id as 'message_id' "
            . "FROM roles r "
            . "JOIN user_roles ur "
            . "ON r.role_id = ur.role_id "
            . "JOIN users u "
            . "ON u.user_id = ur.user_id "
            . "JOIN messages m "
            . "ON m.user_id = u.user_id "
            . "WHERE r.rank > " . $_SESSION['currentUser']['rank'] . " "
            . "OR m.user_id = " . $_SESSION['currentUser']['user_id'] . "; ";
        $result['deleteableMessages'] = QueryDatabase_Serializable($q);

        //Get list of messages user can edit
        $q = "SELECT DISTINCT "
            . "m.message_id as 'message_id' "
            . "FROM messages m "
            . "WHERE m.user_id = " . $_SESSION['currentUser']['user_id'] . "; ";
        $result['editableMessages'] = QueryDatabase_Serializable($q);


        return $this->Response($result);
    }
    /*********************************************************************************
     * Function Name:   DeleteMessage
     * Parameters:      Message ID
     * Returns:         Nothing
     * Description:     Delete a message
     *********************************************************************************/
    private function DeleteMessage($args)
    {
        global $db_connection;

        if (count($args) != 1) {
            return $this->StatusResponse("Bad Request", 400);
        }

        //Check message exists
        $q = "SELECT * "
            . "FROM messages "
            . "where message_id = " . $args[0] . "; ";
        $message = QueryDatabase_Serializable($q);


        if ($message['affectedRows'] == 0) {
            return $this->StatusResponse("Message Not Found", 404);
        }

        //Ensure user has permission to delete
        $user = GetUser_ByID($message['results'][0]['user_id']);

        if ($user['rank'] <= $_SESSION['currentUser']['rank'] && $user['user_id'] != $_SESSION['currentUser']['user_id']) {
            return $this->StatusResponse("Action Prohibited.", 200);
        }

        //Delete Operation
        $db_connection->begin_transaction();
        try {
            //Delete Message
            $q = "DELETE "
                . "FROM messages "
                . "where message_id = " . $args[0] . "; ";
            NonQueryDatabase($q);

            $db_connection->commit();
        } catch (mysqli_sql_exception $err) {
            return $this->StatusResponse("Error:", 500);
        }
        return $this->GetMessages("");
    }

    /*********************************************************************************
     * Function Name:   AddMessage
     * Parameters:      None
     * Returns:         Nothing
     * Description:     Add a new message
     *********************************************************************************/
    private function AddMessage()
    {
        if (!isset($this->safeData['message']) || strlen($this->safeData['message']) == 0) {
            return $this->StatusResponse("Message must not be empty.");
        }

        $q = "INSERT INTO "
            . "messages (user_id, message) "
            . " VALUES (" . $_SESSION['currentUser']['user_id'] . ", '" . $this->safeData['message'] . "');";
        error_log($q);

        NonQueryDatabase($q);


        return $this->GetMessages("");
    }

    /*********************************************************************************
     * Function Name:   EditMessages
     * Parameters:      Message ID
     * Returns:         Nothing
     * Description:     Edit a message
     *********************************************************************************/
    private function EditMessages($args)
    {
        global $db_connection;

        if (count($args) != 1) {
            return $this->StatusResponse("Bad Request", 400);
        }

        //Ensure message exists
        $q = "SELECT * "
            . "FROM messages "
            . "where message_id = " . $args[0] . "; ";
        $message = QueryDatabase_Serializable($q);

        if ($message['affectedRows'] == 0) {
            return $this->StatusResponse("Message Not Found", 404);
        }

        //Ensure user has permissions
        $user = GetUser_ByID($message['results'][0]['user_id']);

        if ($user['user_id'] != $_SESSION['currentUser']['user_id']) {
            return $this->StatusResponse("Action Prohibited.", 403);
        }
        //Ensure Message is not empty
        if (!isset($this->safeData['message']) || strlen($this->safeData['message']) == 0) {
            return $this->StatusResponse("Message must not be empty.");
        }

        //Update  Operation
        $db_connection->begin_transaction();
        try {
            $q = "UPDATE messages "
                . "SET "
                . "message = '" . $this->safeData['message'] . "', "
                . "timestamp = CURRENT_TIMESTAMP "
                . "WHERE message_id = " . $args[0] . "; ";

            NonQueryDatabase($q);
            $db_connection->commit();

        } catch (mysqli_sql_exception $err) {
            return $this->StatusResponse("Error:", 500);
        }
        $data = array();
        $data['id'] = $message['results'][0]['message_id'];
        return $this->StatusResponse("Message Edited Successfully", 200, $data);
    }

    /*********************************************************************************
     * Function Name:   Messages
     * Parameters:      None
     * Returns:         Nothing
     * Description:     Endpoint
     *********************************************************************************/
    private function Messages()
    {
        switch ($this->method) {
            case 'DELETE':
                return $this->DeleteMessage($this->args);
            case 'POST':
                return $this->AddMessage();
            case 'GET':
                return $this->GetMessages($this->verb);
            case 'PUT':
                return $this->EditMessages($this->args);
            default:
                return $this->StatusResponse('Invalid Method', 405);
        }
    }

}