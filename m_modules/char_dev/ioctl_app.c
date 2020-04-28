#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include "ioctl.h"

int main(int argc, char *argv[])
{
	int fd;
	struct ioctl_info set_info, get_info;

	set_info.size = 100;
	strncpy(set_info.buf, "pr0gr4m.0xF", 11);

	if ((fd = open("/dev/chardev0", O_RDWR)) < 0)
		perror("open error");

	if (ioctl(fd, IOCTL_WRITE, &set_info) < 0)
		perror("IOCTL_WRITE error");

	if (ioctl(fd, IOCTL_READ, &get_info) < 0)
		perror("IOCTL_READ error");

	printf("[1] get_info : [%ld/%s]\n", get_info.size, get_info.buf);

	if (ioctl(fd, IOCTL_STATUS) < 0)
		perror("IOCTL_STATUS error");

	if (ioctl(fd, IOCTL_RW, &get_info) < 0)
		perror("IOCTL_RW error");

	printf("[2] get_info : [%ld/%s]\n", get_info.size, get_info.buf);

	if (close(fd) != 0)
		perror("close error");

	return 0;
}
