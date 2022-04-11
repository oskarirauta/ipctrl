#pragma once

#include <optional>
#include <functional>
#include <ostream>
#include <string>

#include "env/utility.hpp"

namespace env {

	class list; // forward declaration of env::list class

	class member {

		public:

			std::string val;

			member(const member& other) : _key(env::trimmed(other._key)), val(other.val), parent(other.parent) {}
			member(const std::string& key, const std::string& val) : _key(env::trimmed(key)), val(val), parent(std::nullopt) {

				if ( env::trimmed(key).empty() || env::trimmed(val).empty())
					throw std::runtime_error("key and value of environment member cannot be empty");
			}

			member(const std::pair<std::string, std::string>& valuepair) : _key(env::trimmed(valuepair.first)), val(valuepair.second) {

				if ( env::trimmed(valuepair.first).empty() || env::trimmed(valuepair.second).empty())
					throw std::runtime_error("key and value of environment member cannot be empty");
			}

			~member() { this -> parent = std::nullopt; }

			member& operator =(const std::string& val);

			operator bool();
			inline operator const std::string() const { return this -> val; }
			inline operator const char*() const { return this -> val.c_str(); }
			inline const std::string key() const { return this -> _key; }
			inline const std::string line() const { return this -> _key + "=" + this -> val; }
			inline const std::string toString() const { return this -> val; }

			friend std::ostream& operator<< (std::ostream& o,const env::member &m) {

				return o << m.val;
			}

		private:

			using parent_type = std::optional<std::reference_wrapper<env::list>>;

			parent_type parent;
			std::string _key;

			member() : _key(""), val(""), parent(std::nullopt) {}
			member(parent_type p) : _key(""), val(""), parent(p) {}
			member(const std::string& key, const std::string& val,
				parent_type p) : _key(key), val(val), parent(p) {}
			member(const member& other, parent_type p) :
				_key(other._key), val(other.val), parent(p) {}
			member(const std::string& key, parent_type p) :
				_key(key), val(""), parent(p) {}

			friend class env::list;

	}; // end of class env::member

} // end of namespace env
