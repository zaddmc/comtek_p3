function setup() {
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
