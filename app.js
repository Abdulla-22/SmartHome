function sendCommand(url) {
    // Fetch the command URL from the ESP32
    fetch(url)
        .then(response => response.text()) // Get the response as text
        .then(data => logMessage(data))    // Display the response in the log area
        .catch(error => logMessage("Error: " + error)); // Log errors
}

function logMessage(message) {
    const logArea = document.getElementById("log-area");
    const timestamp = new Date().toLocaleTimeString();
    logArea.innerHTML += `<p>[${timestamp}] ${message}</p>`; // Append new message with timestamp
    logArea.scrollTop = logArea.scrollHeight; // Scroll to the bottom
}

function clearLogArea() {
    const logArea = document.getElementById("log-area");
    logArea.innerHTML = ''; // Clear the log area content
}
