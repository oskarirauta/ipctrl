#include <iostream>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <unistd.h>
#include <poll.h>
#include <sys/wait.h>

#include "common.hpp"
#include "environ.hpp"
#include "logreader/utils.hpp"
#include "logreader/logread.hpp"

const bool logreader::logread::tail(void) {

	this -> mutex.lock();

	if ( !this -> _running ) {

		if ( this -> _aborted ) { // Do not tail, we have been aborted

			if ( this -> _pid > 0 && kill(this -> _pid, 0) == 0 ) {

				if ( kill(this -> _pid, 0) == 0 ) {

					// forked process is still running though it should not
					// be, so kill it

					kill(this -> _pid, SIGKILL);
				}

				if ( this -> _pid > 0 || this -> _running || this -> _exiting || entries.size() > 0) {

					this -> _pid = 0;
					this -> _running = false;
					this -> _exiting = false;
					this -> entries.clear();
				}
			}

			this -> mutex.unlock();
			return false;
		}

		this -> mutex.unlock();

		if ( this -> _exiting ) // should never happen
			return true;

		if ( !this -> spawn_child()) // forking a child failed
			return false;

		this -> mutex.lock();
		this -> _running = true;
		this -> mutex.unlock();
		return true;
	}

	// is forked process running?
	bool tailing = this -> _pid > 0 && kill(this -> _pid, 0) == 0;

	if ( this -> _exiting ) { // exit forked process

		if ( tailing ) {

			int status;
			kill(this -> _pid, SIGKILL);
			waitpid(this -> _pid, &status, 0);
			tailing = this -> _pid > 0 && kill(this -> _pid, 0) == 0;
		}

		if ( !tailing ) {

			this -> _running = false;
			this -> _exiting = false;
			this -> _pid = 0;
			fclose(this -> _output);
			this -> mutex.unlock();
			return true;
		}

		// failed to kill child process
		this -> _running = true;
		this -> _exiting = true;
		this -> mutex.unlock();
		return false;

	} else if ( !tailing && !this -> _aborted ) { // forked process has died unexpectedly

		fclose(this -> _output); // close old pipe
		this -> mutex.unlock();

		if ( !this -> spawn_child()) { // spawn new one - or fail

			this -> mutex.lock();
			this -> _pid = 0;
			this -> _exiting = false;
			this -> _running = false;
			this -> mutex.unlock();
			return false;
		}

		// we have spawned a new child process

		this -> mutex.lock();
		this -> _exiting = false;
		this -> _running = true;
		this -> mutex.unlock();
		return true;

	} else if ( this -> _aborted ) { // abortion has been requested

		this -> _exiting = true;
		this -> mutex.unlock();
		return true;
	}

	// If there's new data in pipe, retrieve and push to list
	char line[1024];
	struct pollfd fds{ .fd = this -> pipefd[0], .events = POLLIN };
	int res = poll(&fds, 1, 0);
	bool pollerr = fds.revents&(POLLERR | POLLNVAL);
	if ( res == 1 && !pollerr && fgets(line, sizeof(line), this -> _output )) {

		std::string entry(line);
		char last = entry.back();
		while ( last == ' ' || last == '\t' || last == '\n' || last == '\r' || last == '\f' || last == '\v' ) {
			entry.pop_back();
			last = entry.back();
		}

		this -> entries.push_back(entry);
	}

	this -> mutex.unlock();

	// a short delay keeps mutex unlocked from time to time
	std::this_thread::sleep_for(std::chrono::milliseconds(this -> _delay));
	return true;
}

static void logread_signal_handler(int n) {

	// TODO:
	// this should be displayed ONLY when debugging is enabled, but we do not have
	// such option yet
	std::cout << "signal handler received exception [logreader::logread]: " << n << std::endl;
	wait(NULL);
}

bool logreader::logread::spawn_child(void) {

	this -> mutex.lock();

	if ( this -> _aborted ) {
		this -> mutex.unlock();
		return false;
	}

	if ( this -> _pid != 0 ) {

		bool tailing = this -> _pid > 0 && kill(this -> _pid, 0) == 0;
		this -> mutex.unlock();
		return tailing;
	}

	this -> mutex.unlock();

	pipe(this -> pipefd);
	pid_t ch_pid = fork();

	if ( ch_pid == -1 ) {

		// TODO:
		// while debugging only
		std::cout << "fork failed" << std::endl;
		return false;

	} else if ( ch_pid > 0 ) { // fork succeeded

		struct sigaction v;
		v.sa_handler = logread_signal_handler;
		v.sa_flags = SA_NOCLDWAIT;
		sigemptyset(&v.sa_mask);
		sigaction(17, &v, NULL);

		this -> mutex.lock();
		close(this -> pipefd[1]);
		this -> _output = fdopen(this -> pipefd[0], "r");
		this -> _pid = ch_pid;
		this -> mutex.unlock();

		// TODO:
		// while debugging only
		std::cout << "spawn success" << std::endl;
		return true;

	} else {

		// TODO:
		// shell env altering should be moved to it's own file

		std::string env = shell_env();
		std::vector<std::string> valuepairs = common::split(env, '\n');
		valuepairs.push_back("CHILD_OF=ipctrl");
		const char *ch_env[valuepairs.size() + 1];
		new_environ(ch_env, valuepairs);

		close(this -> pipefd[0]);
		dup2(this -> pipefd[1], STDOUT_FILENO);
		dup2(this -> pipefd[1], STDERR_FILENO);
		execle("/sbin/logread", "/sbin/logread", "-f", (char *)NULL, ch_env);
		exit(EXIT_FAILURE);
	}

	// We should never reach this point, but this way function returns a value
	// and compiler does not warn about it
	return false;
}

void logreader::logread::panic(void) {

	// TODO:
	// try locking several times for some time, for example- 5ms

	bool locked = this -> mutex.try_lock(); // Do not force mutex, this is panic
	pid_t ch_pid = this -> _pid;
	this -> _aborted = true;
	this -> entries.clear();

	if ( locked ) // unlock if mutex lock was possible
		this -> mutex.unlock();

	if ( ch_pid > 0 )
		kill(ch_pid, SIGKILL);
}

// logread::logreader must be a singleton only
static bool logread_initialized = false;

logreader::logread::logread(void) {

	if ( logread_initialized )
		throw std::runtime_error("logreader::logread is singleton. Only one instance is allowed.");

	logread_initialized = true;

	// if ipctrl was killed with SIGKILL(9), fork propably left to live, so
	// identify and kill them
	std::list<int> old_childs;
	if ( logreader::find_processes(old_childs, "logread", "CHILD_OF=ipctrl"))
		for ( const auto &pid : old_childs)
			kill(pid, SIGKILL);
}
