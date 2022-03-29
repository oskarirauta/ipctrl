#include <iostream>
#include <algorithm>

#include "common.hpp"
#include "logger.hpp"

#ifdef LOG_DUPLICATE_MSG
static const std::string _dup_msg_str = LOG_DUPLICATE_MSG;
#else
static const std::string _dup_msg_str = "(duplicate message atleast once)";
#endif

const bool logger::entry::equals(logger::entry rhs) {

	return ( this -> type == logger::type::ANY ?
		static_cast<logger::type>(0) : this -> type ) == rhs.type &&
		common::to_lower(this -> msg) == common::to_lower(rhs.msg);
}

const std::list<logger::entry> logger::_private::filtered(void) {

	std::list<logger::entry> _filtered;
	for ( auto it = logger::_private::store.begin(); it != logger::_private::store.end(); ++it ) {
		if ( logger::output_level[it -> type])
			_filtered.push_back(*(it));
	}
	return _filtered;
}

const int logger::_private::lastIndexOf(const logger::type type, const std::string msg) {

	auto idx = std::find_if(
		logger::_private::store.rbegin(),
		logger::_private::store.rend(),
		[&](logger::entry e) { return e.type == type ? true : false; });

	return idx == logger::_private::store.rend() ? 0 :
		( common::to_lower(msg) == common::to_lower(idx -> msg) ? std::distance(idx, logger::_private::store.rend()) : 0);
}

const bool logger::_private::typeShouldEcho(const logger::type type, const bool screenOnly) {

	if ( type == static_cast<logger::type>(1) && // error type
		logger::error_stream != nullptr ) return true;
	else if ( logger::output_stream != nullptr || (
		!screenOnly && logger::file_stream != nullptr ))
		return true;
	else return false;
}

void logger::_private::process_entry(const logger::type type, const std::string msg,
		const bool entry_only, const std::string detailTxt) {

	std::string entry_msg = common::trim_ws(msg);

	if ( entry_msg.empty())
		return;

	logger::entry _last = logger::_private::store.size() > 0 ?
		logger::_private::store.back() : logger::entry({
			.type = logger::type::ANY,
			.msg = "",
	});

	logger::entry _entry = {
		.type = type == logger::type::ANY ?
			static_cast<logger::type>(0) : type,
		.msg = entry_msg,
		.description = detailTxt == "\x1B" ? "" : common::trim_ws(detailTxt),
		.count = 1,
	};

	int _last2_idx = logger::_private::lastIndexOf(_entry.type, entry_msg);

	if ( _entry.equals(_last)) {
		_entry.timestamp = _last.timestamp;
		_entry.timestamp_last = ( _entry.timestamp > _last.timestamp ?
			_entry.timestamp : _last.timestamp);
		_entry.count = _last.count + 1;
		_entry.description = _entry.description.empty() && detailTxt != "\x1B" ?
			_last.description : _entry.description;
		_last2_idx = 0;
	}

	std::list<logger::entry> filtered_list = logger::_private::filtered();
	logger::entry _last_filtered = filtered_list.size() > 0 ?
		filtered_list.back() : logger::entry({
			.type = logger::type::ANY,
			.msg = "",
	});

	if ( _entry.count > 1 )
		logger::_private::store.pop_back();

	while ( logger::_private::store.size() >= EVENT_LOG_MAX_SIZE + 1 )
		logger::_private::store.pop_front();

	if ( !entry_only && logger::_private::typeShouldEcho(type) &&
		logger::output_level[_entry.type]) {
		bool equals = _entry.equals(_last_filtered);
		if ( !equals || ( equals && logger::_private::_last_msg != _dup_msg_str )) {

			std::string _msg = equals ? _dup_msg_str : _entry.msg;
			if ( type != static_cast<logger::type>(1) &&
				logger::output_stream != nullptr)
				*logger::output_stream << _msg << std::endl;
			 else if ( type == static_cast<logger::type>(1) &&
				logger::error_stream != nullptr )
				*logger::error_stream << _msg << std::endl;

			if ( logger::file_stream != nullptr )
				*logger::file_stream << "[" << _entry.type << "] " << _msg << std::endl;

			logger::_private::_last_msg = _msg;
		}
	}

	if ( !_last2_idx )
		logger::_private::store.push_back(_entry);
	else if ( auto it = _last2_idx > 1 ?
			std::next(logger::_private::store.begin(), _last2_idx - 1) :
			logger::_private::store.begin();
		it != logger::_private::store.end()) {

		it -> count++;
		it -> description = detailTxt == "\x1B" ? "" : (
			_entry.description.empty() ? it -> description :
				_entry.description);
	}
}

void logger::_private::flush(const logger::type type) {
	logger::_private::_stream[type].str(std::string());
	logger::_private::_detail[type].clear();
	logger::_private::_stream[type].flush();
}

bool logger::_private::endOfEntry(const logger::type &f) {

	if ( logger::_private::_stream[f].str().find_first_of('\n', 0) !=
		std::string::npos ) {

		bool just_entry = false;
		std::string str, d = logger::_private::_detail[f];
		std::getline(logger::_private::_stream[f], str);

		if ( str.back() == 0x1B ) {
			just_entry = true;
			str.pop_back();
		}

		while ( d.back() == '\n' )
			d.pop_back();

		logger::_private::process_entry(f, str, just_entry, d);
		return true;
	}

	return false;
}

const std::vector<logger::entry> logger::last(int count, const logger::type type) {

	std::vector<logger::entry> store;
	if ( logger::_private::store.size() == 0 )
		return store;

	while ( logger::_private::store.size() >= EVENT_LOG_MAX_SIZE + 1 )
		logger::_private::store.pop_front();

	count = count == 0 || logger::_private::store.size() < count ?
		logger::_private::store.size() : count;

	auto it = count == logger::_private::store.size() ?
		logger::_private::store.begin() :
			std::next(logger::_private::store.begin(),
				logger::_private::store.size() - count);

	for ( auto itl = it; itl != logger::_private::store.end(); ++itl )
		store.push_back(*itl);

	return store;
}
