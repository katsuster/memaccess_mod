
CONFIG_MEMACCESS = m

EXTRA_CFLAGS += -I$(LDDINC)

ma-objs := ma_mod.o
obj-$(CONFIG_MEMACCESS) := ma.o

clean-files := Module.symvers modules.order *.ko *.mod.c *.o .*.cmd
clean-dirs := .tmp_versions
