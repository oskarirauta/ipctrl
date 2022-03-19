#include "environ.hpp"

extern char **environ;

const std::string shell_env(void) {

	int c = 0;
	char **s = environ;
	std::string env;

	for (; *s; s++ ) {
		c++;
		env += env.empty() ? "" : "\n";
		env += *s;
	}

	return env;
}

void  new_environ(const char *ch_env[], std::vector<std::string> &valuepairs) {

	for ( int i = 0; i < valuepairs.size(); i++ )
		ch_env[i] = valuepairs[i].c_str();

	ch_env[valuepairs.size()] = NULL;
}
