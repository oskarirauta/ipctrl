OBJDIR:=$(TOPDIR)/objs
BINDIR:=$(TOPDIR)/bin

CXX?=g++
CXXFLAGS?=--std=c++17
SUBDIRS:=shared logger logreader stdcapture nft
INCLUDES:=-I$(TOPDIR)/include $(addsuffix /include,$(addprefix -I$(TOPDIR)/,$(SUBDIRS)))
LIBS:=

export OBJDIR BINDIR \
	CXX CXXFLAGS \
	SUBDIRS INCLUDES \
	LIBS

COMMON_OBJS:= $(OBJDIR)/common.o

ENVIRON_OBJS:= $(OBJDIR)/environ.o

LOGGER_OBJS:= $(OBJDIR)/logger.o

STDCAPTURE_OBJS:= \
	$(OBJDIR)/stdcapture.o

LOGREADER_OBJS:= \
	$(OBJDIR)/logreader_utils.o \
	$(OBJDIR)/logreader_syslog.o \
	$(OBJDIR)/logreader_logread.o \
	$(OBJDIR)/logreader_file.o

NFT_OBJS:= \
	$(OBJDIR)/nft.o \
	$(OBJDIR)/nft_element.o \
	$(OBJDIR)/nft_exec.o \
	$(OBJDIR)/nft_family.o \
	$(OBJDIR)/nft_query.o \
	$(OBJDIR)/nft_set.o

EXAMPLE_OBJS:= \
	$(OBJDIR)/stdcapture_main.o \
	$(OBJDIR)/syslog_main.o \
	$(OBJDIR)/logread_main.o \
	$(OBJDIR)/logfile_main.o \
	$(OBJDIR)/nftset_main.o

EXAMPLES:= \
	$(BINDIR)/stdcapture \
	$(BINDIR)/syslog \
	$(BINDIR)/logread \
	$(BINDIR)/logfile \
	$(BINDIR)/nftset

OBJS:= \
	$(COMMON_OBJS) $(ENVIRON_OBJS) \
	$(LOGGER_OBJS) $(STDCAPTURE_OBJS) \
	$(LOGREADER_OBJS) $(NFT_OBJS)

export COMMON_OBJS ENVIRON_OBJS LOGGER_OBJS \
	STDCAPTURE_OBJS LOGREADER_OBJS \
	NFT_OBJS EXAMPLE_OBJS EXAMPLES OBJS
