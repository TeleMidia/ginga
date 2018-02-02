#!/bin/sh

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
