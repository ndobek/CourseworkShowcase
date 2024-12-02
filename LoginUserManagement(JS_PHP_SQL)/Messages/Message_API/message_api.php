<?php 

require_once 'message_apiDef.php';

try
{
    $API = new MessageAPI($_REQUEST['request']);
    echo $API->processAPI();
}
catch(Exception $e)
{
    echo json_encode(Array('error' => $e->getMessage()));
}