#!/bin/zsh

pushd /home/jdurand/git/c-marpaESLIF
make marpaESLIF
popd

cc -g -I include -I output/include -I 3rdparty/github/genericLogger/include -I 3rdparty/github/genericLogger/output/include -o /tmp/jdd doc/Tutorial/Calculator/Calculator.c -L output/lib -lmarpaESLIF -L 3rdparty/github/genericLogger/output/lib -lgenericLogger -lm && echo "5 / ( 2 * 3)" | LD_LIBRARY_PATH=`pwd`/output/lib:`pwd`/3rdparty/github/genericLogger/output/lib /tmp/jdd
