#!/bin/sh
set -ex
wget -O c-genericHash-master.zip -- https://github.com/jddurand/c-genericHash/archive/master.zip
unzip c-genericHash-master.zip
cd c-genericHash-master && cmake . && make && sudo make install
