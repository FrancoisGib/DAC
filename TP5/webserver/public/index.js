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

const memoryQuery = async (action) => {
   const res = await fetch("/memory-test", {
      method: "POST", body: action
   });
   const content = await res.text();
   if (res.ok) {
      document.getElementById("memory-used-p").textContent = `Memory used: ${content / 1000000}mo`;
   }
   else {
      document.getElementById("memory-used-p").textContent = "Error";
   }
}

const threadQuery = () => {
   fetch("/calculation-test", {
      method: "POST", body: `NB_THREADS ${document.getElementById("set-threads-input").value}`
   });
}

const delayQuery = () => {
   fetch("/delay-test", {
      method: "POST", body: document.getElementById("set-delay-input").value
   });
}

const linpackQuery = async() => {
   const startTime = new Date();
   const query = fetch("/linpack-test");
   document.getElementById("linpack-p").textContent = "Waiting for Linpack stress test to finish";
   await query;
   const endTime = new Date();
   const timeElapsed = (endTime - startTime) / 1000;
   document.getElementById("linpack-p").textContent = `Linpack stress test done in ${timeElapsed} seconds`;
}

document.getElementById("fetch-button").addEventListener("click", submitQuery);
document.getElementById("increase-button").addEventListener("click", () => memoryQuery("INCREASE"));
document.getElementById("decrease-button").addEventListener("click", () => memoryQuery("DECREASE"));
document.getElementById("set-memory-button").addEventListener("click", () => memoryQuery(`SET ${document.getElementById("set-memory-input").value}`));
document.getElementById("set-threads-button").addEventListener("click", () => threadQuery());
document.getElementById("set-delay-button").addEventListener("click", () => delayQuery());
document.getElementById("linpack-button").addEventListener("click", () => linpackQuery());
