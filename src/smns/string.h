#pragma once
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <iostream>

namespace smns{
	struct String;
}

extern int main();
std::ostream& operator<<(std::ostream& out, const smns::String& str);

namespace smns{
	struct String {
	private:
		size_t sz = 0;
		size_t cap = 0;
		char* _pointer = nullptr;
		static const size_t npos = -1;
	public:
		String();

		explicit String(size_t sz);

		explicit String(char symb);

		explicit String(const char* c_str);

		String(const char* c_str, size_t sz);

		String(size_t sz, char symb);
		
		String(std::initializer_list<char> list);

        String(const String& other);//1 copy constructor

		//copy and swap idiom
		String& operator=(String other);//2 copy assignment operator

		void swap(String& other);

		String& assign(size_t count, char symb);

		String& assign(const String& other);

		String& assign(const String& other, size_t pos, size_t count = npos);

		String& assign(const char* c_str, size_t count);

		String& assign(const char* c_str);

		//--------------------------------------------------------data access--------------------------------------------------------
        char& operator[](size_t index);

        const char& operator[](size_t index) const;

		char& at(size_t index);

		const char& at(size_t index) const;

		const char* data() const;

		char* data();

		const char* c_str() const;

		char& back();

		const char& back() const;

		char& front();

		const char& front() const;

		size_t size() const;

		explicit operator const char*();

		explicit operator char*();


		~String();//3 -> Rule of Three

		friend int ::main();
		friend std::ostream& ::operator<<(std::ostream& out, const smns::String& str);
	};

	namespace literals::string_literals{
		String operator""_s(const char* c_str, size_t sz);
		String operator""_s(const char* c_str);
	}
}

std::ostream& operator<<(std::ostream& out, const smns::String& str);

// std::istream& operator>>(std::istream& in, const smns::String& str) {
// 	// in >> str._pointer;
// }