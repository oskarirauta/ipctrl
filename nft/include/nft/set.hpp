#pragma once

#include <string>
#include <vector>
#include <json/json.h>

#include "nft/family.hpp"
#include "nft/element.hpp"

namespace nft {

	enum SetType: uint8_t {
		ipv4_addr, ipv6_addr, not_supported
	};

	struct Set {

		nft::Family family;
		std::string table;
		std::string name;
		nft::SetType type;
		int handle;
		std::string comment;
		std::vector<std::string> flags;
		std::vector<nft::Element> elements;

		bool has_interval(void);
		bool has_timeout(void);

		bool update(void);
		bool add(const std::string addr, int timeout = 0);
		bool remove(const std::string addr);
		bool flush(void);

		const Json::Value dump(void);
	};

}
