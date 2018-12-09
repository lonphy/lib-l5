const usbDesc = {
	vendorId: 0x2323,
	productId: 0x0001
};

let desc = null;
let usb = null;
let device = null;
const idxOfInterface = 0;

class USBUtil {
	static async open(desc) {
		return new Promise((resolve, reject) => {
			chrome.usb.openDevice(desc, resolve);
		});
	}

	static async getConfig() {
		return new Promise((resolve, reject) => {
			chrome.usb.getConfiguration(usb, resolve);
		});
	}
	static async getConfigs(dev) {
		return new Promise((resolve, reject) => {
			chrome.usb.getConfigurations(dev, resolve);
		});
	}

	static async listInterface() {
		return new Promise((resolve, reject) => {
			chrome.usb.listInterfaces(usb, resolve);
		});
	}

	static async claimInterface(idx) {
		return new Promise( (resolve, reject)=>{
			chrome.usb.claimInterface(usb, idx, resolve);
		});
	}
	static async requestAccess(idx) {
		return new Promise( (resolve, reject)=>{
			chrome.usb.requestAccess(device, idx, resolve);
		});
	}

	static async releaseInterface(idx) {
		return new Promise( (resolve, reject)=>{
			chrome.usb.releaseInterface(usb, idx, resolve);
		});
	}

	static async resetDevice() {
		return new Promise( resolve=>chrome.usb.resetDevice(usb, resolve));
	}
}

function checkRuntimeError() {
	if (chrome.runtime.lastError) {
	  console.error(chrome.runtime.lastError);
	}
}

function usbInit() {

	// 打开一个USB选择对话框
	chrome.usb.getDevices(usbDesc, async ([dev]) => {

		if (!dev) {
			alert('usb device not detected~');
			return;
		}
		device = dev;

		usb = await USBUtil.open(device);

		// let ok = await USBUtil.resetDevice(usb);
		// console.log(ok);

		let ok = await USBUtil.requestAccess(idxOfInterface);
		console.log("请求访问USB:", ok);

		let conf = await USBUtil.getConfig();
		console.log("配置信息:", conf);

		// let iterfaces = await USBUtil.listInterface();
		// console.log("接口信息:", ...iterfaces);
		
		// // 获取接口
		// let result = await USBUtil.claimInterface(idxOfInterface);
		// console.log("认领接口结果:", result);

		// result = await USBUtil.releaseInterface(idxOfInterface);
		// console.log("释放接口结果:", result);

		// 发送一个控制命令
		// chrome.usb.controlTransfer(usb, {
		// 	direction: 'out',  /* 传输方向, 入、出 */
		// 	recipient: 'interface', /* 接收对象, 设备、描述、接口、端点 */
		// 	requestType: 'vendor', /* 请求类型, 标准、提供商、类、其他 */
		// 	request: 1,
		// 	value: 1,
		// 	index: 1,
		// 	data: (new Uint8Array([0,1,2,3,4,5,6])).buffer, /* 需要传输的控制数据 */
		// 	length: 7
		// }, console.log);
		var transfer = {
			direction: 'out',
			endpoint: 1,
			data: (new Uint8Array([0,1,2,3,4,5,6])).buffer, /* 需要传输的控制数据 */
			length: 6
		  };

		//chrome.usb.interruptTransfer(usb, transfer, console.info);

		//chrome.usb.setInterfaceAlternateSetting(usb, idxOfInterface, 0, console.log);
	});
}

usbInit();