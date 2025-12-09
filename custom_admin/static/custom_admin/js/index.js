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
		let [data, layout, config] = load_pie(rented, not_rented)
		Plotly.react('pie_chart', data, layout, config)
	});
}

function make_time(data) {

	let style = window.getComputedStyle(document.body)
	let days = [];
	for (let i = 0; i < data.length; i++) {
		days.push(i);
	}
	var trace1 = {
		x: days,
		y: data,
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
				text: 'Days'
			},
			showgrid: true,
			gridcolor: style.getPropertyValue("--background"),
			zeroline: false
		},
		yaxis: {
			title: {

				text: 'Rents'

			},
			gridcolor: style.getPropertyValue("--background"),
			showline: false

		},
		font: {
			color: style.getPropertyValue("--background"),
		}
	}

	return [data, layout];

}


function setup_time(data) {
	let [data2, layout] = make_time(data)
	Plotly.newPlot('timeline_plot', data2, layout);

	document.addEventListener("theme-changed", () => {
		let [data2, layout] = make_time(data)
		Plotly.react('timeline_plot', data2, layout)
	});
}


function setup_map(data) {
	var map = L.map('map').setView([57.048820, 9.921747], 12);

	L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
		attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
	}).addTo(map);

	min_lat = 100
	min_lon = 100
	max_lat = -100
	max_lon = -100

	for (let i = 0; i < data.length; i++) {
		let data_point = data[i];
		min_lat = data_point.lat < min_lat ? data_point.lat : min_lat
		min_lon = data_point.lon < min_lon ? data_point.lon : min_lon

		max_lat = data_point.lat > max_lat ? data_point.lat : max_lat
		max_lon = data_point.lon > max_lon ? data_point.lon : max_lon


		let marker = L.marker([data_point.lat, data_point.lon]);
		marker.bindPopup(data_point.name).openPopup();
		marker.addTo(map);
	}
	console.log(min_lat);
	console.log(min_lon);
	console.log(max_lon);
	console.log(max_lat);

	map.fitBounds([
		[min_lat, min_lon], [max_lat, max_lon]
	]);

}
