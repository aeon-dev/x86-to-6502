#! /bin/bash

PARAMS=""

while (( "$#" )); do
  case "$1" in
    -g)
      shift
      ;;
    -S)
      shift
      ;;
    -O2)
      shift
      ;;
    -o)
      OUTPUT_FILE=$2
      shift 2
      ;;
    -*|--*=) # unsupported flags
      echo "Error: Unsupported flag $1" >&2
      exit 1
      ;;
    *) # preserve positional arguments
      INPUT_FILE="$1"
      shift
      ;;
  esac
done

clang++ -std=c++17 -c -g -O2 -o- -Wall -Wextra -m32 -march=i386 -ggdb -S $INPUT_FILE > $INPUT_FILE.x86.asm
cat $INPUT_FILE.x86.asm | /home/robindegen/x86-to-6502/build/x86-to-6502 > $INPUT_FILE.6502.asm
cat $INPUT_FILE.6502.asm > $OUTPUT_FILE

