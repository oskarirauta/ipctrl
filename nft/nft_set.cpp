#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <iostream>

#include <json/json.h>

#include "common.hpp"
#include "nft/nft.hpp"
#include "nft/element.hpp"
#include "nft/set.hpp"

std::vector<nft::Set> nft::sets;

const Json::Value nft::Set::dump(void) {

	Json::Value value;

	value["family"] = this -> family.description();
	value["table"] = this -> table;
	value["name"] = this -> name;
	value["type"] = this -> type == nft::SetType::ipv4_addr ? "ipv4_addr" : (
		 this -> type == nft::SetType::ipv6_addr ? "ipv6_addr" : "not_supported" );
	value["handle"] = this -> handle;
	value["comment"] = this -> comment;

	value["flags"] = Json::arrayValue;

	for ( int i = 0; i < this -> flags.size(); i++ )
		value["flags"].append(this -> flags[i]);

	value["elements"] = Json::arrayValue;

	for ( int i = 0; i < this -> elements.size(); i++ )
		value["elements"].append(this -> elements[i].dump());

	return value;
}

const nft::SetType str2settype(const std::string type) {

	std::string _type = common::trim_ws(common::to_lower(type));

	if ( _type == "ipv4_addr" ) return nft::SetType::ipv4_addr;
	else if ( _type == "ipv6_addr" ) return nft::SetType::ipv6_addr;

	return nft::SetType::not_supported;
}

bool nft::update_sets(void) {

	nft::Query query = {
		.cmd = nft::LIST_SETS,
		.family = nft::inet,
		.table = nft::table
	};

	Json::Value result;
	if ( int ret = nft::exec(query, result); ret != 0 ) {
		std::cout << "nft::exec (LIST_SETS) error code: " << ret << std::endl;
		return false;
	}

	// std::cout << "nft::exec (LIST_SETS) json result:\n" << result << std::endl;

	if ( !result["nftables"].isArray()) {
		std::cout << "error (json): nftables node does not exist or is not array" << std::endl;
		return false;
	}

	std::vector<nft::Set> new_sets;

	for ( int i = 0; i < result["nftables"].size(); i++ ) {
		if ( !result["nftables"][i]["set"].isObject()) continue;

		nft::Set new_set = {
			.family = result["nftables"][i]["set"]["family"].isString() ?
				nft::Family(result["nftables"][i]["set"]["family"].asString()) : nft::Family(nft::none),
			.table = result["nftables"][i]["set"]["table"].isString() ?
				result["nftables"][i]["set"]["table"].asString() : "",
			.name = result["nftables"][i]["set"]["name"].isString() ?
				result["nftables"][i]["set"]["name"].asString() : "",
			.type = result["nftables"][i]["set"]["type"].isString() ?
				str2settype(result["nftables"][i]["set"]["type"].asString()) : nft::not_supported,
			.handle = result["nftables"][i]["set"]["handle"].isInt() ?
				result["nftables"][i]["set"]["handle"].asInt() : -1,
			.comment = result["nftables"][i]["set"]["comment"].isString() ?
				common::trim_ws(result["nftables"][i]["set"]["comment"].asString()) : "",
		};

		if ( result["nftables"][i]["set"]["flags"].isArray()) {
			std::vector<std::string> flags;
			for ( int i2 = 0; i2 < result["nftables"][i]["set"]["flags"].size(); i2++ )
				if ( result["nftables"][i]["set"]["flags"][i2].isString())
					flags.push_back(common::to_lower(result["nftables"][i]["set"]["flags"][i2].asString()));
			if ( flags.size() > 0 )
				new_set.flags = flags;
		}

		if ( new_set.family != nft::family || new_set.table != nft::table ||
			new_set.type == nft::not_supported || new_set.handle == -1 ) continue;

		new_set.update();
		new_sets.push_back(new_set);

		//std::cout << "set #" << i << ":\n" << result["nftables"][i]["set"] << std::endl;

	}

	nft::sets = new_sets;

	return true;

}

bool nft::Set::has_interval(void) {

	return std::find(this -> flags.begin(),
		this -> flags.end(), "interval") != this -> flags.end();
}

bool nft::Set::has_timeout(void) {

	return std::find(this -> flags.begin(),
		this -> flags.end(), "timeout") != this -> flags.end();
}

bool nft::Set::update(void) {

	if ( this -> family == nft::none || this -> table.empty() ||
		this -> name.empty() || ( this -> type != nft::ipv4_addr && this -> type != nft::ipv6_addr ))
		return false;

	nft::Query query = {
		.cmd = nft::LIST_ELEMENTS,
		.family = this -> family,
		.table = this -> table,
		.set = this -> name,
	};

	Json::Value result;

	if ( int ret = nft::exec(query, result); ret != 0 ) {
		std::cout << "nft::exec (LIST_ELEMENTS) error code: " << ret << std::endl;
		return false;
	}

	//std::cout << "nft::exec (LIST_SETS) json result:\n" << result << std::endl;

	if ( !result["nftables"].isArray()) {
		std::cout << "error (json): nftables node does not exist or is not array" << std::endl;
		return false;
	}

        std::vector<nft::Element> new_elements;

	for ( int i = 0; i < result["nftables"].size(); i++ ) {
		if ( !result["nftables"][i]["set"].isObject()) continue;
		if ( !result["nftables"][i]["set"]["name"].isString() ||
			result["nftables"][i]["set"]["name"].asString() != this -> name ) continue;
		if ( !result["nftables"][i]["set"]["elem"].isArray()) continue;

		// std::cout << "set #" << i << ":\n" << result["nftables"][i]["set"] << std::endl;

		for ( int i2 = 0; i2 < result["nftables"][i]["set"]["elem"].size(); i2++ ) {

			if ( result["nftables"][i]["set"]["elem"][i2].isString()) {
				nft::Element el = {
					.addr = result["nftables"][i]["set"]["elem"][i2].asString(),
				};

				new_elements.push_back(el);
				//new_elements.push_back(result["nftables"][i]["set"]["elem"][i2].asString());
			} else if ( result["nftables"][i]["set"]["elem"][i2].isObject() &&
				result["nftables"][i]["set"]["elem"][i2]["prefix"].isObject() &&
				result["nftables"][i]["set"]["elem"][i2]["prefix"]["addr"].isString() &&
				result["nftables"][i]["set"]["elem"][i2]["prefix"]["len"].isInt()) {
				/*
				new_elements.push_back(
					result["nftables"][i]["set"]["elem"][i2]["prefix"]["addr"].asString() +
					"/" +
					std::to_string(result["nftables"][i]["set"]["elem"][i2]["prefix"]["len"].asInt())
				);
				*/
				nft::Element el = {
					.addr = result["nftables"][i]["set"]["elem"][i2]["prefix"]["addr"].asString() +
						"/" +
						std::to_string(result["nftables"][i]["set"]["elem"][i2]["prefix"]["len"].asInt()),
				};
				new_elements.push_back(el);
			} else if ( result["nftables"][i]["set"]["elem"][i2].isObject() &&
				result["nftables"][i]["set"]["elem"][i2]["elem"].isObject() &&
				result["nftables"][i]["set"]["elem"][i2]["elem"]["timeout"].isInt() &&
				result["nftables"][i]["set"]["elem"][i2]["elem"]["expires"].isInt() &&
				result["nftables"][i]["set"]["elem"][i2]["elem"]["val"].isString()) {

					//new_elements.push_back(result["nftables"][i]["set"]["elem"][i2]["elem"]["val"].asString());

					int timeout = result["nftables"][i]["set"]["elem"][i2]["elem"]["timeout"].asInt();
					int expires = result["nftables"][i]["set"]["elem"][i2]["elem"]["expires"].asInt();

					std::chrono::seconds now = std::chrono::duration_cast<std::chrono::seconds>
						(std::chrono::system_clock::now().time_since_epoch());

					nft::Element el = {
						.addr = result["nftables"][i]["set"]["elem"][i2]["elem"]["val"].asString(),
						.timeout = {
							.value = std::chrono::seconds(timeout),
							.created = now + std::chrono::seconds(expires) - std::chrono::seconds(timeout),
						},
					};

					new_elements.push_back(el);
					//std::cout << el.dump() << std::endl;;

			} else if ( result["nftables"][i]["set"]["elem"][i2]["range"].isArray() &&
				result["nftables"][i]["set"]["elem"][i2]["range"][0].isString() &&
				result["nftables"][i]["set"]["elem"][i2]["range"][1].isString()) {

				/*
				new_elements.push_back(
					result["nftables"][i]["set"]["elem"][i2]["range"][0].asString() +
					"-" +
					result["nftables"][i]["set"]["elem"][i2]["range"][1].asString()
				);
				*/

				nft::Element el = {
					.addr = result["nftables"][i]["set"]["elem"][i2]["range"][0].asString() +
						"-" +
						result["nftables"][i]["set"]["elem"][i2]["range"][1].asString()
				};

				new_elements.push_back(el);

			} else if ( result["nftables"][i]["set"]["elem"][i2].isObject() &&
				result["nftables"][i]["set"]["elem"][i2]["elem"].isObject() &&
				result["nftables"][i]["set"]["elem"][i2]["elem"]["timeout"].isInt() &&
				result["nftables"][i]["set"]["elem"][i2]["elem"]["expires"].isInt() &&
				result["nftables"][i]["set"]["elem"][i2]["elem"]["val"].isObject() &&
				result["nftables"][i]["set"]["elem"][i2]["elem"]["val"]["range"].isArray() &&
				result["nftables"][i]["set"]["elem"][i2]["elem"]["val"]["range"][0].isString() &&
				result["nftables"][i]["set"]["elem"][i2]["elem"]["val"]["range"][1].isString()) {

				/*
				new_elements.push_back(
					result["nftables"][i]["set"]["elem"][i2]["elem"]["val"]["range"][0].asString() +
					"-" +
					result["nftables"][i]["set"]["elem"][i2]["elem"]["val"]["range"][1].asString()
				);
				*/

				int timeout = result["nftables"][i]["set"]["elem"][i2]["elem"]["timeout"].asInt();
				int expires = result["nftables"][i]["set"]["elem"][i2]["elem"]["expires"].asInt();

				std::chrono::seconds now = std::chrono::duration_cast<std::chrono::seconds>
					(std::chrono::system_clock::now().time_since_epoch());

				nft::Element el = {
					.addr = result["nftables"][i]["set"]["elem"][i2]["elem"]["val"]["range"][0].asString() +
						"-" +
						result["nftables"][i]["set"]["elem"][i2]["elem"]["val"]["range"][1].asString(),
					.timeout = {
						.value = std::chrono::seconds(timeout),
						.created = now + std::chrono::seconds(expires) - std::chrono::seconds(timeout),
					},
				};

				new_elements.push_back(el);

			} else std::cout << "UNKNOWN TYPE!\n" << result["nftables"][i]["set"]["elem"][i2] << std::endl;
		}
	}

	this -> elements = new_elements;

	return true;
}

bool validate_addr_array(std::string &addr, std::string &err, nft::SetType type) {

	std::string saddr(addr);
	saddr.erase(std::remove(saddr.begin(), saddr.end(), ' '), saddr.end());
	std::vector<std::string> addresses = common::split(saddr, ',');

	if ( addresses.size() < 1 ) {

		err = "ip address list incorrectly formatted: " + addr;
		return false;
	}

	saddr = "";

	for ( int i = 0; i < addresses.size(); i++ ) {

		nft::SetType atype = nft::addrType(addresses[i]);

		if ( atype == nft::SetType::not_supported ) {
			err = addresses[i] + " is not IP address";
			return false;
		} else if ( type == nft::SetType::ipv4_addr && atype != type ) {
			err = "address " + addresses[i] + " is not IPv4 address";
			return false;
		} else if ( type == nft::SetType::ipv6_addr && atype != type ) {
			err = "address " + addresses[i] + " is not IPv6 address";
			return false;
		}

		saddr += ( i == 0 ? "" : ", " ) + addresses[i];
	}

	addr = saddr;
	return true;
}

bool nft::Set::add(const std::string addr, int timeout) {

	if ( this -> family == nft::none || this -> table.empty() ||
		this -> name.empty() || ( this -> type != nft::ipv4_addr && this -> type != nft::ipv6_addr ))
		return false;

	nft::Query query = {
		.cmd = nft::ADD_ELEMENTS,
		.family = this -> family,
		.table = this -> table,
		.set = this -> name,
		.saddr = addr,
		.timeout = timeout > 0 ? timeout : 0,
        };

	std::string err;
	if ( !validate_addr_array(query.saddr, err, this -> type)) {
		std::cout << "nft: exec (ADD_ELEMENTS) argument format error: " << err << std::endl;
		return false;
	}

	Json::Value result;

	if ( int ret = nft::exec(query, result); ret != 0 ) {
		std::cout << "nft::exec (ADD_ELEMENTS) error code: " << ret << std::endl;
		return false;
	}

	//std::cout << "nft::exec (ADD_ELEMENTS) result:\n" << result << std::endl;
	return true;
}

bool nft::Set::remove(const std::string addr) {

	if ( this -> family == nft::none || this -> table.empty() ||
		this -> name.empty() || ( this -> type != nft::ipv4_addr && this -> type != nft::ipv6_addr ))
		return false;

	nft::Query query = {
		.cmd = nft::DELETE_ELEMENTS,
		.family = this -> family,
		.table = this -> table,
		.set = this -> name,
		.saddr = addr,
	};

	std::string err;

	if ( !validate_addr_array(query.saddr, err, this -> type)) {
		std::cout << "nft: exec (DELETE_ELEMENTS) argument format error: " << err << std::endl;
		return false;
	}

	Json::Value result;

	if ( int ret = nft::exec(query, result); ret != 0 ) {
		std::cout << "nft::exec (DELETE_ELEMENTS) error code: " << ret << std::endl;
		return false;
	}

	// std::cout << "nft::exec (ADD_ELEMENTS) result:\n" << result << std::endl;
	return true;
}


bool nft::Set::flush(void) {

	if ( this -> family == nft::none || this -> table.empty() ||
		this -> name.empty() || ( this -> type != nft::ipv4_addr && this -> type != nft::ipv6_addr ))
		return false;

	nft::Query query = {
		.cmd = nft::FLUSH_SET,
		.family = this -> family,
		.table = this -> table,
		.set = this -> name,
	};

	Json::Value result;

	if ( int ret = nft::exec(query, result); ret != 0 ) {
		std::cout << "nft::exec (FLUSH_SET) error code: " << ret << std::endl;
		return false;
	}

	// std::cout << "nft::exec (ADD_ELEMENTS) result:\n" << result << std::endl;
	return true;
}
