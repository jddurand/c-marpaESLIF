#!/bin/zsh

pushd ../../..
make marpaESLIF
popd

cc -g -I ../../../include -I ../../../output/include -I ../../../3rdparty/github/genericLogger/include -I ../../../3rdparty/github/genericLogger/output/include -o ./Calculator ./Calculator.c -L ../../../output/lib -lmarpaESLIF -L ../../../3rdparty/github/genericLogger/output/lib -lgenericLogger -lm && echo "5 / ( 2 * 3)" | LD_LIBRARY_PATH=../../../output/lib:../../../3rdparty/github/genericLogger/output/lib ./Calculator
