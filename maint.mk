all:
.PHONY: all

.PHONY: bootstrap
bootstrap:
	./bootstrap
	./configure

.PHONY: etags
etags:
	find . -type f \( -name '*.cpp' -or -name '*.h' \) -print\
	  | etags - --declarations

.PHONY: list-missing-config-h
list-missing-config-h:
	@for src in `find . -name '*.cpp'`; do\
	  fgrep -q '#include "config.h"' "$$src" || echo "$$src";\
	done

.PHONY: list-unused-sources
list-unused-sources:
	@out="./$@.tmp";\
	cat `find . -name 'Makefile.am'` >"$$out";\
	for src in `find . -name '*.cpp'`; do\
	  egrep -q  "\<`basename $$src`\>" "$$out";\
	done;\
	rm -f "$$out";

.PHONY: list-unused-headers
list-unused-headers:
	@out="./$@.tmp";\
	cat `find . \( -name '*.cpp' -or -name '*.h' \) ` >"$$out";\
	for h in `find . -name '*.h'`; do\
	  fgrep -q  "`basename $$h`" "$$out" || echo "$$h";\
	done;\
	rm -f "$$out";
