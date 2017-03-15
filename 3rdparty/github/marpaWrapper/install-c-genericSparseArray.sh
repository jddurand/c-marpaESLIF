#!/bin/sh
set -ex
wget -O c-genericSparseArray-master.zip -- https://github.com/jddurand/c-genericSparseArray/archive/master.zip
unzip c-genericSparseArray-master.zip
cd c-genericSparseArray-master && cmake . && make && sudo make install
