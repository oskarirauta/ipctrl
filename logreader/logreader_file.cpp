#include <chrono>
#include <cstdlib>
#include <stdexcept>
#include <unistd.h>
#include <poll.h>
#include <sys/wait.h>

#include "common.hpp"
#include "environ.hpp"
#include "logger.hpp"
#include "logreader/utils.hpp"
#include "logreader/file.hpp"

static const bool file_exists(std::string fn) {

	if ( FILE *fd = fopen(fn.c_str(), "r")) {
		fclose(fd);
		return true;
	}

	return false;
}

static const int get_fsize(std::ifstream &fd, std::string fn, std::string sender) {

	if ( !fd.is_open()) {

		logger::error << "file " << fn << " is not open (" <<
			sender << ( sender.empty() ? "" : ":" ) << "get_fsize)" << std::endl;
		return -2;
	}

	int fsize = 0;
	fd.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		fd.seekg(0, std::ios::end);
		fsize = fd.tellg();
	} catch ( std::system_error& e ) {

		logger::error << "file error (" << fn << "): " << e.code().message() << " (" <<
			sender << ( sender.empty() ? "" : ":" ) << "get_fsize)" << std::endl;
		return -1;
	}

	return fsize;
}

static const int get_last_ln_pos(std::ifstream &fd, std::string fn, std::string sender) {

	// unnecessary check, we just did check that file is open.. but harmless, so..

	if ( !fd.is_open()) {
		logger::error << "file " << fn << " is not open (" <<
			sender << ( sender.empty() ? "" : ":" ) << "get_last_ln_pos)" << std::endl;
		return -1;
	}

	int fsize;
	char ch;

	if ( fsize = get_fsize(fd, fn, sender + ( sender.empty() ? "" : ":" ) + "get_last_ln_pos"); fsize <= 0 )
		return fsize == 0 ? 0 : -1;

	for ( int i = 1; i < fsize; i++ ) {

		fd.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {

			fd.seekg(fsize - i, std::ios::beg);
			if ( fd.get(ch); ch == 0x0a )
				return fd.tellg();

		} catch ( std::system_error& e ) {
			logger::error << "file error (" << fn << "): " << e.code().message() << " (" << sender << ( sender.empty() ? "" : ":" ) << "get_last_ln_pos)" << std::endl;
			break;
		}
	}

	return -1;
}

const bool logreader::file::tail(void) {

	// a short delay keeps mutex unlocked from time to time
	std::this_thread::sleep_for(std::chrono::milliseconds(this -> _delay));

	std::lock_guard<std::mutex> lock(this -> mutex);

	if ( this -> _aborted ) {

		if ( this -> fd.is_open()) {

			logger::verbose << "closing file " << this -> _name << " and ending tailing" << std::endl;
			this -> fd.close();
		}

		return false;
	}

	if ( !this -> fd.is_open()) {

		if ( !file_exists(this -> _name)) {

			this -> _did_reset = true;
			logger::verbose << "file " << this -> _name << " cannot be opened because it does not exist" << std::endl;
			return false;
		}

		logger::vverbose << "file " << this -> _name << " is not open" << std::endl;

		this -> fd.open(this -> _name, std::ifstream::in);

		if (( this -> fd.rdstate() & ( std::ifstream::failbit | std::ifstream::badbit )) || !this -> fd.is_open()) {
			logger::error << "file " << this -> _name << " open failed" << std::endl;
			return false;
		}

		this -> _fsize = 0;

		if ( this -> _did_reset ) {

			this -> _did_reset = false;
			this -> _fpos = 0;
			return true;
		}

		if ( int new_fsize = get_fsize(this -> fd, this -> _name, "tail"); new_fsize >= 0 )
			this -> _fsize = new_fsize;
		else return false;

		int new_lastln = get_last_ln_pos(this -> fd, this -> _name, "tail");
		this -> _fpos = this -> _fsize > 0 ? ( new_lastln < 0 ? 0 : new_lastln ) : 0;

		logger::debug << "file " << this -> _name << " was opened with size of " << this -> _fsize << " bytes and lastln at pos " << this -> _fpos << std::endl;

		return new_lastln < 0 ? false : true;

	}

	if ( !file_exists(this -> _name)) {

		logger::vverbose << "file " << this -> _name << " disappeared  un-expectedly, removed from filesystem maybe?" << std::endl;
		this -> fd.close();
		return false;
	}

	if ( int new_size = get_fsize(this -> fd, this -> _name, "tail"); new_size < 0 ) {

		return false;
	} else if ( new_size < this -> _fsize ) { // file had shrunken, reset

		logger::verbose << "file " << this -> _name << " was shrunken, resetting tailing" << std::endl;

		this -> fd.close();
		this -> _did_reset = true;

		return true;
	} else if ( new_size == this -> _fsize ) { // File size has not changed..

		logger::debug << "size of " << this -> _name << " did not change (" << new_size << " bytes)" << std::endl;
		return true;
	} else this -> _fsize = new_size;

	fd.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {

		int e_count = 0;
		int new_lastln = this -> _fpos;
		std::string entry;

		this -> fd.seekg(this -> _fpos);

		while ( !this -> fd.eof() && new_lastln < this -> _fsize ) {

			char ch;
			fd.get(ch);

			if ( ch == 0x0a ) {

				e_count++;
				this -> entries.push_back(entry);
				entry = "";
				new_lastln = fd.tellg();
				continue;
			}

			entry += ch;
		}

		if ( new_lastln == this -> _fpos )
			return true; // lastln pos remained the same
		else if ( new_lastln < this -> _fpos ) {
			logger::error << this -> _name << " error: " << (
				new_lastln == 0 ? "file abnormally did reset" :
				"nextln is less than lastnl, but not 0. Should not be possible.."
				) << std::endl;

				this -> fd.close();
				return false;
		}

		logger::debug << "readed " << e_count << " entries from " << this -> _name << std::endl;
		logger::debug << "lastln pos of " << this -> _name << " was updated: " << new_lastln << std::endl;

		this -> _fpos = new_lastln;

	} catch ( std::system_error& e ) {

		logger::error << "file error (" << this -> _name << "): " << e.code().message() << std::endl;
		return false;

	}

	return true;
}
