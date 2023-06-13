using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Fan.Models;
using Microsoft.EntityFrameworkCore;
using Fan.Dto;

namespace Fan.Data
{
    public class FanContext :DbContext
    {
        public FanContext(DbContextOptions<FanContext> options) : base(options)
        {
        }
        public DbSet<User> User { get; set; }
        public DbSet<Device> Device { get; set; }
        public DbSet<UserDevice> UserDevice { get; set; }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            modelBuilder.Entity<UserDevice>()
                .HasOne(o => o.User)
                .WithMany(o => o.UserDevice)
                .HasForeignKey(o => o.UserId);

            modelBuilder.Entity<UserDevice>()
               .HasOne(o => o.Device)
               .WithMany()
               .HasForeignKey(o => o.DeviceId);

            base.OnModelCreating(modelBuilder);
            //指定单数形式的表名称会覆盖默认行为
            modelBuilder.Entity<User>().ToTable("User");
            modelBuilder.Entity<Device>().ToTable("Device");
            modelBuilder.Entity<UserDevice>().ToTable("UserDevice");
        }
    }
}
