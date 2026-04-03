#pragma once
#include <cstdint>
#include <cstring>
#include <iostream>

namespace smns{
	struct String;
}

extern int main();
std::ostream& operator<<(std::ostream& out, const smns::String& str);

namespace smns{
	struct String {
		static constexpr size_t npos = static_cast<size_t>(-1);
	private:
		size_t sz = 0;
		size_t cap = 0;
		char* _pointer = nullptr;
	public:
		String();

		explicit String(size_t sz);

		explicit String(char symb);

		String(const char* c_str);

		String(const char* c_str, size_t sz);

		String(size_t sz, char symb);
		
		String(std::initializer_list<char> list);

		String(const std::string& other);

        String(const String& other);//1 copy constructor

		String(String&&) = delete;



		//copy and swap idiom
		String& operator=(const char* c_str);//2 copy assignment operator

		String& operator=(String other);//2 copy assignment operator

		String& operator=(String&&) = delete;//2 copy assignment operator

		String& operator=(const std::string& other);



		String& assign(size_t count, char symb);

		String& assign(const String& other);

		String& assign(const String& other, size_t pos, size_t count = npos);

		String& assign(const char* c_str, size_t count);

		String& assign(const char* c_str);

		//-------------------------------------------------------- search --------------------------------------------------------

		size_t find(const String& str, size_t pos = 0) const;

		size_t find(const char* s, size_t pos, size_t count) const;

		size_t find(const char* s, size_t pos = 0) const;

		size_t find(char ch, size_t pos = 0) const;



		size_t rfind(const String& str, size_t pos = npos) const;

		size_t rfind(const char* s, size_t pos, size_t count) const;

		size_t rfind(const char* s, size_t pos = npos) const;

		size_t rfind(char ch, size_t pos = npos) const;

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

//--------------------------------------------------------capacity--------------------------------------------------------
		bool empty() const;

		size_t size() const;

		size_t length() const;

		size_t max_size() const;

		void reserve(size_t new_cap);

		size_t capacity() const;

		void shrink_to_fit();

//--------------------------------------------------------modifiers--------------------------------------------------------

		void clear();

		void swap(String& other);



		String& insert(size_t index, size_t count, char ch);

		String& insert(size_t index, const char* c_str);

		String& insert(size_t index, const char* c_str, size_t count);

		String& insert(size_t index, const String& other);

		String& insert(size_t index, const String& other, size_t s_index, size_t count = npos);



		String& replace(size_t pos, size_t count, const String& other);

		String& replace(size_t pos, size_t count, const String& str, size_t pos2, size_t count2 = npos);

		String& replace(size_t pos, size_t count, const char* c_str, size_t count2);

		String& replace(size_t pos, size_t count, const char* c_str);

		String& replace(size_t pos, size_t count, size_t count2, char ch);



		String& erase(size_t index = 0, size_t count = npos);



		String& push_back(char ch);

		void pop_back();




		String& append(size_t count, char ch);

		String& append(const char* c_str, size_t count);

		String& append(const char* c_str);

		String& append(const String& other);

		String& append(const String& other, size_t pos, size_t count = npos);



		String& operator+=(const String& other);

		String& operator+=(char ch);

		String& operator+=(const char* c_str);



		size_t copy(char* c_str, size_t count, size_t pos = 0) const;



		void resize(size_t count);

		void resize(size_t count, char ch);

//--------------------------------------------------------operations--------------------------------------------------------

		String substr(size_t pos = 0, size_t count = npos) const&;

		explicit operator const char*();

		explicit operator char*();

		~String();//3 -> Rule of Three

		friend std::ostream& ::operator<<(std::ostream& out, const smns::String& str);
	};

	namespace literals::string_literals{
		String operator""_s(const char* c_str, size_t sz);
		String operator""_s(const char* c_str);
	}
}

template<typename T, typename V>
requires ((std::is_same_v<T, std::string> || std::is_same_v<T, smns::String>) && (std::is_same_v<V, std::string> || std::is_same_v<V, smns::String>))
bool operator==(const T& l_str, const V& r_str) {
	if (l_str.size() != r_str.size()) return false;
	return std::memcmp(l_str.data(), r_str.data(), r_str.size()) == 0;
}

std::ostream& operator<<(std::ostream& out, const smns::String& str);

// std::istream& operator>>(std::istream& in, const smns::String& str) {
// 	// in >> str._pointer;
// }