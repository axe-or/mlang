#!/usr/bin/env sh
cc=clang
ar=ar

set -eu

Run(){ echo "-> $@"; $@; }

BuildMimalloc(){
	cflags="-std=c11 -Os -fPIC -fno-strict-aliasing"
	back="$(pwd)"

	cd vendor/mimalloc
	Run $cc $cflags -c src/static.c -o mimalloc.o -I./include
	Run ar rcs libmimalloc.a mimalloc.o
	Run cp libmimalloc.a "$back/libmimalloc.a"

	cd "$back"
}

echo 'Building mimalloc'
BuildMimalloc
