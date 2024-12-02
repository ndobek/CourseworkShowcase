//Stores all roles current user has authority over
subordinateRoles = [];

$(() => {
    $("#addRoleButton").on("click", TryAddRole);
    GetAllRoles();
});
/*********************************************************************************
 * Function Name:   TryAddRole
 * Parameters:      None
 * Returns:         Nothing
 * Description:     Packages data for 'addRole' action and submits AJAX request
 *********************************************************************************/
function TryAddRole() {
    let data = {};
    data['name'] = $("#roleName").val();
    data['action'] = 'addRole';

    if (!Val_Role(data['name'])) return;

    Ajax(serverUrl, data, GetAllRolesSuccess)
}
/*********************************************************************************
 * Function Name:   GetAllRoles
 * Parameters:      None
 * Returns:         Nothing
 * Description:     Packages data for 'getAllRoles' action and submits AJAX request
 *********************************************************************************/
function GetAllRoles() {
    let data = {};
    data['action'] = 'getAllRoles';
    Ajax(serverUrl, data, GetAllRolesSuccess);
}

/*********************************************************************************
 * Function Name:   GetAllRolesSuccess
 * Parameters:      Ajax response
 * Returns:         Nothing
 * Description:     Creates Roles table
 *********************************************************************************/
function GetAllRolesSuccess(response) {
    //Update Status
    UpdateStatus(response['status']);
    if (!(response['data'])) return;

    //Populate subordinate roles
    subordinateRoles = [];
    for (role of response['data']['subordinateRoles']) {
        subordinateRoles.push(role['role_id']);
    }

    //Create Table
    let results = response['data']['results'];
    let div = $("#rolesTableDiv")[0];
    div.innerHTML = "";
    div.appendChild(Table_Build(results, "Roles"));
    Table_IDRows("Roles", 0);
    Table_AddColumn(0, "Roles", "Op", CreateMainButtons);

    //Change Permission Columns
    for (i = 5; i < $("#Roles tr")[0].children.length; ++i) {
        Table_TransformColumn(i, "Roles", Cell_TransformToCheckbox);
        Table_TransformColumn(i, "Roles", (c) => Cell_EnableCheckbox(c, false));
    }
}

/*********************************************************************************
 * Function Name:   CreateMainButtons
 * Parameters:      Row element, cell element
 * Returns:         Nothing
 * Description:     returns cell element with buttons from information in row
 *********************************************************************************/
function CreateMainButtons(row, cell = null) {
    if (cell == null) cell = document.createElement("td");
    //Only do if user has authority over id
    if (!subordinateRoles.includes(row.getAttribute("id"))) return cell;

    cell.innerHTML = "";

    //Delete Button
    let deleteButton = document.createElement("button");
    deleteButton.innerHTML = "Delete";
    deleteButton.addEventListener("click", () => TryDeleteRole(row.getAttribute("id")));
    cell.appendChild(deleteButton);

    //Edit Button
    let editButton = document.createElement("button");
    editButton.innerHTML = "Edit";
    editButton.addEventListener("click", () => ChangeRowToEditMode(row));
    cell.appendChild(editButton);

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
        //Only do if user has authority over id
    if (!subordinateRoles.includes(row.getAttribute("id"))) return cell;

    cell.innerHTML = "";

    //Confirm Button
    let confirmButton = document.createElement("button");
    confirmButton.innerHTML = "Confirm";
    confirmButton.addEventListener("click", () => { TryEditRole(row.getAttribute("id")) });
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
    Cell_TransformToInput(row.children[2], "text");
    Cell_TransformToInput(row.children[3], "text");
    Cell_TransformToInput(row.children[4], "number");
    //Permissions
    for (i = 5; i < row.children.length; ++i) {
        Cell_EnableCheckbox(row.children[i], true);
    }
}
/*********************************************************************************
 * Function Name:   ChangeRowToMainMode
 * Parameters:      Row element, boolean indication whether to revert to previous value
 * Returns:         Nothing
 * Description:     Change a single row to main mode
 *********************************************************************************/
function ChangeRowToMainMode(row, getPrevValue = true) {
    CreateMainButtons(row, row.children[0]);
    Cell_TransformFromInput(row.children[2], getPrevValue);
    Cell_TransformFromInput(row.children[3], getPrevValue);
    Cell_TransformFromInput(row.children[4], getPrevValue);
    //Permissions
    for (i = 5; i < row.children.length; ++i) {
        Cell_EnableCheckbox(row.children[i], false, getPrevValue);
    }
}

/*********************************************************************************
 * Function Name:   TryDeleteRole
 * Parameters:      Role ID
 * Returns:         Nothing
 * Description:     Packages data for 'deleteRole' action and submits AJAX request
 *********************************************************************************/
function TryDeleteRole(roleID) {
    let data = {};
    data['action'] = 'deleteRole';
    data['role_id'] = roleID;
    Ajax(serverUrl, data, DeleteRowSuccess);
}
/*********************************************************************************
 * Function Name:   TryEditRole
 * Parameters:      Role ID
 * Returns:         Nothing
 * Description:     Packages data for 'editRole' action and submits AJAX request
 *********************************************************************************/
function TryEditRole(roleID) {
    let data = {};
    data['action'] = 'editRole';
    data['role_id'] = roleID;
    data['name'] = $("#" + roleID + " td[column ='Title'] input").val();
    data['description'] = $("#" + roleID + " td[column ='Description'] input").val();
    data['rank'] = $("#" + roleID + " td[column ='Rank'] input").val();

    data['permissions'] = {};
    $("#" + roleID + " input[type='checkbox']").each((i, e) => {
        data['permissions'][e.parentElement.getAttribute('column')] = e.checked;
    });

    Ajax(serverUrl, data, EditRoleSuccess);
}
/*********************************************************************************
 * Function Name:   EditRoleSuccess
 * Parameters:      Ajax response
 * Returns:         Nothing
 * Description:     Changes row back to main mode
 *********************************************************************************/
function EditRoleSuccess(response) {
    UpdateStatus(response['status']);
    if (!response['data']) return;
    ChangeRowToMainMode($("#" + response['data']['role_id'])[0], false);
}
/*********************************************************************************
 * Function Name:   DeleteRowSuccess
 * Parameters:      Ajax response
 * Returns:         Nothing
 * Description:     Deletes row from table
 *********************************************************************************/
function DeleteRowSuccess(response) {
    UpdateStatus(response['status']);
    if (!response['data']) return;
    $("#" + response['data']['role_id'])[0].remove();
}