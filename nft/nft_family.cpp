#include <string>

#include "common.hpp"
#include "nft/family.hpp"

nft::Family::Family(const std::string type) {

	std::string _type = common::trim_ws(common::to_lower(type));

	if ( _type == "ip" ) this -> value = nft::ip;
	else if ( _type == "ip6" ) this -> value = nft::ip6;
	else if ( _type == "inet" ) this -> value = nft::inet;
	else if ( _type == "arp" ) this -> value = nft::arp;
	else if ( _type == "bridge" ) this -> value = nft::bridge;
	else if ( _type == "netdev" ) this -> value = nft::netdev;
	else this -> value = nft::none;
}

const std::string nft::Family::description(void) {

	switch ( this -> value ) {
		case ip: return "ip";
		case ip6: return "ip6";
		case inet: return "inet";
		case arp: return "arp";
		case bridge: return "bridge";
		case netdev: return "netdev";
		default: return "not-set";
	}
}

const nft::FamilyType nft::Family::type(void) {

	return this -> value;
}
