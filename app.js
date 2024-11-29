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

// Firebase Paths
const indoorLedRef = ref(database, "/LightingSystem/InDoor");
const outdoorLedRef = ref(database, "/LightingSystem/OutDoor");
const gardenSoilRef = ref(database, "/GardenSystem/soilMoisture");
const gardenTempRef = ref(database, "/GardenSystem/temperature");
const envTempRef = ref(database, "/EnvironmentSystem/temperature");
const envHumidityRef = ref(database, "/EnvironmentSystem/humidity");
const motionSensorRef = ref(database, "/SecuritySystem/motion");
const doorSensorRef = ref(database, "/SecuritySystem/door");
const fanRef = ref(database, "/EnvironmentSystem/fan");
const acRef = ref(database, "/EnvironmentSystem/ac");
const alarmRef = ref(database, "/SecuritySystem/alarm");

// UI Elements
// Lighting
const indoorLedOnButton = document.getElementById("indoor-led-on");
const indoorLedOffButton = document.getElementById("indoor-led-off");
const outdoorLedOnButton = document.getElementById("outdoor-led-on");
const outdoorLedOffButton = document.getElementById("outdoor-led-off");

// Garden System
const gardenSoilElement = document.getElementById("garden-soil-moisture");
const gardenTempElement = document.getElementById("garden-temperature");

// Environment Control
const envTempElement = document.getElementById("env-temperature");
const envHumidityElement = document.getElementById("env-humidity");
const fanOnButton = document.getElementById("fan-on");
const fanOffButton = document.getElementById("fan-off");
const acOnButton = document.getElementById("ac-on");
const acOffButton = document.getElementById("ac-off");

// Security System
const activateAlarmButton = document.getElementById("activate-alarm");
const deactivateAlarmButton = document.getElementById("deactivate-alarm");
const motionSensorElement = document.getElementById("motion-sensor-status");
const doorSensorElement = document.getElementById("door-sensor-status");

// Write command to Firebase
const sendCommand = (ref, command) => {
    set(ref, command)
        .then(() => console.log(`Command "${command}" sent successfully to ${ref.path.pieces_.join('/')}.`))
        .catch((error) => console.error("Error sending command:", error));
};

// Update UI with real-time data
const updateUI = (ref, element, label) => {
    onValue(ref, (snapshot) => {
        const data = snapshot.val();
        if (data !== null) {
            element.textContent = `${label}: ${data}`;
        } else {
            element.textContent = `${label}: No data available`;
        }
    });
};

// Lighting Controls
indoorLedOnButton.addEventListener("click", () => sendCommand(indoorLedRef, 1));
indoorLedOffButton.addEventListener("click", () => sendCommand(indoorLedRef, 0));
outdoorLedOnButton.addEventListener("click", () => sendCommand(outdoorLedRef, 1));
outdoorLedOffButton.addEventListener("click", () => sendCommand(outdoorLedRef, 0));

// Garden System Sensors
updateUI(gardenSoilRef, gardenSoilElement, "Soil Moisture");
updateUI(gardenTempRef, gardenTempElement, "Temperature");

// Environment Control
fanOnButton.addEventListener("click", () => sendCommand(fanRef, 1));
fanOffButton.addEventListener("click", () => sendCommand(fanRef, 0));
acOnButton.addEventListener("click", () => sendCommand(acRef, 1));
acOffButton.addEventListener("click", () => sendCommand(acRef, 0));
updateUI(envTempRef, envTempElement, "Temperature");
updateUI(envHumidityRef, envHumidityElement, "Humidity");

// Security System
activateAlarmButton.addEventListener("click", () => sendCommand(alarmRef, 1));
deactivateAlarmButton.addEventListener("click", () => sendCommand(alarmRef, 0));
updateUI(motionSensorRef, motionSensorElement, "Motion Sensor");
updateUI(doorSensorRef, doorSensorElement, "Door Sensor");
