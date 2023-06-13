<template>
	<view>
		<view class="content">
			<image class="logo" src="/static/ble.png"></image>
		</view>
		<view class="text-area">
			<text class="title">{{title}}</text>
		</view>
		<div class="list">
			<text class="title">{{title2}}</text>
			<list>
				<view>
					<!-- 注意事项: 不能使用 index 作为 key 的唯一标识 -->
					<view v-for="(item,index) in devicelist" :item="item" :index="index" :key="item.id"
						@click="wificonnected()" class="connectedlist">
						<text class="connected">{{item}}</text>
					</view>
				</view>
			</list>
		</div>
		<div class="list">
			<text class="title">{{title1}}</text>
			<list>
				<!-- 注意事项: 不能使用 index 作为 key 的唯一标识 -->
				<view v-for="(item,index) in servicelist" :item="item" :index="index" :key="item.id"
					@click="onDeviceClick(item)" class="connectedlist">
					<text class="connected">{{item.name}}</text>
				</view>
			</list>
		</div>
		<view class="uni-padding-wrap uni-common-mt">
			<view class="uni-btn-v">

				<button type="primary" @click="toBle">
					添加蓝牙
				</button>
				<button type="primary" @click="toWIFI">
					添加WIFI
				</button>
				<!-- <button type="primary" @click="controlFan">
					WIFI控制
				</button> -->
			</view>
		</view>
	</view>
</template>

<script>
	import config from '@/config.js';
	export default {
		data() {
			return {
				title: 'Hello,',
				title1: '已配对蓝牙列表',
				title2: 'WIFI在线设备',
				servicelist: [],
				devicelist: [],
				name: [],
			}
		},
		onLoad() {
			var that = this;
			this.condition == null;
			var tmpDeviceId = uni.getStorageSync('deviceId');
			console.log(tmpDeviceId);
			var deviceId = tmpDeviceId[0].deviceId;
			uni.setStorageSync("current", this.condition);
			//获取wifi在线设备
			setInterval(function() {
				uni.request({
					url: config.hostUrl + '/UserDevice/IsDeviceOnline',
					method: "POST",
					data: {
						deviceId: deviceId
					},
					success: res => {
						if (res.data.status == true) {
							that.devicelist = res.data.deviceList;
							that.devicelist.forEach((item, index) =>
							console.log(item, index));
							console.log(res.data.message);
							//that.reNew();
						} else {
							that.devicelist = null;
							console.log(res.data);
							console.log(res.data.message);
						}

					}
				});
			}, 5000);
			this.servicelist = uni.getStorageSync('device');
			this.servicelist.forEach((item, index) =>
				console.log(item.name, index));

			//初始化蓝牙模块
			uni.openBluetoothAdapter({
				success: (res) => { //初始化成功
					uni.getBluetoothAdapterState({ //蓝牙匹配状态
						success: (res1) => {
							console.log(res1, '"本机蓝牙已打开"')

						},
						fail() {
							uni.showToast({
								icon: 'none',
								title: '查看本机蓝牙是否打开'
							})
						}
					})
				},
				fail: (errMsg) => {
					console.log(errMsg);
				}
			});
		},
		methods: {
			reNew(){
				location.reload();
			},
			onDeviceClick(item) {
				console.log(item.name);
				console.log('到位了');
				var tmpDeviceId = uni.getStorageSync('deviceId');
				var deviceId = tmpDeviceId[0].deviceId;
				console.log(deviceId);
				uni.createBLEConnection({
					// 这里的 deviceId 需要已经通过 createBLEConnection 与对应设备建立链接
					deviceId,
					success(res) {
						console.log(res)
						console.log("蓝牙连接成功")
						uni.showToast({
							icon: "success",
							duration: 1000,
						})
						uni.navigateTo({
							url: '../control/control',
							success() {
								console.log('跳转成功');
							},
							fail(res) {
								console.log('跳转失败');
							}
						})
					},
					fail(res) {
						console.log("蓝牙连接失败", res);
						if (res.errCode == -1) {
							uni.navigateTo({
								url: '../control/control',
								success() {
									console.log('跳转成功');
								},
								fail(res) {
									console.log('跳转失败');
								}
							})
						}
					}
				})


			},
			toBle() {
				uni.navigateTo({
					url: '../API/bluetooth',
					success() {
						console.log('跳转成功');
					},
					fail(res) {
						console.log('跳转失败');
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
			// controlFan() {
			// 	uni.navigateTo({
			// 		url: '../control/wificontrol',
			// 		success() {
			// 			console.log('跳转成功');
			// 		},
			// 		fail(res) {
			// 			console.log('跳转失败');
			// 		}
			// 	})
			// },
			wificonnected() {
				uni.navigateTo({
					url: '../control/wificontrol',
					success() {
						console.log('跳转成功');
					},
					fail(res) {
						console.log('跳转失败');
					}
				})
			}
		}
	}
</script>

<style>
	.content {
		display: flex;
		flex-direction: column;
		align-items: center;
		justify-content: center;
	}

	.logo {
		height: 200rpx;
		width: 200rpx;
		margin-top: 200rpx;
		margin-left: auto;
		margin-right: auto;
		margin-bottom: 50rpx;
	}

	.text-area {
		display: flex;
		justify-content: center;
	}

	.title {
		font-size: 36rpx;
		color: #8f8f94;
	}

	.connected {
		text-align: center;
		height: 40upx;
		line-height: 40upx;
		font-size: medium;
	}

	.connectedlist {
		float: left;
		border-bottom: #999999 solid 1px;
		height: 40upx;
		line-height: 40upx;
		padding: 3% 0 3% 2%;
		width: 70%;
		margin: 1% 15%;
		text-align: center;
	}

	.list {
		float: left;
		width: 100%;
		padding: 3% 0 3% 2%;
	}
</style>
