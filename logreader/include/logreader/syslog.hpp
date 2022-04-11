#pragma once

#include <string>
#include <thread>

extern "C" {
#include <libubox/blobmsg_json.h>
#include "libubus.h"
}

#include "logreader/base_class.hpp"

namespace logreader {

	class syslog : public logreader::base_class {

		public:

			const inline std::string name(void) {
				return "@syslog";
			}

			const inline std::string ubus_socket(void) {
				std::lock_guard<std::mutex> lock(this -> mutex);
				return this -> _ubus_socket.empty() ? "NULL" : this -> _ubus_socket;
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
				return false;
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

			void sig_exit(void) {
				std::lock_guard<std::mutex> lock(this -> mutex);
				this -> _aborted = true;
			}

			inline bool reset(void) {

				std::lock_guard<std::mutex> lock(this -> mutex);
				if ( this -> _running || !this -> _aborted )
					return false;

				this -> entries.clear();
				this -> _running = false;
				this -> _aborted = false;
				return true;
			}

			void panic(void) {

				bool locked = false;

				for ( int i = 0; i < 10 && !locked; i++ ) { // attempto to lock several times

					locked = this -> mutex.try_lock(); // but we don't force, this is panic after all..
					if ( i != 0 && !locked )

						std::this_thread::sleep_for(std::chrono::milliseconds(4));
				}

				this -> _aborted = true;
				this -> entries.clear();

				if ( locked ) // unlock if mutex lock was possible
					this -> mutex.unlock();
			}

			syslog(std::string ubus_socket = "");

		private:

			int _delay = 50;
			bool _running = false;
			bool _exiting = false;
			bool _aborted = false;

			uint32_t _last_id = 0;

			std::string _ubus_socket;
	};

}
