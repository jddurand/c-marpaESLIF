#!/bin/sh
set -ex
wget -O c-genericStack-master.zip -- https://github.com/jddurand/c-genericStack/archive/master.zip
unzip c-genericStack-master.zip
cd c-genericStack-master && cmake . && make && sudo make install
