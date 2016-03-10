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
	ioctl(fd,SCULL_IOC_CREAT_PIPE);
	ioctl(fd,SCULL_IOC_SET_DATA,&c);
	close(fd);
}
