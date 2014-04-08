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
