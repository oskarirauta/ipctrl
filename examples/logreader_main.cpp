#include <iostream>
#include <chrono>
#include <thread>

#include <signal.h>
#include <sys/wait.h>

#include "logreader/syslog.hpp"

logreader::syslog syslog;
// to test singleton forcing:
//logreader::syslog syslog2;

static struct sigaction exit_action, ignore_action/*, panic_action*/;
static int stopped = 0;

static void die_handler(int signum) {

	std::cout << "received TERM signal [" << signum << "]" << std::endl;
	syslog.sig_exit(); // ask syslog to quit
	stopped = 1;
	std::cout << "sig exit sent" << std::endl;
}

/*
// not necessary or even useful..
static void panic_handler(int signum) {
	std::cout << "panic - kill child procs" << std::endl;
	syslog.panic();
}
*/

void add_signal_handler(void) {

	exit_action.sa_handler = die_handler;
	sigemptyset(&exit_action.sa_mask);
	exit_action.sa_flags = 0;
	// we handle SA_NOCLDWAIT on logreader::syslog for now
	//exit_action.sa_flags = SA_NOCLDWAIT;

	ignore_action.sa_handler = SIG_IGN;
	sigemptyset(&ignore_action.sa_mask);
	ignore_action.sa_flags = 0;
/*
	panic_action.sa_handler = panic_handler;
	sigemptyset(&panic_action.sa_mask);
	panic_action.sa_flags = 0;
*/
	sigaction(SIGTERM, &exit_action, NULL);
//	sigaction(SIGALRM, &panic_action, NULL);
	sigaction(SIGALRM, &ignore_action, NULL);
	sigaction(SIGHUP, &ignore_action, NULL);
	sigaction(SIGINT, &exit_action, NULL);
	sigaction(SIGPIPE, &ignore_action, NULL);
	sigaction(SIGQUIT, &ignore_action, NULL);
	sigaction(SIGUSR1, &ignore_action, NULL);
	sigaction(SIGUSR2, &ignore_action, NULL);
//	sigaction(17, &exit_action, NULL);
}

int main() {

	add_signal_handler();

	int x = 0;

	while ( !syslog.aborted()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		syslog.tail();

		syslog.mutex.lock();
		while ( syslog.entries.size() > 0 ) {
			std::cout << "recv: \"" << syslog.entries.front() << "\"" << std::endl;
			syslog.entries.pop_front();
		}
		syslog.mutex.unlock();

		if ( stopped > 0 && stopped < 10 ) {
			std::cout << "* * * stop * * *" << std::endl;
			stopped++;
		}
		x++;
		if ( x == 2553500 ) {
			std::cout << "quit" << std::endl;
			syslog.sig_exit(); // ask to abort
		}
	}

	std::cout << "stopping tailing" << std::endl;

	while ( syslog.running()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(80));
		syslog.tail();
	}

	std::cout << "tailing ended" << std::endl;

	return 0;
}
