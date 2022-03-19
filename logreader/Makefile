TOPDIR?=..
CXX?=g++
CXXFLAGS?=--std=c++17

all: $(LOGREADER_OBJS)

$(OBJDIR)/logreader_utils.o: logreader_utils.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

$(OBJDIR)/logreader_syslog.o: syslog.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

clean:
	rm -f $(LOGREADER_OBJS)
