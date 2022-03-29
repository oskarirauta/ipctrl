#pragma once

#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <chrono>
#include <vector>
#include <list>
#include <map>

#ifndef EVENT_LOG_MAX_SIZE
# define EVENT_LOG_MAX_SIZE 100
#endif

namespace logger {

	using endl_type = std::ostream& (std::ostream&);

	inline std::ostream *output_stream = &std::cout; // set to nullptr to quiet
	inline std::ostream *error_stream = &std::cerr; // set to nullptr to quiet
	inline std::ostream *file_stream = nullptr;

	enum type: uint8_t {
		info = 0,
		error = 1,
		verbose,
		vverbose,
		debug = 254,
		ANY = 255
	};

	inline std::ostream &endl(std::ostream &stream) {

		stream << "\x1B\n";
		return stream;
	}

	inline std::ostream& operator << (std::ostream &os, logger::type type) {
		switch ( type ) {
			case logger::type::info: return os << "info";
			case logger::type::error: return os << "error";
			case logger::type::verbose: return os << "verbose";
			case logger::type::vverbose: return os << "vverbose";
			case logger::type::debug: return os << "debug";
			case logger::type::ANY: return os << "ANY";
			default: return os << "unknown";
		}
	}

	inline const std::string description(const logger::type type) {
		switch ( type ) {
			case logger::type::error: return "error";
			case logger::type::debug: return "debug";
			default: return "info";
		}
	}

	inline std::map<logger::type, bool> output_level {
		{ static_cast<logger::type>(0), true },
		{ static_cast<logger::type>(1), true },
	};

	struct entry {

		public:
			logger::type type = static_cast<logger::type>(0);
			std::chrono::seconds timestamp = std::chrono::duration_cast<std::chrono::seconds>
								(std::chrono::system_clock::now().time_since_epoch());
			std::chrono::seconds timestamp_last = timestamp;
			std::string msg;
			std::string description;
			int count = 0;

			const bool equals(logger::entry rhs);
			inline const bool hasDescription() {

				return !this -> description.empty();
			};

	};

	namespace _private { // anonymous namespace / private member(s)

		struct detailTxt {

			public:

				std::string str;

				detailTxt(const std::string str) : str(str) { }
		};

		inline std::list<logger::entry> store;
		inline std::map<logger::type, std::stringstream> _stream;
		inline std::map<logger::type, std::string> _detail;
		inline std::string _last_msg;

		const std::list<logger::entry> filtered(void);
		const int lastIndexOf(const logger::type type, const std::string msg);
		const bool typeShouldEcho(const logger::type type, const bool screenOnly = false);
		void process_entry(const logger::type type, const std::string msg, const bool entry_only = false, const std::string detailTxt = "");
		void flush(const logger::type type);
		bool endOfEntry(const logger::type &f);
	}

	inline const logger::_private::detailTxt detail(const std::string s) {

		std::string str = s;
		while ( str.back() == '\n' )
			str.pop_back();
		return logger::_private::detailTxt(str);
	}

	template<typename T>
	inline const logger::type& operator << ( const logger::type &f, const T input) {
		logger::_private::_stream[f] << input;
		if ( logger::_private::endOfEntry(f))
			logger::_private::flush(f);
		return f;
	};

	inline const logger::type& operator << ( const logger::type &f, const logger::endl_type endl) {
		logger::_private::_stream[f] << endl;
		if ( logger::_private::endOfEntry(f))
			logger::_private::flush(f);
		return f;
	};

	inline const logger::type& operator << ( const logger::type &f, const logger::_private::detailTxt detail) {
		logger::_private::_detail[f] = detail.str.empty() ? "\x1B" : detail.str;
		return f;
	};

	const std::vector<logger::entry> last(int count, const logger::type type = logger::type::ANY);
};
