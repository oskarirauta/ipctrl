#include <iostream>
#include <unistd.h>
#include "env/env.hpp"
#include "env/list.hpp"
#include "shell/exec.hpp"

int main() {

	env::list l1({
		{ "key1", "k1" },
		{ "key2", "k3" },
	});

	// note: l2 has also key key2, with different value than l1

	env::list l2({
		{ "key2", "k2" },
		{ "key3", "k3" },
		{ "key4", "k4" },
	});

	// add some more keys to l2..

	l2 += {
		{ "key5", "k5" },
		{ "key6", "k6" },
	};

	// check previous note, now when l1 and l2 are combined in l3, key2 of
	// list added last will override previously set value
	// while at it- why not add one more key as well.

	// note: when using + operator, std::initializer list cannot be used,
	// if you want to add array, create another list and add that - or add
	// one by one as members.

	env::list l3 = l1 + l2 + env::member("key7", "k7");

	// output l3 list so we can see it's contents

	std::cout << "\ncontents of l3:\n" << l3 << std::endl;

	std::cout << "\nTesting if keys exist in list:" << std::endl;
	std::cout << "[\"key3\"] in list list l3 does " <<  ( l3["key3"] ? ( "exists and has value \"" + (std::string)l3["key3"] + "\"" ) : "not exist" ) << std::endl;
	std::cout << "[\"key9\"] in list list l3 does " <<  ( l3["key9"] ? ( "exists and has value \"" + (std::string)l3["key9"] + "\"" ) : "not exist" ) << std::endl;
	std::cout << "\nl3[\"key3\"] = " << l3["key3"] << std::endl;

	pid_t my_pid = ::getpid();
	std::cout << "\nretrieving env for pid " << my_pid << ":" << std::endl;
	std::cout << env::getenv(my_pid) << std::endl;

#ifdef OSX
	std::string cmd("/usr/bin/env");
#else
	std::string cmd("env");
#endif

	std::cout << "\nExecuting " << cmd << " with shell::exec with default\n" <<
			"shell env  + environmental key hello with value world\n\n" <<
			"result of exec:" << std::endl;

	shell::exec(cmd, "", env::getenv() + env::member("hello", "world"));

	return 0;
}
