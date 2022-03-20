ifndef TOPDIR
  export TOPDIR = $(shell dirname $(dir $(abspath $(firstword $(MAKEFILE_LIST)))))
  include $(TOPDIR)/Makefile.inc
endif

include $(TOPDIR)/Makefile.deps

SHARED_OBJS := $(COMMON_OBJS) $(ENVIRON_OBJS)

all: $(SHARED_OBJS)

$(foreach f, $(SHARED_OBJS), $(eval $(call compile,$(f))))

clean:
	rm -f $(SHARED_OBJS)
