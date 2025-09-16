// We match firmware UUID:
const SERVICE_UUID = '12345678-1234-5678-1234-56789abcdef0';
const CHAR_UUID    = '12345678-1234-5678-1234-56789abcdef1';

let device = null;
let server = null;
let ledChar = null;

function log(msg) {
    const el = document.getElementById('ble-log');
    if (el) el.textContent = msg;
    console.log('[BLE]', msg);
}

export async function connectESP32() {
  if (!('bluetooth' in navigator)) {
    log('Web Bluetooth not supported in this browser.');
    return;
  }
  try {
    device = await navigator.bluetooth.requestDevice({
      filters: [
        { namePrefix: 'ESP32-BLE-Poll' },
        { services: [SERVICE_UUID] }
      ],
      optionalServices: [SERVICE_UUID]
    });

    device.addEventListener('gattserverdisconnected', () => {
      log('Disconnected.');
      server = null;
      ledChar = null;
    });

    log(`Connecting to ${device.name || 'ESP32'}...`);
    server = await device.gatt.connect();

    const service = await server.getPrimaryService(SERVICE_UUID);
    ledChar = await service.getCharacteristic(CHAR_UUID);

    log('Connected! You can control the LED below.');
  } catch (err) {
    log('Error: ' + (err?.message || err));
  }
}

export async function ledOn() {
  if (!ledChar) return log('Not connected.');
  await ledChar.writeValue(new Uint8Array([0x01]));
  log('LED ON (0x01 sent).');
}

export async function ledOff() {
  if (!ledChar) return log('Not connected.');
  await ledChar.writeValue(new Uint8Array([0x00]));
  log('LED OFF (0x00 sent).');
}

export async function blink() {
    if (!ledChar) return log('not connected.');
    await ledChar.writeValue(new Uint8Array([0x7F]));
    log('Blink (0x7F sent).')
}

export function disconnectESP32() {
  try { if (device?.gatt?.connected) device.gatt.disconnect(); } catch (_) {}
  log('Disconnected.');
}