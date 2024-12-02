let serverUrl = 'https://thor.cnt.sast.ca/~ndobek10/danger_breakfast/cmpe2550/labs/lab_03/Common/server.php';


/*********************************************************************************
 * Function Name:   Val_Credentials
 * Parameters:      Username, Password
 * Returns:         Bool
 * Description:     Checks if chosen values meet basic requirements
 *********************************************************************************/
function Val_Credentials(username, password) {
    if (!username) {
        UpdateStatus("Please provide a username.");
        return false;
    }
    if (!password) {
        UpdateStatus("Please provide a password.");
        return false;
    }
    if (username.length > 15) {
        UpdateStatus("Username max length: 15 characters.");
    }
    return true;
}
/*********************************************************************************
 * Function Name:   Val_Role
 * Parameters:      role name
 * Returns:         Bool
 * Description:     Checks if chosen values meet basic requirements
 *********************************************************************************/
function Val_Role(role) {
    if (!role) {
        UpdateStatus("Please provide a role name.");
        return false;
    }
    if (role.length > 10) {
        UpdateStatus("Role name max length: 10 characters.");
        return false;
    }
    return true;
}

/*********************************************************************************
 * Function Name:   TrySubmitCredentials
 * Parameters:      Action(string), Success Function
 * Returns:         Nothing
 * Description:     Collects, Validates, and sends credentials to server with given action.
 *********************************************************************************/
function TrySubmitCredentials(action, successFunc) {
    let cred = {};
    cred['username'] = $("#username").val();
    cred['password'] = $("#password").val();

    if (!Val_Credentials(cred['username'], cred['password'])) return;

    cred['action'] = action;

    Ajax(serverUrl, cred, successFunc)
}


/*********************************************************************************
 * Function Name:   Ajax
 * Parameters:      URL that request is sent to,
 *                  Data in request
 *                  Success Callback
 *                  Request Method: Default POST
 *                  Request DataType: Default JSON
 * Returns:         Nothing
 * Description:     Sends a request to AJAX
 *********************************************************************************/
function Ajax(url, data, onSuccess, method = "POST", dataType = "json",) {
    let request = {}
    request['url'] = url;
    request['method'] = method;
    request['dataType'] = dataType;
    request['data'] = data;
    request['success'] = onSuccess;
    request['error'] = ErrorHandler;
    // request['complete'] = ;

    $.ajax(request);
}

/*********************************************************************************
 * Function Name:   ErrorHandler
 * Parameters:      Standard AJAX parameters
 * Returns:         Nothing
 * Description:     Displays a message to multiple locations in case of error
 *********************************************************************************/
function ErrorHandler(response, status, error) {
    let msg = status + ": " + error;
    console.log(msg);
    UpdateStatus(msg);
    alert(msg);
}

/*********************************************************************************
 * Function Name:   UpdateStatus
 * Parameters:      Message
 * Returns:         Nothing
 * Description:     Displays a message
 *********************************************************************************/
function UpdateStatus(message) {
    $("#status").html(message);
}

/*********************************************************************************
 * Function Name:   Build_Select
 * Parameters:      Data, default value, previous element to populate
 * Returns:         Element
 * Description:     Creates a select input from data
 *********************************************************************************/
function Build_Select(data, selectedItem = null, populateThis = null) {
    let select = populateThis;
    if (populateThis == null) select = document.createElement('select');
    select.innerHTML = "";
    for (optionData of data) {
        let option = document.createElement('option');
        option.innerHTML = optionData;
        option.setAttribute('value', optionData)
        if (optionData == selectedItem) option.setAttribute("selected", "selected");

        select.appendChild(option);
    }
    return select;
}

/*********************************************************************************
 * Function Name:   Table_Build
 * Parameters:      Data, ID for Table
 * Returns:         Table Element
 * Description:     Creates an Element Table from data
 *********************************************************************************/
function Table_Build(tableData, id = null) {
    let table = document.createElement("table");

    //Header
    let header = document.createElement("thead");
    header.appendChild(Table_BuildHeaderRow(tableData[0]));
    table.appendChild(header);

    //Body
    let body = document.createElement("tbody");
    tableData.forEach(row => {
        body.appendChild(Table_BuildDataRow(row));
    });
    table.appendChild(body);

    if (id) table.setAttribute("id", id);
    return table;
}
/*********************************************************************************
 * Function Name:   Table_BuildHeaderRow
 * Parameters:      object
 * Returns:         row Element
 * Description:     Builds a row with key values for displaying column names
 *********************************************************************************/
function Table_BuildHeaderRow(rowData) {
    let row = document.createElement("tr");
    for ([key, value] of Object.entries(rowData)) {
        let cell = document.createElement("th");
        cell.innerHTML = key;
        cell.setAttribute('column', key);
        row.appendChild(cell);
    }
    return row;
}
/*********************************************************************************
 * Function Name:   Table_BuildDataRow
 * Parameters:      object
 * Returns:         row Element
 * Description:     Builds a row with values for displaying column data
 *********************************************************************************/
function Table_BuildDataRow(rowData) {
    let row = document.createElement("tr");
    for ([key, value] of Object.entries(rowData)) {
        let cell = document.createElement("td");
        cell.innerHTML = value;
        cell.setAttribute('column', key);
        row.appendChild(cell);
    }
    return row;
}
/*********************************************************************************
 * Function Name:   Table_IDRows
 * Parameters:      tableID, index
 * Returns:         nothing
 * Description:     Takes the information in the column indicated and uses it as 
 *                      an ID for each row
 *********************************************************************************/
function Table_IDRows(tableID, columnIndex) {
    let rows = $("#" + tableID + " tbody tr");
    rows.each((i, row) => {
        row.setAttribute("id", row.children[columnIndex].innerHTML);
    });
}

/*********************************************************************************
 * Function Name:   Table_AddColumn
 * Parameters:      index(int), tableID(string), header(string), cell creation function(func(row))
 * Returns:         nothing
 * Description:     Creates a new column in the given table, at the given index position.
 *                      The header row with have the text given by "header"
 *                      the data rows will be created by the cell function
 *                      the cell function is passed the row element
 *********************************************************************************/
function Table_AddColumn(index, tableID, header, cellFunction) {
    //Header
    let headerCell = document.createElement("th");
    headerCell.innerHTML = header;

    let headerRow = $("#" + tableID + " thead tr")[0]
    headerRow.insertBefore(headerCell, headerRow.children.item(index));
    headerRow.children[index].setAttribute('column', header);

    //Data
    let rows = $("#" + tableID + " tbody tr");
    rows.each((i, row) => {
        row.insertBefore(cellFunction(row), row.children.item(index));
        row.children[index].setAttribute('column', header);
    });
}

/*********************************************************************************
 * Function Name:   Table_TransformColumn
 * Parameters:      index(int), tableID(string), cell creation function(func(row))
 * Returns:         nothing
 * Description:     Applies the cell function to each data cell in the given column
 *********************************************************************************/
function Table_TransformColumn(index, tableID, cellFunction) {
    //Select column cells, index +1 because nth of type starts at 1
    $("#" + tableID + " tr td:nth-of-type(" + Number(index + 1) + ")").each((i, e) => cellFunction(e));
}

/*********************************************************************************
 * Function Name:   Cell_TransformToInput
 * Parameters:      cell, inputType
 * Returns:         nothing
 * Description:     Creates an input of the given type with the value of the text within the cell
 *********************************************************************************/
function Cell_TransformToInput(cell, inputType) {
    let value = cell.innerHTML;
    cell.innerHTML = "";

    let input = document.createElement("input");
    input.setAttribute('type', inputType);
    input.setAttribute('value', value);
    input.setAttribute('prevValue', value);
    cell.appendChild(input);
}
/*********************************************************************************
 * Function Name:   Cell_TransformFromInput
 * Parameters:      cell, getPreviousValue(bool)
 * Returns:         nothing
 * Description:     Reverts back a change done by Cell_TransformToInput.
 *                      getPrevious Value indicates whether to keep new values or revert to previous one.
 *********************************************************************************/
function Cell_TransformFromInput(cell, getPreviousValue = true) {
    if (getPreviousValue) {
        cell.innerHTML = cell.children[0].getAttribute('prevValue');
    } else {
        cell.innerHTML = cell.children[0].value;
    }
}

/*********************************************************************************
 * Function Name:   Cell_TransformToCheckbox
 * Parameters:      cell
 * Returns:         nothing
 * Description:     Transforms to checkbox, and interprets 'true' as a box that should be checked.
 *********************************************************************************/
function Cell_TransformToCheckbox(cell) {
    let value = cell.innerHTML;
    Cell_TransformToInput(cell, "checkbox");
    cell.children[0].checked = value =='true';
}

/*********************************************************************************
 * Function Name:   Cell_EnableCheckbox
 * Parameters:      cell(element with checkbox as first child), enabled(bool), revert(bool)
 * Returns:         nothing
 * Description:     Changes the enabled state of a checkbox within a cell
 *                      Saves or reverts to previous value based on the revert parameter
 *********************************************************************************/
function Cell_EnableCheckbox(cell, enabled = true, revert = false) {
    if(revert)cell.children[0].checked = cell.getAttribute('prevValue') == 'true';
    else cell.setAttribute('prevValue', cell.children[0].checked);
    cell.children[0].disabled = !enabled;
}