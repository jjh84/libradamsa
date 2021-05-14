#include <radamsa.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

size_t filesize(char* filename) {
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

#define BUFSIZE 1024*1024

void fail(char *why) {
   printf("fail: %s\n", why);
   exit(1);
}

void write_output(char *dpath, char *data, size_t len, int num) {
   char path[256];
   int fd;
   int wrote;
   sprintf(path, "%s/%d", dpath, num); 
   fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
   printf("Opened %s -> %d\n", path, fd);
   if (fd < 0) {
      fail("failed to open output file");
   }
   wrote = write(fd, data, len);
   printf("wrote %d of %zu bytes\n", wrote, len);
   if (wrote != len) {
      fail("failed to write all of output at once");
   }
   close(fd);
   printf("Wrote %zu bytes to %s\n", len, path);
}

// prog <input> <outpath> <count>
int main(int nargs, char **argv) {
   char *spath = argv[1];
   int fd = open(spath, O_RDONLY, 0);
   int rd = open("/dev/urandom", O_RDONLY, 0);
   size_t len;
   char *input;
   char *output;
   int count = atoi(argv[3]);
   if (fd < 0) {
      printf("cannot open input file(%s)", spath);
      return -1;
   }
   if ( rd < 0) {
      printf("cannot open urandom.\n");
      return -1;
   }
   len = filesize(spath);
   input = malloc(len);
   output = malloc(BUFSIZE);
   if (!input || !output) {
      fail("failed to allocate buffers\n");
   }
   init();
   if (len != read(fd, input, len)) {
      fail("failed to read the entire sample at once");
   }

   while(count--) {
      size_t n;
      int seed;
      read(rd, &seed, sizeof(seed));
      n = radamsa((uint8_t *) input, len, (uint8_t *) output, BUFSIZE, seed);
      write_output(argv[2], output, n, count);
      printf("Fuzzed %zu -> %zu bytes\n", len, n);
   }
   free(output);
   free(input);
   close(fd);
   close(rd);
   return 0;
}

