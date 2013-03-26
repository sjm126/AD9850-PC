/* Device driver for AD9850 Direct Digital Synthesizer */
/* Steven J. Merrifield */
/* June 2003 */

/* Based on Leon Heller's dds2.c DOS program for the PC parallel port */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/utsname.h>
#include <linux/major.h>
#include <linux/string.h>
#include <linux/fcntl.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <asm/MC68EZ328.h>

#define DDS_IOC_MAGIC	'k'
#define DDS_IOCTFREQ 	_IO(DDS_IOC_MAGIC,3)
#define DDS_MAJOR 	42

#define DDS_D0		1	/* Port D, bit 0 */
#define DDS_CLK		2	/* Port D, bit 1 */
#define DDS_FQUD	4	/* Port D, bit 2 */

void dds_one(void)
{
	PDDATA = (PDDATA & ~DDS_CLK) | DDS_D0;	/* CLK = 0, DATA = 1 */
	PDDATA = PDDATA | DDS_CLK;		/* CLK = 1, DATA = 1 */
	PDDATA = PDDATA & ~DDS_CLK & ~DDS_D0;	/* CLK = 0, DATA = 0 */
}


void dds_zero(void)
{
	PDDATA = PDDATA & ~DDS_CLK & ~DDS_D0; 	/* CLK = 0, DATA = 0 */
	PDDATA = PDDATA | DDS_CLK;		/* CLK = 1, DATA = 0 */
	PDDATA = PDDATA & ~DDS_CLK;		/* CLK = 0, DATA = 0 */
}


void dds_latch(void)
{
	PDDATA = PDDATA | DDS_FQUD;	/* FQUD = 1 */
	PDDATA = PDDATA & ~DDS_FQUD;	/* FQUD = 0 */
}


void byte_out(unsigned char byte)
{
        int i;

        for (i = 0; i < 8; i++)
        {
                if ((byte & 1) == 1)
                        dds_one();
                else
                        dds_zero();
                byte >>= 1;
        }
}


void tuning_word_out(unsigned int tuning_word)
{
        int i;

        for (i = 0; i < 32; i++)
        {
                if ((tuning_word & 1) == 1)
                        dds_one();
                else
                        dds_zero();
                tuning_word >>= 1;
        }
	byte_out(0x00);
	dds_latch();
}


int dds_open(struct inode *inode, struct file *filp)
{
	MOD_INC_USE_COUNT;
        return(0);
}


int dds_release(struct inode *inode, struct file *filp)
{
	MOD_DEC_USE_COUNT;
        return(0);
}


int dds_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int dds_freq;
	int err=0;

	if (_IOC_TYPE(cmd) != DDS_IOC_MAGIC) return(-EINVAL);

	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
	if (err) return(-EINVAL);

	switch(cmd)
	{
		case DDS_IOCTFREQ:
			dds_freq = arg;
			break;
		default:
			return(-EINVAL);
			break;
	}
	tuning_word_out(dds_freq);
	return(0);
}

static struct file_operations dds_fops = {
        ioctl:          dds_ioctl,
        open:           dds_open,
        release:        dds_release,
};


static int __init dds_init(void)
{
	SET_MODULE_OWNER(&dds_fops);
	printk(KERN_INFO "dds: Copyright (c) Steven J. Merrifield 2003\n");
	if (register_chrdev(DDS_MAJOR,"dds",&dds_fops) < 0)
	{
		printk("dds: can't get major number %d\n",DDS_MAJOR);
		return(-EBUSY);
	}

        /* No need for select, since bits 3-0 are always IO */

	/* setup FQUD, CLK and D0 as outputs */
        PDDIR = PDDIR | DDS_FQUD | DDS_CLK | DDS_D0;          
	// PDDIR = 0x07;

        /* set FQUD, CLK and D0 low */       
        PDDATA = PDDATA & ~DDS_FQUD & ~DDS_CLK & ~DDS_D0;
	// PDDATA = 0x00;

	return(0);
}

static void __exit dds_exit(void)
{
}

module_init(dds_init);
module_exit(dds_exit);

MODULE_AUTHOR("Steven Merrifield");
MODULE_DESCRIPTION("AD9850 Direct Digital Synthesizer");
MODULE_LICENSE("GPL");

