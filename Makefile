ARCH?=x86
DESTDIR?=base-system
PREFIX?=/usr

# -- Do not edit below this line --

VERSION:="$(shell git describe --abbrev=4 --dirty --always --tags)"
ARCH:=$(ARCH)
DESTDIR:=base-system/
PREFIX:=/usr/
export ARCH VERSION PREFIX

SUBDIRS:=kernel \
	 bin \
	 lib \
	 sbin \
	 usr.sbin \

.PHONY: all clean install install-headers $(SUBDIRS)

all: $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) $(MAKECMDGOALS) -C $@

clean: $(SUBDIRS)

install: $(SUBDIRS)
