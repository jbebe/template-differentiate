#include <iostream>
#include <string>
#include <cmath>
#include <cxxabi.h>
#include <memory>

#include "ctd.hpp"

int main(int argc, char** argv) {
    
	// using the namespace for shorter code
	using namespace ctd;
	
	// define symbolic variables 
	auto x = symbol<1>::value{10};
	auto y = symbol<2>::value{11};
	auto z = symbol<3>::value{12};
	
	// define function (cannot be constant because of the auto keyword)
    auto f = 3 * x;
    
	// print out the tree and then the partial derivative
	std::cout.precision(30);
    std::cout << std::fixed << f.value(0, 0) << std::endl;
    std::cout << std::fixed << f.diff(1, 5.0) << std::endl;
	
	// get the expression template tree
    std::cout << "expression tree: " << std::endl;
    const char *type_ptr = abi::__cxa_demangle(typeid(f).name(), 0, 0, NULL);
	/*
	// and print it out in a fancy form (implementation is ugly though)
	std::string tab{"\n"};
	bool indent_happened = false;
	bool left_indent = false;
	bool comma = false;
	while (*type_ptr != '\0'){
		switch (*type_ptr){
			case '<':
				putchar('<');
				tab.append("   ");
				indent_happened = true;
				break;
			case '>':
				tab.resize(tab.length()-3);
				indent_happened = true;
				left_indent = true;
				break;
			case ',':
				comma = true;
				break;
			default:
				if (indent_happened){
					printf("%s", (tab.c_str()));
					indent_happened = false;
				}
				if (left_indent){
					putchar('>');
					left_indent = false;
				}
				if (comma){
					printf(",%s", (tab.c_str()));
					comma = false;
				}
				putchar(*type_ptr);
		}
		type_ptr++;
		
	}*/
	std::cout << type_ptr << std::endl;

    return 0;
}
