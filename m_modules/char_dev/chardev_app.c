#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUF_LEN		1024
#define TEXT_LEN	5
 
int main()
{
    static char buf[1024];
    int fd;
 
    if ((fd = open("/dev/chardev0", O_RDWR)) < 0) {
		perror("open error");
    }
 
    if (write(fd, "hello", TEXT_LEN) < 0) {
		perror("write error");
    }
 
    if (read(fd, buf, TEXT_LEN) < 0) {
		perror("read error");
    } else {
        printf("%s\n", buf);
    }
 
    if (close(fd) != 0) {
		perror("close error");
    }

    return 0;
}
