#pragma once

#include <string>
#include "env/env.hpp"
#include "env/list.hpp"

namespace shell {

	const int exec(const std::string& cmd, const std::string& args = "", const env::list& shell_env = env::list());
	const pid_t execf(const std::string& cmd, const std::string& args = "", const env::list& shell_env = env::list());

}
