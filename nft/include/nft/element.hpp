#pragma once

#include <string>
#include <chrono>
#include <json/json.h>

namespace nft {

	struct Element {

		struct Timeout {

			std::chrono::seconds value = std::chrono::seconds(0);
			std::chrono::seconds created = std::chrono::seconds(0);
			const std::chrono::seconds expires();

			const Json::Value dump(void);
		};

		std::string addr;
		nft::Element::Timeout timeout;

		const Json::Value dump(void);
	};

}
