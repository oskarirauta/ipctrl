#include <iostream>
#include "nft/nft.hpp"

int main() {
/*
	std::cout << "hello" << std::endl;

	nft::Query query {
		.cmd = nft::LIST_SETS,
//		.family = nft::inet
	};

	std::cout << "family: " << query.family.description() << std::endl;
	std::cout << "cmd: " << query.toString() << std::endl;

	std::string result = "";
	int ret = nft::exec(query, result);

	std::cout << "nft exec return code: " << ret << std::endl;
	std::cout << "nft exec result:\n" << result << std::endl;
*/
	if ( !nft::update_sets()) {
		std::cout << "nft::update_sets error" << std::endl;
	}

//	nft::sets[0].add("100.100.100.1 - 100.100.100.3");
//	nft::sets[0].add("102.102.0.1 - 102.102.0.4", 500);
//	nft::sets[0].add("106.100.100.1", 560);
//	nft::sets[0].add("111.112.113.114");
	//nft::sets[0].add("999.999.998.887");
	//nft::sets[0].add("auto nominen");
//	nft::sets[0].add("201.199.129.1/9");
	//nft::sets[1].add("19.18.19.1");

//	nft::sets[0].update();
//	nft::sets[1].update();
/*
	std::cout << "{ \"sets\": [";
	for ( int i = 0; i < nft::sets.size(); i++ ) {
		std::cout << nft::sets[i].dump() << ( i < nft::sets.size() - 1 ? ",\n" : "\n" );
	}
	std::cout << "]}" << std::endl;
*/

	std::cout << nft::dump() << std::endl;

	return 0;
}
