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
#include "logger.hpp"
#include "logreader/utils.hpp"
#include "logreader/file.hpp"

const bool logreader::file::tail(void) {

	int new_lastln = -1;
	int new_fsize = -1;
	int e_count = 0;

	// a short delay keeps mutex unlocked from time to time
	std::this_thread::sleep_for(std::chrono::milliseconds(this -> _delay));

	std::lock_guard<std::mutex> lock(this -> mutex);

	if ( !this -> fd.is_open()) {

		logger::debug << "file " << this -> _name << " is not open" << std::endl;

		this -> fd.open(this -> _name, std::ifstream::in);

		if (( fd.rdstate() & ( std::ifstream::failbit | std::ifstream::badbit )) || !this -> fd.is_open()) {
			logger::debug << "file " << this -> _name << " open failed" << std::endl;
			return false;
		}

		fd.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			this -> fd.seekg(0, std::ios::end);
			new_fsize = fd.tellg();

			// TODO:
			this -> _fsize = 0; //new_fsize;
			new_lastln = 0; // get_last_ln_pos(fd);
			this -> _fpos = new_lastln < 0 ? 0 : new_lastln;
		} catch ( std::system_error& e ) {
			logger::error << "file error (" << this -> _name << "): " << e.code().message() << std::endl;
			return false;
		}

		this -> _fpos = new_lastln < 0 ? 0 : new_lastln;

		logger::debug << "file " << this -> _name << " is opened" << std::endl;

		return new_lastln < 0 ? false : true;

	} else {

		fd.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			this -> fd.seekg(0, std::ios::end);
			int new_size = fd.tellg();

			if ( new_size < this -> _fsize ) { // file had shrunken, reset

				this -> fd.close();
				logger::error << "file " << this -> _name << " was shrunken, resetting tailing" << std::endl;
				return true;
			} else if ( new_size == this -> _fsize ) { // file did not grow
				logger::debug << "filesize for " << this -> _name << " remained the same: " << new_size << std::endl;
				return true;
			}

			char ch = 0;
			std::string entry;
			e_count = 0;

			this -> fd.seekg(this -> _fpos);
			new_lastln = this -> _fpos;

			while ( !this -> fd.eof() && new_lastln < new_size ) {

				fd.get(ch);

				if ( ch == 0x0a ) {

					e_count++;
					entry = "";
					new_lastln = fd.tellg();
				} else entry += ch;
			}

		} catch ( std::system_error& e ) {
			logger::error << "file error (" << this -> _name << "): " << e.code().message() << std::endl;
			return false;
		}
	}

	if ( new_lastln == this -> _fpos ) return true; // lastln pos remained same
	else if ( new_lastln < this -> _fpos ) {

		logger::error << this -> _name << " error: " << ( new_lastln == 0 ? "file abnormally did reset" : "nextln is less than lastnl, but not 0. Should not be possible.." ) << std::endl;
		this -> fd.close();
		return false;
	}

	logger::debug << "readed " << e_count << " entries from " << this -> _name << std::endl;
	logger::debug << "lastln pos of " << this -> _name << " was updated: " << new_lastln << std::endl;

	this -> _fpos = new_lastln;
	return true;

}
