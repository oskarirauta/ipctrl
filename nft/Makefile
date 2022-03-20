ifndef TOPDIR
  export TOPDIR = $(shell dirname $(dir $(abspath $(firstword $(MAKEFILE_LIST)))))
  include $(TOPDIR)/Makefile.inc
endif

include $(TOPDIR)/Makefile.deps

all: $(NFT_OBJS)

$(foreach f, $(NFT_OBJS), $(eval $(call compile,$(f))))

clean:
	rm -f $(NFT_OBJS)
