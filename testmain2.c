
#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include"testscull.h"
int main()
{
	int fd;	
	fd=open("/dev/test0",O_RDWR);
	char c[100];
	long num;
	ioctl(fd,SCULL_IOCGETDATA,&c);
	printf("%s\n",c);
	ioctl(fd,SCULL_IOCDELETEPIPE);
	close(fd);
}
