#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>

#include <signal.h>
#include <sys/wait.h>

#include "logger.hpp"
#include "logreader/file.hpp"

static bool exists(std::string name) {

	if ( FILE *fd = fopen(name.c_str(), "r")) {
		fclose(fd);
		return true;
	}

	return false;
}

static void usage(std::string name, bool verbose) {

	std::cout << "\nusage: " << name.substr(name.find_last_of("/\\") + 1) <<
		( verbose ? " [options] filename\n" : " filename\n" ) << std::endl;

	if ( !verbose ) return;

	std::cout << "options:" << std::endl;

	std::cout << "\t--help  | --h | -h: display help" << std::endl;
	std::cout << "\t--debug | --d | -d: enable debug output" << std::endl;
	std::cout << std::endl;
}

int main(int argc, char *argv[]) {

	std::string filename, app_name(argv[0]);
	bool verbose, debug;

	for ( int i = 1; i < argc; i++ ) {
		std::string arg(argv[i]);
		if ( arg == "-h" || arg == "--h" || arg == "--help" || arg == "-help" ) verbose = true;
		else if ( arg == "-d" || arg == "--d" || arg == "--debug" || arg == "-debug" ) debug = true;
		else filename = arg;
	}

	if ( verbose || argc < 2 || filename.empty()) {

		usage(app_name, verbose);
		return 0;
	} else if ( !exists(filename)) {

		std::cout << "\nerror: file " << filename << " does not exist or is not readable." << std::endl;
		usage(app_name, verbose);
		return -1;
	}

	if ( debug ) {

		logger::output_level[logger::verbose] = true;
		logger::output_level[logger::vverbose] = true;
		logger::output_level[logger::debug] = true;

		logger::debug << "debug output enabled." << std::endl;
		std::cout << std::endl;
	}

	logreader::file logfile(filename);

	std::cout << "\ntry appending to file " << filename << ", for example, like this:\n" <<
		"\techo \"my text\" >> " << filename << "\n" << std::endl;

	while ( !logfile.aborted()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		logfile.tail();

		logfile.mutex.lock();
		while ( !logfile.entries.empty()) {
			std::cout << logfile.entries.front() << std::endl;
			logfile.entries.pop_front();
		}
		logfile.mutex.unlock();

	}

	return 0;
}
