$(() => {
    $("#logoutButton").on("click", TryLogout);
});
/*********************************************************************************
 * Function Name:   TryLogout
 * Parameters:      None
 * Returns:         Nothing
 * Description:     Packages data for 'logout' action and submits AJAX request
 *********************************************************************************/
function TryLogout(){
    let data = {};
    data['action'] = 'logout';
    Ajax(serverUrl, data, LogoutSuccess)
}
/*********************************************************************************
 * Function Name:   LogoutSuccess
 * Parameters:      Ajax Response
 * Returns:         Nothing
 * Description:     Redirects to login page
 *********************************************************************************/
function LogoutSuccess(response, status){
    window.location.replace("./Login/login.php");
}

