function load_pie(rented, not_rented) {
	let style = window.getComputedStyle(document.body)
	var data = [{
		type: "pie",
		values: [rented, not_rented],
		labels: ["Rented", "Not Rented"],
		textinfo: "label+percent",
		textposition: "inside",
		insidetextorientation: "radial"
	}]

	var layout = {
		paper_bgcolor: "rgb(0,0,0,0)",
		autosize: true,
		margin: { "b": 0, "l": 0, "t": 0, "r": 0 },
		showlegend: true,
		width: 300,
		height: 300,
		legend: {
			x: 0.2,
			y: -0.1,
			bgcolor: style.getPropertyValue("--background"),
			font: {
				color: style.getPropertyValue("--text"),
			}
		}


	}
	var config = {
		responesive: true
	}

	return [data, layout, config]

}

function setup_pie(rented, not_rented) {
	let [data, layout, config] = load_pie(rented, not_rented)
	Plotly.newPlot('pie_chart', data, layout, config);
	document.addEventListener("theme-changed", () => {
	});
}

function setup_time(data) {
	var trace1 = {
		x: [1, 2, 3, 4],
		y: [10, 15, 13, 17],
		type: 'scatter'
	};

	var data = [trace1];
	var layout = {
		plot_bgcolor: "rgb(0,0,0,0)",
		paper_bgcolor: "rgb(0,0,0,0)",

		margin: { "b": 30, "l": 40, "t": 0, "r": 0 },
		height: 180,
		xaxis: {
			title: {
				text: 'Month'
			},
			showgrid: false,

			zeroline: false
		},
		yaxis: {
			title: {

				text: 'Rents'

			},
			showline: false

		}
	}

	Plotly.newPlot('timeline_plot', data, layout);
}

function setup_map(data) {
	var map = L.map('map').setView([57.048820, 9.921747], 12);

	L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
		attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
	}).addTo(map);

}
