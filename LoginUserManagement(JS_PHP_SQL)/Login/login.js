$(() => {
    $("#loginButton").on("click", () => { TrySubmitCredentials('login', LoginSuccess) });
    $("#registerButton").on("click", () => { TrySubmitCredentials('register', RegisterSuccess) });
});

/*********************************************************************************
 * Function Name:   LoginSuccess
 * Parameters:      Ajax response
 * Returns:         Nothing
 * Description:     Handles Login
 *********************************************************************************/
function LoginSuccess(response) {
    if (response['success']) {
        window.location.replace("./../index.php");
    } else{
        UpdateStatus(response['status']);
    }
}
/*********************************************************************************
 * Function Name:   RegisterSuccess
 * Parameters:      Ajax response
 * Returns:         Nothing
 * Description:     Handles Register
 *********************************************************************************/
function RegisterSuccess(response) {
    UpdateStatus(response['status']);
}