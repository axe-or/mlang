#!/usr/bin/env sh


cc='g++'
cflags='-std=c++20 -fno-strict-aliasing -fno-exceptions -fno-rtti -nodefaultlibs'

incflags='-I.'
ldflags='-L. -L/usr/local/lib -lc -fuse-ld=mold'
optflags='-O0 -g'
wflags='-Wall -Wextra -Werror=return-type'

Run(){ echo "-> $@"; $@; }

set -eu

mode="${1:?Usage: build.sh [debug|release|test] <options...>}"
shift

case $mode in
	"debug")   optflags='-O0 -g';;
	"release") optflags='-O2' ;;
	"test")    optflags='-Os -g';;
	*) echo "Unknown mode: $mode"; exit 1 ;;
esac

use_mimalloc=1
use_static=0

for arg in "$@"; do
	case $arg in
		"-no-mimalloc") use_mimalloc=0 ;;
		"-static") use_static=1 ;;
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

if [ "$mode" = "test" ]; then
	Run $cc $cflags $incflags $optflags $wflags testmain.cpp -o test.exe $ldflags
else
	Run $cc $cflags $incflags $optflags $wflags main.cpp -o main.exe $ldflags
fi

