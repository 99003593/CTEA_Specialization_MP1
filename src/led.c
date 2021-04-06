#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#include "led.h"

FILE *export_gpio = NULL;
FILE *unexport_gpio = NULL;
FILE *io_direction = NULL;
FILE *io_value = NULL;
char str1[] = "0";
char str2[] = "1";
char str3[] = "out";
char str4[] = "49";
// exporting GPIO49

void led_initialize()
{
  export_gpio = fopen("/sys/class/gpio/export", "w");
  fwrite(str4, 1, sizeof(str4), export_gpio);
  fclose(export_gpio);
  // changing the direction to out
  io_direction = fopen("/sys/class/gpio/gpio49/direction", "w");
  fwrite(str3, 1, sizeof(str3), io_direction);
  fclose(io_direction);
  //delay(5000);
}

void led_on()
{
  // setting the value to HIGH
  io_value = fopen("/sys/class/gpio/gpio49/value", "w");
  fwrite(str2, 1, sizeof(str2), io_value);
  fclose(io_value);
}

void led_off()
{
  // setting the value to LOW
  io_value = fopen("/sys/class/gpio/gpio49/value", "w");
  fwrite(str1, 1, sizeof(str1), io_value);
  fclose(io_value);
}

void led_deinitialize()
{
  unexport_gpio = fopen("/sys/class/gpio/unexport", "w");
  fwrite(str4, 1, sizeof(str4), unexport_gpio);
  fclose(unexport_gpio);
}

void led_blink()
{
  for (int i = 0; i < 10; i++)
  {
    led_on();
    usleep(200000);
    led_off();
    usleep(200000);
  }
}

// int main()
// {
//   led_initialize();
//   led_blink();
//   //led_deinitialize();
//   return 0;
// }
