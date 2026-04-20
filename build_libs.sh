#!/usr/bin/env sh
cc=clang
ar=ar

set -eu

Run(){ echo "-> $@"; $@; }

BuildMimalloc(){
	cflags="-std=c11 -Os -fPIC -fno-strict-aliasing"
	back="$(pwd)"
	cd vendor/mimalloc

	Run mkdir -p build
	Run $cc $cflags -c src/static.c -o build/mimalloc.o -I./include
	Run ar rcs build/libmimalloc.a build/mimalloc.o
	Run cp build/libmimalloc.a "$back/libmimalloc.a"

	cd "$back"
}

echo 'Building mimalloc'
BuildMimalloc
