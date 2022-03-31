#include <iostream>
#include <chrono>
#include <thread>

#include <signal.h>
#include <sys/wait.h>

#include "logger.hpp"
#include "logreader/logread.hpp"

logreader::logread logread;

static struct sigaction exit_action, ignore_action/*, panic_action*/;
static int stopped = 0;

static void die_handler(int signum) {

	std::cout << "received TERM signal [" << signum << "]" << std::endl;
	logread.sig_exit(); // ask syslog to quit
	stopped = 1;
	std::cout << "sig exit sent" << std::endl;
}

void add_signal_handler(void) {

	exit_action.sa_handler = die_handler;
	sigemptyset(&exit_action.sa_mask);
	exit_action.sa_flags = 0;

	ignore_action.sa_handler = SIG_IGN;
	sigemptyset(&ignore_action.sa_mask);
	ignore_action.sa_flags = 0;

	sigaction(SIGTERM, &exit_action, NULL);
	sigaction(SIGALRM, &ignore_action, NULL);
	sigaction(SIGHUP, &ignore_action, NULL);
	sigaction(SIGINT, &exit_action, NULL);
	sigaction(SIGPIPE, &ignore_action, NULL);
	sigaction(SIGQUIT, &ignore_action, NULL);
	sigaction(SIGUSR1, &ignore_action, NULL);
	sigaction(SIGUSR2, &ignore_action, NULL);
}

static void usage(std::string name) {

	std::cout << "\nusage: " <<
		name.substr(name.find_last_of("/\\") + 1) <<
		" [options]\n\n" <<
		"options:\n" <<
		"\t--help  | --h | -h: display help\n" <<
		"\t--debug | --d | -d: enable debug output\n" << std::endl;
}

int main(int argc, char *argv[]) {

	std::string app_name(argv[0]);
	int x = 0;
	bool debug = false;

	add_signal_handler();

	for ( int i = 1; i < argc; i++ ) {

		std::string arg(argv[i]);
		if ( arg == "-h" || arg == "--h" || arg == "--help" || arg == "-help" ) {

			usage(app_name);
			return 0;
		} else if ( arg == "-d" || arg == "--d" || arg == "--debug" || arg == "-debug" )
			debug = true;
		else {

			std::cout << "\nUnknown arg: " << arg << std::endl;
			usage(app_name);
			return -1;
		}
	}

	if ( debug ) {

		std::cout << std::endl;
		logger::output_level[logger::verbose] = true;
		logger::output_level[logger::vverbose] = true;
		logger::output_level[logger::debug] = true;

		logger::debug << "debug output enabled." << std::endl;
		std::cout << std::endl;

		// Displays messages is previous processes were killed during init

		std::vector<logger::entry> hist = logger::last(0);
		for ( auto it = hist.begin(); it != hist.end(); ++it )
			std::cout << it -> msg << std::endl;
	}

	while ( !logread.aborted() || logread.running()) {

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		logread.tail();
		logread.iterate([](auto name, auto txt) { std::cout << txt << std::endl; });

		x++;
		if ( x == 2553500 ) {
			std::cout << "quit" << std::endl;
			logread.sig_exit(); // ask to abort
		}
	}

	std::cout << "logread tailing has ended" << std::endl;

	return 0;
}
