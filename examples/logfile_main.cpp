#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>

#include <signal.h>
#include <sys/wait.h>

#include "logreader/file.hpp"

static bool exists(std::string name) {

	if ( FILE *fd = fopen(name.c_str(), "r")) {
		fclose(fd);
		return true;
	}

	return false;
}

static void usage(std::string name, bool verbose) {

	std::cout << "\nusage:" << ( verbose ? " " : "\n\t" ) << name.substr(name.find_last_of("/\\") + 1) <<
		( verbose ? " [options] filename\n" : " filename\n" ) << std::endl;

	if ( !verbose ) return;

	std::cout << "options:" << std::endl;

	std::cout << "\t--help | --h | -h: display help" << std::endl;
	std::cout << std::endl;
}

int main(int argc, char *argv[]) {

	std::string app_name(argv[0]);

	if ( argc < 2 ) {
		usage(app_name, false);
		return 0;
	} else if ( argc >= 2 ) {

		bool verbose = false;
		for ( int i = 1; i < argc; i++ ) {
			std::string arg(argv[i]);
			if ( arg == "-h" || arg == "--h" || arg == "--help" || arg == "-help" ) verbose = true;
		}

		if ( verbose || argc > 2 ) {
			usage(app_name, verbose);
			return argc > 2 ? -1 : 0;
		}
	}

	const std::string filename(argv[1]);

	if ( !exists(filename)) {
		std::cout << "error: file " << filename << " does not exist or is not readable" << std::endl;
		return -1;
	}

	logreader::file logfile(filename);

	std::cout << "\ntry appending to file " << filename << ", for example, like this:\n" <<
		"\techo \"my text\" >> " << filename << "\n" << std::endl;

	while ( !logfile.aborted()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		logfile.tail();
	}

	return 0;
}
