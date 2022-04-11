#pragma once

#include <string>

namespace env {

	inline const std::string trimmed(const std::string& str) {

		const std::string ws = " \t\n\r\f\v";
		std::string ret(str);

		ret.erase(ret.find_last_not_of(ws.c_str()) + 1);
		ret.erase(0, ret.find_first_not_of(ws.c_str()));
		return ret;
	}

}
