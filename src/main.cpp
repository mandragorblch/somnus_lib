#include <iostream>
#include <string>
#include "smns/string.h"

int main() {
{	
	using namespace smns;
	String str1("0123456789");
	str1.reserve(100);
	std::string str(str1.data());
	str.reserve(100);
	size_t index = 2;
	//size_t count = 15;
	str.insert(index, str);
	str1.insert(index, str1);

	std::cout << str << '\n' << str1;
}
	std::cout << '\n';
	system("pause");
}