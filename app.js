let bluetoothDevice;
let bluetoothServer;
let bluetoothCharacteristic;

// UUIDs for the Bluetooth service and characteristic (adjust based on your module)
const SERVICE_UUID = '0000ffe0-0000-1000-8000-00805f9b34fb'; // Example UUID
const CHARACTERISTIC_UUID = '0000ffe1-0000-1000-8000-00805f9b34fb';

document.getElementById('connect-btn').addEventListener('click', async () => {
    try {
        // Request Bluetooth device
        bluetoothDevice = await navigator.bluetooth.requestDevice({
            acceptAllDevices: true,
            optionalServices: [SERVICE_UUID]
        });

        // Connect to the Bluetooth server
        bluetoothServer = await bluetoothDevice.gatt.connect();

        // Get the primary service
        const service = await bluetoothServer.getPrimaryService(SERVICE_UUID);

        // Get the characteristic
        bluetoothCharacteristic = await service.getCharacteristic(CHARACTERISTIC_UUID);

        alert('Connected to Arduino!');
    } catch (error) {
        console.error('Bluetooth connection failed:', error);
        alert('Bluetooth connection failed!');
    }
});

document.getElementById('led-on-btn').addEventListener('click', () => {
    sendData('1'); // Command to turn LED on
});

document.getElementById('led-off-btn').addEventListener('click', () => {
    sendData('0'); // Command to turn LED off
});

async function sendData(command) {
    if (!bluetoothCharacteristic) {
        alert('Not connected to Arduino!');
        return;
    }
    const encoder = new TextEncoder();
    await bluetoothCharacteristic.writeValue(encoder.encode(command));
    alert(`Command "${command}" sent to Arduino`);
}
