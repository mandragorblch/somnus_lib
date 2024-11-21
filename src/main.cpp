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
	size_t s_index = 3;
	size_t count = 4;
	//size_t count = 15;
	str.insert(index, str, s_index, count);
	str1.insert(index, str1, s_index, count);

	std::cout << str << '\n' << str1;
}
	std::cout << '\n';
	system("pause");
}