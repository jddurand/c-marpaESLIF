#!/bin/sh
set -ex
wget -O c-tconv-master.zip -- https://github.com/jddurand/c-tconv/archive/master.zip
unzip c-tconv-master.zip
cd c-tconv-master && cmake . && make && sudo make install
