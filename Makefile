CC=i686-elf-gcc
INCLUDE?=-Iinclude
CFLAGS?=-O2 -ffreestanding -std=gnu11
LDFLAGS?=-nostdlib
LIBS?=-lgcc

# -- Do not edit below this line --

VERSION:="$(shell git describe --abbrev=4 --dirty --always --tags)"
INCLUDE:=$(INCLUDE)
CFLAGS:=$(CFLAGS) -Wall -Wextra -DVERSION=\"$(VERSION)\" -ggdb
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS)

ARCHDIR=arch/i386

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)

KERNEL=vmbox

KERNEL_OBJS=$(KERNEL_ARCH_OBJS) \
	    kernel/init.o \
	    kernel/string.o \
	    kernel/io.o \

OBJS=$(ARCHDIR)/boot/crti.o \
     $(ARCHDIR)/crtbegin.o \
     $(KERNEL_OBJS) \
     $(ARCHDIR)/crtend.o \
     $(ARCHDIR)/boot/crtn.o \

LINK_LIST=$(LDFLAGS) \
	  $(ARCHDIR)/boot/crti.o \
	  $(ARCHDIR)/crtbegin.o \
	  $(KERNEL_OBJS) \
	  $(LIBS) \
	  $(ARCHDIR)/crtend.o \
	  $(ARCHDIR)/boot/crtn.o \

.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .c .s

all: $(KERNEL)

$(KERNEL): $(OBJS) $(ARCHDIR)/linker.ld
	@$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(LINK_LIST)
	@echo [LD] $@

$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o:
	@OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

.c.o:
	@$(CC) -MD -c $< -o $@ $(CFLAGS) $(INCLUDE)
	@echo [CC] $@

.s.o:
	@$(CC) -MD -c $< -o $@ $(CFLAGS) $(INCLUDE)
	@echo [AS] $@

clean:
	$(RM) $(KERNEL)
	$(RM) $(OBJS) *.o */*.o */*/*.o
	$(RM) $(OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve-timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel: $(KERNEL)
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp $(KERNEL) $(DESTDIR)$(BOOTDIR)

-include $(OBJS:.o=.d)
