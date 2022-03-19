TOPDIR?=..
CXX?=g++
CXXFLAGS?=--std=c++17

all: $(EXAMPLES)

$(OBJDIR)/stdcapture_main.o: stdcapture_main.cpp
	 $(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

$(OBJDIR)/logreader_main.o: logreader_main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

$(OBJDIR)/nft_main.o: nft_main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

$(BINDIR)/stdcapture: $(STDCAPTURE_OBJS) $(OBJDIR)/stdcapture_main.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) -o $@;

$(BINDIR)/logreader: $(SHARED_OBJS) $(LOGREADER_OBJS) $(OBJDIR)/logreader_main.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) -o $@;

$(BINDIR)/nftset: $(SHARED_OBJS) $(STDCAPTURE_OBJS) $(NFT_OBJS) $(OBJDIR)/nft_main.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) $(NFT_LIBS) $(JSON_LIBS) -o $@;

clean:
	rm -f $(EXAMPLE_OBJS) $(EXAMPLES)
