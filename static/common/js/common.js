function setup() {
	const event = new Event("theme-changed");
	var toggle = document.getElementById("theme_toggle");

	var storedTheme = localStorage.getItem('theme') || (window.matchMedia("(prefers-color-scheme: dark)").matches ? "dark" : "light");
	if (storedTheme) {
		document.documentElement.setAttribute('data-theme', storedTheme)
		document.dispatchEvent(event)
	}

	toggle.onclick = function() {
		var currentTheme = document.documentElement.getAttribute("data-theme");
		var targetTheme = "light";

		if (currentTheme === "light") {
			targetTheme = "dark";
		}

		document.documentElement.setAttribute('data-theme', targetTheme)
		localStorage.setItem('theme', targetTheme);
		document.dispatchEvent(event)
	};


	let figures = document.querySelectorAll(".figure");

	console.log(figures.length);
	for (let i = 0; i < figures.length; i++) {

		let audio = figures[i].querySelector(".figure_audio");
		figures[i].addEventListener("click", () => {
			audio.pause();
			audio.currentTime = 0;
			audio.play();
			figures[i].classList.remove("figure-go")
			void figures[i].offsetWidth;
			figures[i].classList.add("figure-go")
		});
	}

}
setup();
