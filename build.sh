#!/usr/bin/env sh

# TODO: Separate mimalloc build

cc='clang++'
cflags='-std=c++20 -fno-strict-aliasing -fno-exceptions -fno-rtti -nodefaultlibs'

incflags='-I. -I./vendor/mimalloc/include'
ldflags='-L. -lc -fuse-ld=mold'
optflags='-O0 -g'
wflags='-Wall -Wextra -Werror=return-type'

Run(){ echo "-> $@"; $@; }

mode="$1"
set -eu

case $mode in
	"release") optflags='-O2' ;;
	*) ;;
esac

Run $cc $cflags $incflags $optflags $wflags main.cpp -o main.exe $ldflags

