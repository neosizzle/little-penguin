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
