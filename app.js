import { initializeApp } from "https://www.gstatic.com/firebasejs/9.17.1/firebase-app.js";
import { getDatabase, ref, set, onValue } from "https://www.gstatic.com/firebasejs/9.17.1/firebase-database.js";

// Firebase Configuration
const firebaseConfig = {
    apiKey: "AIzaSyB0S9NtzkxVAHmU3A-7zLnE3-yTbrPB4dY",
    authDomain: "espcontroller-dc45b.firebaseapp.com",
    databaseURL: "https://espcontroller-dc45b-default-rtdb.firebaseio.com",
    projectId: "espcontroller-dc45b",
    storageBucket: "espcontroller-dc45b.firebasestorage.app",
    messagingSenderId: "504579441072",
    appId: "1:504579441072:web:4c4e5e25f0b8c762411675"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

// Get references to Firebase paths
// const commandRef = ref(database, "/control/command");
const led = ref(database, "/led/state");
// const sensorDataRef = ref(database, "/sensor/data");

// UI Elements
const ledOnButton = document.getElementById("led-on");
const ledOffButton = document.getElementById("led-off");
// const sensorDataElement = document.getElementById("sensor-data");

// Write command to Firebase
const sendCommand = (command) => {
    set(led, command)
        .then(() => console.log(`Command "${command}" sent successfully.`))
        .catch((error) => console.error("Error sending command:", error));
};

// Update the UI with sensor data
// onValue(sensorDataRef, (snapshot) => {
//     const data = snapshot.val();
//     if (data !== null) {
//         sensorDataElement.textContent = `Sensor Value: ${data}`;
//     } else {
//         sensorDataElement.textContent = "No data available";
//     }
// });

// Add event listeners for buttons
ledOnButton.addEventListener("click", () => sendCommand(1));
ledOffButton.addEventListener("click", () => sendCommand(0));
