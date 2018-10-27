'use strict';

function $(id) {
    return document.querySelector(id);
}

function log(msg) {
    let d = $('#output');
	d.innerHTML += `<li>${msg}</li>`;
	d.lastChild.scrollIntoViewIfNeeded();
}

// get Serial ports
async function setupComX() {
    let ports = await L5.Serial.getDevices();
    ports = ports.filter(v=>v.path.match(/tty/));

    let domPorts = $('#serialPort');
    if (ports.length===0) {
        domPorts.innerHTML = '<option>no serial available</option>';
        $('#connect').disabled = true;
        $('#testSerial').disabled = true;
        return;
    }
    domPorts.innerHTML = ports.reduce((t, v)=>t + `<option value="${v.path}">${v.path.split('/').pop()}</option>`, '');
}

function setupEvents() {

	// button for connect to usart
    $('#connect').addEventListener('click', async evt => {
		$('#connect').disabled = true;
        let port = $('#serialPort').value;
        let bitrate = $('#serialRateBit').value;		
        let conn = L5.UsartConn.get();
        if (await conn.open(port, {bitrate: bitrate})) {
            log('connect success :)');
			
			// TODO: get hardware infomation
			$('#serialPort').disabled = true;
			$('#serialRateBit').disabled = true;
        } else {
			$('#connect').disabled = false;
        	log('connect error :(')
		}
	});
	
	// led toggle
	$('#toggleLed').addEventListener('click', evt => {
		L5.UsartConn.get().sendData([1,0]);
		log('send(LED): Toggle');
	});

	// send string to lcd12864 display
	$('#lcdStr').addEventListener('click', async evt => {
		let val = $('#lcdStrInput').value;
		if (val.length == 0) {
			return;
		}

		if (val.length > 18) {
			val = val.substr(0, 18);
		}

		$('#lcdStr').disabled = true;
		let buf = new Uint8Array(2 + val.length);
		buf[0] = 0x02;
		buf[1] = val.length;
		buf.set(L5.string2bin(val), 2);
		await L5.UsartConn.get().sendData(buf);
		log('send (LCD Display): '+ val);
		$('#lcdStr').disabled = false;
	});
}

async function init() {
	L5.EventCore.on('log', msg=>$('#output').innerHTML += msg);

    setupComX();
    setupEvents();
}

window.addEventListener('DOMContentLoaded', init, false);
