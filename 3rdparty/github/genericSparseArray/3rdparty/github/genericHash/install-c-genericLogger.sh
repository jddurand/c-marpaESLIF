#!/bin/sh
set -ex
wget -O c-genericLogger-master.zip -- https://github.com/jddurand/c-genericLogger/archive/master.zip
unzip c-genericLogger-master.zip
cd c-genericLogger-master && cmake . && make && sudo make install
