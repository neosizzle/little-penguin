/*  
 * hello-1.c - The simplest kernel module.
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/usb.h>		/* Needed for driver macros*/
#include <linux/usb/input.h>
#include <linux/hid.h>

MODULE_AUTHOR("jng");
MODULE_LICENSE("GPL");

/*  
 * mouse_table - hardware definitions for mouse and keyboard
 */
static struct usb_device_id definition_table[] = {
	{USB_INTERFACE_INFO(
		USB_INTERFACE_CLASS_HID,
		USB_INTERFACE_SUBCLASS_BOOT,
		USB_INTERFACE_PROTOCOL_MOUSE
	)},
	{USB_INTERFACE_INFO(
		USB_INTERFACE_CLASS_HID,
		USB_INTERFACE_SUBCLASS_BOOT,
		USB_INTERFACE_PROTOCOL_KEYBOARD
	)},
	{0}
};
MODULE_DEVICE_TABLE(usb, definition_table);

// function to handle probe
int handle_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	printk(KERN_INFO "Usb PROBED !\n");
	return 0;
}

// function to handle disconnect
void handle_disconnect(struct usb_interface *intf)
{
	printk(KERN_INFO "Usb DCED !\n");
}


// register device
static struct usb_driver input_driver = {
 .name = "inputDriver",
 .id_table = definition_table,
 .probe = handle_probe,
 .disconnect = handle_disconnect,
};

int init_module(void)
{
	printk(KERN_INFO "Hello world !\n");

	/* 
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	int result = usb_register(&input_driver);
	if (result)
		printk(KERN_INFO "Usb register failed !\n");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
	usb_deregister(&input_driver);
}
