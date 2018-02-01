# maint.mk -- Maintainer's makefile.
# Copyright (C) 2014-2018 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

ME:= $(firstword $(MAKEFILE_LIST))

all: usage
.PHONY: all

# Use cfg.mk to override defaults.
include cfg.mk
BOOTSTRAP?= bootstrap
GIT?= git
GREP?= grep
PERL?= perl
WGET?= wget
build_aux?= build-aux
configure_ac?= configure.ac
debian?= contrib/debian

# Verbosity.
DEFAULT_VERBOSITY?= 0

V_P= $(_v_P_$(V))
_v_P_= $(_v_P_$(DEFAULT_VERBOSITY))
_v_P_0= false
_v_P_1= :

V_GEN= $(_v_GEN_$(V))
_v_GEN_= $(_v_GEN_$(DEFAULT_VERBOSITY))
_v_GEN_0= @echo "  GEN     " $@;
_v_GEN_1=

V_at= $(_v_at_$(V))
_v_at_= $(_v_at_$(DEFAULT_VERBOSITY))
_v_at_0= @
_v_at_1=

# Fetch remote file using Perl.
# usage: FETCH [-dir=OUTPUT-DIR] URI...
perl_FETCH_sub=\
  use File::Basename;\
  sub FETCH {\
    my ($$uri, $$dest) = @_;\
    $$dest = ($$dest || ".") . "/" . basename ($$uri);\
    my $$v = ("$(V_P)" eq ":") ? "-v" : "-nv";\
    `$(WGET) "$$v" -O "$$dest" "$$uri"` and die;\
  }\
  $(NULL)

perl_FETCH=\
  $(perl_FETCH_sub)\
  foreach my $$uri (@ARGV) {\
    (defined $$dir) ? FETCH "$$uri", "$$dir" : FETCH "$$uri";\
  };\
  $(NULL)

FETCH= $(PERL) -swe '$(perl_FETCH)' --


# Prints usage message and exits.
perl_usage=\
  BEGIN {\
    $$/ = "";\
  }\
  /\#\s([^\n]+)\n(\.PHONY:|SC_RULES\+=)\s([\w-]+)\n/ and do {\
    my $$tgt = $$3;\
    my $$doc = lc $$1;\
    $$doc =~ s/\.//;\
    printf ("  %-20s  %s\n", $$tgt, $$doc);\
  };\
  END { print ""; }\
  $(NULL)

.PHONY: usage
usage:
	@echo "Usage: $(MAKE) -f $(ME) [V=1] TARGET"
	@echo "Maintainer's makefile; the following targets are supported:"
	@$(PERL) -wnle '$(perl_usage)' $(MAKEFILE_LIST) | sort


perl_bootstrap_utilm4_options=\
  /AU_ARG_ENABLE_ANSI/ and print " --enable-ansi";\
  /AU_ARG_ENABLE_DEBUG/ and print " --enable-debug";\
  /AU_ARG_ENABLE_WARNINGS/ and print " --enable-warnings";\
  $(NULL)

bootstrap_default_options=\
  $(shell\
    $(PERL) -0777 -wne '$(perl_bootstrap_utilm4_options)' $(configure_ac))\
  $(NULL)

# Bootstraps project for debugging.
.PHONY: bootstrap
bootstrap:
	$(V_at)./$(BOOTSTRAP)
	$(V_at)./configure $(bootstrap_default_options) $(BOOTSTRAP_EXTRA)


perl_dist_get_package=\
  /^AU_GIT_VERSION_GEN\(\[(.*?)\]\)/ and print $$1;\
  $(NULL)

dist_get_package=\
  $(shell\
    if test -f $(build_aux)/git-version-gen\
      && $(GREP) -q 'AU_GIT_VERSION_GEN' $(configure_ac); then\
        $(PERL) -wnle '$(perl_dist_get_package)' $(configure_ac);\
    elif test -f Makefile; then\
      $(PERL) -wnle '/^PACKAGE\s*=\s*(.*)$$/ and print $$1' Makefile;\
    else\
      echo UNKNOWN;\
    fi;)
  $(NULL)

dist_get_version=\
  $(shell\
    if test -f $(build_aux)/git-version-gen\
      && $(GREP) -q AU_GIT_VERSION_GEN $(configure_ac); then\
      $(SHELL) $(build_aux)/git-version-gen .;\
    elif test -f Makefile; then\
      $(PERL) -wnle '/^VERSION\s*=\s*(.*)$$/ and print $$1;' Makefile;\
    else\
      echo UNKNOWN;\
    fi;)\
  $(NULL)

PACKAGE= $(call dist_get_package)
VERSION= $(call dist_get_version)

# Outputs project name version.
.PHONY: dist-get-version
dist-get-version:
	@echo $(PACKAGE) $(VERSION)


# Makes Debian binaries.
.PHONY: dist-deb
dist-deb:
	$(V_at)\
	if ! test -d "$(debian)"; then\
	  echo "Skipping target $@: cannot access '$(debian)'";\
	  exit 0;\
	fi;\
	set -e;\
	$(MAKE) dist;\
	test -f Makefile || exit 1;\
	version=$(VERSION);\
	package=$(PACKAGE)-$$version;\
	rm -rf ./$@ && mkdir -p ./$@;\
	mv $$package.tar.xz ./$@/$(PACKAGE)_$$version.orig.tar.xz;\
	(cd ./$@ && tar -xf $(PACKAGE)_$$version.orig.tar.xz);\
	cp -r ./contrib/debian ./$@/$(PACKAGE)-$$version;\
	(cd ./$@/$(PACKAGE)-$$version && debuild -us -uc);\
	rm -rf ./$@/$(PACKAGE)-$$version


# Makes mingw32 binaries.
.PHONY: dist-win32
dist-win32:
	@$(V_at)\
	set -e;\
	test -f Makefile || exit 1;\
	version=$(VERSION);\
	package=$(PACKAGE)-$$version;\
	rm -rf ./$@;\
	./configure --prefix=$(PWD)/$$package-win32;\
	make install;\
	cp ./AUTHORS ./$$package-win32/AUTHORS.txt;\
	cp ./COPYING ./$$package-win32/COPYING.txt;\
	if test -f ./README; then\
	  cp ./README ./$$package-win32/README.txt;\
	fi;\
	find ./$$package-win32 -name '*.la' -delete;\
	zip -r ./$$package-win32.zip ./$$package-win32;\
	rm -rf ./$$package-win32;\
	make distclean


REMOTE_FILES?=
REMOTE_FILES+=\
  $(REMOTE_SCRIPTS)\
  $(build_aux)/manywarnings.m4\
  $(build_aux)/perl.m4\
  $(build_aux)/visibility.m4\
  $(build_aux)/warnings.m4\
  $(NULL)

REMOTE_SCRIPTS?=
REMOTE_SCRIPTS+=\
  $(build_aux)/git-version-gen\
  $(build_aux)/gitlog-to-changelog\
  $(build_aux)/useless-if-before-free\
  $(NULL)

gnulib:= http://git.savannah.gnu.org/cgit/gnulib.git/plain
perl_fetch_remote_utilm4=\
  $(perl_FETCH_sub)\
  /AU_ARG_ENABLE_WARNINGS/ and do {\
    FETCH "$(gnulib)/m4/manywarnings.m4", "$(build_aux)";\
    FETCH "$(gnulib)/m4/warnings.m4", "$(build_aux)";\
  };\
  /AU_GITLOG_TO_CHANGELOG/ and do {\
    FETCH "$(gnulib)/build-aux/gitlog-to-changelog", "$(build_aux)";\
  };\
  /AU_GIT_VERSION_GEN/ and do {\
    FETCH "$(gnulib)/build-aux/git-version-gen", "$(build_aux)";\
  };\
  /AU_PROG_CC_VISIBILITY/ and do {\
    FETCH "$(gnulib)/m4/visibility.m4", "$(build_aux)";\
  };\
  /AU_PROG_UNIX_TOOLS/ and do {\
    FETCH "$(gnulib)/m4/perl.m4", "$(build_aux)";\
  };\
  $(NULL)

# Fetches remote files.
.PHONY: fetch-remote
fetch-remote:
	@case " $(REMOTE_FILES) " in \
	*\ $(ME)\ *)\
	  $(V_P) && echo "chmod +w $(ME)";\
	  chmod +w $(ME);\
	  $(V_P) && echo "rm -f $(filter-out $(ME), $(REMOTE_FILES))";\
	  rm -f $(filter-out $(ME), $(REMOTE_FILES));;\
	*)\
	  $(V_P) && echo "rm -f $(REMOTE_FILES)";\
	  rm -f $(REMOTE_FILES);;\
	esac
	@if test -f $(configure_ac); then\
	  $(PERL) -0777 -wne '$(perl_fetch_remote_utilm4)' $(configure_ac);\
	fi;
	@$(FETCH) -dir=build-aux '$(gnulib)/build-aux/useless-if-before-free'
	$(V_at)$(MAKE) -f $(ME) fetch-remote-local
	@for file in $(REMOTE_SCRIPTS); do\
	  if test -f "$$file"; then\
	    $(V_P) && echo "chmod +x $$file";\
	    chmod +x "$$file";\
	  fi;\
	done
	@for file in $(REMOTE_FILES); do\
	  if test -f "$$file"; then\
	    $(V_P) && echo "chmod a-w $$file";\
	    chmod a-w "$$file";\
	  fi;\
	done

.PHONY: fetch-remote-local
fetch-remote-local:

# Cleans remote files.
.PHONY: clean-remote
clean-remote:
	test -n "$(filter-out $(ME), $(REMOTE_FILES))"\
	  && rm -f $(filter-out $(ME), $(REMOTE_FILES))


# Lists project files.
vc_list_exclude= $(filter-out $(2), $(1))
VC_LIST_AC:= $(shell $(GIT) ls-files '*.ac')
VC_LIST_ALL:= $(shell $(GIT) ls-files | $(PERL) -wnle '-T and print;')
VC_LIST_AM:= $(shell $(GIT) ls-files '*.am' 'build-aux/Makefile.am.*')
VC_LIST_C:= $(shell $(GIT) ls-files '*.[ch]')
VC_LIST_LUA:= $(shell $(GIT) ls-files '*.lua')
VC_LIST_MK:= $(shell $(GIT) ls-files '*.mk')
VC_LIST_SH:= $(shell $(GIT) ls-files '*.sh')


perl_list_gcc_extra_warnings:=\
  /\s\s(\-W[\w-]+)\s/ and print $$1;\
  $(NULL)

list_gcc_extra_warnings:=\
  $(CC) --help=warnings | $(PERL) -nle '$(perl_list_gcc_extra_warnings)'\
  $(NULL)

# Lists extra GCC warnings.
.PHONY: gcc-extra-warnings
gcc-extra-warnings:
	@if ! test -f $(build_aux)/manywarnings.m4; then\
	  echo "Skipping target $@: cannot access '$(build_aux)/manywarnings.m4'";\
	  exit 0;\
	fi;\
	for w in `$(list_gcc_extra_warnings)`; do\
	  $(GREP) -q -e "$$w" $(configure_ac) $(build_aux)/manywarnings.m4\
	  || echo "$$w";\
	done


INDENT?= indent
INDENT_TYPES?=
INDENT_OPTIONS?=\
  --else-endif-column0\
  --gnu-style\
  --indent-label-1\
  --leave-preprocessor-space\
  --no-tabs\
  -l76\
  $(NULL)

INDENT_INCLUDE?=
INDENT_EXCLUDE?=
INDENT_EXCLUDE+= $(REMOTE_FILES)

INDENT_JOIN_EMPTY_LINES_EXCLUDE?=
INDENT_JOIN_EMPTY_LINES_EXCLUDE+= $(REMOTE_FILES)

perl_after_indent=\
  s:{\s+([\w\"]):{$$1:g;\
  s:([\w\"\-])\s+}:$$1}:g;\
  $$t=join "|", qw($(INDENT_TYPES));\
  length ($$t) and s:($$t)(\s\*+)\s+(\w):$$1$$2$$3:g;\
  $(NULL)

perl_indent_join_empty_lines=\
  my @files = @ARGV;\
  $$^I = "~";\
  for my $$file (@files) {\
    local $$/;\
    @ARGV = $$file;\
    while (<>) {\
      s/\n\n\n+/\n\n/gs;\
      print;\
    }\
  }\
  $(NULL)

indent_vc_list_c=\
  $(call vc_list_exclude, $(VC_LIST_C) $(INDENT_INCLUDE),\
    $(INDENT_EXCLUDE))\
  $(NULL)

# Formats source code.
.PHONY: indent
indent:
	$(V_at)for file in $(indent_vc_list_c); do\
	  $(INDENT) $(INDENT_OPTIONS) $$file || exit 1;\
	done
	$(V_at)for file in $(indent_vc_list_c); do\
	  $(INDENT) $(INDENT_OPTIONS) $$file || exit 1;\
	done
	$(V_at)$(PERL) -i'~' -wple '$(perl_after_indent)'\
	  $(indent_vc_list_c)
	$(V_at)$(PERL) -we '$(perl_indent_join_empty_lines)'\
	  $(call vc_list_exclude, $(VC_LIST_ALL),\
            $(INDENT_JOIN_EMPTY_LINES_EXCLUDE))


perl_list_c_names:=\
  (/^()()(\w+)\s*\(/ or /^(static\s+)?(const\s+)?\w+\s+\**(\w+)\s+=/)\
  and print "$$ARGV:$$.:$$3";\
  eof and close ARGV;\
  $(NULL)

perl_list_lua_names:=\
  (/^(local\s+)?function\s*([\w\.]+?)\s*\(/ or /^(local\s+)?(\w+)\s*=/)\
  and print "$$ARGV:$$.:$$2";\
  eof and close ARGV;\
  $(NULL)

perl_list_mk_names:=\
  (/^([\w\-]+?):/ or /^(\w+\s*)=/)\
  and print "$$ARGV:$$.:$$1";\
  eof and close ARGV;\
  $(NULL)

# Lists names of C functions and variables.
.PHONY: list-c-names
list-c-names:
	@list='$(VC_LIST_C)';\
	if test -n "$$list"; then\
	  $(PERL) -wnle '$(perl_list_c_names)' $$list;\
	fi

# Lists names of Lua functions and variables.
.PHONY: list-lua-names
list-lua-names:
	@list='$(VC_LIST_LUA)';\
	if test -n "$$list"; then\
	  $(PERL) -wnle '$(perl_list_lua_names)' $$list;\
	fi

# Lists names of Makefile targets and variables.
.PHONY: list-mk-names
list-mk-names:
	@list='$(VC_LIST_MK) $(VC_LIST_AM)';\
	if test -n "$$list"; then\
	  $(PERL) -wnle '$(perl_list_mk_names)' $$list;\
	fi


# Checks for untracked files.
.PHONY: maintainer-clean-diff
maintainer-clean-diff:
	@test `$(GIT) ls-files --other | wc -l` -ne 0 &&\
	  { echo "error: untracked files not removed by maintainer-clean";\
	    $(GIT) ls-files --other; exit 1;  } || :


SC_RULES?=
SC_BASE_EXCLUDE?=
SC_BASE_EXCLUDE+= $(SYNTAX_CHECK_EXCLUDE)

# Runs build-aux/syntax-check script.
SC_RULES+= sc-base
sc-base:
	$(V_at)$(build_aux)/syntax-check\
	  $(call vc_list_exclude,\
	    $(VC_LIST_AM)\
	    $(VC_LIST_C)\
	    $(VC_LIST_LUA)\
	    $(VC_LIST_MK)\
	    $(VC_LIST_PL)\
	    $(VC_LIST_SH),\
	    $(SC_BASE_EXCLUDE))


SC_USELESS_IF_BEFORE_FREE_ALIASES?=
SC_USELESS_IF_BEFORE_FREE_EXCLUDE?=
SC_USELESS_IF_BEFORE_FREE_EXCLUDE+= $(SYNTAX_CHECK_EXCLUDE)

# Checks for useless if before free().
SC_RULES+= sc-useless-if-before-free
sc-useless-if-before-free:
	$(V_at)$(SHELL) $(build_aux)/useless-if-before-free\
	  $(SC_USELESS_IF_BEFORE_FREE_ALIASES:%=--name=%)\
	  $(call vc_list_exclude,\
	    $(VC_LIST_C),\
	    $(SC_USELESS_IF_BEFORE_FREE_EXCLUDE))\
	  && exit 1 || :


SYNTAX_CHECK_EXCLUDE?=
SYNTAX_CHECK_EXCLUDE+= $(REMOTE_FILES)

# Run all syntax checks.
.PHONY: syntax-check
.PHONY: $(SC_RULES)
syntax-check: $(SC_RULES)


UPDATE_COPYRIGHT_EXCLUDE?=
UPDATE_COPYRIGHT_EXCLUDE= $(REMOTE_FILES)
COPYRIGHT_YEAR?= $(shell date +%Y)
COPYRIGHT_HOLDER?= Free Software Foundation, Inc.

perl_update_copyright:=\
  s:(\W*Copyright\s\(C\)\s\d+)-?\d*(\s\Q$(COPYRIGHT_HOLDER)\E\b)\
  :$$1-$(COPYRIGHT_YEAR)$$2:x;

# Updates copyright year.
.PHONY: update-copyright
update-copyright:
	$(V_at)\
	list="$(call vc_list_exclude, $(VC_LIST_ALL), $(UPDATE_COPYRIGHT_EXCLUDE))";\
	if test -n "$$list"; then\
	  $(PERL) -i'~' -wple '$(perl_update_copyright)' $$list;\
	fi
