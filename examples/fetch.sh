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
URI_PATCHS=https://raw.githubusercontent.com/andrelbd1/ginga_aplications/master/
# declare -A PATCHS
PATCHS="comerciais-proview/main.ncl comerciais-proview/connbase.ncl formula1/formula1.ncl hackerteen/hackerteen.lua hackerteen/hackerteen.ncl matrix/matrix.ncl matrixEstresse30.conn orchestra/orchestra.ncl pixelation/pixelation.ncl proderj/proderj.ncl roteiro-do-dia/connectorBase.ncl roteiro-do-dia/descriptorBase.ncl roteiro-do-dia/main.ncl roteiro-dlso-dia/regionBase.ncl velha/connbase.ncl velha/main.ncl vivamais-alim/vivamais.ncl vivamais-alim/vivamais30.conn vivamais-peso/peso.deps vivamais-peso/peso.lua"

for app in $APPS; do
  wget --show-progress -q -r -N -np -nH --cut-dirs 3 \
    -R "index.html*" "$URI/$app/"
done

git clone https://github.com/robertogerson/nclua-tutorial.git

for patch in $PATCHS; do
  rm $patch
  wget $URI_PATCHS/$patch -P $(dirname $patch)/
done
