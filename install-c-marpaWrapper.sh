#!/bin/sh
set -ex
wget -O c-marpaWrapper-master.zip -- https://github.com/jddurand/c-marpaWrapper/archive/master.zip
unzip c-marpaWrapper-master.zip
cd c-marpaWrapper-master && cmake . && make && sudo make install
