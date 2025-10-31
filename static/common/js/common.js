let audioContext = null;
let audioBuffer = [];
function setup() {
	const event = new Event("theme-changed");
	var toggle = document.getElementById("theme_toggle");

	var storedTheme = localStorage.getItem('theme') || (window.matchMedia("(prefers-color-scheme: dark)").matches ? "dark" : "light");
	if (storedTheme) {
		document.documentElement.setAttribute('data-theme', storedTheme)
		document.dispatchEvent(event)
	}
	var toggle = document.getElementById("theme_toggle");


	toggle.onclick = function() {
		var currentTheme = document.documentElement.getAttribute("data-theme");
		var targetTheme = "light";

		if (currentTheme === "light") {
			targetTheme = "dark";
		}

		document.documentElement.setAttribute("data-theme", targetTheme);
		localStorage.setItem("theme", targetTheme);
		document.dispatchEvent(event)
	};


	let figures = document.querySelectorAll(".figure");


	async function loadAudio(url) {
		return await fetch(url)
			.then(response => response.arrayBuffer())
			.then(arrayBuffer => audioContext.decodeAudioData(arrayBuffer))
			.then(buffer => {
				return buffer
			});
	}
	function playBuffer(audio_buffer) {
		if (!audio_buffer) return;
		const source = audioContext.createBufferSource();
		source.buffer = audio_buffer;
		source.connect(audioContext.destination);
		source.start();
	}


	console.log(figures.length);
	for (let i = 0; i < figures.length; i++) {
		audioBuffer.push(null)
		let url = figures[i].getAttribute("data-audio-src")
		console.log(url)
		figures[i].addEventListener("click", async () => {
			if (!audioContext) {
				audioContext = new (window.AudioContext || window.webkitAudioContext)()
			}
			if (!audioBuffer[i]) {
				audioBuffer[i] = await loadAudio(url);
			}
			console.log(audioBuffer[i])

			playBuffer(audioBuffer[i]);
			figures[i].classList.remove("figure-go");
			void figures[i].offsetWidth;
			figures[i].classList.add("figure-go");
		});
	}

}

setup();
