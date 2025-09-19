// @ts-check
/**
 * @param {Event} event 
	*/
function handle_notif(event) {

	const target = /**@type {BluetoothRemoteGATTCharacteristic  } */ (event.target);
	const val = target.value;
	if (!val) {
		return;
	}
	let a = [];

	for (let i = 0; i < val.byteLength; i++) {
		a.push(val.getUint8(i).toString());
	}
	console.log(`${a.join(" ")}`)

}
/**
	*@param {BufferSource} input_txt 
	*/
async function get_ble_devices(input_txt) {
	let filters = [{ name: "DUMME_DAVID" }]
	/**@type {RequestDeviceOptions} */
	let options = { filters: filters, optionalServices: [0x1815] };
	try {

		const device = await navigator.bluetooth.requestDevice(options)
		console.log(`Trying to connect to device: ${device.name}`);
		const server = await device.gatt?.connect();

		console.log(`Connected to device: ${device.name}`);

		console.log(`Getting service`);

		const service = await server?.getPrimaryService(0x1815);

		const charas = await service?.getCharacteristic(0x2A37);

		if (charas) {
			//await charas.startNotifications();

			//charas.addEventListener("characteristicvaluechanged", handle_notif);
			await charas.writeValueWithoutResponse(input_txt);

		}

	}
	catch (e) {
		console.log(e);
	}
}

function setup() {
	const encoder = new TextEncoder();
	let ble_input =/**@type {HTMLInputElement | null} */  (document.getElementById("BLEInput"));
	let ble_button =/**@type {HTMLButtonElement | null} */   (document.getElementById("connectBtn"));

	if (ble_button == null) {
		return;
	}

	if (ble_input == null) {
		return;
	}

	ble_button.addEventListener("click", () => {
		const ble_input_txt = ble_input.value;
		const encoded_txt = encoder.encode(ble_input_txt);
		get_ble_devices(encoded_txt);
	})

}


setup()



