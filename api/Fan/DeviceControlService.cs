using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Net.NetworkInformation;
using System.Net.WebSockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Fan
{
    // You may need to install the Microsoft.AspNetCore.Http.Abstractions package into your project
    public class DeviceControlService
    {
        private readonly RequestDelegate _next;

        private const string _routePostfix = "/ws";

        private static ILogger<DeviceControlService> _logger;

        /// <summary>
        /// 缓冲区大小
        /// </summary>
        private const int bufferSize = 1024 * 4;

        private static ConcurrentDictionary<string, WebSocket> _socketsList = new ConcurrentDictionary<string, WebSocket>();


        public DeviceControlService(RequestDelegate next, ILogger<DeviceControlService> logger)
        {
            _next = next;
            _logger = logger;
        }

        public async Task Invoke(HttpContext context)
        {
            //判断当前请求是否为WebSocket
            if (!IsWebSocket(context))
            {
                await _next.Invoke(context);
                return;
            }

            //将请求转换为 WebSocket 连接
            var webSocket = await context.WebSockets.AcceptWebSocketAsync();

            try
            {
                while (webSocket.State == WebSocketState.Open)
                {
                    //接收消息
                    var entity = await ReceiveEntity<MessageEntity>(webSocket);
                    if (entity != null)
                    {
                        this.RegistDevice(entity.Params[0], webSocket);
                    }
                }
                await webSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, "Close", default(CancellationToken));
            }
            catch (Exception ex)
            {
                var tmp = ex;
            }
        }

        //public async Task Invoke(HttpContext context)
        //{
        //    if (!IsWebSocket(context))
        //    {
        //        await _next.Invoke(context);
        //        return;
        //    }

        //    var webSocket = await context.WebSockets.AcceptWebSocketAsync();

        //    var socketFinishedTcs = new TaskCompletionSource<object>();

        //    this.Loop(webSocket);

        //    await socketFinishedTcs.Task;
        //}

        //async void Loop(WebSocket webSocket)
        //{
        //    WebSocketReceiveResult result;
        //    _logger.LogError("websocket连接");
        //    try
        //    {
        //        var buffer = new byte[_bufferSize];
        //        result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);

        //        while (!result.CloseStatus.HasValue)
        //        {
        //            await webSocket.SendAsync(new ArraySegment<byte>(buffer, 0, result.Count),result.MessageType,result.EndOfMessage,CancellationToken.None);
        //            //var entity = this.ReceiveEntity<MessageEntity>(buffer);
        //            //switch (entity.MessageType)
        //            //{
        //            //    case "Register":
        //            //        this.RegistDevice(entity.Params[0], webSocket);
        //            //        _logger.LogError(entity.Params[0]);
        //            //        break;
        //            //}

        //            result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);
        //        }

        //        await webSocket.CloseAsync(result.CloseStatus.Value, result.CloseStatusDescription, CancellationToken.None);
        //    }
        //    catch (Exception ex)
        //    {
        //        var tmp = ex;
        //    }
        //}

        /*        public static async Task SendMessage(string deviceId, MessageEntity message)
                {
                    try
                    {
                        var tmp = JsonConvert.SerializeObject(message);
                        _logger.LogError(tmp);
                        await SendMessage(deviceId, tmp);
                    }
                    catch (Exception ex)
                    {
                        var tmp = ex;
                    }

                }*/

        public static async Task SendMessage(string deviceId, string message)
        {
            try
            {
                if (!_socketsList.ContainsKey(deviceId))
                {
                    _logger.LogError("设备不在线");
                    return;
                }
                var webSocket = _socketsList[deviceId];
                var bytes = Encoding.UTF8.GetBytes(message);
                await webSocket.SendAsync(
                    new ArraySegment<byte>(bytes),
                    WebSocketMessageType.Text,
                    true,
                    CancellationToken.None
                );
            }
            catch (Exception ex)
            {
                var tmp = ex;
            }

        }

        /// <summary>
        /// 判断设备是否在线
        /// </summary>
        // <param name="deviceId"></param>
        /// <returns></returns>
        public static bool IsDeviceOnline(string deviceId)
        {
            /*return _socketsList.Keys.Contains(deviceId);*/
            return _socketsList.ContainsKey(deviceId);
        }

        /// <summary>
        /// 当前请求是否为WebSocket
        /// </summary>
        // <param name="context">Http上下文</param>
        /// <returns></returns>
        private bool IsWebSocket(HttpContext context)
        {
            return context.WebSockets.IsWebSocketRequest &&
                context.Request.Path == _routePostfix;
        }

        /// <summary>
        /// 接收消息
        /// </summary>
        // <param name="webSocket">WebSocket</param>
        //<typeparam name="TEntity">typeof(TEntity)</typeparam>
        /// <returns></returns>
        //private TEntity ReceiveEntity<TEntity>(ArraySegment<byte> buffer)
        //{
        //    var json = Encoding.UTF8.GetString(buffer.Array);
        //    json = json.Replace("\0", "").Trim();

        //    return JsonConvert.DeserializeObject<TEntity>(json, new JsonSerializerSettings()
        //    {
        //        DateTimeZoneHandling = DateTimeZoneHandling.Local
        //    });
        //}

        /// <summary>
        /// 接收消息
        /// </summary>
        // <param name="webSocket">WebSocket</param>
        // <typeparam name="TEntity">typeof(TEntity)</typeparam>
        /// <returns></returns>
        private async Task<TEntity> ReceiveEntity<TEntity>(WebSocket webSocket)
        {
            try
            {
                if (webSocket.State == WebSocketState.Open)
                {
                    var buffer = new ArraySegment<byte>(new byte[bufferSize]);
                    var result = await webSocket.ReceiveAsync(buffer, CancellationToken.None);
                    while (!result.EndOfMessage)
                    {
                        result = await webSocket.ReceiveAsync(buffer, default(CancellationToken));
                    }
                    var json = Encoding.UTF8.GetString(buffer.Array);
                    json = json.Replace("\0", "").Trim();
                    return JsonConvert.DeserializeObject<TEntity>(json, new JsonSerializerSettings()
                    {
                        DateTimeZoneHandling = DateTimeZoneHandling.Local
                    });
                }
                else
                {
                    return default;
                }

            }
            catch
            {
                await webSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, "Close", default(CancellationToken));
                return default;
            }
        }


        private void RegistDevice(string DeviceId, WebSocket webSocket)
        {
            if (_socketsList.ContainsKey(DeviceId))
            {
                _socketsList[DeviceId] = webSocket;
            }
            else
            {
                _socketsList.TryAdd(DeviceId, webSocket);
            }
        }
    }

    // Extension method used to add the middleware to the HTTP request pipeline.
    public static class DeviceControlServiceExtensions
    {
        public static IApplicationBuilder UseDeviceControlService(this IApplicationBuilder builder)
        {
            return builder.UseMiddleware<DeviceControlService>();
        }
    }

    public class MessageEntity
    {
        //public string MessageType { get; set; }
        public List<string> Params { get; set; }
    }
}
