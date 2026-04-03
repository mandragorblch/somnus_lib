#include "string.h"
#include <fstream>
#include <stddef.h>
#include <algorithm>

namespace smns {

    String::String(): String(0uz) {}

    String::String(size_t sz)				
            : sz(sz)
            , cap(sz + 1)
            , _pointer(new char[cap]) {
        _pointer[sz] = '\0';
    }

    String::String(char symb): String(1uz) {
        _pointer[0] = symb;
    }

    String::String(const char* c_str): String(std::strlen(c_str)) {
        std::memcpy(_pointer, c_str, sz);
    }

    String::String(const char* c_str, size_t sz): String(sz) {
        std::memcpy(_pointer, c_str, sz);
    }

    String::String(size_t count, char symb): String(count) {
        std::memset(_pointer, symb, count);
    }	
    
    String::String(std::initializer_list<char> list): String(list.size()) {
        std::copy(list.begin(), list.end(), _pointer);
    }

    String::String(const std::string& other) : String(other.size()) {
        std::memcpy(_pointer, other.data(), sz);
    }

    String::String(const String &other) : String(other.sz) {
        std::memcpy(_pointer, other._pointer, sz);
    }

    
    
    String& String::operator=(const char* c_str) {
        String other = c_str;
        this->swap(other);    //copy-and-swap idiom
        return *this;
    }

    String& String::operator=(String other) {
        this->swap(other);    //copy-and-swap
        return *this;
    }

    String& String::operator=(const std::string& other_std) {
        String other = other_std;
        this->swap(other);    //copy-and-swap idiom
        return *this;
    }

    
    
    void String::swap(String& other) {
        std::swap(sz, other.sz);
        std::swap(cap, other.cap);
        std::swap(_pointer, other._pointer);
    }

    
    
    String& String::assign(size_t count, char symb) {
        clear();
        resize(count, symb);
        return *this;
    }

    String& String::assign(const String& other) {
        return *this = other;
    }

    String& String::assign(const String& other, size_t pos, size_t count) {
        if(pos > other.sz) throw std::out_of_range("out of range");
        return assign(other._pointer + pos, std::min(count, other.sz - pos));
    }
    //no checks for nullptr, neither for that [c_str, c_str + count] is valid range
    String& String::assign(const char* c_str, size_t count) {
        reserve(count + 1);//+ 1 for '\0'

        std::memcpy(_pointer, c_str, count);
        _pointer[count] = '\0';
        sz = count;
        return *this;
    }

    String& String::assign(const char* c_str) {
        return assign(c_str, std::strlen(c_str));
    }

    
    
    size_t String::find(const String& str, size_t pos) const {
        return find(str._pointer, pos, str.sz);
    }

    size_t String::find(const char* s, size_t pos, size_t count) const {
        if(count == 0) return pos < sz ? pos : sz;
        if(pos >= sz) return npos;

        auto end = _pointer + sz;
        auto beginning = std::search(_pointer + pos, end, s, s + count);

        //beggining >= pointer = no overflow
        if(beginning != end) return static_cast<size_t>(beginning - _pointer);
        return npos;
    }

    size_t String::find(const char* s, size_t pos) const { 
        return find(s, pos, std::strlen(s)); 
    }

    size_t String::find(char ch, size_t pos) const { 
        return find(&ch, pos, 1); 
    }

    
    
    size_t String::rfind(const String& str, size_t pos) const {
        return rfind(str._pointer, pos, str.sz);
    }

    size_t String::rfind(const char* s, size_t pos, size_t count) const {
        if(count == 0) return pos < sz ? pos : sz;
        if(pos > sz) pos = sz;

        auto end = _pointer + std::min(pos + count, sz);
        auto beginning = std::find_end(_pointer, end, s, s + count);

        //beggining >= pointer = no overflow
        if(beginning != end) return static_cast<size_t>(beginning - _pointer);
        return npos;
    }

    size_t String::rfind(const char* s, size_t pos) const { 
        return rfind(s, pos, std::strlen(s)); 
    }

    size_t String::rfind(char ch, size_t pos) const {
        //const char str[2] = {ch, '\0'};//to ensure nothing bad will happen
        return rfind(&ch, pos, 1); 
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
        if(index >= sz) throw std::out_of_range("out of range");
        return _pointer[index];
    }

    
    
    bool String::empty() const { return sz == 0; }

    size_t String::size() const { return sz; }

    size_t String::length() const { return sz; }

    size_t String::max_size() const { return std::numeric_limits<size_t>::max(); }

    void String::reserve(size_t new_cap) {
        if(new_cap <= cap) return;

        char* new_pointer = new char[new_cap];
        std::memcpy(new_pointer, _pointer, sz);
        new_pointer[sz] = '\0';

        delete[] _pointer;
        _pointer = new_pointer;

        cap = new_cap;
    }
    
    size_t String::capacity() const { return cap; }

    void String::shrink_to_fit() {
        if(cap == sz + 1) return;

        char* new_pointer = new char[sz + 1];

        std::memcpy(new_pointer, _pointer, sz);
        new_pointer[sz] = '\0';

        delete[] _pointer;
        _pointer = new_pointer;

        cap = sz + 1;
    }

    
    
    const char* String::data() const {
        return _pointer;
    }

    char* String::data() {
        return _pointer;
    }

    const char* String::c_str() const {
        return _pointer;
    }

    
    
    char& String::back() {
        return _pointer[sz - 1];
    }

    const char& String::back() const {
        return _pointer[sz - 1];
    }

    
    
    char& String::front() {
        return _pointer[0];
    }

    const char& String::front() const {
        return _pointer[0];
    }

    
    //don't check for sz = 0
    void String::clear() {
        resize(0);
    }

    
    
    String& String::insert(size_t index, size_t count, char ch) {
        return replace(index, 0, count, ch);
    }

    String& String::insert(size_t index, const char* c_str) {
        return replace(index, 0, c_str, std::strlen(c_str));
    }

    String& String::insert(size_t index, const char* c_str, size_t count) {
        if(index > sz) throw std::out_of_range("out of range");
        return replace(index, 0, c_str, count);
    }

    String& String::insert(size_t index, const String& other) {
        return replace(index, 0, other._pointer, other.sz);
    }

    String& String::insert(size_t index, const String& other, size_t s_index, size_t count) {
        if(s_index > other.sz) throw std::out_of_range("out of range");
        return replace(index, 0, other._pointer + s_index, count);
    }



    String& String::replace(size_t pos, size_t count, const String& other) {
        return replace(pos, count, other._pointer, other.sz);
    }

    String& String::replace(size_t pos, size_t count, const String& other, size_t pos2, size_t count2) {
        if(pos2 > other.sz) throw std::out_of_range("out of range");
        return replace(pos, count, other._pointer + pos2, std::min(count2, other.sz - pos2));
    }

    String& String::replace(size_t pos, size_t count, const char* c_str, size_t count2) {
        if(pos > sz) throw std::out_of_range("out of range");
        count = std::min(count, sz - pos);
        size_t new_sz = sz + count2 - count; 
        char* dst = _pointer + pos;
        if(cap <= new_sz) {
            //no reserve for neat optimization
            char* new_pointer = new char[new_sz + 1];
            std::memcpy(new_pointer, _pointer, pos);
            std::memcpy(new_pointer + pos, c_str, count2);
            std::memcpy(new_pointer + pos + count2, _pointer + pos + count, sz - (pos + count));

            delete[] _pointer;
            _pointer = new_pointer;
            cap = new_sz + 1;
        } else {
            //check if we can place whole thing into destination
            if(count >= count2) {
                std::memmove(dst, c_str, count2);
                std::memmove(dst + count2, dst + count, sz - (pos + count));
            } else {
                //to prevent data loss we have to shift elements to the right (count2 > count)
                std::memmove(dst + count2, dst + count, sz - (pos + count));
                //now we have to check overlaps + how exactly sequence affected
                if(c_str >= _pointer + sz || c_str + count2 <= dst + count) {
                    //no overlap occured => sequence wasn't affected
                    std::memmove(dst, c_str, count2);
                } else {
                    //overlap occured
                    //now check if the whole sequence is shifted to the right
                    if(c_str >= dst + count) {
                        std::memmove(dst, c_str + (count2 - count), count2);
                    } else {
                        //worst case, sequence was parted
                        ptrdiff_t first_part = dst + count - c_str;
                        std::memmove(dst, c_str, first_part);
                        std::memmove(dst + first_part, c_str + first_part + (count2 - count), count2 - first_part);
                    }
                }
            }
        }
        sz = new_sz;
        _pointer[sz] = '\0';
        return *this;
    }

    String& String::replace(size_t pos, size_t count, const char* c_str) {
        return replace(pos, count, c_str, std::strlen(c_str));
    }

    String& smns::String::replace(size_t pos, size_t count, size_t count2, char ch) {
        if(pos > sz) throw std::out_of_range("out of range");
        count = std::min(count, sz - pos);
        size_t new_sz = sz + count2 - count; 
        if(cap <= new_sz) {
            //no reserve for neat optimization
            char* new_pointer = new char[new_sz + 1];
            std::memcpy(new_pointer, _pointer, pos);
            std::memset(new_pointer + pos, ch, count2);
            std::memcpy(new_pointer + pos + count2, _pointer + pos + count, sz - (count + pos));

            delete[] _pointer;
            _pointer = new_pointer;
            cap = new_sz + 1;
        } else {
            std::memmove(_pointer + pos + count2, _pointer + pos + count, sz - (pos + count));
            std::memset(_pointer + pos, ch, count2);
        }
        sz = new_sz;
        _pointer[sz] = '\0';
        return *this;
    }

    
    
    String& String::erase(size_t index, size_t count) {
        return replace(index, count, 0, '\0');
    }

    String& String::push_back(char ch) {
        return replace(sz, 0, 1, ch);
    }

    void String::pop_back() {
        erase(sz - 1, 1);
    }

    
    
    String& String::append(size_t count, char ch) {
        return replace(sz, 0, count, ch);
    }

    String& String::append(const char* c_str, size_t count) {
        return replace(sz, 0, c_str, count);
    }

    String& String::append(const char* c_str) {
        return replace(sz, 0, c_str, std::strlen(c_str));
    }

    String& String::append(const String& other) {
        return replace(sz, 0, other._pointer, other.sz);
    }

    String& String::append(const String& other, size_t pos, size_t count) {
        if(pos > other.sz) throw std::out_of_range("out of range");
        return replace(sz, 0, other._pointer + pos, std::min(count, other.sz - pos));
    }

    

    String& String::operator+=(const String& other) {
        return append(other);
    }

    String& String::operator+=(char ch) {
        return append(1, ch);
    }
    //no checks for nullptr
    String& String::operator+=(const char* c_str) {
        return append(c_str);
    }

    
    
    size_t String::copy(char* c_str, size_t count, size_t pos) const {
        if(pos > sz) throw std::out_of_range("pos exceed size of string");
        
        count = std::min(count, sz - pos);
        std::memcpy(c_str, _pointer + pos, count);
        return count;
    }

    
    //no checks for max_size
    void String::resize(size_t count) {
        resize(count, '\0');
    }
    //no checks for max_size
    void String::resize(size_t count, char ch) {
        if(count <= sz) {
            sz = count;
            _pointer[count] = '\0';
            return;
        }

        reserve(count + 1);//+ 1 for '\0'

        std::memset(_pointer + sz, ch, count - sz);
        sz = count;
        _pointer[count] = '\0';
    }

    
    
    String String::substr(size_t pos, size_t count) const& {
      return String(_pointer + pos, count); //RVO
    }

    String::operator char*() {
        return _pointer;
    }

    String::operator const char*() {
        return _pointer;
    }

    String::~String() {
        delete[] _pointer;
    }

   namespace literals::string_literals {
        String operator""_s(const char *c_str, size_t sz) {
            return String(c_str, sz);//RVO
        }   

        String operator""_s(const char *c_str) {
            return String(c_str);//RVO
        }
   }
}

std::ostream& operator<<(std::ostream& out, const smns::String& str) {
	out << str._pointer;
	return out;
}
