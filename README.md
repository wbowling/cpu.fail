# POCs for https://cpu.fail/

Sample POCs for breaking KASLR with the latest round of CPU bugs

[![asciicast](https://asciinema.org/a/246920.svg?speed=2)](https://asciinema.org/a/246920?speed=2)

## zombieload_kaslr.c

Based on https://github.com/IAIK/ZombieLoad, using [ZombieLoad](https://zombieloadattack.com/zombieload.pdf) to break KASLR.

## fallout_kaslr.c

Based on https://github.com/IAIK/ZombieLoad, using [Fallout](https://mdsattacks.com/files/fallout.pdf) to break KASLR (requires TSX).
