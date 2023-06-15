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
				<view>
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
							<view class="uni-list-cell-left">
										时
							</view>
							<button type="primary" :disabled="disabled[11]" @click="lazyStart_time">
								定时启动(按时)
							</button>
						</view>
					</view>
					<view class="uni-title uni-common-pl">按秒定时</view>
					<view class="uni-list">
						<view class="uni-list-cell">
							<view class="uni-list-cell-left">
										当前选择
							</view>
							<view class="uni-list-cell-db">
								<picker mode="selector" :range="time_range" @change="bindSecondChange">
									<view class="uni-input">{{lazy_seconds}}</view>
								</picker>
							</view>
							<view class="uni-list-cell-left">
										秒
							</view>
							<button type="primary" :disabled="disabled[11]" @click="lazyStart_sec">
								定时启动(按秒)
							</button>
						</view>
					</view>
					<view class="uni-title uni-common-pl">自定义持续时间</view>
					<view class="uni-list">
						<view class="uni-list-cell">
							<view class="uni-list-cell-left">
										当前选择
							</view>
							<view class="uni-list-cell-db">
								<picker mode="selector" :range="time_range" @change="bindSusTimeChange">
									<view class="uni-input">{{susTime}}</view>
								</picker>
							</view>
							<view class="uni-list-cell-left">
										分钟
							</view>
							<button type="primary" :disabled="disabled[11]" @click="setSusTime">
								设置持续时间
							</button>
						</view>
					</view>
				</view>
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
			var time_range=[];
			for(let i=0;i<=60;i++)
			{
				time_range.push(i);
			}
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
				susTime: 0,
				time:"12:00",
				time_range:time_range,
				lazy_seconds:0
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
			 * 挡位循环
			 */
			loopGear() {
				var tmp = {
						command:'5',
					};
				this.writeToServer(tmp);
			},
			/**
			 * 定时启动
			 */
			bindTimeChange: function(e) {
			           this.time = e.detail.value
			       },
			bindSecondChange: function(e) {
			           this.lazy_seconds = e.detail.value
			       },
			lazyStart_time() {
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
			lazyStart_sec() {
				var tmp = {
						command:'6',
						seconds:parseInt(this.lazy_seconds)
					};
				this.writeBLECharacteristicValue(tmp);
			},
			/**
			 * 自定义持续时间
			 */
			/**
			 * 自定义持续时间
			 */
			bindSusTimeChange: function(e)
			{
				this.susTime=e.detail.value;
			},
			setSusTime()
			{
				var value=this.susTime*60;
				var tmp = {
						command:'7',
						seconds:value
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
