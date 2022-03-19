#pragma once

#include <string>

namespace nft {

	enum FamilyType: uint8_t {
		none, ip, ip6, inet, arp, bridge, netdev
	};

	class Family {

		public:

			Family(std::string type);

			Family() = default;
			constexpr Family(FamilyType type) : value(type) { }

			operator FamilyType() const { return this -> value; }
			constexpr bool operator == (nft::Family family) const { return this -> value == family.value; }
			constexpr bool operator != (nft::Family family) const { return this -> value != family.value; }
			constexpr bool operator == (nft::FamilyType familyType) const { return this -> value == familyType; }
			constexpr bool operator != (nft::FamilyType familyType) const { return this -> value != familyType; }

			const nft::FamilyType type(void);
			const std::string description(void);

		private:
			nft::FamilyType value;
	};

}
