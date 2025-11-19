#!/bin/sh

# script used to retrieve all the macros used in a header file and printing corresponding undefines
# usage example: ./generate_undefines.sh src/arena.h
grep -Po '(?<=^#(?:ifn?def|define) )([A-Z_][A-Z0-9_]*)' "$1" | sort -u | sed -r 's/^/#undef /'

