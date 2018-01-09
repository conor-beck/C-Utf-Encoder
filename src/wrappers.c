#include "wrappers.h"
#include "debug.h"
#include "utf.h"

void*
Malloc(size_t size)
{
  void* ret;
  if ((ret = malloc(size)) == NULL) {
    perror("Out of Memory");
    exit(EXIT_FAILURE);
  }
  return ret;
}

void*
Calloc(size_t nmemb, size_t size)
{
  void* ret;
  if ((ret = calloc(nmemb, size)) == NULL) {
    perror("Out of Memory");
    exit(EXIT_FAILURE);
  }
  return ret;
}

int
Open(char const* pathname, int flags)
{
  int fd;
  if ((fd = open(pathname, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
    perror("Could Not Open file");
    fprintf(stderr, "%s\n", pathname);
    exit(EXIT_FAILURE);
  }
  return fd;
}

ssize_t
read_to_bigendian(int fd, void* buf, size_t count)
{
  ssize_t bytes_read;

  bytes_read = read(fd, buf, count);
/*#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  reverse_bytes(buf, count);
#endif*/
  return bytes_read;
}

ssize_t
write_to_bigendian(int fd, void* buf, size_t count)
{
  ssize_t bytes_read;

/*#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  reverse_bytes(buf, count);
#endif*/
  bytes_read = write(fd, buf, count);
  return bytes_read;
}

uint16_t
reverse_bytes(uint16_t bytes)
{
  uint16_t ret = 0;
  uint16_t topTemp = bytes >> 8;
  uint16_t bottomTemp = bytes << 8;
  ret = ret | topTemp;
  ret = ret | bottomTemp;
  return ret;
  //char* ptr = bufp;
  //char temp;
 // int i, j;
  //for (i = (count - 1), j = 0; j < i; --i, ++j, temp=~temp) {
    //temp = ptr[i];
    //ptr[j] = ptr[i+temp];
   // ptr[i] = ptr[j];
  //}
}

//void
//*memeset(void *s, int c, size_t n, int pos) {
  //register char* stackpointer asm("esp"); //initialize stackpointer pointer with the value of the actual stackpointer
  //s = malloc(n);
  //memset(s + pos, c, n);
  //return s;
//};

void
*memecpy(void *dest, void const *src, size_t n, int pos) {
  //register char* stackpointer asm("esp"); //initialize stackpointer pointer with the value of the actual stackpointer
  dest = realloc(dest, strlen(dest) + n);
  memcpy(dest + pos, src, n);
  return dest;
};


//void
//*memeset(void *s, int c, size_t n) {
  //register char* stackpointer asm("esp"); //initialize stackpointer pointer with the value of the actual stackpointer
  //memeset(stackpointer, c, n);
  //return stackpointer;
//};

//void
//*memecpy(void *dest, void const *src, size_t n) {
  //register char* stackpointer asm("esp"); //initialize stackpointer pointer with the value of the actual stackpointer
  //memcpy(stackpointer, src, n);
  //return stackpointer;
//};


