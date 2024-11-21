#include "string.h"

namespace smns {
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

    String& String::operator=(String other) {
        this->swap(other);    //copy-and-swap idiom
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

    bool String::empty() const { return sz == 0; }

    size_t String::size() const { return sz; }

    size_t String::length() const { return sz; }

    size_t String::max_size() const { return std::numeric_limits<size_t>::max(); }

    void String::reserve(size_t new_cap) {
        if(new_cap <= cap) return;

        cap = new_cap;
        char* new_pointer = new char[cap];

        memcpy(new_pointer, _pointer, sz);
        new_pointer[sz] = '\0';

        std::swap(_pointer, new_pointer);
        delete[] new_pointer;
    }

    size_t String::capacity() const { return cap; }

    void String::shrink_to_fit() {
        if(cap == sz + 1) return;

        cap = sz + 1;
        char* new_pointer = new char[cap];

        memcpy(new_pointer, _pointer, sz);
        new_pointer[sz] = '\0';

        std::swap(_pointer, new_pointer);
        delete[] new_pointer;
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

    void String::clear() {
        sz = 0;
        *_pointer = '\0';
    }

    String& String::insert(size_t index, size_t count, char ch) {
        if(index > sz) throw std::out_of_range("out of range");
        sz += count;
        if(cap < sz + 1) {
            cap = sz + 1;
            char* new_pointer = new char[cap];
            memcpy(new_pointer, _pointer, index);
            memset(new_pointer + index, ch, count);
            memcpy(new_pointer + index + count, _pointer + index, sz - count - index);
            new_pointer[sz] = '\0';
            std::swap(_pointer, new_pointer);
            delete[] new_pointer;
            
        } else {
            for (size_t i = sz; i > index; --i)
            {
                _pointer[i] = _pointer[i - count];
            }
            
            memset(_pointer + index, ch, count);
        }
        return *this;
    }

    String& String::insert(size_t index, const char* c_str) {
        this->insert(index, c_str, std::strlen(c_str));
        return *this;
    }

    String& String::insert(size_t index, const char* c_str, size_t count) {
        if(index > sz) throw std::out_of_range("out of range");
        sz += count;
        if(cap < sz + 1) {
            cap = sz + 1;
            char* new_pointer = new char[cap];
            memcpy(new_pointer, _pointer, index);
            memcpy(new_pointer + index, c_str, count);
            memcpy(new_pointer + index + count, _pointer + index, sz - count - index);
            new_pointer[sz] = '\0';
            std::swap(_pointer, new_pointer);
            delete[] new_pointer;
        } else {
            //move data after index to the right. memmove is necessary, because overlap may occur
            memmove(_pointer + index + count, _pointer + index, (sz - count - index));
            //copy data from src. memmove is necessary, because overlap may occur
            memmove(_pointer + index, c_str, count);
            _pointer[sz] = '\0';
        }
        return *this;
    }

    String& String::insert(size_t index, const String& other) {
        this->insert(index, other.c_str(), other.sz);
        return *this;
    }

    String& String::insert(size_t index, const String& other, size_t s_index, size_t count) {
        this->insert(index, other.substr(s_index, count));
        return *this;
    }

    String String::substr(size_t pos, size_t count) const& {
      return String(_pointer + pos, count);
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

std::ostream& operator<<(std::ostream& out, const smns::String& str) {
	out << str._pointer;
	return out;
}
