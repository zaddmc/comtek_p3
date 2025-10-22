// @ts-check 




function setup_taskbar() {
	console.log("droppies");

	let taskbar_tabs = document.querySelectorAll(".taskbar_dropdown");
	if (!taskbar_tabs) {
		console.log("no taskbar tabas");
		return;
	}
	console.log(taskbar_tabs.length)

	for (let i = 0; i < taskbar_tabs.length; i++) {
		let current_dropdown =/** @type {HTMLDivElement} **/ (taskbar_tabs[i]);
		current_dropdown.addEventListener("mouseover", () => {
			for (let y = 0; y < taskbar_tabs.length; y++) {
				if (taskbar_tabs[y] != current_dropdown) {
					taskbar_tabs[y].style.overflow = "hidden";
				}
			}
			current_dropdown.style.overflow = "visible";
		});

		current_dropdown.addEventListener("mouseleave", () => {
			for (let y = 0; y < taskbar_tabs.length; y++) {
				taskbar_tabs[y].style.overflow = "hidden";
			}
		});

	}

}

setup_taskbar();
