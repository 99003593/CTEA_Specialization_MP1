#include "uart.h"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

struct termios old_tty_opts = { 0 };

int open_uart(char const *file)
{
    int ret = 0, fd;
    struct termios tty_opts = { 0 };
    struct termios new_tty_opts = { 0 };

    ret = open(file, O_RDWR | O_NOCTTY);
    if (ret < 0)
    {
        // perror("open");
        goto open_err;
    }
    fd = ret;

    // Save previous state
    tcgetattr(fd, &old_tty_opts);

    // Configure
    tty_opts.c_cflag = B115200 | CS8 | CREAD | CLOCAL | HUPCL;
    tty_opts.c_iflag = ICRNL | IMAXBEL | IUTF8;
    tty_opts.c_oflag = ONLCR | OPOST;
    tty_opts.c_lflag = ECHO | ECHOCTL | ECHOE | ECHOK | ECHOKE | ECHOPRT | ICANON | IEXTEN | ISIG;  // canonical input

    memcpy(&tty_opts.c_cc, &old_tty_opts.c_cc, sizeof(sizeof(tty_opts.c_cc)));

    tcflush(fd, TCIFLUSH);
    ret = tcsetattr(fd, TCSANOW, &tty_opts);
    if (ret) {
        // perror("tcsetattr");
        goto open_err_attr;
    }
    tcgetattr(fd, &new_tty_opts);


    ret = fd;
    goto open_okay;

open_err_attr:
    close(fd);
open_err:
open_okay:
    return ret;
}

void close_uart(int fd)
{
    int ret = 0;
    ret = tcsetattr(fd, TCSAFLUSH, &old_tty_opts);
    if (ret) {
        perror("tcsetattr");
    }
    close(fd);
}
