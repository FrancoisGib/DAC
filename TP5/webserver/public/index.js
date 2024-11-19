const submitQuery = async () => {
   const nameInput = document.getElementById("name-input");
   const passwordInput = document.getElementById("password-input");
   if (nameInput.value != "" && passwordInput.value != "" && nameInput.value != undefined && passwordInput.value != undefined) {
      const res = await fetch("/hostname", {
         method: "POST", body: `username: ${nameInput.value}, password: ${passwordInput.value}`
      });
      const content = await res.text();
      if (res.ok) {
         document.getElementById("content-p").innerHTML = content;
      }
      else {
         document.getElementById("content-p").innerHTML = "<p>UNAUTHORIZED</p>";
      }
   }
}

document.getElementById("fetch-button").addEventListener("click", submitQuery);
