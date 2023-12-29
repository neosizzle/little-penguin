# Little penguin
This project is about solving a series of problems from the eudyptula challenge, and this document will serve as a writeup on those challenges itself as well as a study list to get started with to keep up with the contents of this project.

## Prerequisutes
## Kernel Modules
>In computing, a loadable kernel module (LKM) is an object file that contains code to extend the running kernel, or so-called base kernel, of an operating system. LKMs are typically used to add support for new hardware (as device drivers) and/or filesystems, or for adding system calls. When the functionality provided by an LKM is no longer required, it can be unloaded in order to free memory and other resources.

:::success
https://tldp.org/LDP/lkmpg/2.6/html/x40.html
:::

Kernel modules are something like shared objects in linux binaries, where the base program is able to extend its features by linking to those shared objects and being able to use their code on runtime. Both entities are compilied seperately and are different entities.

It is used to add additional hardware support like device drivers and custom system calls on to the base kernel. Although, there are some critisism regarding a modular kernel which may cause [storage fragmentation](https://en.wikipedia.org/wiki/Fragmentation_(computing))

## Module loading
When the kernel encounters a feature that is required to acomplish by a module, the [kernel module daemon](https://man7.org/linux/man-pages/man8/kmod.8.html) will attempt to call `modprobe` on the missing module. The missing module can be passed in one 2 forms: the module name or an alias.

In the case of an alias, `modprobe` will look through its config files in `/etc/modprobe.conf` to find the actual module name. It will also locate any dependency modules in `/lib/modules/version/modules.dep` that needed to be loaded first.

Once everything is in order, `modprobe` calls [insmod](https://linux.die.net/man/8/insmod) to actually load the module. `insmod` does not do any alias-checking or dependency mapping, it just receives a file name and attempts to load it into the kernel. Pretty dumb.

## Implementation
>https://www.kernel.org/doc/Documentation/kbuild/modules.txt

Below is a list of useful commands you can use to debug and test modules : 
- lsmod (list all installed modules)
- modprobe (isntall a module)
- modinfo (get information about a module)
- rmmod (uninstall a module)

requried headers:
`linux/kernel.h`
`linux/module.h`


required functions : 
```
int init_module(void); // module init function / entry point

void cleanup_module(void)// module exit funtion
```
`init_module(void)` will be the entry point of your module, usually it will contain initialization processes followed by the logic of what your kernel is supposed to do.


`cleanup_module(void)` contains, as the name suggests, cleanup functions for your module logics. Memory deallocation, device deregistration, destruction of nodes and so on.

makefile rules and goals
```
obj-m += hello-1.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```
The `obj-m` rule is required by the kernels makefile to make a kernel module with the name the same as your .o's name compared to an object file or executable. If your kernel module consists of a collection of objects, you should use `obj-y` instead.

We would also want to call `make modules` in the kernels makefile and specify our working directory to search instead of compiling on our machine. After all, a .o is not the same as a .ko because a .ko also links with some kernel sources to make the .ko fully functional.

## module license
There are some conventions surrounding the use of `MODULE_LICENSE` macro which specifies what can be done to a software.




## Installation
Since modules vary for different kernels, We will need to use the Kernels `Makefile` to compile our module. For linux kernels, head to `/lib/modules/<kernel-version>/` and run `make SUBDIRS=<directory of source code> modules` to compile the module source into the native kernel module, and run `make SUBDIRS=<directory of source code> modules_install` to install the module into the system.

Once those are done, use `modprobe(8)` to load the module into the kernel and you should have it running already.

## Hot plugging

compile procedure:
1. inject driver code at module source https://static.lwn.net/images/pdf/LDD3/ch13.pdf
2. compile module and put it in a place where depmod can find `/lib/modules`
3. run depmod to generate dependency map to be read by udev

hotplug procedure:
1. kernel fires event uevent at udev
2. udev rules will traverse dependency and alias list and set env vars for device type
3. modprobe alias is executed, module(s) are loaded for every alias match
4. hardware is registered based on the evr vars above.

## Device drivers
### General
Drivers for any hardware share some common code and procedure as follows:

1. Definition of hardware
    > In the module, you will need to define an ID table to specify which hardware is supported by the module itself. Every device class will have their id struct that you need to initialize. A function returning an array of those structs are needed to tell the kernel that this module is meant for those devices.
2. Exposing definition to userspace
    > Once the hardware definitions are done in kernelspace, Those definitions will be known to the userspace via the `MODULE_DEVICE_TABLE` macro. This macro will create a local variable called `__mod_class_device` in the module that point to the ID list created earlier. programs like `depmod` will look for this symbols and generate a dependency map so that this module is recognized by high level device handlers like udev.
3. Create driver instance.
    > Once we have our definitions ready, its time to create the real driver code. For the device that you are listening for, there will be a driver struct named like `dev_driver`. This structure consists of a number of function callbacks and variables that describe the PCI driver to the PCI core. Below is how a driver struct for PCI devices look like 
    ```
        static struct pci_driver pci_driver = {
             .name = "pci_skel",
             .id_table = ids,
             .probe = probe,
             .remove = remove,
        };
    ```
4. Register and unregister
    > Once we have the prerequisites ready, its time to register the priver with the core kernel so that it is runnable by doing `device_register_driver(&pci_driver);`. You can unregister by also doing `device_unregister_driver(&pci_driver);`

### USB
>https://www.kernel.org/doc/html/latest/driver-api/usb/writing_usb_driver.html
>https://static.lwn.net/images/pdf/LDD3/ch13.pdf

>The universal serial bus (USB) is a connection between a host computer and a number of peripheral devices.

The reason behind the creation is to solve compatibility issues across different external devices and to replace various slow busses. 

The USB Host controller is a mechanism in the host that is in charge of listening to data sent by USB devices. The USB devices will not send data unless its asked by the USB Host Controller hence making it to be able to du plug-and-play.

There is also the USB protocol which specifies the standard and the convention data which were to be transferred by a USB device. Non-Adherence to the standard will require the user to install the driver for the said device.

For USB Classes and protocol codes when creating a device or interface ids using `USB_DEVICE_INFO` or `USB_INTERFACE_INFO` can be found [here](https://www.usb.org/defined-class-codes)

There are 2 types of USB drivers, a USB Host driver is installed in the host and controls the USB devices which are plugged in. A USB gadget driver controls the how a single USB device looks from a host computer.

![](https://i.imgur.com/mhRwCJu.png)
The [USBcore](https://docs.kernel.org/driver-api/usb/callbacks.html) is a linux subsystem that helps us deal with the complexity of a USB device.

![](https://i.imgur.com/RbYHp0G.png)
An endpoint is a channel that allows data to travel in inly 1 direction. It is similar to a unifirectional pipe.

A USB endpoint can be one of four different types that describe how the data is
transmitted:

- CONTROL
    > The control endpoint is used to configure the device and get information about the device, it is relatively small in side and the protocol will always ensure that there is enough space before any transmission is done on this endpoint
- INTERRUPT
    > These endpoints are used by the host device when it wants to request for more data and its typically used in mice and keyboards. It is also small in size and the USB protocol will also reserve its size before transmissions
- BULK
    > These endpoints are large in size as they are used to transfer large amounts of data. The data is transfered in BULK packets and splitting usually happens when a single transmission isnt enough due to bandwith limitations. It is commonly used in pen drives, network devices and printers.

- ISOCHRONOUS
    > These endpoints also transmits large amount of data in a stream, but there might be some data loss in between. The endpoints are common in media devices such as webcams and mics.

Control and bulk endpoints are used for asynchronous data transfers, whenever the driver decides to use them. Interrupt and isochronous endpoints are periodic. This means that these endpoints are set up to transfer data at fixed times continuously, which causes their bandwidth to be reserved by the USB core.

The USB code in the Linux kernel communicates with all USB devices using something called a urb (USB request block). This request block is described with the struct urb structure and can be found in the include/linux/usb.h file.

```
struct urb
{
// (IN) device and pipe specify the endpoint queue
      struct usb_device *dev;         // pointer to associated USB device
      unsigned int pipe;              // endpoint information

      unsigned int transfer_flags;    // URB_ISO_ASAP, URB_SHORT_NOT_OK, etc.

// (IN) all urbs need completion routines
      void *context;                  // context for completion routine
      usb_complete_t complete;        // pointer to completion routine

// (OUT) status after each completion
      int status;                     // returned status

// (IN) buffer used for data transfers
      void *transfer_buffer;          // associated data buffer
      u32 transfer_buffer_length;     // data buffer length
      int number_of_packets;          // size of iso_frame_desc

// (OUT) sometimes only part of CTRL/BULK/INTR transfer_buffer is used
      u32 actual_length;              // actual data buffer length

// (IN) setup stage for CTRL (pass a struct usb_ctrlrequest)
      unsigned char *setup_packet;    // setup packet (control only)

// Only for PERIODIC transfers (ISO, INTERRUPT)
  // (IN/OUT) start_frame is set unless URB_ISO_ASAP isn't set
      int start_frame;                // start frame
      int interval;                   // polling interval

  // ISO only: packets are only "best effort"; each can have errors
      int error_count;                // number of errors
      struct usb_iso_packet_descriptor iso_frame_desc[0];
};
```

A urb is used to send or receive data to or from a specific USB endpoint on a specific USB device in an asynchronous manner. Every endpoint in a device can handle a queue of urbs, so that multiple urbs can be sent to the same endpoint before the queue is empty. An URB undergoes the following lifecycle : 


1. Creation by the USB Driver
2. Assigned to a specific endpoint of a specific USB device.
3.  Submitted to the USB core, by the USB device driver.
4.  Submitted to the specific USB host controller driver for the specified device by the USB core.
5.  Processed by the USB Host Controller and URB is consumed as data is sent
6.  Host Contrller notifies device driver about completion of transfer

### Character devices and misc devices
>https://static.lwn.net/images/pdf/LDD3/ch03.pdf
>https://www.linuxjournal.com/article/2920
>https://stackoverflow.com/questions/18456155/what-is-the-difference-between-misc-drivers-and-char-drivers

A character device is a device that have streams of charcaters written or read from it.

Char devices can be access by its name in the file system, and it is conventionally placed in the `/dev/` directory. 
A **Major number** of a device is an identification of the driver for the device. Devices handled by the same driver share the same major number. The **Minor number** refers to which device exactly it is referred to and it is generated by the drivers themselves. The number pair is used to identify devices in a linux system. The numbers can be allocated using `int alloc_chrdev_region(dev_t *dev, unsigned int firstminor,
 unsigned int count, char *name);` in a module and required to be freed in the end.
 
The kernel data structure *file_operations* is used by a file object as a reference to common system calls `read, write...`  as it contains pointers to functions implemented in the char device driver.

The struct *file*, defined in ``<linux/fs.h>``, is the second most important data structure used in device drivers. The file structure represents an open file descriptor in the kernelspace. The contents of that struct can be found [here](https://www.oreilly.com/library/view/linux-device-drivers/0596000081/ch03s04.html).

The struct *inode* in the kernel is used internally to represent files. In contract to the *file* struct which is used to represent open file desciptors

 
Every driver needs to first generate a major and minor number pair. After that, a device class should be created at /sys/class using `class_create`, using that class we can create a device node at `/dev/device` using `device_create`, and register the device in kernelspace using `cdev_init` and `cdev_add` .

Similar to character devices, misc devices also has the usual `file_operations` and can behave like a character device (or any other device if you want it to). The actual purpose of misc devices is to act as a classification for devices which dont fit into any mainstream category. 

Misc devices dont need to allocate space for a major number, since all misc devices are classified under one major number. You can see what number it is by reading `/proc/devices`. 

## Processes
In every operating system, there will be a structure which represents a process in the kernel. This structure is called a Process Control Block (PCB).

Processes in the linux kernel space is represented by [`task_struct`](https://docs.huihoo.com/doxygen/linux/kernel/3.7/structtask__struct.html)s.

In our file handler, to get the current process which initiated the system call, you can do so by reading the `current` macro which is defined [here](https://elixir.bootlin.com/linux/latest/source/arch/x86/include/asm/current.h).


## Directory listing (fs access)
To access the file system of a process, there is a member in the `task_struct` structure in the kernel, which allows you to get the representation of a file system which the process is in. 

To start traversing a file system from its root, we can use the `fs` member to get the file system of the process, and access its root in the file system. the `root` member is actually a `path` struct which represents a directory parh in kernelspace. There will be a `debtry *` inside of the path struct, which you can use to access its children via iteration using `list_for_each_entry`


## Linux hardware devices, sysfs, udev
## Busses and device files
>In computer architecture, a bus is a communication system that transfers data between components inside a computer, or between computers. 

![bussin](https://upload.wikimedia.org/wikipedia/commons/thumb/6/68/Computer_system_bus.svg/600px-Computer_system_bus.svg.png)

For internal CPU operations, we have **internal busses** (address bus, memory bus, control bus...) which connects computer components like the CPU and memory inside the motherboard.

**External busses** are ports that connects external physical devices from outside the computer like USBs, network adapters and so on.

A **Device driver** is a program that allows other software to communicate and use external devices the driver is programmed for. It provides an interface for other applications so they are able to invoke instructions to the hardware without knowing about the hardware details. The communication to the hardware is typically done by the External busses.

Usually the drivers are run in user mode (Ring 3 for x86 CPUs) as opposed to Kernel mode (Ring 0 for x86 CPUs) for better stability as a poorly written driver may be able to crash the system by overwriting memory.

In linux, a **Device file** is an interface to a device driver that appears as an ordinary file in the file system. Users or applications can read / write using syscalls to device files to invoke calls to the driver, which indirectly gives instructions to the hardware devices itself.

For example, calling `write()` to `fd 1` reads the file desciptor `1` at `/dev/fd` which is a device file. That device file corresponds to the output interface to the terminal. Any operations even reading and writing memory/data requires an I/O interface to commence, e.g. CPU writes data to RAM using [memory mapped I/O](https://en.wikipedia.org/wiki/Memory-mapped_I/O_and_port-mapped_I/O)

![devices](https://upload.wikimedia.org/wikipedia/commons/thumb/6/65/Simplified_Structure_of_the_Linux_Kernel.svg/1200px-Simplified_Structure_of_the_Linux_Kernel.svg.png)

>Device files usually provide simple interfaces to standard devices (such as printers and serial ports), but can also be used to access specific unique resources on those devices, such as disk partitions. Additionally, device files are useful for accessing system resources that have no connection with any actual device, such as data sinks and random number generators.

There are 3 types of devices/interfaces when it comes to linux I/O : 
- Character devices
    - Devices that provide direct and _Unbuffered_ read and write access to the component
- Block devices
    - Unlike character devices, block devices provide buffered access to the component, internally managing all duplex I/O without the need for user intervention.
- Pseudo devices
    - Unlike the previous two, pseudo devices do not map to a physical hardware. They provide various functions handled by the operating system. Some of the most commonly used (character-based) pseudo-devices include `/dev/null`, `/dev/rand` ...

**Device nodes** are data units that linux use to represent file-like resources that is allocated by the kernel like named pipes, device files and pseudo files. Creation of these nodes are done by the [mknod](https://man7.org/linux/man-pages/man2/mknod.2.html) syscall.


## sysfs
>sysfs is a pseudo file system provided by the Linux kernel that exports information about various kernel subsystems, hardware devices, and associated device drivers from the kernel's device model to user space through virtual files.

A kernel object contains data relevant to its type. For example, a file object contains read-write-execute permissions, file size, data offset and a process object has a process Id, exit mode and priority. The memory which is used to store these objects can be only accessed by the kernel, hence the name kernel object.

sysfs is able to represent and expose these kernel objects as file system tree so that applications and users able to map any bi-directional changes from userspace to the kernel data itself. 

The `udev` program leverages sysfs functionality to provide users a more high level control over their hardware. Each directory and file in sysfs has their own meanings, and the specifications are stated [here](https://mirrors.edge.kernel.org/pub/linux/kernel/people/mochel/doc/papers/ols-2005/mochel.pdf)

## udev
>udev (userspace /dev) is a device manager for the Linux kernel. As the successor of devfsd and hotplug, udev primarily manages device nodes in the /dev directory. At the same time, udev also handles all user space events raised when hardware devices are added into the system or removed from it, including firmware loading as required by certain devices.

For every operating system, there will be a system or mechanism that provides user abstraction to manage hardware devices. For linux, hardware can be accessed via device nodes or system calls.

![udev and systemd](https://upload.wikimedia.org/wikipedia/commons/thumb/7/7b/Free_and_open-source-software_display_servers_and_UI_toolkits.svg/525px-Free_and_open-source-software_display_servers_and_UI_toolkits.svg.png)

essentially, udev is a daemon ran by linux operating systems and its always listening to device changes (discovery, state change) via `udevd` emmited by device drivers called `uevents`. The udev package comes installed a set of rules to refer against when a new device is discovered. The device will then match certain rules and udev will respond and configure the device in userspace accordingly in respect of said rules and a new device node will be created in sysfs.

An internet browser is able to establish connections to the internet using the machines network card with the following toolchain with udev:

$$
Kernel \rightarrow  udev \rightarrow  Network Manager \rightleftharpoons  DBus \rightleftharpoons Firefox
$$

- **udev** the low level applications that manages kernel device nodes that corresponds to hardware
- **dbus** is a framework and API to establish inter-application communications
- [**Network manager**](https://en.wikipedia.org/wiki/NetworkManager) is a daemon that sits on top of udev library that provides a high-level interface to network configuration and setup and is accessible via dbus to apps.

## Boot configuration
- [The Boot and The Startup](https://hackmd.io/rpewLqCCTN6JfitSESiLig)

To configure a GRUB bootloader, you need to first install it to a block device by doing `grub-install /dev/sda`, replace `/dev/sda` with the block device you are using

:::danger
This will override the current bootloader of your machine and your machine will attempt to boot to the linux system on next reboot, so please be careful
:::

After installation, you need to tell the bootloader to locate your kernel and how to run it. Here is the configuration I used: 
```
# Begin /boot/grub/grub.cfg
set default=0
set timeout=5

insmod ext2
set root=(hd0,6)

menuentry "GNU/Linux, Linux 6.1.11-lfs-11.3" {
        linux   /boot/vmlinuz-6.1.11-jng root=/dev/sda2 ro
}
```
the `set root=(hd0,6)` line represents which partition is gurb installed in. the `hd(0, 6)` section corresponds to `sda6` in such a way that `hd` means hard drive, `0` means sda and `6` means 6.

the `linux   /boot/vmlinuz-6.1.11-jng root=/dev/sda2 ro` line specifies the kernel location as well as the root of the new linux system. The bootloader will read this configration and execute startup scrips after.

Upon loading a bootloader, system v or any central manager will runs some scripts defined by the user / by default to set up and verify the health of the system before any startup processes are initialized. Here are the things the script runs in order prior to startup : 

![](https://i.imgur.com/w0oXHmC.png)
1. Mounts virtual fs
    >Creates and mounts `/pros`, `/sys`, `/dev`. Mounts `/run` (tmpfs or in memory fs to store runtime info)
2. Loads required modules
    > Use [modprobe](https://linux.die.net/man/8/modprobe) to load modules in `/etc/sysconfig/modules`. Modules are pieces of code that can be loaded/executed and unloaded into kernel on-demand
3. Udev cold plugging
    > Cold plugging is the process of shutting down a computer in order to add or remove a component or to synchronize data with the computer
    > This procedure populates /dev with device nodes and starts udev daemon. Udev will then create and validate the device nodes as per defined.
4. Deactivates and activates swap partitions if any
5. Store and restore time in hardware clock (hwclock)
6. Checks and verifies the health of file systems
    > [man fsck](https://linux.die.net/man/8/fsck)
7. Mounts local filesystems defined in `/etc/fstab`
8. Replays any failed uevents during boot and creates additional devices
9. Cleans temporary directories and `/run` and `/tmp`. Also creates any files in `/etc/sysconfig/createfiles`
10. Sets up console
    > Fonts, locales, keyboards defined in `/etc/sysconfig/console`
11. Starts the local network
    > sets hostname and initializes loopback interface
12. Run sysctl
    > sysctl is used to modify kernel parameters at runtime. It reads from `/etc/sysctl.conf` and set values accordingly
13. Start log daemons for kernel and system.
14. Starts and configure network interfaces.
    > Stops and unmounts all network related devices / interfaces.
    > Start all network interfaces defined in ifconfig files
16. Kills remaining processes.

## ex00
The goal of ex00 is to compile a kernel with the `CONFIG_LOCALVERSION_AUTO` flag enabled. To look for this configuration, look for the .config file in the directory where the kernel is to be compiled and change the line to `CONFIG_LOCALVERSION_AUTO=y`. That flag will then be activated upon compilation.

After compiling the kernel, replace the kernel executable in your devices boot drive.

The option we enabled just now prints extra information about our kernel version, in the boot log. `Linux version 6.4.0-rc4-00013-ge338142b39cf` is shown as opposed to `Linux version 6.4.0` without the flag enabled.

## ex01
This exercise wants us to compile and load a custom hello world kernel module. The module code itself will follow the kernel module format; it will print hello world to the kernel logs which is accessible by the command `dmesg -w`

## ex02
This exercise wants us to create a patchfile for the kernel source that follows [linux submitting-patches.rst](https://github.com/torvalds/linux/blob/master/Documentation/process/submitting-patches.rst)
 guidelines.

 The purpose of the patch is to add a custom version to our kernel, with the help of `CONFIG_LOCALVERSION_AUTO` from ex00 and a new kernel flag, `EXTRAVERSION` field in the kernels Makefile

 ## ex03
This exercise exposes us to the coding style of the linux kernel [here](https://github.com/torvalds/linux/blob/master/Documentation/process/4.Coding.rst). We have to change a pre given source code to fir this style. 

Or we can use the clang formatter that came with kernel source to automatically format codes that can be formatted automatically

## ex04
In this exercise, we will be implementing hotplug USB devices. When a USB device is plugged in, we need to be able to load the module to support that device and to be able to run some logic on probe and on disconnect. For now, the module that we would be loading is a simple hello world kernel.

To acheive this, our module needs to have a `usb_driver` struct (defined by kernel) that will contain all the functions for disconnect and probing, as well as some metadata describing what type of usb device this is. Once we have the struct ready, we can call another kernel function, `‎usb_register` to notify the rest of the kernel about this new usb device.

In the USB rules file in udev, we need to add the following lines. This file automatically take action on behalf of you when a USB device connects, it will work on small modules.


```
# /etc/udev/rules.d/79-usb.rules  
# module to /lib/modules/6.1.11
# depmod
# udevadm control --reload-rules && udevadm trigger
ACTION=="add", SUBSYSTEM=="usb", PROGRAM="/usr/sbin/modprobe hello-2"
ACTION=="remove", SUBSYSTEM=="usb", PROGRAM="/usr/sbin/rmmod hello-2""
```
