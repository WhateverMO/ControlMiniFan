const {
  dbCmd,
  LOG_TYPE,
  deviceCollection,
  userCollection
} = require('../../common/constants')

/**
 * 用户退出登录
 * @tutorial https://uniapp.dcloud.net.cn/uniCloud/uni-id-pages.html#logout
 * @returns
 */
module.exports = async function () {
  const {
    uniIdToken,
    deviceId
  } = this.getClientInfo()
  const {
    uid
  } = await this.uniIdCommon.checkToken(
    uniIdToken,
    {
      autoRefresh: false
    }
  )

  // 删除token
  await userCollection.doc(uid).update({
    token: dbCmd.pull(uniIdToken)
  })

  // 仅当device表的device_id和user_id均对应时才进行更新
  await deviceCollection.where({
    device_id: deviceId,
    user_id: uid
  }).update({
    token_expired: 0
  })
  await this.middleware.uniIdLog({
    data: {
      user_id: uid
    },
    type: LOG_TYPE.LOGOUT
  })
  return {
    errCode: 0
  }
}
