#include "logreader/base_class.hpp"

void logreader::base_class::iterate(logreader_handler_t handler) {

	std::string name = this -> name();
	std::lock_guard<std::mutex> lock(this -> mutex);

	while ( !this -> entries.empty()) {

		handler(name, this -> entries.front());
		this -> entries.pop_front();
	}
}
