#include "env/member.hpp"
#include "env/list.hpp"

void env::list::push(const env::member& m) noexcept {

	std::string key(env::trimmed(m._key));
	if ( key.empty()) // do not add member with empty key
		return;
	else if ( env::trimmed(m.val).empty()) { // value is empty, remove key from list

		this -> erase(key);
		return;
	}

	env::member trimmed(env::trimmed(m._key), m.val, std::ref(*this));

	if ( this -> head == nullptr ) { // list is empty

		this -> head = std::make_unique<node>(env::member(trimmed, std::ref(*this)));
		this -> _size++;
		return;
	}

	node* prev = nullptr;
	node* curr = this -> head.get();

	while ( curr != nullptr && curr -> data._key != key ) {

		prev = curr;
		curr = curr -> next.get();
	}

	if ( curr == nullptr ) { // push_back

		prev -> next = std::make_unique<node>(env::member(trimmed, std::ref(*this)));
		this -> _size++;
		return;
	}

	// entry existed, so change value only.. and delete from list if empty..
	curr -> data.val = m.val;
}

bool env::list::erase(const std::string& key) noexcept {

	if ( this -> head == nullptr ) // list is empty, don't bother
		return false;

	std::string _key = env::trimmed(key);
	node* curr = this -> head.get();

	if ( curr -> data._key == _key ) { // member to remove is at head..

		std::unique_ptr<node> tmp = std::move(curr -> next);
		std::swap(this -> head, tmp);
		this -> _size--;
		return true;
	}

	while ( curr -> next != nullptr ) {

		node* next = curr -> next.get();
		if ( next -> data._key == _key ) {

			std::swap(curr -> next, next -> next);
			next -> next = nullptr;
			this -> _size--;
			return true;
		}

		curr = curr -> next.get();
	}

	return false;
}

env::member& env::list::operator [](const std::string& key) {

	std::string _key = env::trimmed(key);

	if ( _key.empty())
		throw std::runtime_error("key on env::list (or env::member either..) cannot be empty");

	if ( this -> head == nullptr ) {

		this -> head = std::make_unique<node>(env::member(_key, std::ref(*this)));
		this -> _size++;
		return this -> head -> data;
	}

	node* prev = nullptr;
	node* curr = this -> head.get();

	while ( curr != nullptr && curr -> data._key != _key ) {

		prev = curr;
		curr = curr -> next.get();
	}

	if ( curr == nullptr ) {

		prev -> next = std::make_unique<node>(member(_key, std::ref(*this)));
		this -> _size++;
		return prev -> next -> data;
	}

	return curr -> data;
}

env::list& env::list::operator +(const env::member& m) const noexcept {

	env::list* n = new env::list(*this);
	if ( !env::trimmed(m._key).empty() && !env::trimmed(m.val).empty())
		n -> push(env::member(env::trimmed(m._key), m.val));
	return *n;
}

env::list& env::list::operator +(const env::list& other) const noexcept {

	env::list* n = new env::list(*this);

	if ( other.head != nullptr ) {

		node* curr = other.head.get();

		while ( curr != nullptr ) {
			n -> push(env::member(curr -> data._key, curr -> data.val));
			curr = curr -> next.get();
		}
	}
	return *n;
}

env::list& env::list::operator +=(const env::member& m) noexcept {

	if ( !env::trimmed(m._key).empty() && !env::trimmed(m.val).empty())
		this -> push(env::member(env::trimmed(m._key), m.val));
	return *this;
}

env::list& env::list::operator +=(const std::pair<std::string, std::string>& m) noexcept {

	this -> push(env::member(m.first, m.second));
	return *this;
}

env::list& env::list::operator +=(const std::initializer_list<std::pair<std::string, std::string>>& ilist) noexcept {

	for ( auto m : ilist )
		this -> push(env::member(m.first, m.second));
	return *this;
}

env::list& env::list::operator +=(const env::list& other) noexcept {

	if ( other.head == nullptr )
		return *this;

	node* curr = other.head.get();

	while ( curr != nullptr ) {
		this -> push(env::member(curr -> data._key, curr -> data.val, std::ref(*this)));
		curr = curr -> next.get();
	}
	return *this;
}
