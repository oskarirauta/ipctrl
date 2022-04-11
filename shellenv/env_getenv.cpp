#include <fstream>
#include "env/member.hpp"
#include "env/list.hpp"
#include "env/env.hpp"

extern char **environ;

const env::list env::getenv() {

	char **s = environ;
	env::list n;

	for (; *s; s++ ) {
		std::string e(*s);
		auto delim = e.find_first_of("=");
		if ( delim == std::string::npos || e.substr(0,delim).empty() || e.substr(delim + 1, e.size() - delim - 1).empty())
			continue;
		n += env::member(e.substr(0, delim), e.substr(delim + 1, e.size() - delim - 1));
	}

	return n;
}

const env::list env::getenv(pid_t pid) {

	std::string env_file = "/proc/" + std::to_string(pid) + "/environ";
	std::ifstream fd(env_file.c_str());
	env::list n;

	if ( fd.fail() || !fd.is_open())
		return n;

	char ch;
	std::string key, val;
	bool got_key = false;

	while ( fd >> ch && ch != -1 ) { // -1 = eof

		if ( fd.fail())
			break;

		if ( !got_key && ch == '=' ) {

			got_key = true;
			continue;
		}

		if ( ch == 0 ) {

			n[key] = val;
			key = "";
			val = "";
			got_key = false;
			continue;
		}

		if ( !got_key ) key += ch;
		else val += ch;
	}

	fd.close();
	return n;
}
