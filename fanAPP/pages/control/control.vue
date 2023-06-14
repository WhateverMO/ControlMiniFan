<template>
	<view>
		<page-head :title="title"></page-head>
		<view class="uni-padding-wrap uni-common-mt">
			<view class="uni-btn-v">
				<button type="primary" :disabled="disabled[11]" @click="toBind">
					绑定设备
				</button>
				<button type="primary" @click="toWIFI">
					WIFI连接
				</button>
				<button type="primary" :disabled="disabled[11]" @click="openGear">
					打开风扇
				</button>
				<button type="primary" :disabled="disabled[11]" @click="oneGear">
					一档
				</button>
				<button type="primary" :disabled="disabled[11]" @click="twoGear">
					二档
				</button>
				<button type="primary" :disabled="disabled[11]" @click="threeGear">
					三档
				</button>
				<button type="primary" :disabled="disabled[11]" @click="loopGear">
					挡位循环
				</button>
				<view class="uni-title uni-common-pl">时间选择</view>
				<view class="uni-list">
					<view class="uni-list-cell">
						<view class="uni-list-cell-left">
									当前选择
						</view>
						<view class="uni-list-cell-db">
							<picker mode="time" :value="time" start="00:00" end="24:00" @change="bindTimeChange">
								<view class="uni-input">{{time}}</view>
							</picker>
						</view>
					</view>
				</view>
				<button type="primary" :disabled="disabled[11]" @click="lazyStart">
					定时启动
				</button>
				<button type="primary" :disabled="disabled[11]" @click="offGear">
					关闭风扇
				</button>
			</view>
		</view>
	</view>
</template>
<script>
	import config from '@/config.js';
	export default {
		data() {
			return {
				title: '风扇控制',
				disabled: [false, false, false, true, true, true, true, true, true, true, true, false],
				newDeviceLoad: false,
				searchLoad: false,
				maskShow: false,
				equipment: '',
				adapterState: {
					discovering: false,
					available: false
				},
				connected: false,
				showMaskType: 'device',
				servicesData: '',
				characteristicsData: '',
				valueChangeData: {},
				isStop: true,
				list: [],
				time:"12:00"
			};
		},
		onLoad(option) {
			this.equipment = uni.getStorageSync('deviceId');
			this.servicesData = uni.getStorageSync('serviceId');
			this.characteristicsData = uni.getStorageSync('characteristicId');
		},
		methods: {
			bindTimeChange: function(e) {
			           this.time = e.detail.value
			       },
			str2ab(str) {
			    var buf = new ArrayBuffer(str.length*2); // 每个字符占用2个字节
			    var bufView = new Uint8Array(buf);
			    for (var i=0, strLen=str.length; i<strLen; i++) {
			         bufView[i] = str.charCodeAt(i);
			    }
			    return bufView;
			},
			/**
			 * 打开风扇
			 */
			openGear() {
				//打开风扇10%
				var tmp = {
						command:'3',
						method:true
					};
				this.writeBLECharacteristicValue(tmp);
			},
			/**
			 * 一档
			 */
			oneGear() {
				//一档传入数据tmp=1
				var tmp = {
						command:'4',
						speed:'1'
					};
				this.writeBLECharacteristicValue(tmp);
			},
			/**
			 * 二档
			 */
			twoGear() {
				//二档传入数据tmp=2
				var tmp = {
						command:'4',
						speed:'2'
					};
				this.writeBLECharacteristicValue(tmp);
			},
			/**
			 * 三档
			 */
			threeGear() {
				//三档传入数据tmp=3
				var tmp = {
						command:'4',
						speed:'3'
					};
				this.writeBLECharacteristicValue(tmp);
			},
			/**
			 * 挡位循环
			 */
			loopGear() {
				var tmp = {
						command:'5',
					};
				this.writeBLECharacteristicValue(tmp);
			},
			/**
			 * 定时启动
			 */
			lazyStart() {
				let aData = new Date();
				let stHour=aData.getHours();
				let stMinute=aData.getMinutes();
				let endHour=this.time.slice(0,2);
				let endMinute=this.time.slice(3,5);
				var value=(parseInt(endHour)*60+parseInt(endMinute)-parseInt(stHour)*60-parseInt(stMinute))*60;
				if (value<0)
				{
					value=0;
				}
				var tmp = {
						command:'6',
						seconds:value
					};
				this.writeBLECharacteristicValue(tmp);
			},
			/**
			 * off档
			 */
			offGear() {
				//off档传入数据tmp
				var tmp = {
						command:'3',
						method:false
					};
				this.writeBLECharacteristicValue(tmp);
			},
			/**
			 * 写入数据
			 */
			writeBLECharacteristicValue(tmp) {
				let deviceId = this.equipment[0].deviceId;
				let serviceId = this.servicesData[0].uuid;
				let characteristicId = this.characteristicsData[0].uuid;
				console.log(deviceId)
				console.log(serviceId)
				console.log(characteristicId)
				console.log(tmp)
				var jsonstr1=JSON.stringify(tmp);
				var buffer= this.str2ab(jsonstr1);
				// 转换成ArrayBuffer类型
				uni.writeBLECharacteristicValue({
					// 这里的 deviceId 需要在 getBluetoothDevices 或 onBluetoothDeviceFound 接口中获取
					deviceId,
					// 这里的 serviceId 需要在 getBLEDeviceServices 接口中获取
					serviceId,
					// 这里的 characteristicId 需要在 getBLEDeviceCharacteristics 接口中获取
					characteristicId,
					// 这里的value是ArrayBuffer类型
					value: buffer,
					success(res) {
						// console.log(res)
						// console.log('writeBLECharacteristicValue success', res.errMsg)
						console.log("写入成功");
					},
					fail(res) {
						console.log("写入失败")
						// 重新写入
						setTimeout(() => {
							thit.writeBLECharacteristicValue(e)
						}, 100)
					}
				})
			},
			toWIFI() {
				uni.navigateTo({
					url: '../WIFI/wifi',
					success() {
						console.log('跳转成功');
					},
					fail(res) {
						console.log('跳转失败');
					}
				})
			},
			toBind() {
				var that = this;
				var DeviceList = uni.getStorageInfoSync('DeviceList');
				console.log(DeviceList);
				console.log(this.equipment[0]);
				var k = 0;
				for (var q = 0; q++; q < DeviceList.length) {
					if (DeviceList[k] == this.equipment[0].deviceId) {
						k++;
					}
				}
				if (k == 0) {
					var DeviceUUID = this.equipment[0].deviceId;
					console.log(this.equipment[0].deviceId);
					var DeviceName = this.equipment[0].name;
					console.log(this.equipment[0].name);
					var UserName = uni.getStorageSync('UserName');
					console.log(UserName);
					uni.request({
						url: config.hostUrl + '/UserDevice',
						method: 'POST',
						data: {
							UserName: UserName,
							DeviceUUID: DeviceUUID,
							DeviceName: DeviceName,
						},
						success: res => {
							if (res.data.status == true) {
								uni.showToast({
									title: res.data.message,
									icon:"success",
									duration: 1000,
									success() {
										DeviceList.push(that.equipment[0].deviceId);
										uni.setStorageSync('DeviceList');
									}
								});
							} else {
								uni.showToast({
									title: res.data.message,
									icon:"error",
									duration: 1000
								});
							}
						},
						fail() {
							console.log('传数据失败')
						}
					});
				} else {
					uni.showToast({
						title: "设备已经注册",
						duration: 1000
					});
				}
			},
		}
	};
</script>

<style>
</style>
