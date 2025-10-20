// @ts-check 
import { setup_signup_drop_down } from "./dropdown.js";

/**
 * @typedef {import("./dropdown").DropDownInfo} DropDownInfo 
*/
/**
 *@param {string} cvr_url 
 */
export function setup(cvr_url) {
	let company_name_input = /** @type {HTMLInputElement } */ (document.getElementById("id_company_name"));
	let cvr_input = /** @type {HTMLInputElement } */ (document.getElementById("id_company_cvr"));
	let industri_code_input = /** @type {HTMLInputElement } */ (document.getElementById("id_company_industri_code"));
	let drop_down_div = /** @type {HTMLDivElement} */(document.getElementById("drop_down"));
	let drop_down_button = /** @type {HTMLButtonElement} */(document.getElementById("drop_down_button"));
	let drop_down_values = /** @type {HTMLUListElement} */ (document.getElementById("drop_down_values"));

	if (!company_name_input) {
		console.log("Company name input is null");
		return;
	}
	if (!cvr_input) {
		console.log("CVR input is null");
		return;
	}
	if (!industri_code_input) {
		console.log("Industri code input is null");
		return;
	}
	if (!drop_down_div) {
		console.log("Dropw down div is null");
		return;
	}
	if (!drop_down_button) {
		console.log("Dropw down button is null");
		return;
	}
	if (!drop_down_values) {
		console.log("Dropw down values is null");
		return;
	}
	/**  @type {DropDownInfo}*/
	const drop_down_info = {
		div: drop_down_div,
		input: company_name_input,
		button: drop_down_button,
		value_list: drop_down_values,
		industricode_input: industri_code_input,
		cvr_input: cvr_input
	};
	drop_down_div.prepend(cvr_input);

	setup_signup_drop_down(drop_down_info, cvr_url);
}
