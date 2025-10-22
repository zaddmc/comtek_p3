function setup() {
	var toggle = document.getElementById("theme_toggle");

	var storedTheme = localStorage.getItem('theme') || (window.matchMedia("(prefers-color-scheme: dark)").matches ? "dark" : "light");
	if (storedTheme)
		document.documentElement.setAttribute('data-theme', storedTheme)


	toggle.onclick = function() {
		var currentTheme = document.documentElement.getAttribute("data-theme");
		var targetTheme = "light";

		if (currentTheme === "light") {
			targetTheme = "dark";
		}

		document.documentElement.setAttribute('data-theme', targetTheme)
		localStorage.setItem('theme', targetTheme);
	};


	let figures = document.querySelectorAll(".figure");
	let audio = document.getElementById("audio_tag");

	for (let i = 0; i < figures.length; i++) {
		figures[i].addEventListener("click", () => {
			audio.pause();
			audio.currentTime = 0;
			audio.play();
			figures[i].classList.remove("figure_go")
			void figures[i].offsetWidth;
			figures[i].classList.add("figure_go")
		});
	}

}
setup();
