(function() {
	'use strict';
	let L5 = Object.create(null);
	window.L5 = L5;

	L5.delay = async function(ms) {
		return new Promise(resolve => setTimeout(resolve, ms));
	};

	L5.string2bin = function(str) {
		return str.split('').map(v=>v.charCodeAt(0))
	}

	L5.Serial = {
		defaultConfig:{
			persistent:    false,
			name:          "name",
			bufferSize:    1024, // reciveve buffer size
			bitrate:       9600,
			dataBits:      chrome.serial.DataBits.EIGHT,
			parityBit:     chrome.serial.ParityBit.NO,
			stopBits:      chrome.serial.StopBits.ONE,
			ctsFlowControl:false, // enable RTS/CTS
			receiveTimeout:20000, // ms
			sendTimeout:   20000 // ms
		},

		async open(path, config) {
			return new Promise(function(resolve, reject) {
				chrome.serial.connect(path, config, (info) => {
					switch (info.connectionId) {
						case - 1:
							reject();
							break;
						default:
							console.info(`connect to ${path} success:`, info);
							resolve(info.connectionId);
					}
				});
			});
		},

		async close (id) {
			return new Promise(function(resolve, reject) {
				chrome.serial.disconnect(id, result => (result ? resolve() : reject()));
			});
		},

		async write (id, data) {
			return new Promise(function(resolve, reject) {
				chrome.serial.send(id, data, (result) => result ? resolve(result) : reject(result));
			});
		},

		async flush (id) {
			return new Promise(resolve => chrome.serial.flush(id, resolve));
		},

		async update (id, config) {
			return new Promise(resolve => chrome.serial.update(id, config, resolve));
		},

		async pause (id, on) {
			return new Promise(resolve => chrome.serial.setPaused(id, ! ! on, resolve));
		},

		async getDevices() {
			return new Promise(resolve => chrome.serial.getDevices(resolve));
		},

		setMessageHandler(handler) {
			chrome.serial.onReceive.addListener(handler);
			chrome.serial.onReceiveError.addListener(handler);
		},

		async control(id, opt) {
			return new Promise(resolve=> chrome.serial.setControlSignals(id, opt, resolve));
		},
		async getSignals(id) {
			return new Promise(resolve => chrome.serial.getControlSignals(id, resolve));
		}

	};

})();



