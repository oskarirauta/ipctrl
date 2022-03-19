#all: stdcapture_test test syslog_test

CXX?=g++
CXXFLAGS?=--std=c++17

include Makefile.inc

SUBDIRS:=shared logreader stdcapture nft
INCLUDES:=-I$(TOPDIR)/include $(addsuffix /include,$(addprefix -I$(TOPDIR)/,$(SUBDIRS)))
LIBS:=

export SUBDIRS
export INCLUDES
export LIBS

all:
	for dir in $(addprefix $(TOPDIR)/,$(SUBDIRS)) ; do \
		make -C $$dir ; \
	done
	make -C $(addprefix $(TOPDIR)/,examples)

clean:
	for dir in $(addprefix $(TOPDIR)/,$(SUBDIRS)) ; do \
		make -C $$dir clean ; \
	done
	make -C $(addprefix $(TOPDIR)/,examples) clean
