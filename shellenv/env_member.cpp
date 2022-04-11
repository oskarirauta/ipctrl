#include "env/utility.hpp"
#include "env/list.hpp"
#include "env/member.hpp"

env::member::operator bool() {

	if ( env::trimmed(this -> val).empty()) {

		if ( this -> parent )
			this -> parent -> get().erase(this -> _key);
		return false;
	}

	return true;
}

env::member& env::member::operator =(const std::string& val) {

	if ( this -> parent && env::trimmed(val).empty())
		this -> parent -> get().erase(this -> _key);
	else this -> val = val;

	return *this;
}
