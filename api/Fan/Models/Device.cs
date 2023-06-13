using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.ComponentModel.DataAnnotations;

namespace Fan.Models
{
    public class Device
    {
        public int Id { get; set; }
        public string DeviceUUID { get; set; }
        public string DeviceName { get; set; }
        //然后在FanContext.cs中申明
    }
}
