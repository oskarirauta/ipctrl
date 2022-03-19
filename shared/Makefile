TOPDIR?=..
CXX?=g++
CXXFLAGS?=--std=c++17

all: $(SHARED_OBJS)

$(OBJDIR)/common.o: common.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

$(OBJDIR)/environ.o: environ.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

clean:
	rm -f $(SHARED_OBJS)
