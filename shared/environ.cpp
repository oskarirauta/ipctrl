#include "common.hpp"
#include "environ.hpp"

extern char **environ;

const std::string env::get(void) {

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

void env::add(std::string &new_env, std::string name, std::string value) {

	new_env += "\n" + name + "=" + value;
}

int env::new_size(std::string new_env) {

	return common::split(new_env, '\n').size() + 1;
}

void env::mk_env(const char *ch_env[], std::string new_env) {

	std::vector<std::string> valuepairs = common::split(new_env, '\n');

	for ( int i = 0; i < valuepairs.size(); i++ )
		ch_env[i] = valuepairs[i].c_str();

	ch_env[valuepairs.size()] = NULL;
}
