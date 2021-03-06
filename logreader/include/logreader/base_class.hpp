#pragma once

#include <string>
#include <list>
#include <mutex>

typedef void (*logreader_handler_t)(std::string name, std::string entry);

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

			void iterate(logreader_handler_t handler);

			const bool stopped(void) {

				bool r = this -> running();
				bool e = this -> exiting();
				bool a = this -> aborted();

				return a & !r && !e;
			}

	};
}
