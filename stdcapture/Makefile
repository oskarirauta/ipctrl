ifndef TOPDIR
  export TOPDIR = $(shell dirname $(dir $(abspath $(firstword $(MAKEFILE_LIST)))))
  include $(TOPDIR)/Makefile.inc
endif

include $(TOPDIR)/Makefile.deps

all: $(STDCAPTURE_OBJS)

$(foreach f, $(STDCAPTURE_OBJS), $(eval $(call compile,$(f))))

clean:
	rm -f $(STDCAPTURE_OBJS)
