ifndef TOPDIR
  export TOPDIR = $(shell dirname $(dir $(abspath $(firstword $(MAKEFILE_LIST)))))
  include $(TOPDIR)/Makefile.inc
endif

include $(TOPDIR)/Makefile.deps

all: $(LOGREADER_OBJS)

$(foreach f, $(LOGREADER_OBJS), $(eval $(call compile,$(f))))

clean:
	rm -f $(LOGREADER_OBJS)
