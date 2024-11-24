import { initializeApp } from "https://www.gstatic.com/firebasejs/9.17.1/firebase-app.js";
import { getDatabase, ref, set, onValue } from "https://www.gstatic.com/firebasejs/9.17.1/firebase-database.js";

// Firebase Configuration
const firebaseConfig = {
    apiKey: "AIzaSyAfLEWczY84xV9KDQbFKAugAIXrBwKeP_w",
    authDomain: "smatrhome.firebaseapp.com",
    databaseURL: "https://smatrhome-default-rtdb.firebaseio.com",
    projectId: "smatrhome",
    storageBucket: "smatrhome.appspot.com",
    messagingSenderId: "123456789012",
    appId: "1:123456789012:web:abcdef123456"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

// Get references to Firebase paths
const commandRef = ref(database, "/control/command");
const sensorDataRef = ref(database, "/sensor/data");

// UI Elements
const ledOnButton = document.getElementById("led-on");
const ledOffButton = document.getElementById("led-off");
const sensorDataElement = document.getElementById("sensor-data");

// Write command to Firebase
const sendCommand = (command) => {
    set(commandRef, command)
        .then(() => console.log(`Command "${command}" sent successfully.`))
        .catch((error) => console.error("Error sending command:", error));
};

// Update the UI with sensor data
onValue(sensorDataRef, (snapshot) => {
    const data = snapshot.val();
    if (data !== null) {
        sensorDataElement.textContent = `Sensor Value: ${data}`;
    } else {
        sensorDataElement.textContent = "No data available";
    }
});

// Add event listeners for buttons
ledOnButton.addEventListener("click", () => sendCommand("LED_ON"));
ledOffButton.addEventListener("click", () => sendCommand("LED_OFF"));
