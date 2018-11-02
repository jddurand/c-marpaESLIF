#!/bin/sh
set -ex
wget -O c-lua534-master.zip -- https://github.com/jddurand/c-lua534/archive/master.zip
unzip c-lua534-master.zip
cd c-lua534-master && cmake . && make && sudo make install
