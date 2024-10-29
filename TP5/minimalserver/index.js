const usernameInput = document.getElementById("username-input");
const passwordInput = document.getElementById("password-input");

const login = () => {
    if (usernameInput.value != "" && passwordInput.value != "")
        fetch("http://localhost:8080/login", {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'X-HTTP-Method': 'POST'
            },
            body: { username: usernameInput.value, password: passwordInput.value }
        });
}

document.getElementById("login-button").addEventListener("click", login);