#pragma once

#include <thread>
#include <cstdio>

#include "logreader/base_class.hpp"

#include <iostream>

#include <list>
#include <string>
#include <mutex>
#include <thread>

namespace logreader {

	class syslog : public logreader::base_class {

		public:

			const inline std::string name(void) {
				return "@syslog";
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

			const inline pid_t pid(void) {
				std::lock_guard<std::mutex> lock(this -> mutex);
				return this -> _pid;
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
				if ( this -> _running || this -> _exiting || this -> _pid > 0 )
					return false;

				fclose(this -> _output);
				this -> _pid = 0;
				this -> _running = false;
				this -> _exiting = false;
				this -> _aborted = false;
				return true;
			}

			void panic(void);

			syslog();

		private:

			pid_t _pid = 0;
			int _delay = 50;
			bool _running = false;
			bool _exiting = false;
			bool _aborted = false;
			int pipefd[2];
			FILE *_output;

			bool spawn_child(void);

	};

}
