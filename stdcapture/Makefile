TOPDIR?=..
CXX?=g++
CXXFLAGS?=--std=c++17

all: $(STDCAPTURE_OBJS)

$(OBJDIR)/stdcapture.o: stdcapture.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

clean:
	rm -f $(STDCAPTURE_OBJS)
