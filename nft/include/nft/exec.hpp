#pragma once

#include <string>
#include <json/json.h>

#include "nft/query.hpp"

namespace nft {

	const int exec(std::string query, std::string &result, bool json);
	const int exec(std::string query, std::string &result);
	const int exec(nft::Query query, std::string &result);
	const int exec(std::string query, Json::Value &result);
	const int exec(nft::Query query, Json::Value &result);
}
