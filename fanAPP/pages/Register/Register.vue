<!-- 账号注册页 -->
<template>
	<view class="uni-content">
		<uni-forms ref="form" :value="formData" :rules="rules" validate-trigger="submit" err-show-type="toast">
			<uni-forms-item name="username" required>
				<uni-easyinput :inputBorder="false" :focus="focusUsername"
					@blur="bindTextAreaBlur" class="input-box" placeholder="请输入用户名"
					v-model="formData.username" trim="both" />
			</uni-forms-item>
			<!-- 			<uni-forms-item name="nickname">
				<uni-easyinput :inputBorder="false" :focus="focusNickname" @blur="focusNickname = false" class="input-box" placeholder="请输入用户昵称" v-model="formData.nickname"
					trim="both" />
			</uni-forms-item> -->
			<uni-forms-item name="password" v-model="formData.password" required>
				<uni-easyinput :inputBorder="false" :focus="focusPassword" @blur="focusPassword = false"
					class="input-box" maxlength="20" placeholder="请输入6-20位密码" type="password"
					v-model="formData.password" trim="both" />
			</uni-forms-item>
			<uni-forms-item name="password2" v-model="formData.password2" required>
				<uni-easyinput :inputBorder="false" :focus="focusPassword2" @blur="focusPassword2 =false"
					class="input-box" placeholder="再次输入密码" maxlength="20" type="password" v-model="formData.password2"
					trim="both" />
			</uni-forms-item>
			<button class="uni-btn" type="primary" @click="submit">注册</button>
			<button class="uni-btn" type="primary" @click="navigateBack">返回</button>
		</uni-forms>
	</view>
</template>

<script>
	import rules from '@/uni_modules/uni-id-pages/pages/register/validator.js';
	import mixin from '@/uni_modules/uni-id-pages/common/login-page.mixin.js';
	import config from '@/config.js';
	//const uniIdCo = uniCloud.importObject("uni-id-co")
	export default {
		mixins: [mixin],
		data() {
			return {
				formData: {
					username: "",
					nickname: "",
					password: "",
					password2: "",
				},
				rules,
				focusUsername: false,
				focusNickname: false,
				focusPassword: false,
				focusPassword2: false
			}
		},
		onReady() {
			this.$refs.form.setRules(this.rules)
		},
		methods: {
			bindTextAreaBlur() {
				uni.request({
					url: config.hostUrl + '/User/VerifyUser',
					method: 'POST',
					data: {
						UserName: this.formData.username,
					},
					success: res => {
						console.log(res.data)
						//在数据缓存中存入当前点击的设备
						uni.setStorageSync('data', res.data.status);
						if (res.data.status == false) {
							uni.showToast({
								icon: 'none',
								title: '用户名重复',
							});
						} else {
							console.log("用户名通过验证");
						}
					}
				})
			},
			/**
			 * 触发表单提交
			 */
			submit() {
				this.$refs.form.validate().then((res) => {
					console.log('获取注册信息成功', res);
					var status = uni.getStorageSync('data');
					console.log(status);
					if (status == true) {
						uni.request({
							url: config.hostUrl + '/User',
							method: 'POST',
							data: {
								UserName: this.formData.username,
								Password: this.formData.password,
							},
							success: res => {
								uni.showToast({
									title: res.data.message,
									duration: 1000,
									success() {
										setTimeout(function() {
											uni.navigateBack({
												url: '/pages/login/login',
											});
										}, 1000);
									}
								})
							}
						})
					};
					if (status == false) {
						uni.showToast({
							title: "用户名重复2",
							duration: 1000,
						})
					};
				})
			},
			navigateBack() {
				uni.navigateBack()
			}
		}
	}
</script>

<style lang="scss">
	@import "@/uni_modules/uni-id-pages/common/login-page.scss";

	.uni-content {
		margin-top: 15px;
	}

	.uni-content ::v-deep .uni-forms-item__label {
		position: absolute;
		left: -15px;
	}

	button {
		margin-top: 15px;
	}
</style>
