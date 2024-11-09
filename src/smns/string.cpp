#include "string.h"

namespace smns {
    String::String() {

    }
    
    String::String(size_t sz)				
            : sz(sz)
            , cap(sz + 1)
            , _pointer(new char[cap]) {
        _pointer[sz] = '\0';
    }

    String::String(char symb): String(static_cast<size_t>(1)) {
        _pointer[0] = symb;
    }

    String::String(const char* c_str): String(std::strlen(c_str)) {
        memcpy(_pointer, c_str, sz);
    }

    String::String(const char* c_str, size_t sz): String(sz) {
        memcpy(_pointer, c_str, sz);
    }

    String::String(size_t count, char symb): String(count) {
        memset(_pointer, symb, count);
    }	
    
    String::String(std::initializer_list<char> list): String(list.size()) {
        std::copy(list.begin(), list.end(), _pointer);
    }

    String::String(const String &other) : String(other.sz) {
        memcpy(_pointer, other._pointer, sz + 1);
    }
    //copy-and-swap idiom
    String& String::operator=(String other) {
        this->swap(other);
        return *this;
    }

    void String::swap(String& other) {
        std::swap(sz, other.sz);
        std::swap(cap, other.cap);
        std::swap(_pointer, other._pointer);
    }

    String& String::assign(size_t count, char symb) {
        sz = count;
        if(sz >= cap) {
            cap = sz + 1;
            delete[] _pointer;
            _pointer = new char[cap];
        }

        memset(_pointer, symb, sz);
        _pointer[sz] = '\0';

        return *this;
    }

    String &String::assign(const String& other) {
        sz = other.size();
        if(sz >= cap) {
            cap = sz + 1;
            delete[] _pointer;
            _pointer = new char[cap];
        }

        memcpy(_pointer, other._pointer, sz);
        _pointer[sz] = '\0';

        return *this;
    }

    String& String::assign(const String& other, size_t pos, size_t count) {
        if(pos > other.sz) throw std::out_of_range("out of range");
        if(count == npos) count = other.sz - pos;
        sz = count;
        if(sz >= cap) {
            cap = sz + 1;
            delete[] _pointer;
            _pointer = new char[cap];
        }

        memcpy(_pointer, other._pointer + pos, count);
        _pointer[count] = '\0';
        return *this;
    }

    String& String::assign(const char* c_str, size_t count) {
        sz = count;
        if(sz >= cap) {
            cap = sz + 1;
            delete[] _pointer;
            _pointer = new char[cap];
        }

        memcpy(_pointer, c_str, count);
        _pointer[count] = '\0';
        return *this;
    }

    String& String::assign(const char* c_str) {
        this->assign(c_str, std::strlen(c_str));
        return *this;
    }

    char& String::operator[](size_t index) {
        return _pointer[index];
    }

    const char& String::operator[](size_t index) const {
        return _pointer[index];
    }

    char& String::at(size_t index) {
        if(index >= sz) throw std::out_of_range("out of range");
        return _pointer[index];
    }

    const char& String::at(size_t index) const {
        if(index >= sz) {
            throw std::out_of_range("out of range");
        }
        return _pointer[index];
    }

    size_t String::size() const {
        return sz;
    }

    const char *String::data() const {
        return _pointer;
    }

    char *String::data() {
        return _pointer;
    }

    const char *String::c_str() const {
        return _pointer;
    }

    char& String::back() {
        return _pointer[sz - 1];
    }

    const char& String::back() const {
        return _pointer[sz - 1];
    }

    char& String::front() {
        return *_pointer;
    }

    const char& String::front() const {
        return *_pointer;
    }

    String::operator const char*() {
        return _pointer;
    }

    String::operator char*() {
        return _pointer;
    }

    String::~String() {
        delete[] _pointer;
    }

   namespace literals::string_literals {
        String operator""_s(const char *c_str, size_t sz) {
            return String(c_str, sz);
        }   

        String operator""_s(const char *c_str) {
            return String(c_str);
        }
   }
}

std::ostream &operator<<(std::ostream &out, const smns::String &str) {
	out << str._pointer;
	return out;
}
