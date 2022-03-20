ifndef TOPDIR
  export TOPDIR = $(shell dirname $(dir $(abspath $(firstword $(MAKEFILE_LIST)))))
  include $(TOPDIR)/Makefile.inc
endif

include $(TOPDIR)/Makefile.deps

all: $(EXAMPLE_OBJS) $(EXAMPLES)

$(foreach f, $(EXAMPLE_OBJS), $(eval $(call compile,$(f))))
$(foreach f, $(EXAMPLES), $(eval $(call link,$(f),_main)))

clean:
	rm -f $(EXAMPLE_OBJS) $(EXAMPLES)
