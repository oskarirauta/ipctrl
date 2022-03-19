#pragma once

#include <string>
#include <list>
#include <mutex>

namespace logreader {

	class base_class {

		public:

			std::list<std::string> entries;
			std::mutex mutex;

			virtual const std::string name(void) = 0;
			virtual const int delay(void) = 0;
			virtual const bool running(void) = 0;
			virtual const bool exiting(void) = 0;
			virtual const bool aborted(void) = 0;

			virtual const bool tail(void) = 0;
			virtual void set_delay(int millis) = 0;
			virtual void sig_exit(void) = 0;
			virtual bool reset(void) = 0;
			virtual void panic(void) = 0;
	};
}
