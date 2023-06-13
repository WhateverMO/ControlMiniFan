<template>
	<view>
		<page-head :title="title"></page-head>
		<view class="uni-padding-wrap uni-common-mt">
			<view class="uni-btn-v">
				<button type="primary" :disabled="disabled[11]" @click="toBind">
					绑定设备
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
				list: []
			};
		},
		methods: {
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
				this.writeToServer(tmp);
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
				this.writeToServer(tmp);
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
				this.writeToServer(tmp);
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
				this.writeToServer(tmp);
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
				this.writeToServer(tmp);
			},
			/**
			 * 写入数据
			 */
			writeToServer(tmp) {
				var tmpDeviceId = uni.getStorageSync('deviceId');
				var deviceId = tmpDeviceId[0].deviceId;
				console.log(tmp)
				console.log(deviceId);
				var jsonstr1=JSON.stringify(tmp);
				var buffer= this.str2ab(jsonstr1);
				// 转换成ArrayBuffer类型
				uni.request({
					url:config.hostUrl + '/UserDevice/DeviceCommand',
					data:{
						deviceId,
						// deviceId:item.deviceId,
						message:jsonstr1,
					},
					method:"POST",
					success: res => {
						console.log(res.data);
						if( res.data== false){
							uni.showToast({
								icon: 'none',
								title: '无法打开风扇',
							});
							return;
						}
					},
					fail:res =>{
						console.log('fail');
					},
				})
			},
			toBind() {
				var DeviceList = uni.getStorageInfoSync('DeviceList');
				console.log(DeviceList);
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
										DeviceList.push(this.equipment[0].deviceId);
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
