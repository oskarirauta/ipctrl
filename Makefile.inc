TOPDIR:=$(shell pwd)
OBJDIR:=$(TOPDIR)/objs
BINDIR:=$(TOPDIR)/bin

SHARED_OBJS:= \
	$(OBJDIR)/common.o \
	$(OBJDIR)/environ.o

STDCAPTURE_OBJS:= \
	$(OBJDIR)/stdcapture.o

LOGREADER_OBJS:= \
	$(OBJDIR)/logreader_utils.o \
	$(OBJDIR)/logreader_syslog.o

NFT_OBJS:= \
	$(OBJDIR)/nft.o \
	$(OBJDIR)/nft_element.o \
	$(OBJDIR)/nft_exec.o \
	$(OBJDIR)/nft_family.o \
	$(OBJDIR)/nft_query.o \
	$(OBJDIR)/nft_set.o

NFT_LIBS:=-lnftables -lmnl -lnftnl -ljansson

JSON_LIBS:=-ljsoncpp

OBJS:=$(SHARED_OBJS) $(STDCAPTURE_OBJS) $(LOGREADER_OBJS) $(NFT_OBJS)

EXAMPLE_OBJS:= \
	$(OBJDIR)/syslog_main.o \
	$(OBJDIR)/stdcapture_main.o \
	$(OBJDIR)/nft_main.o

EXAMPLES:= \
	$(BINDIR)/stdcapture \
	$(BINDIR)/syslog \
	$(BINDIR)/nftset

export TOPDIR
export OBJDIR
export BINDIR
export SHARED_OBJS
export LOGREADER_OBJS
export STDCAPTURE_OBJS
export NFT_OBJS
export NFT_LIBS
export JSON_LIBS
export OBJS
export EXAMPLE_OBJS
export EXAMPLES
