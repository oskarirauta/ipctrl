ifndef TOPDIR
  export TOPDIR = $(shell dirname $(dir $(abspath $(firstword $(MAKEFILE_LIST)))))
  include $(TOPDIR)/Makefile.inc
endif

include $(TOPDIR)/Makefile.deps

all: $(LOGGER_OBJS)

$(foreach f, $(LOGGER_OBJS), $(eval $(call compile,$(f))))

clean:
	rm -f $(LOGGER_OBJS)
