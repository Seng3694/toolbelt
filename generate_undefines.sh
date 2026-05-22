#!/bin/sh

# script used to retrieve all the macros used in a template header file and printing corresponding undefines
# usage example: ./generate_undefines.sh src/hashmap.h

# grep -P '(?<=^#(?:ifn?def|define|if))' "$1" | grep -Po '[A-Z0-9_]{4,}' | sort -u | sed -r 's/^/#undef /' 

grep -Po '(?<=(?:#ifn?def |#define |defined\())[A-Z][A-Z0-9_]*'  "$1" | sort -u | sed -r 's/^/#undef /' 

