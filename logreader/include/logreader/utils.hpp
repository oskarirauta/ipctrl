#pragma once

#include <string>
#include <list>

namespace logreader {

	const bool find_processes(std::list<int> &list, const std::string name, const std::string env = "");

};
