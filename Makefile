export TOPDIR = $(shell pwd)
include Makefile.inc

all: $(OBJS)

$(OBJS):
	for dir in $(addprefix $(TOPDIR)/,$(SUBDIRS)) ; do \
		make -C $$dir ; \
	done

examples: $(OBJS)
	make -C examples ;

clean:
	for dir in $(addprefix $(TOPDIR)/,$(SUBDIRS)) $(TOPDIR)/examples ; do \
		make -C $$dir clean ; \
	done
