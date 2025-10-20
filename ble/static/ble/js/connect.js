// @ts-check 

const BLE_SERVICE_UUID = 0x1815;
const WRITE_CHR_UUID = 0x2A37;

const BLE_CONNECT_BUTTON_ID = "ble_connect_btn";
const BLE_UNLOCK_BUTTON_ID = "ble_unlock_btn";
const BLE_DISCONNECT_BUTTON_ID = "ble_disconnect_btn";

/**
 *
 *@param {Event} event 
 */
function handle_notif(event) {
	console.log("GOT SOMETHING")
	const val = /** @type {BluetoothRemoteGATTCharacteristic} */ (event.target).value;
	if (!val) {
		return;
	}
	let a = [];

	for (let i = 0; i < val.byteLength; i++) {
		a.push(val.getUint8(i));
	}
	console.log(`${a.join(" ")}`)

	if (a[1] == 0) {
		console.log("Successfull unlock")
	}
	else {
		console.log("Not successfull unlock")
	}

}


/**
 * @param {String} ble_name 
 * @returns {Promise<[BluetoothRemoteGATTServer| null,BluetoothRemoteGATTCharacteristic| null]>} 
 */
async function connect_to_ble_characteristic(ble_name) {
	/** @type {BluetoothServiceUUID[]} */
	const service_uuids = [BLE_SERVICE_UUID];
	/** @type {BluetoothLEScanFilter[]} */
	const filters = [{ name: ble_name }];
	/** @type {RequestDeviceOptions} */
	const options = { filters: filters, optionalServices: service_uuids };
	try {
		/** @type {BluetoothDevice} */
		let device = await navigator.bluetooth.requestDevice(options);
		console.log(`Attempting to connect to ${device.name}`);
		if (!device.gatt) {
			console.log("Device has no gatt server");
			return [null, null];
		}
		/** @type {BluetoothRemoteGATTServer} */
		const gatt_server = await device.gatt.connect();
		console.log(`Successfully connected to ${device.name}`);
		console.log(`Getting service`);
		/** @type {BluetoothRemoteGATTService} */
		const ble_write_service = await gatt_server.getPrimaryService(BLE_SERVICE_UUID);
		/** @type {BluetoothRemoteGATTCharacteristic} */
		const ble_write_characteristic = await ble_write_service.getCharacteristic(WRITE_CHR_UUID);

		await ble_write_characteristic.startNotifications();
		ble_write_characteristic.addEventListener("characteristicvaluechanged", handle_notif);

		return [gatt_server, ble_write_characteristic];

	}
	catch (e) {
		console.log(e)
		return [null, null]
	}

}
/**
 * @param {BluetoothRemoteGATTCharacteristic} chr 
 * @param {String} msg 
 */
async function write_to_characteristic(chr, msg) {
	const encoder = new TextEncoder();
	const byte_msg = encoder.encode(msg);
	await chr.writeValueWithResponse(byte_msg);
	console.log("Wrote to device");

}
/**
 *@param {String} ble_name 
 *@param {String} encoded_key 
 *
 */
function setup(ble_name, encoded_key) {
	if (encoded_key == "null") {
		console.log("No encoded key")
		return;
	}
	console.log(encoded_key)
	let connect_button = /** @type {HTMLButtonElement | null} */ (document.getElementById(BLE_CONNECT_BUTTON_ID));
	let unlock_button = /** @type {HTMLButtonElement | null} */ (document.getElementById(BLE_UNLOCK_BUTTON_ID));
	let disconnect_button = /** @type {HTMLButtonElement | null} */ (document.getElementById(BLE_DISCONNECT_BUTTON_ID));
	/** @type {BluetoothRemoteGATTCharacteristic | null} */
	let chr = null;

	/** @type {BluetoothRemoteGATTServer| null} */
	let gatt_server = null;

	if (connect_button == null) {
		console.log("No connect button");
		return;
	}

	if (unlock_button == null) {
		console.log("No unlock button");
		return;
	}

	if (disconnect_button == null) {
		console.log("No disconnect button");
		return;
	}
	connect_button.addEventListener("click", async () => {
		/** @type{[BluetoothRemoteGATTServer| null,BluetoothRemoteGATTCharacteristic| null]} */
		const [temp_gatt, temp_chr] = await connect_to_ble_characteristic(ble_name);
		if (temp_gatt == null) {
			console.log(`Could not connect to ${ble_name}`);
			return;
		}
		if (temp_chr == null) {
			console.log(`Could not connect to ${ble_name}`);
			return;
		}
		chr = temp_chr;
		gatt_server = temp_gatt;

		console.log(`Connected to ${ble_name}`);
		return;
	});
	unlock_button.addEventListener("click", async () => {
		if (chr == null) {
			console.log("CHR is not set");
			return;
		}
		await write_to_characteristic(chr, encoded_key);
	});
	disconnect_button.addEventListener("click", async () => {
		if (chr == null) {
			console.log("CHR is not set");
			return;
		}
		if (gatt_server == null) {
			console.log("Device is not set");
			return;
		}
		gatt_server.disconnect();

		gatt_server = null;
		chr = null;
		console.log("Disconnected")

	});
}
