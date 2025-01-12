SUBDIRS:=bxpkg \
	 gfdisk \

export ARCH VERSION PREFIX

.PHONY: all clean $(SUBDIRS)

all: $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) $(MAKECMDGOALS) -C $@

clean: $(SUBDIRS)

install: $(SUBDIRS)
