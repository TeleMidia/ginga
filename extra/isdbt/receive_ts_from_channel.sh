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

isdbt-capture -c $1 -o $2
