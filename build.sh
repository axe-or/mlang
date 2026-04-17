#!/usr/bin/env sh

cc='clang++'
cflags='-std=c++20 -fno-strict-aliasing -fno-exceptions -fno-rtti'
optflags='-O0 -g'
warnings='-Wall -Wextra -Werror=return-type'

Run(){ echo "-> $@"; $@; }

mode="$1"
set -eu

case $mode in
	"release") optflags='-O2' ;;
	*) ;;
esac

Run $cc $cflags $optflags $warnings main.cpp -o main.exe

