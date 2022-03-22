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
#include "logreader/file.hpp"

static const int get_last_ln_pos(std::ifstream &fd) {

	// TODO: unnecessary check, we just did check that it is open..
	if ( !fd.is_open()) {
		std::cerr << "get_last_ln_pos: file is not open" << std::endl;
		return -1;
	}

	fd.seekg(0, std::ios::end);
	if ( fd.rdstate() & ( std::ifstream::failbit | std::ifstream::badbit )) {
		std::cerr << "get_last_ln_pos: file seek failed" << std::endl;
		return -1;
	}

	int lastln = 0;
	int fsize = fd.tellg();
	char ch;

	for ( int i = 1; i < fsize; i++ ) {

		fd.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {

			fd.seekg(fsize - i - 1, std::ios::beg);
			if ( fd.get(ch); ch == 0x0a )
				lastln = fd.tellg();
		} catch ( std::system_error& e ) {
			std::cerr << e.code().message() << std::endl;
			return -1;
		}
	}

	return lastln;
}

const bool logreader::file::tail(void) {

	int new_lastln = -1;
	int new_fsize = -1;
	int e_count = 0;

	// a short delay keeps mutex unlocked from time to time
	std::this_thread::sleep_for(std::chrono::milliseconds(this -> _delay));

	std::lock_guard<std::mutex> lock(this -> mutex);

	if ( !this -> fd.is_open()) {

		std::cout << "file is not open" << std::endl;

		this -> fd.open(this -> _name, std::ifstream::in);

		if (( fd.rdstate() & ( std::ifstream::failbit | std::ifstream::badbit )) || !this -> fd.is_open()) {
			std::cerr << "file open failed.." << std::endl;
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
			std::cerr << e.code().message() << std::endl;
			return false;
		}

		this -> _fpos = new_lastln < 0 ? 0 : new_lastln;
		std::cout << "file " << this -> _name << " opened" << std::endl;
		return new_lastln < 0 ? false : true;

	} else {

		fd.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			this -> fd.seekg(0, std::ios::end);
			int new_size = fd.tellg();

			if ( new_size < this -> _fsize ) { // file had shrunken, reset

				this -> fd.close();
				std::cerr << "file shrunken, reset tailing" << std::endl;
				return true;
			} else if ( new_size == this -> _fsize ) { // file did not grow
				std::cout << "filesize remained the same: " << new_size << std::endl;
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
					std::cout << "#" << e_count << ": \"" << entry << "\"" << std::endl;
					entry = "";
					new_lastln = fd.tellg();
				} else entry += ch;
			}

		} catch ( std::system_error& e ) {
			std::cerr << e.code().message() << std::endl;
			return false;
		}
	}

	if ( new_lastln == this -> _fpos ) return true; // lastln pos remained same
	else if ( new_lastln < this -> _fpos ) {

		std::cerr << ( new_lastln == 0 ? "file abnormally did reset" : "nextln is less than lastnl, but not 0. Should not be possible.." ) << std::endl;
		this -> fd.close();
		return false;
	}

	std::cout << "readed " << e_count << " entries\n" << "lastln pos updated: " << new_lastln << std::endl;

	this -> _fpos = new_lastln;
	return true;

}
