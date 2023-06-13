<template>
	<view class="uni-content">
		<text class="title title-box">WIFI连接</text>
		<uni-forms>
			<uni-forms-item name="username">
				<uni-easyinput :focus="focusUsername" @blur="focusUsername = false" class="input-box"
					:inputBorder="false" v-model="username" placeholder="请输入WIFI名称">
				</uni-easyinput>
			</uni-forms-item>
			<uni-forms-item name="password">
				<uni-easyinput :focus="focusPassword" @blur="focusPassword = false" class="input-box" clearable
					type="password" :inputBorder="false" v-model="password" placeholder="请输入WIFI密码">
				</uni-easyinput>
			</uni-forms-item>
		</uni-forms>
		<button class="uni-btn" type="primary" @click="pwdWIFI">登录</button>
		<button class="uni-btn" type="primary" @click="navigateBack">返回</button>
	</view>
</template>

<script>
	import config from '@/config.js';
	export default {
		data() {
			return {

				"password": "",
				"username": "",
				"focusUsername": false,
				"focusPassword": false,

			}
		},
		methods: {
			// ArrayBuffer转为字符串，参数为ArrayBuffer对象
			ab2str(buf) {
			   return String.fromCharCode.apply(null, new Uint8Array(buf));
			},
			// 字符串转为ArrayBuffer对象，参数为字符串
			str2ab(str) {
			    var buf = new ArrayBuffer(str.length*2); // 每个字符占用2个字节
			    var bufView = new Uint8Array(buf);
			    for (var i=0, strLen=str.length; i<strLen; i++) {
			         bufView[i] = str.charCodeAt(i);
			    }
			    return bufView;
			},
			controlFan(){
				uni.navigateTo({
					url:'../control/wificontrol',
					success(){
						console.log('跳转成功');
					},
					fail(res){
						console.log('跳转失败');
					}
				})
			},
			pwdWIFI() {
				if (!this.password.length) {
					this.focusPassword = true
					return uni.showToast({
						title: '请输入密码',
						icon: 'none'
					});
				}
				if (!this.username.length) {
					this.focusUsername = true
					return uni.showToast({
						title: '请输入用户名',
						icon: 'none'
					});
				}
				let data = {
					"password": this.password,
				}

				if (/^1\d{10}$/.test(this.username)) {
					data.mobile = this.username
				} else {
					data.username = this.username
				}
				var tmp = {
					command:'2',
					params:{
						wifiname:this.username,
						password:this.password
					}
				};
				var jsonstr1=JSON.stringify(tmp);
				console.log(jsonstr1);
				var buffer= this.str2ab(jsonstr1);
				console.log(buffer);
				var tmpDeviceId = uni.getStorageSync('deviceId') 
				var deviceId = tmpDeviceId[0].deviceId;
				console.log(deviceId);
				var tmpServiceId = uni.getStorageSync('serviceId');
				var serviceId = tmpServiceId[0].uuid;
				console.log(serviceId);
				var tmpCharacteristicId = uni.getStorageSync('characteristicId');
				var characteristicId = tmpCharacteristicId[0].uuid;
				console.log(characteristicId);
				var thit = this;
				uni.writeBLECharacteristicValue({
					characteristicId,
					deviceId,
					serviceId,
					value:buffer,
					success(res) {
						// console.log(res)
						// console.log('writeBLECharacteristicValue success', res.errMsg)
						console.log("写入成功");
						uni.showToast({
							icon:"success",
							duration: 1000,
						});
						thit.controlFan();
					},
					fail(res) {
						console.log("写入失败")
						// 重新写入
						setTimeout(() => {
							thit.writeBLECharacteristicValue(e)
						}, 100)
					}
				});
				
			},
			navigateBack() {
				uni.navigateBack()
			}
		},
	}
</script>

<style lang="scss" scoped>
	@import "@/uni_modules/uni-id-pages/common/login-page.scss";

	.forget {
		font-size: 12px;
		color: #8a8f8b;
	}

	.link-box {
		/* #ifndef APP-NVUE */
		display: flex;
		/* #endif */
		flex-direction: row;
		justify-content: space-between;
		margin-top: 20px;
	}

	.link {
		font-size: 12px;
	}
</style>
