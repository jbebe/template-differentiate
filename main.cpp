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

	TEST(    x,  'x', 5,   6, std::cout);
	TEST(    x,  'y', 5, 6.1, std::cout);
	TEST(1 + x,  'x', 5, 6.1, std::cout);
	TEST(x + 1,  'x', 5, 6.1, std::cout);
	TEST(2 * x,  'x', 5, 6.1, std::cout);
	TEST(x * 2,  'x', 5, 6.1, std::cout);
	TEST(x * x,  'x', 5, 6.1, std::cout);
	TEST(1 / x,  'x', 5, 6.1, std::cout);
	TEST(x / 1,  'x', 5, 6.1, std::cout);
	TEST(x / x,  'x', 5, 6.1, std::cout);
	TEST(x ^ 2,  'x', 5, 6.1, std::cout);
	TEST(2 ^ x,  'x', 5, 6.1, std::cout);
	TEST(x ^ x,  'x', 5, 6.1, std::cout);
	TEST(sin(x), 'x', 5, 6.1, std::cout);
	TEST(cos(x), 'x', 5, 6.1, std::cout);
	TEST(log(x), 'x', 5, 6.1, std::cout);
	
	TEST((sin(cos(log((((((2 * (((1 + x) + 1) * 2)) * x) / x) ^ 2) ^ x))))), 
		'x', 5, 6.1, std::cout);
	
	// get the expression template tree
	auto sample_tree = sin(cos(log((((((2 * (((1 + x) + 1) * 2)) * x) / x) ^ 2) ^ x))));
    std::cout << "expression tree: " << std::endl;
    const char *type_ptr = abi::__cxa_demangle(typeid(sample_tree).name(), 0, 0, NULL);
	std::cout << type_ptr << std::endl;
	
	// fancy print:
	auto print_expr = sin(2*x);
	std::cout << "fancy print tree: " << print_expr.printValue('x') << std::endl;
	std::cout << "fancy print derivative: " << print_expr.printDiff('x') << std::endl;

    return 0;
}
