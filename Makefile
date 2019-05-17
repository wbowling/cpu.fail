zombieload_kaslr: zombieload_kaslr.c cacheutils.h
	gcc zombieload_kaslr.c -o zombieload_kaslr


fallout_kaslr: fallout_kaslr.c
	gcc fallout_kaslr.c -o fallout_kaslr -mrtm