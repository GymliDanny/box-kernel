CC=i686-elf-gcc
INCLUDE?=include
CFLAGS?=-O2 -g
LDFLAGS?=
LIBS?=

INCLUDE:=$(INCLUDE)
CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra -I$(INCLUDE)
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lgcc

ARCHDIR=arch/i386

include $(ARCHDIR)/make.config

INCLUDE:=$(INCLUDE) $(KERNEL_ARCH_INCLUDE)
CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS) -I$(INCLUDE)
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
	@$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS)
	@echo [CC] $@

.s.o:
	@$(CC) -MD -c $< -o $@ $(CFLAGS)
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
