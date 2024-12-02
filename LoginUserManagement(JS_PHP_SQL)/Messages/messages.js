let messageAPI_url = "Message_API/"
let deleteableMessages = [];
let editableMessages = [];

$(() => {
    Submit_GetMessages();
    $("#filterButton").on("click", () => Submit_GetMessages($("#filter").val()));
    $("#messageButton").on("click", () => Submit_AddMessage($("#message").val()));
});


/*********************************************************************************
 * Function Name:   Submit_GetMessages
 * Parameters:      Filter
 * Returns:         Nothing
 * Description:     Requests messages that match the given filter, or all if no filter provided
 *********************************************************************************/
function Submit_GetMessages(filter = "") {
    Ajax(messageAPI_url + "Messages/" + filter, {}, Callback_GetMessages, "GET");
}
/*********************************************************************************
 * Function Name:   Callback_GetMessages
 * Parameters:      AJAX Response
 * Returns:         Nothing
 * Description:     Creates table from recieved messages
 *********************************************************************************/
function Callback_GetMessages(response) {

    //Populate editable messages
    deleteableMessages = [];
    for (message of response['deleteableMessages']['results']) {
        deleteableMessages.push(message['message_id']);
    }

    //Populate deletable messages
    editableMessages = [];
    for (message of response['editableMessages']['results']) {
        editableMessages.push(message['message_id']);
    }

    if (response['status']) {
        $("#status").html(response['status']);

    }
    if (response['affectedRows'] == 0) {
        $("#status").html("No Messages Found");
        $("#messages").html("");
        return;
    }
    if (!response['results']) return;

    $("#status").html("Found " + response['affectedRows'] + " Messages");
    $("#messages").html(Table_Build(response['results'], "messageTable"));
    Table_IDRows("messageTable", 0);
    Table_AddColumn(0, "messageTable", "Op", CreateMainButtons);
}
/*********************************************************************************
 * Function Name:   Submit_DeleteMessage
 * Parameters:      Message ID
 * Returns:         Nothing
 * Description:     Requests a delete from server
 *********************************************************************************/
function Submit_DeleteMessage(id) {
    Ajax(messageAPI_url + "Messages/" + id, {}, Callback_GetMessages, "DELETE");
}
/*********************************************************************************
 * Function Name:   Submit_EditMessage
 * Parameters:      Message ID
 * Returns:         Nothing
 * Description:     Submits edited message to server
 *********************************************************************************/
function Submit_EditMessage(id) {
    let data = {};
    data['message'] = $("#" + id + " td[column='Message'] input").val();
    Ajax(messageAPI_url + "Messages/" + id, data, Callback_EditMessage, "PUT");
}
/*********************************************************************************
 * Function Name:   Submit_AddMessage
 * Parameters:      Message
 * Returns:         Nothing
 * Description:     Submits new message to server
 *********************************************************************************/
function Submit_AddMessage(message) {
    let data = {};
    data['message'] = message;
    Ajax(messageAPI_url + "Messages/", data, Callback_GetMessages, "POST");
}
/*********************************************************************************
 * Function Name:   Callback_EditMessage
 * Parameters:      AJAX response
 * Returns:         Nothing
 * Description:     Confirms message was edited
 *********************************************************************************/
function Callback_EditMessage(response) {
    UpdateStatus(response['status']);
    if (!response['id']) return;
    ChangeRowToMainMode($("#" + response['id'])[0], false);
}

/*********************************************************************************
 * Function Name:   CreateMainButtons
 * Parameters:      Row element, cell element
 * Returns:         Nothing
 * Description:     returns cell element with buttons from information in row
 *********************************************************************************/
function CreateMainButtons(row, cell = null) {
    if (cell == null) cell = document.createElement("td");


    cell.innerHTML = "";
    //Only do if user has authority over message
    if (deleteableMessages.includes(row.getAttribute("id"))) {
        //Delete Button
        let deleteButton = document.createElement("button");
        deleteButton.innerHTML = "Delete";
        deleteButton.addEventListener("click", () => Submit_DeleteMessage(row.getAttribute("id")));
        cell.appendChild(deleteButton);
    }
    if (editableMessages.includes(row.getAttribute("id"))) {
        //Edit Button
        let editButton = document.createElement("button");
        editButton.innerHTML = "Edit";
        editButton.addEventListener("click", () => ChangeRowToEditMode(row));
        cell.appendChild(editButton);
    }
    return cell;
}

/*********************************************************************************
 * Function Name:   CreateEditButtons
 * Parameters:      Row element, cell element
 * Returns:         Nothing
 * Description:     returns cell element with buttons from information in row
 *********************************************************************************/
function CreateEditButtons(row, cell = null) {
    if (cell == null) cell = document.createElement("td");
    //Only do if user has authority over message
    if (!editableMessages.includes(row.getAttribute("id"))) return cell;

    cell.innerHTML = "";

    //Confirm Button
    let confirmButton = document.createElement("button");
    confirmButton.innerHTML = "Confirm";
    confirmButton.addEventListener("click", () => { Submit_EditMessage(row.getAttribute("id")) });
    cell.appendChild(confirmButton);

    //Cancel Button
    let cancelButton = document.createElement("button");
    cancelButton.innerHTML = "Cancel";
    cancelButton.addEventListener("click", () => ChangeRowToMainMode(row));
    cell.appendChild(cancelButton);

    return cell;
}
/*********************************************************************************
 * Function Name:   ChangeRowToEditMode
 * Parameters:      Row element
 * Returns:         Nothing
 * Description:     Change a single row to edit mode
 *********************************************************************************/
function ChangeRowToEditMode(row) {
    CreateEditButtons(row, row.children[0]);
    Cell_TransformToInput(row.children[3], "text");

}
/*********************************************************************************
 * Function Name:   ChangeRowToMainMode
 * Parameters:      Row element, boolean indication whether to revert to previous value
 * Returns:         Nothing
 * Description:     Change a single row to main mode
 *********************************************************************************/
function ChangeRowToMainMode(row, getPrevValue = true) {
    CreateMainButtons(row, row.children[0]);
    Cell_TransformFromInput(row.children[3], getPrevValue);
}
