
KERNEL_DIR ?= /lib/modules/`uname -r`/build/
PWD        := $(shell pwd)
MFLAGS     := -C $(KERNEL_DIR) M=$(CURDIR) LDDINC=$(PWD)

modules:
	$(MAKE) $(MFLAGS) modules

install: modules_install

modules_install: modules
	$(MAKE) $(MFLAGS) INSTALL_MOD_PATH=$(MAKETOP) modules_install

clean:
	$(MAKE) $(MFLAGS) clean

distclean: clean

.PHONY: modules install modules_install clean distclean
