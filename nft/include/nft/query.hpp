#pragma once

#include <string>
#include "nft/family.hpp"

namespace nft {

	enum Cmd {
		LIST_SETS,
		FLUSH_SET,
		LIST_ELEMENTS,
		ADD_ELEMENTS,
		DELETE_ELEMENTS
	};

	struct Query {
		nft::Cmd cmd;
		nft::Family family = none;
		std::string table = "";
		std::string set = "";
		std::string rule = "";
		std::string saddr = "";
		std::string dport = "";
		int timeout = 0;

		const std::string toString(void);
	};

}
