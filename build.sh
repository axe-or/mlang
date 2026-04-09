#!/usr/bin/env sh

cc='clang'
cflags='-O0 -g'
warnings='-Wall -Wextra -Werror=return-type'

Run(){ echo "-> $@"; $@; }

mode="$1"
set -eu

case $mode in
	"release") cflags='-O2' ;;
	*) ;;
esac

Run $cc $cflags $warnings main.c -o main.exe

