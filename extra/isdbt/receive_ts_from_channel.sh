#!/bin/sh

if ! [ -x "$(command -v isdbt-capture)" ]; then
  echo "Error: isdbt-capture is not installed."
  echo "Install it from https://github.com/rafael2k/isdbt-capture"
  exit 1
fi

if [ -z "$1" ]; then
  if [ -z "$2" ]; then
    echo "Error: Wrong arguments."
    echo "Usage: $0 [channel_numer] [output_ts]"
    exit 1
  fi
fi

isdbt-capture -c $1 $2
