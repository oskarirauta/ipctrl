#include <fstream>
#include <dirent.h>
#include "logreader/utils.hpp"

static const std::string proc_dir(const struct dirent *dirp, const std::string fn) {

	return std::string("/proc/") + dirp -> d_name +
		std::string(fn.empty() ? "" : ( "/" + fn ));
}

static bool cmd_base_name(std::string &cmdLine) {

	if ( auto pos = cmdLine.find('\0'); pos != std::string::npos )
		cmdLine = cmdLine.substr(0, pos);

	if ( auto pos = cmdLine.rfind('/'); pos != std::string::npos )
		cmdLine = cmdLine.substr(pos + 1);

	return !cmdLine.empty();
}

const bool logreader::find_processes(std::list<int> &list, const std::string name, const std::string env) {

	if ( DIR *dp = opendir("/proc"); dp != NULL ) {

		int pid = -1;
		struct dirent *dirp;

		while ( pid < 0 && ( dirp = readdir(dp))) {

			if ( int id = atoi(dirp -> d_name); id > 0 ) {

				std::string line;
				std::ifstream cmdFile(proc_dir(dirp, "cmdline").c_str());
				if ( !std::getline(cmdFile, line) || line.empty())
					continue;

				if ( !cmd_base_name(line)) continue;
				else if ( line == name && env.empty())
					list.push_back(id);
				else if ( line == name && !env.empty()) {

					std::ifstream envFile(proc_dir(dirp, "environ").c_str());
					std::getline(envFile, line);

					if ( line.length() >= env.length() && line.find(env) != std::string::npos )
						list.push_back(id);
				}
			}
		}
		return true;

	}

	// error trying to open directory /proc
	return false;

}
