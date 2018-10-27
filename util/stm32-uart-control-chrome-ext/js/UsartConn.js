(function () {
	'use strict';

	class EventCore {
		static on(eventType, handler) {
			if (!EventCore.evtHandlers.has(eventType)) {
				EventCore.evtHandlers.set(eventType, [handler]);
			} else {
				EventCore.evtHandlers.get(eventType).push(handler);
			}
		}

		static off(eventType, handler) {
			if (EventCore.evtHandlers.has(eventType)) {
				let s = EventCore.evtHandlers.get(eventType);
				s.splice(s.indexOf(handler), 1);
			}
		}
	}
	EventCore.evtHandlers = new Map();
	L5.EventCore = EventCore;

	// -------------------------------------------------------------------------------
	var connInstance = null;
	class UsartConn extends EventCore {
		static get() {
			if (connInstance == null) {
				connInstance = new UsartConn();
			}
			return connInstance;
		}

		constructor() {
			super();
			this.connectionID = - 1;
			this.port = '';
			this._config = {
				persistent: false,
				name: "usart conn",
				bufferSize: 1024,
				bitrate: 115200,
				dataBits: 'eight',
				parityBit: 'no',
				stopBits: 'one',
				ctsFlowControl: false,
				receiveTimeout: 0,
				sendTimeout: 0
			}
		}

		messageHandler(info) {
			console.info(`readed:`, info);
		}

		async open(port, options) {
			L5.Serial.setMessageHandler( this.messageHandler.bind(this) );
			
			this.port = port;
			let opts = Object.assign({}, options, this._config);
			this.connectionID = await L5.Serial.open(this.port, opts);
			return true;
		}

		async read() {

		}

		async close() {
			return L5.Serial.close(this.connectionID);
		}

		async sendData(data) {
			let buf = new Uint8Array(data);
			let result = await L5.Serial.write(this.connectionID, buf.buffer);
			console.log(result);
			result = await L5.Serial.flush(this.connectionID);
			console.log(result);
		}
	}

	L5.UsartConn = UsartConn;
})();