#!/bin/bash

sudo service lirc start


sudo killall irexec
sudo irexec &

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/../../
./src-gui/gingagui -b
