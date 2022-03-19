#include <netdb.h>

#include "common.hpp"
#include "nft/nft.hpp"

const nft::SetType nft::addrType(const std::string ipaddr) {

	struct addrinfo *ai, hints = {
		.ai_flags = AI_NUMERICHOST,
		.ai_family = PF_UNSPEC,
	};

	if ( ipaddr.find('/') != std::string::npos ) {

		std::vector<std::string> parts = common::split(ipaddr,'/');

		if ( parts.size() != 2 || !common::is_number(parts[1]))
			return nft::SetType::not_supported;

		int subnet;

		try {
			subnet = stoi(parts[1]);
		} catch (...) {
			return nft::SetType::not_supported;
		}

		if ( subnet < 0 || subnet > 255 )
			return nft::SetType::not_supported;

		if ( getaddrinfo(parts[0].c_str(), NULL, &hints, &ai ) != 0 )
			return nft::SetType::not_supported;

	} else if ( ipaddr.find('-') != std::string::npos ) {

		if ( ipaddr.find('/') != std::string::npos ||
			ipaddr.find(':') != std::string::npos )
			return nft::SetType::not_supported;

		std::vector<std::string> parts = common::split(ipaddr, '-');

		if ( !common::is_number(common::trim(parts[0], ". \n\t")) ||
			!common::is_number(common::trim(parts[1], ". \n\t")))
			return nft::SetType::not_supported;

		int val1 = stoi(common::trim(parts[0], ". \n\t"));
		int val2 = stoi(common::trim(parts[1], ". \n\t"));

		if ( val1 > val2 )
			return nft::SetType::not_supported;

		struct addrinfo *ai2, hints2 = {
			.ai_flags = AI_NUMERICHOST,
			.ai_family = PF_UNSPEC,
		};


		if ( getaddrinfo(parts[0].c_str(), NULL, &hints, &ai) != 0 )
			return nft::SetType::not_supported;

		if ( getaddrinfo(parts[1].c_str(), NULL, &hints2, &ai2) != 0 ) {
			freeaddrinfo(ai);
			return nft::SetType::not_supported;
		}

		bool range_ok = ai -> ai_family == AF_INET && ai2 -> ai_family == AF_INET;

		freeaddrinfo(ai);
		freeaddrinfo(ai2);

		return range_ok ? nft::SetType::ipv4_addr : nft::SetType::not_supported;

	} else if ( getaddrinfo(ipaddr.c_str(), NULL, &hints, &ai) != 0 )
		return nft::SetType::not_supported;

	nft::SetType family = ai -> ai_family == AF_INET ? nft::SetType::ipv4_addr :
		( ai -> ai_family == AF_INET6 ? nft::SetType::ipv6_addr : nft::SetType::not_supported);

	freeaddrinfo(ai);
	return family;
}

const Json::Value nft::dump(void) {

	Json::Value value;

	value["sets"] = Json::arrayValue;

	for ( int i = 0; i < nft::sets.size(); i++ )
		value["sets"].append(nft::sets[i].dump());

	return value;
}
