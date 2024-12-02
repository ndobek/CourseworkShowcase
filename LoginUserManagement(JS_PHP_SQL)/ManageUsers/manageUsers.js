//Stores all roles current user has authority over
subordinateRoles = [];

$(() => {
    Populate();
    $("#addUserButton").on("click", TryAddUser);
});

/*********************************************************************************
 * Function Name:   TryAddUser
 * Parameters:      None
 * Returns:         Nothing
 * Description:     Packages data for 'register' action and submits AJAX request
 *********************************************************************************/
function TryAddUser() {
    let cred = {};
    cred['username'] = $("#username").val();
    cred['password'] = $("#password").val();
    cred['role'] = $("#role option:selected").val();

    if (!Val_Credentials(cred['username'], cred['password'])) return;

    cred['action'] = 'register';

    Ajax(serverUrl, cred, GetAllUsersSuccess)
}
/*********************************************************************************
 * Function Name:   GetAllUsers
 * Parameters:      None
 * Returns:         Nothing
 * Description:     Packages data for 'getAllUsers' action and submits AJAX request
 *********************************************************************************/
function GetAllUsers() {
    let data = {};
    data['action'] = 'getAllUsers';
    Ajax(serverUrl, data, GetAllUsersSuccess);
}
/*********************************************************************************
 * Function Name:   GetAllUsersSuccess
 * Parameters:      Ajax Response
 * Returns:         Nothing
 * Description:     Creates users table
 *********************************************************************************/
function GetAllUsersSuccess(response) {
    UpdateStatus(response['status']);
    //Get Data
    if (!(response['data'])) return;
    let results = response['data']['results'];
    //Create Table
    let div = $("#usersTableDiv")[0];
    div.innerHTML = "";
    div.appendChild(Table_Build(results, "Users"));
    Table_IDRows("Users", 0);
    Table_AddColumn(0, "Users", "Op", CreateDeleteButton);
    Table_TransformColumn(4, "Users", TransformRoleCell);

}
/*********************************************************************************
 * Function Name:   CreateDeleteButton
 * Parameters:      Row element
 * Returns:         Cell element
 * Description:     Creates cell for delete button
 *********************************************************************************/
function CreateDeleteButton(row) {
    let cell = document.createElement("td");
    if (!subordinateRoles.includes(row.children[3].innerHTML)) return cell;

    let button = document.createElement("button");
    button.innerHTML = "Delete";
    button.addEventListener("click", () => TryDeleteUser(row.getAttribute("id")));

    cell.appendChild(button);
    return cell;
}
/*********************************************************************************
 * Function Name:   TransformRoleCell
 * Parameters:      Cell element
 * Returns:         Nothing
 * Description:     Creates a drop-down menu for all roles that the user has the authority to change
 *********************************************************************************/
function TransformRoleCell(cell) {
    role = cell.innerHTML
    if (!subordinateRoles.includes(role)) return cell;

    cell.innerHTML = "";
    let select = Build_Select(subordinateRoles, role);
    select.addEventListener('change', () => TryChangeUserRole(cell.parentElement.getAttribute('id'), select.options[select.selectedIndex].text))

    cell.appendChild(select);
}
/*********************************************************************************
 * Function Name:   TryDeleteUser
 * Parameters:      UserID
 * Returns:         Nothing
 * Description:     Packages data for 'deleteUser' action and submits AJAX request
 *********************************************************************************/
function TryDeleteUser(userID) {
    let data = {};
    data['action'] = 'deleteUser';
    data['user_id'] = userID;
    Ajax(serverUrl, data, GetAllUsersSuccess);
}
/*********************************************************************************
 * Function Name:   TryChangeUserRole
 * Parameters:      UserID, newRole
 * Returns:         Nothing
 * Description:     Packages data for 'editUserRole' action and submits AJAX request
 *********************************************************************************/
function TryChangeUserRole(userID, newRole) {
    let data = {};
    data['action'] = 'editUserRole';
    data['user_id'] = userID;
    data['role'] = newRole;
    Ajax(serverUrl, data, GetAllUsersSuccess);
}
/*********************************************************************************
 * Function Name:   Populate
 * Parameters:      None
 * Returns:         Nothing
 * Description:     Gets subordinate roles, and then gets all users after that.
 *********************************************************************************/
function Populate() {
    let data = {};
    data['action'] = 'getSubordinateRoles';
    Ajax(serverUrl, data, (r, s) => {
        subordinateRoles = [];
        for (role of r['data']['results']) {
            subordinateRoles.push(role['role']);
        }
        GetAllUsers();
        Build_Select(subordinateRoles, null, $("#role")[0]);
    });
}