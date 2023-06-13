using Fan.Data;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Http;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.EntityFrameworkCore;
using Fan.Models;

namespace Fan.Controllers
{
    [Route("[controller]")]
    [ApiController]
    public class UserController : ControllerBase
    {
        private readonly FanContext _context;
        public UserController(FanContext context)
        {
            _context = context;
        }

        //Get:User
        [HttpGet]
        public async Task<ActionResult<IEnumerable<User>>> GetTableUser()
        {
            return await _context.User.ToListAsync();
        }

        //POST: User/Login
        // To protect from overposting attacks, enable the specific properties you want to bind to, for
        // more details, see https://go.microsoft.com/fwlink/?linkid=2123754.
        [HttpPost("Login")]
        //登录
        public ActionResult<CommonResult> Login(User postData)
        {
            bool isExsits = _context.User.Any(o => o.UserName == postData.UserName);
            if (isExsits)
            {
                var password = _context.User.Where(o => o.UserName == postData.UserName).Select(o => o.Password).Single();
                if (postData.Password == password)
                {
                    //FirstOrDefault用于返回序列中的第一个元素，当序列为空时会报异常
                    var userid = _context.User.FirstOrDefault(o => o.UserName == postData.UserName).Id;
                    var DeviceList = _context.UserDevice.Where(o => o.UserId == userid).Select(o => o.DeviceId).ToArray();
                    if (DeviceList != null)
                    {
                        return new CommonResult()
                        {
                            Status = true,
                            Message = "登录成功！",
                            DeviceList = DeviceList
                        };
                    }
                }
                else return new CommonResult()
                {
                    Status = false,
                    Message = "密码错误！"
                };
            }
            return new CommonResult()
            {
                Status = false,
                Message = "请注册！"
            };
        }
        //Post:User
        [HttpPost]
        public ActionResult<CommonResult> Register(User postData)
        {
            bool isExsits = _context.User.Any(o => o.UserName == postData.UserName);
            if (isExsits)
            {
                return new CommonResult()
                {
                    Status = false,
                    Message = "用户已经存在"
                };
            }
            else
            {
                _context.User.Add(postData);
                _context.SaveChanges();
                return new CommonResult()
                {
                    Status = true,
                    Message = "注册成功！"
                };
            }
        }
        // User/VerifyUser
        [HttpPost("VerifyUser")]
        public ActionResult<CommonResult> VerifyUser(User postData)
        {
            bool isExsits = _context.User.Any(o => o.UserName == postData.UserName);
            if (isExsits)
            {
                return new CommonResult()
                {
                    Status = false,
                    Message = "用户已存在"
                };
            }
            else return new CommonResult()
            {
                Status = true,
                Message = "请输入密码！"
            };
        }
    }
}
