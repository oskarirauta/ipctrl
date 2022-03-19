TOPDIR?=..
CXX?=g++
CXXFLAGS?=--std=c++17

all: $(NFT_OBJS)

$(OBJDIR)/nft.o: nft.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

$(OBJDIR)/nft_element.o: nft_element.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

$(OBJDIR)/nft_exec.o: nft_exec.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

$(OBJDIR)/nft_family.o: nft_family.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

$(OBJDIR)/nft_query.o: nft_query.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

$(OBJDIR)/nft_set.o: nft_set.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

clean:
	rm -f $(NFT_OBJS)
