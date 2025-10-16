// @ts-check 
/**
 * @typedef {{name: string, cvr_number:number, industrycode:string}} Company 
*/

/**
 * @typedef {{div: HTMLDivElement, value_list:HTMLUListElement, button:HTMLButtonElement,input:HTMLInputElement,cvr_input:HTMLInputElement,industricode_input:HTMLInputElement}} DropDownInfo 
*/


/**
	*@param {DropDownInfo} input 
	*@param {Company} company_info 
	*/
function set_input_val(input, company_info) {
	return () => {
		console.log("Hello");
		input.input.value = company_info.name;
		input.cvr_input.value = company_info.cvr_number.toString();
		input.industricode_input.value = company_info.industrycode;
	};
}
/**
 *@param {string} company_name
 *@param {number} cvr_number 
 *@param {string} industricode
 @returns {HTMLLIElement}
 */
function create_cvr_list_element(company_name, cvr_number, industricode) {

	const parent_list = document.createElement("li");
	const wrapper = document.createElement("div");
	const name_display = document.createElement("h4");
	const cvr_display = document.createElement("h4");
	const industricode_display = document.createElement("h4");

	name_display.innerHTML = "Name: " + company_name;
	cvr_display.innerHTML = "CVR: " + cvr_number.toString();
	industricode_display.innerHTML = "Industricode: " + industricode;

	wrapper.appendChild(name_display);
	wrapper.appendChild(cvr_display);
	wrapper.appendChild(industricode_display);

	parent_list.appendChild(wrapper);

	parent_list.classList.add("dropw_down_value");

	return parent_list;
}

/**
 * @param {DropDownInfo} input 
 *@param {Company[]} companies 
 */
async function update_dropdown(input, companies) {

	for (let i = 0; i < companies.length; i++) {
		let company = companies[i];
		let new_dropdown_value = create_cvr_list_element(company.name, company.cvr_number, company.industrycode);
		new_dropdown_value.addEventListener("click", set_input_val(input, company));
		input.value_list.appendChild(new_dropdown_value);
	}
}
/**
 *
 *@param {string} cvr_url 
 *@param {string} company_name 
 *@returns {Promise< Company[] | undefined>}
 */
async function handle_input_company_name(cvr_url, company_name) {
	const resp = await fetch(cvr_url + "name/" + company_name);
	if (resp.status < 200 || resp.status >= 300) {
		return;
	}
	/** @type {{companies: Company[]}} */
	const json_obj = await resp.json();

	return json_obj.companies;
}

/**
 *
 *@param {string} cvr_url 
 *@param {number} cvr 
 *@returns {Promise< Company[] | undefined>}
 */
async function handle_input_cvr(cvr_url, cvr) {
	const cvr_number = cvr.toString()
	const resp = await fetch(cvr_url + "cvr/" + cvr_number);
	if (resp.status < 200 || resp.status >= 300) {
		return;
	}
	/** @type {{companies: Company[]}} */
	const json_obj = await resp.json();
	return json_obj.companies;
}

/**
 * @param {DropDownInfo} drop_down_info
 *@param {string} cvr_url 
 */
function handle_cvr_keyup(drop_down_info, cvr_url) {
	/**
	   *@param {Event| undefined} event 
	 */
	return async (event) => {
		if (!event) {
			return;
		}
		if (!event.target) {
			return;
		}
		const target = /** @type {HTMLInputElement} */  (event.target);
		const target_val_str = target.value;
		const MIN_ACTIVATION_LENGTH = 2;

		if (target_val_str.length < MIN_ACTIVATION_LENGTH) {
			return;
		}
		const target_val_int = parseInt(target_val_str);
		let companies;
		if (target_val_str.length == 8 && !isNaN(target_val_int)) {
			console.log(target_val_int);
			companies = await handle_input_cvr(cvr_url, target_val_int);
		}
		else {
			companies = await handle_input_company_name(cvr_url, target_val_str);
		}
		if (!companies) {
			return;
		}
		drop_down_info.value_list.innerHTML = "";
		update_dropdown(drop_down_info, companies);
	}
}
/**
 *@param {HTMLDivElement} drop_down_div 
 */
function toggle_drop_down_display(drop_down_div) {
	return (event) => {
		if (drop_down_div.style.display == "") {
			drop_down_div.style.display = "none";
		}
		else {
			drop_down_div.style.display = "";
		}

	}
}

/**
	*@param {DropDownInfo} drop_down_info 
	*@param {string} cvr_url 
	*/
export function setup_signup_drop_down(drop_down_info, cvr_url) {
	drop_down_info.input.addEventListener("keyup", handle_cvr_keyup(drop_down_info, cvr_url));
	drop_down_info.button.addEventListener("click", toggle_drop_down_display(drop_down_info.div));
}
