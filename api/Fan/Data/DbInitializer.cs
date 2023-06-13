using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Fan.Models;

namespace Fan.Data
{
    public static class DbInitializer
    {
        public static void Initializer(FanContext context)
        {
            context.Database.EnsureCreated();

            if (context.User.Any())
            {
                return; //DB 已经 seeded
            }

            var uers = new User[]
            {
                new User{
                    Id=1,
                    UserName="posty",
                    Password="574822"
                }
            };
            foreach (User u in uers)
            {
                context.User.Add(u);
            }
            context.SaveChanges();

            var devices = new Device[]
            {
                new Device
                {
                    Id=1,
                    DeviceUUID="0b466f5e-1c11-413e-92cf-986e42af4861",
                    DeviceName="Posty's BLE Device"
                }
            };
            foreach(Device d in devices)
            {
                context.Device.Add(d);
            }
            context.SaveChanges();

            var userDevice = new UserDevice[]
            {
                new UserDevice
                {
                    Id = 1,
                    UserId = 1,
                    DeviceId = 1
                }
            };
            foreach (UserDevice j in userDevice)
            {
                context.UserDevice.Add(j);
            }
            context.SaveChanges();
        }
    }
}
