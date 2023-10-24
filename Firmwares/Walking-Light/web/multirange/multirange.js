(function() {
"use strict";

var supportsMultiple = self.HTMLInputElement && "valueLow" in HTMLInputElement.prototype;
var descriptor = Object.getOwnPropertyDescriptor(HTMLInputElement.prototype, "value");

self.rangeMul = function(input) {
	if (supportsMultiple || input.classList.contains("rangeMul")) {
		return;
	}

	var value = input.getAttribute("value");
	var values = value === null ? ["-","-"] : value.split(",");
	var min = 0, max = 15;
	var r2 = input.cloneNode();

	input.classList.add("rangeMul", "min");
	input.value = values[0];
	r2.classList.add("rangeMul", "max");
	r2.value = values[1];

	input.parentNode.insertBefore(r2, input.nextSibling);

	Object.defineProperty(input, "originalValue", descriptor.get ? descriptor : {
		get: function() { return this.value; },
		set: function(v) { this.value = v; }
	});

	Object.defineProperties(input, {
		valueLow: {
			get: function() { return Math.min(this.originalValue, r2.value); },
			set: function(v) { this.originalValue = v; },
			enumerable: true
		},
		valueHigh: {
			get: function() { return Math.max(this.originalValue, r2.value); },
			set: function(v) { r2.value = v; },
			enumerable: true
		}
	});

	if (descriptor.get) {
		Object.defineProperty(input, "value", {
			get: function() { return this.valueLow + "," + this.valueHigh; },
			set: function(v) {
				var values = v.split(",");
				this.valueLow = values[0];
				this.valueHigh = values[1];
				update();
			},
			enumerable: true
		});
	}

	function update() {
		r2.style.setProperty("--low", 100 * ((input.valueLow - min) / (max - min)) + 1 + "%");
		r2.style.setProperty("--high", 100 * ((input.valueHigh - min) / (max - min)) - 1 + "%");
        console.log([input.valueLow,input.valueHigh]);
    }
	input.addEventListener("input", update);
	r2.addEventListener("input", update);
	update();
};

rangeMul.init = function() {
	[].slice.call(document.querySelectorAll("input[type=range][multiple]:not(.rangeMul)")).forEach(rangeMul);
};

if (document.readyState == "loading")
	document.addEventListener("DOMContentLoaded", rangeMul.init);
else
	rangeMul.init();

})();
