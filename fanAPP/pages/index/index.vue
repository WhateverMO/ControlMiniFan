<template>
	<view>
		<view class="content">
			<image class="logo" src="/static/fan.png"></image>
		</view>
		<view class="text-area">
			<text class="title">{{title}}</text>
		</view>
		<view class="uni-padding-wrap uni-common-mt">
			<view class="uni-btn-v">
				<button type="primary"  @click="toLogin">
					用户登录/注册
				</button>
<!-- 				<button type="primary"  @click="toBle">
					蓝牙控制
				</button> -->
			</view>
		</view>
	</view>
</template>

<script>
	export default {
		data() {
			return {
				title: 'Hello,'
			}
		},
		onLoad() {
			console.log('首页已加载');
			// this.condition = uni.getStorageSync('disabled')
			// console.log(this.condition)
			// if(this.condition == 'ok'){
			// 	this.$set(this.disabled, 1,false);
			// }
			

		},
		methods: {
			// toBle() {
			// 	uni.navigateTo({
			// 		url: '../API/bluetooth',
			// 		success() {
			// 			console.log('跳转成功');
			// 		},
			// 		fail(res) {
			// 			console.log('跳转失败');
			// 		}
			// 	})
			// },
			toLogin(){
				uni.navigateTo({
					url: '../Login/login',
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
</style>
