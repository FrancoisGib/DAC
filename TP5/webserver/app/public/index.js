const usernameInput = document.getElementById("username-input");
const passwordInput = document.getElementById("password-input");

const login = async () => {
    if (usernameInput.value != "" && passwordInput.value != "") {
        const res = await fetch("http://localhost:8080/login", {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ username: usernameInput.value, password: passwordInput.value })
        });
        const hostnameElement = document.getElementById("hostname-p");
        if (res.ok) {
            const hostname = await res.text();
            hostnameElement.textContent = `Hostname: ${hostname}`;
        }
        else {
            hostnameElement.textContent = "";
        }
    }
}

document.getElementById("login-button").addEventListener("click", login);