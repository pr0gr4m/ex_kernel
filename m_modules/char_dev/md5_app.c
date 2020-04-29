#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEV_NAME	"/dev/md5"
#define MAX_LEN		16

static void hexdump(unsigned char *buf, unsigned int len)
{
	while (len--)
		printf("%02x", *buf++);
	printf("\n");
}

int main(int argc, char *argv[])
{
	int dev;
	char buf[MAX_LEN];

	if (argc < 2) {
		printf("Usage: %s data\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	memset(buf, 0, MAX_LEN);
	puts("Device driver test");

	if ((dev = open(DEV_NAME, O_RDWR)) < 0) {
		perror("open error");
		exit(EXIT_FAILURE);
	}
	
	write(dev, argv[1], strlen(argv[1]));
	read(dev, buf, MAX_LEN);

	hexdump(buf, 16);

	close(dev);
	return 0;
}
