#include <iostream>
#include <string>
#include <cstring>
#include <iomanip>
#include <cmath>
#include <cxxabi.h>
#include <memory>
#include <functional>

#include "ctd.hpp"

template <typename Function>
void test_value(std::string stringified, Function func, int id, double at, double expected, std::basic_ostream<char> &output = std::cout){
	double val = ctd::value(func, id, at);
	char strVal[512];
	char strExpected[512];
	
	std::sprintf(strVal, "%.10f", val);
	std::sprintf(strExpected, "%.10f", expected);
	int maxLen = std::min(strlen(strVal), strlen(strExpected));
	
	int errPos = -1;
	for (int i = 1; i < maxLen && (errPos = i) && std::strncmp(strVal, strExpected, i) == 0; i++);
	
	if (errPos == -1){
		output << "[OK]\n";
	}
	else {
		output << "[ERROR]\n";
		output << "in: \"" << stringified << "\"\n";
		output << strExpected << " (expected)\n";
		output << strVal << " (value)\n";
		output << std::setw(errPos+1) << "^\n";
	}
}

#define TEST(FUNC,ID,AT,EX,OUT) test_value((#FUNC), (FUNC), (ID), (AT), (EX), (OUT))

int main(int argc, char** argv) {
    
	// using the namespace for shorter code
	using namespace ctd;
	
	// define symbolic variables 
	auto x = symbol<1>::value{10};
	auto y = symbol<2>::value{11};
	auto z = symbol<3>::value{12};
	
	TEST(    x,  1, 5,   6, std::cout);
	TEST(    x,  2, 5, 6.1, std::cout);
	TEST(1 + x,  1, 5, 6.1, std::cout);
	TEST(x + 1,  1, 5, 6.1, std::cout);
	TEST(2 * x,  1, 5, 6.1, std::cout);
	TEST(x * 2,  1, 5, 6.1, std::cout);
	TEST(x * x,  1, 5, 6.1, std::cout);
	TEST(1 / x,  1, 5, 6.1, std::cout);
	TEST(x / 1,  1, 5, 6.1, std::cout);
	TEST(x / x,  1, 5, 6.1, std::cout);
	TEST(x ^ 2,  1, 5, 6.1, std::cout);
	TEST(2 ^ x,  1, 5, 6.1, std::cout);
	TEST(x ^ x,  1, 5, 6.1, std::cout);
	TEST(sin(x), 1, 5, 6.1, std::cout);
	TEST(cos(x), 1, 5, 6.1, std::cout);
	TEST(log(x), 1, 5, 6.1, std::cout);
	
	TEST((sin(cos(log((((((2 * (((1 + x) + 1) * 2)) * x) / x) ^ 2) ^ x))))), 
		1, 5, 6.1, std::cout);
	
	auto sample_tree = sin(cos(log((((((2 * (((1 + x) + 1) * 2)) * x) / x) ^ 2) ^ x))));
	
	// get the expression template tree
    std::cout << "expression tree: " << std::endl;
    const char *type_ptr = abi::__cxa_demangle(typeid(sample_tree).name(), 0, 0, NULL);
	
	// and print it out in a fancy form (implementation is ugly though)
	std::cout << type_ptr << std::endl;

    return 0;
}
