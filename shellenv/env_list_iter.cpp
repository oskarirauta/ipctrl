#include "env/member.hpp"
#include "env/member.hpp"
#include "env/list.hpp"

env::list::Iterator env::list::begin() const {

	return Iterator(this -> head);
}

env::list::Iterator env::list::end() const {

	return Iterator();
}

env::list::Iterator& env::list::Iterator::operator ++() noexcept {

	if ( curr == nullptr )
		return *this;

	curr = curr -> next.get();
	return *this;
}

env::list::Iterator env::list::Iterator::operator ++(int) noexcept {

	Iterator tmp = *this;
	++*this;
	return tmp;
}

bool env::list::Iterator::operator !=(const env::list::Iterator& other) noexcept {

	return this -> curr != other.curr;
}

env::member& env::list::Iterator::operator *() const noexcept {

	return this -> curr -> data;
}

env::member env::list::Iterator::operator ->() const noexcept {

	return *(&(this -> curr -> data));
}
