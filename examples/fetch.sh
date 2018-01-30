# fetch.sh -- Fetch extra examples.
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

APPS="$APPS africa2010"
APPS="$APPS comerciais-proview"
APPS="$APPS formula1"
APPS="$APPS hackerteen"
APPS="$APPS matrix"
APPS="$APPS orchestra"
APPS="$APPS pixelation"
APPS="$APPS proderj"
APPS="$APPS velha"
APPS="$APPS velha"
APPS="$APPS vivamais-peso"

URI=http://www.telemidia.puc-rio.br/~gflima/misc/nclapps
for app in $APPS; do
  wget --show-progress -q -r -N -np -nH --cut-dirs 3\
       -R "index.html*" "$URI/$app/"
done

git clone https://github.com/robertogerson/nclua-tutorial.git

