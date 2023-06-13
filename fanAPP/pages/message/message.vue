<template>
	<view class="content">
		<image class="img" src="../../static/astronauts.png"> </image>
		<button @click="logout" type="warn" class="exit">退出登录</button>
	</view>
</template>

<script>
	export default {
		data() {
			
		},
		methods: {
			logout(url){
				uni.navigateTo({
				    url:'../index/index'
				})
			}	
		}
	}
</script>

<style>
	.content{
		padding-top: 50px;
	}
	.img{
		width: 250upx;
		height: 250upx;
		margin: 0 33% 2%;
	}
    .exit{
		width: 250upx;
		height: 40px;
		line-height: 40px;
	}
</style>
