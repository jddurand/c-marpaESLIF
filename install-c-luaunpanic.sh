#!/bin/sh
set -ex
wget -O c-luaunpanic-master.zip -- https://github.com/jddurand/c-luaunpanic/archive/master.zip
unzip c-luaunpanic-master.zip
cd c-luaunpanic-master && cmake . && make && sudo make install
