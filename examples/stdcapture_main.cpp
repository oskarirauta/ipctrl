#include <iostream>
#include <stdcapture.hpp>

static std::capture capturer;

int main() {

	std::cout << "std::capturer test program\n" << std::endl;

	std::cout << "This text was sent to std::cout without capturing" << std::endl;
	std::cerr << "And this to std::cerr without capturing" << std::endl;
	std::cout << "\nCapturing begins" << std::endl;
	capturer.begin();

	std::cout << "This text was sent to captured std::cout" << std::endl;
	std::cerr << "And this one went to captured std::cerr" << std::endl;

	bool res = capturer.end();

	std::cout << "Capturing ended with result of " << ( res ? "success" : "failure" ) << std::endl;
	std::cout << "Captured contents:" << std::endl;
	std::cout << capturer.result << std::endl;

	return 0;

}
