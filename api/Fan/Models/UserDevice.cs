using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.ComponentModel.DataAnnotations;

namespace Fan.Models
{
    public class UserDevice
    {
        public int Id { get; set; }
        public int UserId { get; set; }
        public int DeviceId { get; set; }
        public virtual User User { get; set; }
        //public virtual是继承虚类,会生成对应外键
        public virtual Device Device { get; set; }
        //public virtual是继承虚类
    }
}
