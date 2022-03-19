#pragma once

#include <string>
#include <vector>
#include <json/json.h>

#include "nft/family.hpp"
#include "nft/set.hpp"
#include "nft/query.hpp"
#include "nft/exec.hpp"

namespace nft {

	const nft::Family family = nft::inet;
	const std::string table = "fw4";
	extern std::vector<nft::Set> sets;

	const nft::SetType addrType(const std::string ipaddr);

	bool update_sets(void);

	const Json::Value dump(void);
}
