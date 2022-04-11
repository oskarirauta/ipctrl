#pragma once

#include <memory>
#include <string>
#include <initializer_list>

#include "env/utility.hpp"
#include "env/member.hpp"

namespace env {

	class list {

		private:

			struct node {

				env::member data{};
				std::unique_ptr<node> next;

				node(const env::member& m,
					std::optional<std::reference_wrapper<env::list>> p) :
					data(member(m, p)), next(nullptr) {}
				node(const env::member& m) : data(m), next(nullptr) {}
			};

			std::unique_ptr<node> head = nullptr;
			std::size_t _size = 0;

			void push(const env::member& m) noexcept;

		public:

			list() = default;

			inline std::size_t size() noexcept { return this -> _size; }
			inline std::size_t size() const noexcept { return this -> _size; }
			inline bool empty() noexcept { return this -> _size == 0; }
			inline bool empty() const noexcept { return this -> _size == 0; }
			bool erase(const std::string& key) noexcept;

			inline void clear() noexcept {

				while ( this -> head != nullptr )
					std::exchange(this -> head, std::move(this -> head -> next));

				this -> _size = 0;
			}

			env::member& operator [](const std::string& key);

			list& operator +(const env::member& m) const noexcept;
			list& operator +(const env::list& other) const noexcept;

			list& operator +=(const env::member& m) noexcept;
			list& operator +=(const std::pair<std::string, std::string>& m) noexcept;
			list& operator +=(const std::initializer_list<std::pair<std::string, std::string>>& ilist) noexcept;
			list& operator +=(const env::list& other) noexcept;

			struct Iterator; // forward declaration of Iterator struct

			Iterator begin() const;
			Iterator end() const;

			// Constructors

			list(const std::initializer_list<env::member>& ilist) noexcept {

				for ( auto& m : ilist )
					if ( !env::trimmed(m._key).empty() && !env::trimmed(m.val).empty())
						this -> push(env::member(env::trimmed(m._key), m.val));
			}

			list(const std::string& key, const std::string& val) noexcept {

				if ( !env::trimmed(key).empty() && !env::trimmed(val).empty())
					this -> push(env::member(env::trimmed(key), val));
			}

			list(const env::list& other) {

				for ( auto& m : other )
					this -> push(m);
			}

			list(env::list &other) {

				for ( auto& m : other )
					this -> push(m);
			}

			~list() {

				while ( this -> head != nullptr )
					std::exchange(this -> head, std::move(this -> head -> next));

				this -> _size = 0;
			}

			struct Iterator {

				public:

					friend class env::list;

					Iterator() noexcept : curr(nullptr) {}
					Iterator(const std::unique_ptr<env::list::node>& node) noexcept :
						curr(node.get()) {}

					Iterator& operator ++() noexcept;
					Iterator operator ++(int) noexcept;
					bool operator !=(const Iterator& other) noexcept;
					env::member& operator *() const noexcept;
					env::member operator ->() const noexcept;

				private:

					list::node* curr;
			};

			friend std::ostream& operator << (std::ostream& o, const env::list& l) {

				if ( l.head == nullptr )
					return o;

				std::string front_key = l.head.get() -> data.key();

				for ( auto& m : l )
					o << ( front_key == m.key() ? "" : "\n"  ) << m.key() << "=" << m.val;

				return o;
			}

	}; // end of class env::list

} // end of namespace env
