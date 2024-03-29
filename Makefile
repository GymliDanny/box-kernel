INCLUDE?=-Iinclude
CFLAGS?=-O0 -ffreestanding -std=gnu11
LDFLAGS?=-nostdlib
LIBS?=-lgcc
ARCH?=i386

# -- Do not edit below this line --

CC:=i686-elf-gcc
VERSION:="$(shell git describe --abbrev=4 --dirty --always --tags)"
INCLUDE:=$(INCLUDE)
CFLAGS:=$(CFLAGS) -Wall -Wextra -DVERSION=\"$(VERSION)\" -ggdb
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS)
ARCH:=$(ARCH)

ARCHDIR=arch/$(ARCH)

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)

KERNEL=vmbox

KERNEL_OBJS=$(KERNEL_ARCH_OBJS) \
	    kernel/init.o \
	    kernel/string.o \
	    kernel/io.o \
	    kernel/panic.o \
	    kernel/sched.o \

OBJS=$(ARCHDIR)/boot/crti.o \
     $(ARCHDIR)/crtbegin.o \
     $(KERNEL_OBJS) \
     $(ARCHDIR)/crtend.o \
     $(ARCHDIR)/boot/crtn.o \

LINK_LIST=$(LDFLAGS) \
	  $(KERNEL_OBJS) \
	  $(LIBS) \

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

install-disk: $(KERNEL)
	rm a.img
	bximage -q -func=create -fd=1.44M a.img
	mkdosfs a.img
	syslinux -i a.img
	mcopy -i a.img img/libcom32.c32 ::libcom32.c32
	mcopy -i a.img img/mboot.c32 ::mboot.c32
	mcopy -i a.img img/syslinux.cfg ::syslinux.cfg
	mcopy -i a.img vmbox ::vmbox

run: $(KERNEL)
	qemu-system-i386 -kernel $(KERNEL) -serial stdio

debug: $(KERNEL)
	qemu-system-i386 -kernel $(KERNEL) -s -S

-include $(OBJS:.o=.d)
