#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUF_LEN		1024
#define DEV_NAME	"/dev/pr0gr4m-blkdev0"
 
int main()
{
    static char buf[1024];
    int fd;
	off_t off;

    if ((fd = open(DEV_NAME, O_RDWR)) < 0) {
		perror("open error");
    }
 
    if (write(fd, "hello", strlen("hello")) < 0) {
		perror("write error");
    }
 
	off = lseek(fd, 0, SEEK_SET);
    if (read(fd, buf, strlen("hello")) < 0) {
		perror("read error");
    } else {
        printf("%s\n", buf);
    }

	off = lseek(fd, 1024, SEEK_SET);
	if (write(fd, "pr0gr4m", strlen("pr0gr4m")) < 0) {
		perror("write error");
    }
 
	off = lseek(fd, 1024, SEEK_SET);
    if (read(fd, buf, strlen("pr0gr4m")) < 0) {
		perror("read error");
    } else {
        printf("%s\n", buf);
    }
 
    if (close(fd) != 0) {
		perror("close error");
    }

    return 0;
}
