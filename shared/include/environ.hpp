#pragma once

#include <string>

namespace env {

	const std::string get(void);
	void add(std::string &new_env, std::string name, std::string value);
	int new_size(std::string new_env);
	void mk_env(const char *ch_env[], std::string new_env);
}
