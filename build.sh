#!/usr/bin/env sh


cc='clang++'
cflags='-std=c++20 -fno-strict-aliasing -fno-exceptions -fno-rtti -nodefaultlibs'

incflags='-I.'
ldflags='-L. -lc -fuse-ld=mold'
optflags='-O0 -g'
wflags='-Wall -Wextra -Werror=return-type'

Run(){ echo "-> $@"; $@; }

set -eu

mode="${1:?Usage: build.sh [debug|release] <options...>}"
shift

case $mode in
	"debug")   optflags='-O0 -g' ;;
	"release") optflags='-O2' ;;
	*) echo "Unknown mode: $mode"; exit 1 ;;
esac

use_mimalloc=0
use_static=0

for arg in "$@"; do
	case $arg in
		"-mimalloc") use_mimalloc=1 ;;
		"-static")   use_static=1 ;;
		*) echo "Unknown option: $arg"; exit 1 ;;
	esac
done

if [ $use_mimalloc -eq 1 ]; then
	incflags="$incflags -I./vendor/mimalloc/include"
	ldflags="$ldflags -lmimalloc"
	cflags="$cflags -DBUILD_USE_MIMALLOC"
fi

if [ $use_static -eq 1 ]; then
	ldflags="$ldflags -static"
fi

Run $cc $cflags $incflags $optflags $wflags main.cpp -o main.exe $ldflags

