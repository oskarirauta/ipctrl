#pragma once

/*
  Based on work of Staszek at
  https://stackoverflow.com/a/68348821
*/

#include <mutex>
#include "property.hpp"

#ifndef STD_OUT_FD
#define STD_OUT_FD (fileno(stdout))
#endif

#ifndef STD_ERR_FD
#define STD_ERR_FD (fileno(stderr))
#endif

namespace std {

	class capture {

		public:

			capture(int BUFSIZE = 1025);

			void begin(void);
			bool end(void);

			Property<std::string> result {
				this -> m_captured,
				[&](std::string &r) {
					return this -> get_locked() ?
						this -> l_captured : this -> get_result();
				}, nullptr,
			};

			Property<bool> capturing {
				this -> m_capturing,
				[&](bool &r) {
					 return this -> get_locked() ?
						this -> l_capturing : this -> get_capturing();
				}, nullptr,
			};

			Property<bool> locked {
				this -> m_locked,
				[&](bool &r) { return this -> get_locked(); },
				[&](bool &r, bool t) {
					bool c = this -> get_locked();
					if ( t && t != c ) this -> lock();
					else if ( !t && t != c ) this -> unlock();
				},
			};

			Property<int>::ReadOnly bufsize {
				this -> BUFSIZE
			};

			void lock(void);
			void unlock(void);

		private:

			int BUFSIZE = 1025;
			enum PIPES { READ, WRITE };

			int secure_dup(int src);
			void secure_pipe(int * pipes);
			void secure_dup2(int src, int dest);
			void secure_close(int & fd);

			int m_pipe[2];
			int m_oldStdOut;
			int m_oldStdErr;
			bool m_capturing;
			bool m_locked;
			std::mutex m_mutex, l_mutex;
			std::string m_captured;

			std::string get_result();
			bool get_capturing();
			bool get_locked();

			std::string l_captured;
			bool l_capturing;
	};
}
