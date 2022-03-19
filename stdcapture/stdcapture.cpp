#include <cstdio>
#include <chrono>
#include <thread>

extern "C" {
#include <unistd.h>
}

#include "stdcapture.hpp"

std::capture::capture(int BUFSIZE): m_capturing(false), BUFSIZE(BUFSIZE) {

	// make stdout & stderr streams unbuffered
	// so that we don't need to flush the streams
	// before capture and after capture
	// (fflush can cause a deadlock if the stream is currently being

	std::lock_guard<std::mutex> lock(this -> m_mutex);

	std::setvbuf(stdout, NULL, _IONBF, 0);
	std::setvbuf(stderr, NULL, _IONBF, 0);
}

void std::capture::begin(void) {

	std::lock_guard<std::mutex> lock(this -> m_mutex);

	if ( this -> m_capturing )
		return;

	this -> secure_pipe(this -> m_pipe);
	this -> m_oldStdOut = this -> secure_dup(STD_OUT_FD);
	this -> m_oldStdErr = this -> secure_dup(STD_ERR_FD);
	this -> secure_dup2(this -> m_pipe[WRITE],STD_OUT_FD);
	this -> secure_dup2(m_pipe[WRITE],STD_ERR_FD);
	this -> m_capturing = true;
	this -> secure_close(m_pipe[WRITE]);
}

bool std::capture::get_capturing(void) {

	std::lock_guard<std::mutex> lock(this -> m_mutex);
	return this -> m_capturing;
}

bool std::capture::end(void) {

	std::lock_guard<std::mutex> lock(this -> m_mutex);
	if ( !this -> m_capturing )
		return true;

	this -> m_captured.clear();
	this -> secure_dup2(this -> m_oldStdOut, STD_OUT_FD);
	this -> secure_dup2(this -> m_oldStdErr, STD_ERR_FD);

	const int bufSize = this -> BUFSIZE;
	char buf[bufSize];

	while ( true ) {

		int bytesRead = read(this -> m_pipe[READ], buf, bufSize - 1);

		if ( bytesRead > 0 ) {
			buf[bytesRead] = 0;
			m_captured += buf;
		} else if ( bytesRead < 0  && ( errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR )) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		if ( bytesRead != ( bufSize - 1))
			break;
	}

	this -> secure_close(this -> m_oldStdOut);
	this -> secure_close(this -> m_oldStdErr);
	this -> secure_close(this -> m_pipe[READ]);
	this -> m_capturing = false;
	return true;
}

std::string std::capture::get_result(void) {

	std::lock_guard<std::mutex> lock(this -> m_mutex);
	return this -> m_captured;
}

int std::capture::secure_dup(int src) {

	int ret = -1;
	while ( ret < 0 ) {

		ret = dup(src);
		if ( errno == EINTR ||  errno == EBUSY )
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return ret;
}

void std::capture::secure_pipe(int * pipes) {

	int ret = -1;
	while ( ret < 0 ) {

		ret = pipe(pipes) == -1;
		if ( errno == EINTR ||  errno == EBUSY )
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void std::capture::secure_dup2(int src, int dest) {

	int ret = -1;
	while ( ret < 0 ) {

		ret = dup2(src,dest);
		if ( errno == EINTR || errno == EBUSY )
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void std::capture::secure_close(int &fd) {

	int ret = -1;
	while ( ret < 0 ) {

		ret = close(fd);
		if ( errno == EINTR )
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	fd = -1;
}

void std::capture::lock(void) {

	this -> l_capturing = this -> get_capturing();
	this -> l_captured = this -> get_result();

	std::lock_guard<std::mutex> lock(this -> l_mutex);
	this -> m_locked = true;
	this -> m_mutex.lock();
	this -> l_capturing = this -> m_capturing;
	this -> l_captured = this -> m_captured;
}

void std::capture::unlock(void) {

	std::lock_guard<std::mutex> lock(this -> l_mutex);
	this -> m_mutex.unlock();
	this -> m_locked = false;
}

bool std::capture::get_locked(void) {

	std::lock_guard<std::mutex> lock(this -> l_mutex);
	return this -> m_locked;
}
