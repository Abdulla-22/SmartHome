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
const GarageSystemStuteRef = ref(database, "/EnvironmentSystem/GarageSystemStute");
const EnvironmentSystemStuteRef = ref(database, "/EnvironmentSystem/Auto");
const EnvironmentSystemACStuteRef = ref(database, "/EnvironmentSystem/ACState");
const EnvironmentSystemTempeRef = ref(database, "/EnvironmentSystem/Temperature");
const EnvironmentSystemHumidityRef = ref(database, "/EnvironmentSystem/Humidity");
const SecuritySystemAlarmRef = ref(database, "/SecuritySystem/Alarm");
const SecuritySystemLastAccessRef = ref(database, "/SecuritySystem/LastAccess");
const GardenSystemStuteRef = ref(database, "/GardenSystem/Auto");
const GarageSystemWaterPumpRef = ref(database, "/GardenSystem/WaterPump");

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
const GarageSystemOpne = document.getElementById("opne-garag");
const GarageSystemClose = document.getElementById("close-garag");
const EnableEnvironmentAuto = document.getElementById("enable-environment-auto");
const EnableEnvironmentManual = document.getElementById("disable-environment-auto");
const ACState = document.getElementById("environment-ac-state");
const EnvironmentTempElement = document.getElementById("environment-temperature");
const EnvironmentHumElement = document.getElementById("environment-humidity");
const SecuritySystemAlarm = document.getElementById("Alarm");
const GarageSystemAuto = document.getElementById("water-garden-auto");
const GarageSystemManual = document.getElementById("water-garden-manual");
const GarageSystemWaterPumpOn = document.getElementById("water-garden-on");
const GarageSystemWaterPumpOff = document.getElementById("water-garden-off");
const GarageSystemStute = document.getElementById("garden-status");




// Garden System
const gardenSoilElement = document.getElementById("garden-soil-moisture");
const gardenRainElement = document.getElementById("garden-rain-sensor");
const gardenTempElement = document.getElementById("garden-temperature");
const gardenHumidityElement = document.getElementById("garden-humidity");
const greenLedElement = document.getElementById("green-led");
const redLedElement = document.getElementById("red-led");
const GarageSystemStuteElement = document.getElementById("GarageSystemStute");
const EnvironmentSystemStuteElement = document.getElementById("environment-status");

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
GarageSystemOpne.addEventListener("click", () => set(GarageSystemStuteRef, true));
GarageSystemClose.addEventListener("click", () => set(GarageSystemStuteRef, false));
EnableEnvironmentAuto.addEventListener("click", () => set(EnvironmentSystemStuteRef, true));
EnableEnvironmentManual.addEventListener("click", () => set(EnvironmentSystemStuteRef, false));
GarageSystemAuto.addEventListener("click", () => set(GardenSystemStuteRef, true));
GarageSystemManual.addEventListener("click", () => set(GardenSystemStuteRef, false));
GarageSystemWaterPumpOn.addEventListener("click", () => set(GarageSystemWaterPumpRef, true));
GarageSystemWaterPumpOff.addEventListener("click", () => set(GarageSystemWaterPumpRef, false));

// Arm and Disarm Buttons
armSecurityButton.addEventListener("click", () => set(securityArmedRef, 1));
disarmSecurityButton.addEventListener("click", () => set(securityArmedRef, 0));

// Garden System Sensors
updateUI(gardenSoilRef, gardenSoilElement, "Soil Moisture");
updateUI(gardenRainRef, gardenRainElement, "Rain", (data) => (data ? "Yes" : "No"));
updateUI(gardenTempRef, gardenTempElement, "Temperature");
updateUI(gardenHumidityRef, gardenHumidityElement, "Humidity");
updateUI(GarageSystemStuteRef, GarageSystemStuteElement, "Garage Stute");
updateUI(EnvironmentSystemStuteRef, EnvironmentSystemStuteElement, "Mode", (data) => (data ? "Auto" : "Manual"));
updateUI(EnvironmentSystemACStuteRef, ACState, "AC State", (data) => (data ? "ON" : "OFF"));
updateUI(EnvironmentSystemTempeRef, EnvironmentTempElement, "Temperature");
updateUI(EnvironmentSystemHumidityRef, EnvironmentHumElement, "Humidity");
updateUI(SecuritySystemAlarmRef, SecuritySystemAlarm, "Alarm", (data) => (data ? "ON" : "OFF"));
updateUI(SecuritySystemLastAccessRef, lastAccessElement, "Last Access was");
updateUI(GardenSystemStuteRef, GarageSystemStute, "Garden Stute", (data) => (data ? "Auto" : "Manual"));

// // Update LEDs for garden system
// onValue(gardenSoilRef, (snapshot) => {
//     const soilMoisture = snapshot.val();
//     if (soilMoisture !== null) {
//         const rain = gardenRainElement.textContent.includes("Yes");
//         const greenState = soilMoisture > 400 || rain; // Assume 400 as soil moisture threshold
//         const redState = !greenState;
//         toggleLed(greenLedElement, greenState, "green");
//         toggleLed(redLedElement, redState, "red");
//     }
// });

onValue(GarageSystemWaterPumpRef, (snapshot) => {
    const isWaterPumpOn = snapshot.val();
    toggleLed(greenLedElement, isWaterPumpOn, "green");
    toggleLed(redLedElement, !isWaterPumpOn, "red");
});

// Update UI with real-time data
onValue(securityArmedRef, (snapshot) => {
    const isArmed = snapshot.val();
    securityStatusElement.textContent = `Status: ${isArmed ? "Armed" : "Disarmed"}`;
});
