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
APPS="$APPS orchestra"
APPS="$APPS pixelation"
APPS="$APPS proderj"
APPS="$APPS velha"
APPS="$APPS velha"
APPS="$APPS vivamais-peso"

URI=http://www.telemidia.puc-rio.br/~gflima/misc/nclapps
URI_PATCHS=https://raw.githubusercontent.com/andrelbd1/ginga_aplications/master/
PATCHS="comerciais-proview/main.ncl comerciais-proview/connbase.ncl comerciais-proview/app_cover.png comerciais-proview/app_thumbnail.png formula1/formula1.ncl formula1/app_cover.png formula1/app_thumbnail.png hackerteen/hackerteen.lua hackerteen/hackerteen.ncl hackerteen/app_cover.png hackerteen/app_thumbnail.png orchestra/orchestra.ncl orchestra/app_cover.png orchestra/app_thumbnail.png pixelation/pixelation.ncl pixelation/app_cover.png pixelation/app_thumbnail.png proderj/proderj.ncl proderj/app_cover.png proderj/app_thumbnail.png roteiro-do-dia/connectorBase.ncl roteiro-do-dia/descriptorBase.ncl roteiro-do-dia/main.ncl roteiro-dlso-dia/regionBase.ncl roteiro-do-dia/app_cover.png roteiro-do-dia/app_thumbnail.png velha/connbase.ncl velha/main.ncl velha/app_cover.png velha/app_thumbnail.png vivamais-alim/vivamais.ncl vivamais-alim/vivamais30.conn vivamais-alim/app_cover.png vivamais-alim/app_thumbnail.png vivamais-peso/peso.deps vivamais-peso/peso.lua vivamais-peso/app_cover.png vivamais-peso/app_thumbnail.png"

for app in $APPS; do
  wget --show-progress -q -r -N -np -nH --cut-dirs 3 \
    -R "index.html*" "$URI/$app/"
done

git clone https://github.com/robertogerson/nclua-tutorial.git

for patch in $PATCHS; do
  rm $patch
  wget $URI_PATCHS/$patch -P $(dirname $patch)/
done
