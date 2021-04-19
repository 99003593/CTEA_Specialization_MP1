#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "uart.h"
#include "led.h"

__sig_atomic_t run = 1;

void sig_handler(int sig_num)
{
    if (sig_num == SIGINT)
    {
        run = 0;
    }
    run = 0;
    printf("\nQuitting...\n");
}

const char * const gpio_file = "";

#define CMP_STR(vstr, cstr) strncmp(vstr, cstr, sizeof(cstr))

int process_cmd(const char * in)
{
    int ret;
    char cmd[16] = { 0 };
    // cppcheck-suppress variableScope
    char opt[16] = { 0 };
    const char *buff = in, *off;

    off = strchr(buff, ' ');
    if (off == NULL) off = strchr(buff, '\0');
    memcpy(cmd, buff, off - buff);
    buff = off + 1;

    if(CMP_STR(cmd, "led") == 0)
    {
        off = strchr(buff, ' ');
        if (off == NULL) off = strchr(buff, '\0');
        memcpy(opt, buff, off - buff);
        buff = off+1;

        if(CMP_STR(opt, "on") == 0)
        {
            led_on();
            ret = 0;
        }
        else if (CMP_STR(opt, "off") == 0)
        {
            led_off();
            ret = 0;
        }
        else if (CMP_STR(opt, "blink") == 0)
        {
            led_blink();
            ret = 0;
        }
        else
        {
            ret = -EINVAL;
        }
    }
    else if (CMP_STR(cmd, "exit") == 0)
    {
        run = 0;
        ret = 0;
    }
    else
    {
        ret = -ENXIO;
    }
    return ret;
}

int main(const int argc, const char ** argv)
{
    int ret = 0, fd = -1;
    FILE *fp;

#define BUFF_SIZE   (128)
    char buff[BUFF_SIZE] = { 0 };

    const char * uart_file = "/dev/ttyO1";

    if (argc > 1)
        uart_file = argv[1];

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    ret = open_uart(uart_file);
    if (ret < 0) {
        perror(uart_file);
        goto main_err;
    }
    fd = ret;

    led_initialize();

    write(fd, "ledctl\r\n", 8);
    fp = fdopen(fd, "rw");

    do
    {
        // TODO : Create a reader thread ?
        write(fd, ">", 1);
        fscanf(fp, "\n%128[^\n]", buff);
        printf("Got '%s'\n", buff);
        ret = process_cmd(buff);
        if( ret < 0)
        {
            errno = ret;
            perror(buff);
            errno = 0;
        }
        fflush(fp);
    } while (run);

    led_deinitialize();

    ret = close_uart(fd);
    if (ret) {
        perror("tcsetattr");
    }

main_err:
    return ret;
}
