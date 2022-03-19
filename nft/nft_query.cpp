#include <string>
#include "nft/query.hpp"

const std::string nft::Query::toString(void) {

	switch ( this -> cmd ) {
		case nft::LIST_SETS:
			return "list sets" + ( this -> family == nft::none ? "" :
				( " " + this -> family.description()));
		case nft::FLUSH_SET:
			return "flush set" +
				( this -> family == nft::none ? "" : ( " " + this -> family.description())) +
				( this -> table.empty() ? "" : ( " " + this -> table )) +
				" " + this -> set;
		case nft::LIST_ELEMENTS:
			return "list set" +
				( this -> family == nft::none ? "" : ( " " + this -> family.description())) +
				( this -> table.empty() ? "" : ( " " + this -> table )) +
				" " + this -> set;
		case nft::ADD_ELEMENTS:
			return "add element" +
				( this -> family == nft::none ? "" : ( " " + this -> family.description())) +
				( this -> table.empty() ? "" : ( " " + this -> table )) +
				( this -> set.empty() ? "" : ( " " + this -> set )) +
				" { " + this -> saddr + ( this -> timeout > 0 ? ( " timeout " + std::to_string(this -> timeout) +"s" ) : "" ) + " }";
		case nft::DELETE_ELEMENTS:
			return "delete element" +
				( this -> family == nft::none ? "" : ( " " + this -> family.description())) +
				( this -> table.empty() ? "" : ( " " + this -> table )) +
				( this -> set.empty() ? "" : ( " " + this -> set )) +
				" { " + this -> saddr + " }";
		default: return "unknown";
	}
}
