#include <stdio.h> // printf
#include <fcntl.h> // open, O_RDONLY
#include <unistd.h> // close

int main(void) {
  int fd = open("/dev/my_device", O_RDONLY);
  if (fd == -1) {
    printf("Failed to open device\n");

    return -1;
  }

  printf("Device opened\n");
  close(fd);

  return 0;
}
