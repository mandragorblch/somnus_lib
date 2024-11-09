#include <iostream>
#include <string>
#include "smns/string.h"

int main() {
	using namespace smns;
	String str("0123456789");
	std::string ref_str = str.data();

	String str1("asdfg");
	str1.assign("");

	std::cout << str << '\n' << str1;

	std::cout << '\n';
	system("pause");
}