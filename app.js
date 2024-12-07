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
const securityArmedRef = ref(database, "/SecuritySystem/Armed");
const lastAccessRef = ref(database, "/SecuritySystem/LastAccess");
const indoorLightAutoRef = ref(database, "/LightingSystem/Indoor/Auto");
const indoorLedRef = ref(database, "/LightingSystem/Indoor/State");
const outdoorLightAutoRef = ref(database, "/LightingSystem/Outdoor/Auto");
const outdoorLedRef = ref(database, "/LightingSystem/Outdoor/State");
const gardenSoilRef = ref(database, "/GardenSystem/SoilMoisture");
const gardenRainRef = ref(database, "/GardenSystem/Rain");
const gardenTempRef = ref(database, "/GardenSystem/Temperature");
const gardenHumidityRef = ref(database, "/GardenSystem/Humidity");

// UI Elements
// Lighting
const indoorLedOnButton = document.getElementById("indoor-led-on");
const indoorLedOffButton = document.getElementById("indoor-led-off");
const outdoorLightAutoButton = document.getElementById("outdoor-light-auto");
const outdoorLightManualButton = document.getElementById("outdoor-light-manual");
const indoorLightAutoButton = document.getElementById("indoor-light-auto");
const indoorLightManualButton = document.getElementById("indoor-light-manual");
const outdoorLedOnButton = document.getElementById("outdoor-led-on");
const outdoorLedOffButton = document.getElementById("outdoor-led-off");
const armSecurityButton = document.getElementById("arm-security");
const disarmSecurityButton = document.getElementById("disarm-security");
const securityStatusElement = document.getElementById("security-status");
const lastAccessElement = document.getElementById("last-access");

// Garden System
const gardenSoilElement = document.getElementById("garden-soil-moisture");
const gardenRainElement = document.getElementById("garden-rain-sensor");
const gardenTempElement = document.getElementById("garden-temperature");
const gardenHumidityElement = document.getElementById("garden-humidity");
const greenLedElement = document.getElementById("green-led");
const redLedElement = document.getElementById("red-led");

// Utility Functions
const toggleLed = (element, state, color) => {
    element.className = state ? `led-on-${color}` : "led-off";
};

// Update UI with real-time data
const updateUI = (ref, element, label, transformFn) => {
    onValue(ref, (snapshot) => {
        const data = snapshot.val();
        const displayValue = transformFn ? transformFn(data) : data;
        element.textContent = `${label}: ${displayValue}`;
    });
};

// Lighting Controls
indoorLedOnButton.addEventListener("click", () => set(indoorLedRef, 1));
indoorLedOffButton.addEventListener("click", () => set(indoorLedRef, 0));
outdoorLightAutoButton.addEventListener("click", () => set(outdoorLightAutoRef, true));
outdoorLightManualButton.addEventListener("click", () => set(outdoorLightAutoRef, false));
indoorLightAutoButton.addEventListener("click", () => set(indoorLightAutoRef, true));
indoorLightManualButton.addEventListener("click", () => set(indoorLightAutoRef, false));
outdoorLedOnButton.addEventListener("click", () => set(outdoorLedRef, 1));
outdoorLedOffButton.addEventListener("click", () => set(outdoorLedRef, 0));

// Arm and Disarm Buttons
armSecurityButton.addEventListener("click", () => set(securityArmedRef, true));
disarmSecurityButton.addEventListener("click", () => set(securityArmedRef, false));

// Garden System Sensors
updateUI(gardenSoilRef, gardenSoilElement, "Soil Moisture");
updateUI(gardenRainRef, gardenRainElement, "Rain", (data) => (data ? "Yes" : "No"));
updateUI(gardenTempRef, gardenTempElement, "Temperature");
updateUI(gardenHumidityRef, gardenHumidityElement, "Humidity");
// updateUI(outdoorLedRef, gardenRainElement, "Outdoor LED", (data) => (data === 1 ? "ON" : "OFF"));

// Update LEDs for garden system
onValue(gardenSoilRef, (snapshot) => {
    const soilMoisture = snapshot.val();
    if (soilMoisture !== null) {
        const rain = gardenRainElement.textContent.includes("Yes");
        const greenState = soilMoisture > 400 || rain; // Assume 400 as soil moisture threshold
        const redState = !greenState;
        toggleLed(greenLedElement, greenState, "green");
        toggleLed(redLedElement, redState, "red");
    }
});

// Update UI with real-time data
onValue(securityArmedRef, (snapshot) => {
    const isArmed = snapshot.val();
    securityStatusElement.textContent = `Status: ${isArmed ? "Armed" : "Disarmed"}`;
});

onValue(lastAccessRef, (snapshot) => {
    const lastAccess = snapshot.val();
    lastAccessElement.textContent = `Last Access: ${lastAccess || "None"}`;
});