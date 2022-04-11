#pragma once

#include "env/member.hpp"
#include "env/list.hpp"

namespace env {

	const env::list getenv();
	const env::list getenv(pid_t pid);

}
