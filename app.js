function sendCommand(url) {
    fetch(url)
        .then(response => response.text())
        .then(data => logMessage(data))
        .catch(error => logMessage("Error: " + error));
}

function logMessage(message) {
    const logArea = document.getElementById("log-area");
    const timestamp = new Date().toLocaleTimeString();
    logArea.innerHTML += `<p>[${timestamp}] ${message}</p>`;
    logArea.scrollTop = logArea.scrollHeight;
}

function clearLogArea() {
    const logArea = document.getElementById("log-area");
    logArea.innerHTML = '';
}
