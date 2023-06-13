using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Fan.Models
{
    public class CommonResult
    {
        public bool Status { get; set; }
        public string Message { get; set; }
        public Array DeviceList { get; set; }
    }
}
