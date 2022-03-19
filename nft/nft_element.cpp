#include "nft/element.hpp"

const std::chrono::seconds nft::Element::Timeout::expires(void) {
	return this -> created + this -> value;
}

const Json::Value nft::Element::Timeout::dump(void) {

	Json::Value ret;

	if ( this -> value.count() == 0 )
		return ret;

	std::chrono::seconds now = std::chrono::duration_cast<std::chrono::seconds>
		(std::chrono::system_clock::now().time_since_epoch());

	ret["value"] = this -> value.count();
	ret["created"] = this -> created.count();
	ret["expires"] = this -> expires().count();
	ret["ends"] = this -> expires().count() - now.count();

	return ret;
}

const Json::Value nft::Element::dump(void) {

	Json::Value ret;

	ret["addr"] = this -> addr;
	ret["timeout"] = this -> timeout.dump();

	return ret;
}
