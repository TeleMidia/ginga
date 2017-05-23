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
