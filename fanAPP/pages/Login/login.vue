<template>
	<view class="uni-content">
	<text class="title title-box">账号密码登录</text>
	<uni-forms>
		<uni-forms-item name="username">
			<uni-easyinput :focus="focusUsername" @blur="focusUsername = false" class="input-box" :inputBorder="false" v-model="username" placeholder="请输入用户名">
			</uni-easyinput>
		</uni-forms-item>
		<uni-forms-item name="password">
			<uni-easyinput :focus="focusPassword" @blur="focusPassword = false" class="input-box" clearable type="password" :inputBorder="false" v-model="password" placeholder="请输入密码">
			</uni-easyinput>
		</uni-forms-item>
	</uni-forms>
	<view class="link-box">
		<text class="link" @click="toRegister">注册账号</text>
	</view>
	<button class="uni-btn" type="primary" @click="pwdLogin">登录</button>
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
				"focusUsername":false,
				"focusPassword":false,
				
			}
		},
		methods:{
			/**
			 * 密码登录
			 */
			pwdLogin() {
				if(!this.password.length){
					this.focusPassword = true
					return uni.showToast({
						title: '请输入密码',
						icon: 'none'
					});
				}
				if(!this.username.length){
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
				}else{
					data.username = this.username
				}
				
				console.log('获取登录数据成功',data);
				console.log(data.username+'  '+data.password)
				uni.request({
					url:config.hostUrl + '/User/Login',
					data:{
						userName:this.username,
						Password:this.password
					},
					method:"POST",
					success:res => {
						console.log(res.data.deviceList);
						console.log(res.data.status);
						uni.setStorageSync('DeviceList',res.data.deviceList);
						console.log(res.data.message);
						if(res.data.status == true){
							uni.showToast({
								title:res.data.message,
								duration:1000,
								success() {
									uni.navigateTo({
										url:'../index/index1',
										success() {
											console.log('跳转成功');
										},
										fail(res) {
											console.log('跳转失败');
										}
									})					
								}
							});
						};
						if( res.data.status== false){
							uni.showToast({
								icon: 'error',
								title: res.data.message,
							});
						};	
					},
					fail: res => {
						console.log(res.errMsg+'fail');
					},
					complete: () => {
									
					},
					
				})
					uni.setStorageSync("UserName",this.username);
					console.log("用户名",this.username);	
			},
			/* 前往注册 */
			toRegister(e) {
				console.log(e);
				uni.navigateTo({
					url: '../Register/Register'
				})
			},
			navigateBack() {
				uni.navigateBack()
			}
		},
	}
</script>

<style lang="scss" scoped>
	@import "@/uni_modules/uni-id-pages/common/login-page.scss";

	.forget{
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