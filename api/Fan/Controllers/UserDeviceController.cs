using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Fan.Models;
using Microsoft.AspNetCore.Mvc;
using Fan.Data;
using Microsoft.EntityFrameworkCore;
using Fan.Dto;

namespace Fan.Controllers
{
    [Route("[controller]")]
    [ApiController]
    public class UserDeviceController : ControllerBase
    {
        private readonly FanContext _context;
        public UserDeviceController(FanContext context)
        {
            _context = context;
        }

        //GET:UserDevice
        [HttpGet]
        //get整个UserDevice的List
        public async Task<ActionResult<IEnumerable<UserDevice>>> GetUserDevice()
        {
            return await _context.UserDevice.ToListAsync();
        }

        //GET:UserDevice/
        [HttpGet("{id}")]
        //获取UserDevice中的id
        public async Task<ActionResult<UserDevice>> GetUserDevice(int id)
        {
            var userDevice = await _context.UserDevice.FindAsync(id);

            //如果UserDevice中的id不存在，即返回NotFound()给后端
            if (userDevice == null)
            {
                return NotFound();
            }

            return userDevice;
        }
        // POST: UserDevice
        // To protect from overposting attacks, enable the specific properties you want to bind to, for
        // more details, see https://go.microsoft.com/fwlink/?linkid=2123754.
        [HttpPost]
        public ActionResult<CommonResult> PostUserDevice(UserDeviceDto userDevice)
            //UserDiveceDto为前端传过来的数据，需要自己定义有那些属性
        {
            var device = new Device
            //var 一个device为Device表类型，接收从前端传过来的DeviceUUID和DeviceName
            {
                DeviceUUID = userDevice.DeviceUUID,
                DeviceName = userDevice.DeviceName
            };
            if (_context.Device.Any(o => o.DeviceUUID == userDevice.DeviceUUID) == false)
            {
                //若传过来的DeviceUUId在Device表中不存在，则把device当作新设备注册进Device表中
                _context.Device.Add(device);
                _context.SaveChanges();
            }
            //先判断设备是否注册到了Device表中，再判断UserName和和DeviceUUId是否成对在UserDevice表中
            //即注册了，判断是否配对绑定
            var user = _context.User.First(o => o.UserName == userDevice.UserName);

            var getDevice = new UserDevice
            //var 一个 getDevice为UserDevice表类型，接收从
            {
                //找到传过来的userDevice中的UserName和User表中相同的UserName，查出UserName在User中对应的Id
                UserId = _context.User.FirstOrDefault(o => o.UserName == userDevice.UserName).Id,
                //User = user,
                //找到userDevice中的UUID和Device中的UUID相同的UUID在Device中对应的Id
                DeviceId = _context.Device.FirstOrDefault(o => o.DeviceUUID == userDevice.DeviceUUID).Id
                //Device = device
            };
            bool i = _context.UserDevice.Any(o => o.UserId == getDevice.UserId);
            //判断UserDevice中是否存在已经绑定了的UserId

            if (i)
            {
                //如果UserId已经存在，并且该UserId对应的UUID等于传入的UUID，则证明UserId和UUID已经双向绑定
                if (_context.UserDevice.Any(o => o.UserId == getDevice.UserId && o.DeviceId == getDevice.DeviceId))
                {
                    return new CommonResult()
                    {
                        Status = false,
                        Message = "请勿重复绑定"
                    };
                }
                else
                {

                    //UserId存在于配对列表中，但是对应的UUID为其他设备，则绑定UserId和传入的新的UUID，即绑定新设备
                    _context.UserDevice.Add(getDevice);
                    _context.SaveChanges();
                    return new CommonResult()
                    {
                        Status = true,
                        Message = "绑定新设备成功"
                    };
                }
            }
            else
            {
                //UserId没有进行过任何绑定，在绑定列表中没有记录，则直接将UserId和UUID进行绑定
                _context.UserDevice.Add(getDevice);
                //将获取的UserId和DeviceId绑定
                _context.SaveChanges();
                return new CommonResult()
                {
                    Status = true,
                    Message = "注册绑定成功"
                };
            }
        }

        [HttpPost("DeviceCommand")]
        public async Task<ActionResult<bool>> DeviceCommand(Temp data)
        {
            var deviceId = (string)data.DeviceId;
            var message = (string)data.Message;
            await DeviceControlService.SendMessage(deviceId, message);
            return true;
        }

        [HttpPost("IsDeviceOnline")]
        public CommonResult IsDeviceOnline(Temp data)
        {
            var deviceId = data.DeviceId; ;
            //从前端获取的UUID，根据UUID在数据库中的Device中查找DeviceName，并返回前端
            var DeviceList = _context.Device.Where(o => o.DeviceUUID == deviceId).Select(o => o.DeviceName).ToArray();
            var getDeviceId = DeviceControlService.IsDeviceOnline(deviceId);
            if (getDeviceId)
            {
                var result = new CommonResult()
                {
                    Status = true,
                    Message = "设备在线！",
                    DeviceList = DeviceList
                };
                return result;
            }
            return new CommonResult()
            {
                Status = false,
                Message = "设备不在线!"
            };
        }

    }
}
