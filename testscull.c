#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/device.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<asm/uaccess.h>
#include<linux/ioctl.h>
#include<linux/slab.h>
#include"testscull.h"

MODULE_LICENSE("GPL");

int testscull_open(struct inode *, struct file *);
int testscull_release(struct inode *, struct file *);
ssize_t testscull_read(struct file *,char *,size_t ,loff_t *);
ssize_t testscull_write(struct file *,const char *,size_t ,loff_t *);
long  testscull_unlocked_ioctl(struct file *,unsigned int,unsigned long);

int dev_major=50;
int dev_minor=0;
char *pipe;

struct class *my_class;

struct testscull_dev
{
	struct cdev  cdev;
	int testdata; 
};

struct file_operations testscull_fops={
     .owner=THIS_MODULE,
     .read=testscull_read,
     .write=testscull_write,
     .unlocked_ioctl=testscull_unlocked_ioctl,
     .open=testscull_open,
     .release=testscull_release
};

struct testscull_dev * my_dev;

static void testscull_exit(void)
{
	dev_t devno=MKDEV(dev_major,dev_minor);
 	cdev_del(&my_dev->cdev);
 	kfree(my_dev);
 	unregister_chrdev_region(devno,1);
	device_destroy(my_class,devno);
	class_destroy(my_class);
 	printk("unregister success");
}

static int   testscull_init(void)
{
 	int ret,err;
  	dev_t devno=MKDEV(dev_major,dev_minor);
 	ret=alloc_chrdev_region(&devno,dev_minor,1,"testscull");
 	dev_major=MAJOR(devno);
	if(ret<0)
 	{
		printk("testscull register failture\n");
    	testscull_exit();
    	return ret;
   	}
	else 
	{
 	    printk("testscull register success\n");
	}
	my_dev=kmalloc(sizeof(struct testscull_dev),GFP_KERNEL);
	my_class=class_create(THIS_MODULE,"test");
	pipe=0;
	if(IS_ERR(my_class))
	{
		printk("create class Error\n");
		return -1;
	}
	device_create(my_class,NULL,MKDEV(dev_major,0),NULL,"test%d",0);
	if(!my_dev)
	 {
	     ret=-ENOMEM;
         printk("creat device failed\n"); 
     }
	else 
	 {
		 my_dev->testdata=0;
	  	 cdev_init(&my_dev->cdev,&testscull_fops);
	  	 my_dev->cdev.owner=THIS_MODULE;
	  	 my_dev->cdev.ops=&testscull_fops;
         err=cdev_add(&my_dev->cdev,devno,1); 
         if(err<0) printk("add device failed\n");
	 }
	return ret;
}

int testscull_open(struct inode *inode,struct file *filp)
{
	 struct testscull_dev * dev;
	 dev=container_of(inode->i_cdev,struct testscull_dev,cdev);
	 filp->private_data=dev;
  	 printk("open success\n");
	 return 0;
}

int testscull_release(struct inode *inode,struct file *filp)
{
	return 0;
}
ssize_t testscull_read(struct file *filp,char *buf,size_t len,loff_t *off)
{
	struct testscull_dev *dev;
 	dev=filp->private_data;
   	if(copy_to_user(buf,&dev->testdata,sizeof(int)))
	{
		return -EFAULT;
	}
	 return sizeof(int);
}

ssize_t testscull_write(struct file *filp,const char *buf,size_t len,loff_t *off)
{
	 struct testscull_dev *dev;
	 dev=filp->private_data;
	 if(copy_from_user(&dev->testdata,buf,sizeof(int)))
   	 {
		return -EFAULT;
	 }
	 return sizeof(int);
}

long testscull_unlocked_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
   	 int err=0;
	 long retval=0;
	 if(_IOC_TYPE(cmd)!=SCULL_IOC_MAGIC) return -ENOTTY;
	 if(_IOC_NR(cmd)>SCULL_IOC_MAXNR) return -ENOTTY;

	 if(_IOC_DIR(cmd) & _IOC_READ) 
		 err=!access_ok(VERIFY_WRITE,(void __user *)arg, _IOC_SIZE(cmd));
	 else 	if(_IOC_DIR(cmd) & _IOC_WRITE) 
         		err=!access_ok(VERIFY_READ,(void __user *)arg, _IOC_SIZE(cmd));
	 if(err)  return -EFAULT;
	 switch(cmd)
		{

			case SCULL_IOC_GET_DATA: 
				retval=copy_to_user((char __user *)arg,pipe,sizeof(char)*100);
				*pipe='\0';
				break;
			case SCULL_IOC_SET_DATA:
				retval=copy_from_user(pipe,(char __user *)arg,sizeof(char)*100);
				break;
			case SCULL_IOC_CREAT_PIPE:	
	 			if(pipe==0) pipe=kmalloc(sizeof(char)*100,GFP_KERNEL);
				break;
			case SCULL_IOC_DELETE_PIPE:	
				if(pipe!=0)kfree(pipe);
				break;
			default:
				return -ENOTTY;
		}
	 return retval;
}

module_init(testscull_init);
module_exit(testscull_exit);
