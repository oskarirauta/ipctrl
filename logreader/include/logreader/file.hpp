#pragma once

#include <thread>
#include <cstdio>

#include "logreader/base_class.hpp"

#include <iostream>

#include <list>
#include <string>
#include <mutex>
#include <thread>
#include <fstream>

namespace logreader {

	class file : public logreader::base_class {

		public:

			const inline std::string name(void) {
				std::lock_guard<std::mutex> lock(this -> mutex);
				return this -> _name;
			}

			const inline int last_fpos(void) {
				std::lock_guard<std::mutex> lock(this -> mutex);
				return this -> _fpos;
			}

			const inline int last_fsize(void) {
				std::lock_guard<std::mutex> lock(this -> mutex);
				return this -> _fsize;
			}

			const inline int filesize(void) {

				std::lock_guard<std::mutex> lock(this -> mutex);

				if ( !this -> fd.is_open())
					return -1;

				int cur = this -> fd.tellg();
				this -> fd.seekg(0, std::ios::end);
				int sz = fd.tellg();
				this -> fd.seekg(cur, std::ios::beg);
				return sz;
			}

			const inline int delay(void) {
				std::lock_guard<std::mutex> lock(this -> mutex);
				return this -> _delay;
			}

			const inline bool running(void) {
				std::lock_guard<std::mutex> lock(this -> mutex);
				return this -> _running;
			}

			const inline bool exiting(void) {
				std::lock_guard<std::mutex> lock(this -> mutex);
				return this -> _exiting;
			}

			const inline bool aborted(void) {
				std::lock_guard<std::mutex> lock(this -> mutex);
				return this -> _aborted;
			}

			const bool tail(void);

			inline void set_delay(int millis) {
				std::lock_guard<std::mutex> lock(this -> mutex);
				this -> _delay = millis;
			}

			inline void sig_exit(void) {
				std::lock_guard<std::mutex> lock(this -> mutex);
				this -> _aborted = true;
			}

			inline bool reset(void) {
				std::lock_guard<std::mutex> lock(this -> mutex);
				if ( this -> _running || this -> _exiting )
					return false;

				this -> fd.close();
				this -> _running = false;
				this -> _exiting = false;
				this -> _aborted = false;
				return true;
			}

			inline void panic(void) {

				// TODO:
				// try locking several times for some time, for example- 5ms

				bool locked = this -> mutex.try_lock(); // Do not force mutex, this is panic
				this -> _aborted = true;
				this -> entries.clear();
				this -> fd.close();

				if ( locked ) // unlock if mutex lock was possible
				this -> mutex.unlock();
			}

			file(std::string filename)  : _name(filename) {}

		private:

			std::string _name;
			std::ifstream fd;
			int _fpos = 0;
			int _fsize = 0;

			int _delay = 50;
			bool _running = false;
			bool _exiting = false;
			bool _aborted = false;

	};

}
