#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include"testscull.h"
main()
{
	int 	fd;
	long	num;
	fd=open("/dev/test0",O_RDWR);
	char c[100];	
	scanf("%s",c);
	ioctl(fd,SCULL_IOCCREATPIPE);
	ioctl(fd,SCULL_IOCSETDATA,&c);
	close(fd);
}
