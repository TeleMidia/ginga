#!/bin/sh
# Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia
#
# This file is part of Ginga (Ginga-NCL).
#
# Ginga is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# Ginga is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Ginga.  If not, see <https://www.gnu.org/licenses/>.

if ! [ -x "$(command -v demuxfs)" ]; then
  echo "Error: demuxfs is not installed."
  echo "Install it from https://github.com/lucasvr/demuxfs"
  exit 1
fi

if ! [ -x "$(command -v ginga)" ]; then
  echo "Error: ginga is not installed."
  echo "Install it from https://github.com/TeleMidia/ginga"
  exit 1
fi

if [ -z "$1" ]; then
  echo "Error: Wrong arguments."
  echo "Usage: $0 [stream.ts]"
  exit 1
fi

rm -rf mnt
rm -rf app
mkdir mnt
mkdir app
demuxfs -o backend=filesrc -o filesrc=$1 -o fileloop=-1 mnt

cd mnt

echo -n "Waiting for DSM-CC assembly"
while [ ! -d DSM-CC ]; do
  echo -n "."
  sleep 1
done

echo

cp -a DSM-CC/*/* ../app
cd ../app/
ginga main.ncl
cd ../
umount mnt
rmdir mnt
