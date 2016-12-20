for src in `find . -name '*.cpp'`; do
  found=false
  for mk in `find . -name 'Makefile.am'`; do
    if grep -q  "`basename $src`" "$mk"; then
      found=true
    fi
  done
  ! $found && echo "$src"
done
