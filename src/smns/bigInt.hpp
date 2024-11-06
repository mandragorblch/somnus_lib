#pragma once
#include <iostream>
#include <cstdint>
#include <limits>
#include <compare>
#include <algorithm>
#include <iterator>
#include <cassert>

template<typename T, typename U>
struct is_same_template_v : std::is_same<T,U>
{};

template<template<typename> typename T, typename Args1, typename Args2>
struct is_same_template_v<T<Args1>, T<Args2>> : std::true_type
{};

////is not working, bad aligning for undefined reason
// template<size_t sz>
// union Number {//with sign flag in last bit
//     unsigned char val[sz];
// private:
//     struct {
//         uint8_t : (sz * 8 - 1);
//         bool SF : 1;
//     } SF;
// public:
//     bool getSign() const{
//         return SF.SF;
//     }
// };

template<size_t sz>
struct Number {
    unsigned char val[sz];
    bool getSign() const{
        return reinterpret_cast<const char&>(val[sz - 1]) < 0;
    }
};

template<typename T>
requires(std::is_integral_v<T> && std::is_unsigned_v<T>)
bool add_overflow(T a, T b, T* dst){
    using u_T_r = std::make_unsigned_t<T>&;
    return reinterpret_cast<u_T_r>(*dst = a + b) < reinterpret_cast<u_T_r>(b);
}

template<typename T>
requires(std::is_integral_v<T> && std::is_unsigned_v<T>)
bool add_overflow(T& dst, T src){
    using u_T_r = std::make_unsigned_t<T>&;
    return reinterpret_cast<u_T_r>(dst += src) < reinterpret_cast<u_T_r>(src);
}

template<typename T>
requires(std::is_integral_v<T> && std::is_unsigned_v<T>)
bool add_overflow_OF(T& dst, T var, bool OF){
    // if(OF) {
    //     if(++var == 0) return true;//overflow happened, var = 0 -> dst + var = dst[without any change]
    // }
    //return add_overflow(dst, var);
    return (OF && (++var == 0)) || add_overflow(dst, var);
}

template<typename T>
requires(std::is_integral_v<T> && std::is_unsigned_v<T>)
bool add_overflow_OF_with_255(T& dst, bool OF){
    //if OF -> 255 + 1 = 0(char overflow) ->
    return OF || (--dst != 255);
}

template<typename T>
requires(std::is_integral_v<T> && std::is_unsigned_v<T>)
bool sub_overflow(T& a, T b){
    using u_T_r = std::make_unsigned_t<T>&;
    bool OF = reinterpret_cast<u_T_r>(a) < reinterpret_cast<u_T_r>(b);
    a -= b;
    return OF;
}

template<typename T>
requires(std::is_integral_v<T> && std::is_unsigned_v<T>)
bool sub_overflow(T a, T b, T* result){
    using u_T_r = std::make_unsigned_t<T>&;
    (*result) = a - b;
    return reinterpret_cast<u_T_r>(a) < reinterpret_cast<u_T_r>(b);
}

template<typename T>
requires(std::is_integral_v<T> && std::is_unsigned_v<T>)
bool sub_overflow_OF(T a, T b, bool OF, T* result){
    //++b because b is substracting from a i.e. (a - b - 1) = a - (b + 1)
    return (OF && (++b == 0)) || sub_overflow(a, b, result);
}

template<typename T>
requires(std::is_integral_v<T> && std::is_unsigned_v<T>)
bool sub_overflow_OF_with_255(T& a, bool OF){
    return OF || (++a != 0);
}

template<size_t sz>
std::weak_ordering unsigned_compare(const uint8_t* data, const uint8_t* other_data){
    for(size_t i = (sz - 1); i < std::numeric_limits<size_t>::max(); --i){
        if(data[i] == other_data[i]) continue;
        return (data[i] < other_data[i]) ? std::weak_ordering::less : std::weak_ordering::greater;
    }
    return std::weak_ordering::equivalent;//is equal
}

template<size_t end, size_t begin>
bool all_is_same(const void* data, const void* other_data){

    for(size_t i = begin; i < end; ++i){
        if(reinterpret_cast<const uint8_t*>(data)[i] != reinterpret_cast<const uint8_t*>(other_data)[i]) return false;//0 - signed extension. if data[i] == 0 for will continue automaticaly
    }
    return true;
}

template<typename T, size_t end, size_t begin, T val>
bool all_is_same(const void* data){
    for(size_t i = begin; i < end; ++i){
        if(reinterpret_cast<const uint8_t*>(data)[i] != val) return false;//0 - signed extension. if data[i] == 0 for will continue automaticaly
    }
    return true;
}

//void if there is no fitting int
template<size_t sz>
struct size_to_int {
    using type = void;
};

template<>
struct size_to_int<1>{
    using type = int8_t;
};

template<>
struct size_to_int<2>{
    using type = int16_t;
};

template<>
struct size_to_int<4>{
    using type = int32_t;
};

template<>
struct size_to_int<8>{
    using type = int64_t;
};

template<size_t sz>
using size_to_int_t = typename size_to_int<sz>::type;



template<size_t sz>
struct bigInt {
    Number<sz> number;
    uint8_t (&data)[sz] = number.val;

    //explicit static conversion to type T
    template<typename T>
    requires (std::is_integral_v<std::remove_cvref_t<T>> && sizeof(std::remove_cvref_t<T>) <= sz)
    explicit operator T(){
        using clear_type = std::remove_cvref_t<T>;
        #ifdef ENABLE_LOGGING
        std::cout << "explicitly casted!\n";
        #endif
        constexpr T u_T_max = std::numeric_limits<std::make_unsigned_t<T>>::max(); 
        std::make_signed_t<T> extend = number.getSign() ? u_T_max : 0;//signed/unsigned extention
        memcpy(&extend, &data[0], sz);
        return reinterpret_cast<clear_type&>(data[0]);
    }



    bigInt() = delete;
    
    template<typename T>
    requires (std::is_integral_v<T>)
    bigInt(const T& other) {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << "> constructor from integer!\n";
        #endif

        (*this) = other;
    }

    template<size_t other_sz>
    bigInt(const bigInt<other_sz>& other) {
        #ifdef ENABLE_LOGGING
        std::cout << "constructed from bigInt!\n";
        #endif   

        (*this) = other;            
    }

    //copy constructor
    bigInt(const bigInt& other) {
        #ifdef ENABLE_LOGGING
        std::cout << "constructed from bigInt with same size!\n";
        #endif   

        (*this) = other;     
    }



    template<typename T>
    requires (std::is_integral_v<T>)
    bigInt& operator=(const T& other)& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator= with integer sizeof(T) = " << sizeof(T) << "!\n";
        #endif    

        if constexpr(sz == sizeof(T)) {
            reinterpret_cast<T&>(data[0]) = other;
        } else if(sz < sizeof(T)) {
            memcpy(&data[0], &other, sz);
        } else {//sz > sizeof(T)
            if constexpr(std::is_signed_v<T>) {
                memset(&data[sizeof(T)], other < 0 ? 255 : 0, sz - sizeof(T));
            } else { 
                memset(&data[sizeof(T)], 0, sz - sizeof(T));
            }
            memcpy(&data[0], &other, sizeof(T));
        }
        return *this;
    }

    template<size_t other_sz>
    bigInt& operator=(const bigInt<other_sz>& other)& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator= with bigInt<" << other_sz << ">!\n";
        #endif 

        if constexpr(sz > other_sz){
            memset(&data[other_sz], other.number.getSign() ? 255 : 0, sz - other_sz);
            memcpy(&data[0], &other.data[0], other_sz);
        } else {//sz < other_sz
            memcpy(&data[0], &other.data[0], sz);
        }
        return *this;
    }

    //copy assignment 
    bigInt& operator=(const bigInt& other)& {
        #ifdef ENABLE_LOGGING
        std::cout << "equaled to bigInt with same size!\n";
        #endif  

        if (this == &other)
            return *this;

        memcpy(&data[0], &other.data[0], sz);      
        return *this;
    }

    //------------------------------------arithmetic---------------------------------------

    template<typename T>
    requires (std::is_integral_v<T>)
    bigInt& operator+=(const T& other)& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator+= with integer sizeof(T) = " << sizeof(T) << "!\n";
        #endif  

        if constexpr (sz == sizeof(T)) {
            reinterpret_cast<std::make_signed_t<T>&>(data[0]) += other;
        } else if (sz < sizeof(T)) {
            if constexpr(!std::is_same_v<size_to_int_t<sz>, void>) {
                reinterpret_cast<size_to_int_t<sz>&>(data[0]) += other;
            } else {
                const uint8_t* other_chunks = reinterpret_cast<const uint8_t*>(&other);
                bool OF = add_overflow(data[0], other_chunks[0]);
                for (size_t i = 1; i < sz - 1; ++i) {
                    OF = add_overflow_OF(data[i], other_chunks[i], OF);
                }

                data[sz - 1] += other_chunks[sz - 1] + OF;
            }
        } else {//sz > sizeof(T)
            const uint8_t* other_chunks = reinterpret_cast<const uint8_t*>(&other);
            bool OF = add_overflow(data[0], other_chunks[0]);
            for (size_t i = 1; i < sizeof(T); ++i) {
                OF = add_overflow_OF(data[i], other_chunks[i], OF);
            }

            if constexpr(std::is_signed_v<T>){
                if(other < 0) {//sign extention
                    for (size_t i = sizeof(T); i < sz; ++i) {
                        OF = add_overflow_OF_with_255(data[i], OF);
                    }
                    //data[sz - 1] += other_chunks[sz - 1] + OF; other_chunks[sz - 1] = 255, unsigned char => x += 255 is same as --x;  
                } else {
                    if(OF) {
                        for (size_t i = sizeof(T); (i < sz) && (++data[i] == 0); ++i);
                    }
                }
            } else {//T is unsigned
                if(OF) {
                    for (size_t i = sizeof(T); (i < sz) && (++(data[i]) == 0); ++i);
                }
            }
        }

        return *this;
    }

    template<size_t other_sz>
    bigInt& operator+=(const bigInt<other_sz>& other)& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator+= with bigInt<" << other_sz << ">!\n";
        #endif

        constexpr size_t min_sz = (std::min)(sz, other_sz);
        bool OF = add_overflow(data[0], other.data[0]);
        for (size_t i = 1; i < min_sz - 1; ++i) {
            OF = add_overflow_OF(data[i], other.data[i], OF);
        }

        if constexpr (sz < other_sz) {
            data[sz - 1] += other.data[sz - 1] + OF;
        } else {
            OF = add_overflow_OF(data[other_sz - 1], other.data[other_sz - 1], OF);
            if(other.number.getSign()) {
                for (size_t i = other_sz; i < sz - 1; ++i) {
                    OF = add_overflow_OF_with_255(data[i], OF);
                }
                --data[sz - 1] += OF;    
            } else {
                if(OF) {
                    for (size_t i = other_sz; (i < sz) && (++data[i] == 0); ++i);
                } 
            }  
        }

        return *this;
    }

    bigInt& operator+=(const bigInt& other)& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator+= with bigInt same size!\n";
        #endif  

        bool OF = add_overflow(data[0], other.data[0]);
        for (size_t i = 1; i < sz - 1; ++i) {
            OF = add_overflow_OF(data[i], other.data[i], OF);
        }

        data[sz - 1] += other.data[sz - 1] + OF;     

        return *this;
    }



    template<typename T>
    requires (std::is_integral_v<T>)
    bigInt& operator-=(const T& other)& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator-= with integer sizeof(T) = " << sizeof(T) << "!\n";
        #endif  

        if constexpr (sz == sizeof(T)) {
            reinterpret_cast<std::make_signed_t<T>&>(data[0]) -= other;
        } else if(sz < sizeof(T)) {
            if constexpr(!std::is_same_v<size_to_int_t<sz>, void>) {
                reinterpret_cast<size_to_int_t<sz>&>(data[0]) -= other;
            } else {
                const uint8_t* other_chunks = reinterpret_cast<const uint8_t*>(&other);
                bool OF = sub_overflow(data[0], other_chunks[0]);
                for (size_t i = 1; i < sz - 1; ++i) {
                    OF = sub_overflow_OF(data[i], other_chunks[i], OF);
                }

                data[sz - 1] -= (other_chunks[sz - 1] + OF);
            }
        } else {//sz > sizeof(T)
            const uint8_t* other_chunks = reinterpret_cast<const uint8_t*>(&other);
            bool OF = sub_overflow(data[0], other_chunks[0], &data[0]);
            for (size_t i = 1; i < sizeof(T); ++i) {
                OF = sub_overflow_OF(data[i], other_chunks[i], OF, &data[i]);
            }

            if constexpr(std::is_signed_v<T>){
                if(other < 0){
                    for (size_t i = sizeof(T); i < sz - 1; ++i) {
                        OF = sub_overflow_OF_with_255(data[i], OF);
                    }
                    //value[sz - 1] -= (other_chunks[sz - 1] + OF); other_chunks[sz - 1] = 255, unsigned char => x -= 255 is same as ++x;
                    ++data[sz - 1] -= OF;    
                } else {
                    if(OF) {
                        for (size_t i = sizeof(T); (i < sz) && (--data[i] == 255); ++i);
                    }
                }
            } else {
                if(OF) {
                    for (size_t i = sizeof(T); (i < sz) && (--data[i] == 255); ++i);
                }
            }
        }      
        
        return *this;
    }

    template<size_t other_sz>
    bigInt& operator-=(const bigInt<other_sz>& other)& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator-= with bigInt<" << other_sz << ">!\n";
        #endif

        constexpr size_t min_sz = (std::min)(sz, other_sz);
        bool OF = sub_overflow(data[0], other.data[0], &data[0]);
        for (size_t i = 1; i < min_sz; ++i) {
            OF = sub_overflow_OF(data[i], other.data[i], OF, &data[i]);
        }

        if constexpr (sz < other_sz) {
            data[sz - 1] -= (other.data[sz - 1] + OF);
        } else {//sz > other_sz
            OF = sub_overflow_OF(data[other_sz - 1], other.data[other_sz - 1], OF, &data[other_sz - 1]);
            if(other.number.getSign()){
                for (size_t i = other_sz; i < sz - 1; ++i) {
                    OF = sub_overflow_OF_with_255(data[i], OF);
                }
                //value[sz - 1] -= (other.value[sz - 1] + OF); other.value[sz - 1] = 255, unsigned char => x -= 255 is same as ++x;
                ++data[sz - 1] -= OF;    
            } else {
                if(OF) {
                    for (size_t i = other_sz; (i < sz) && (--data[i] == 255); ++i);
                }
            }
        }
        return *this;
    }

    bigInt& operator-=(const bigInt& other)& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator-= with bigInt same size!\n";
        #endif

        bool OF = sub_overflow(data[0], other.data[0], &data[0]);
        for (size_t i = 1; i < sz - 1; ++i) {
            OF = sub_overflow_OF(data[i], other.data[i], OF, &data[i]);
        }

        data[sz - 1] -= (other.data[sz - 1] + OF);
        return *this;
    }



    template<typename T>
    requires (std::is_integral_v<T>)
    bigInt& operator/=(const T& other)& {
        if constexpr (sz == sizeof(T)) {
            reinterpret_cast<std::make_signed_t<T>&>(data[0]) /= other;
        } else if (sz < sizeof(T)) {
            if constexpr(!std::is_same_v<size_to_int_t<sz>, void>) {
                reinterpret_cast<size_to_int_t<sz>&>(data[0]) /= other;
            } else {
                assert(other != 0 && "division by zero!");
        
                bool sign = number.getSign();
                if(sign) changeSign();
                bool other_sign = (other < 0);
                constexpr size_t bits_in_dividend = sizeof(T) * 8;

                bigInt<sizeof(T) * 2> remainder(*this);
                bigInt<sizeof(T)> quotient = *this;
                //TODO: divisor is const, shifted by whole bytes => we can use add function, that skip first sz bytes(they are zeros)
                bigInt<sizeof(T) * 2> divisor = (other_sign ? -other : other);
                divisor <<= (sizeof(T) * 8);

                for(size_t i = bits_in_dividend - 1; i < std::numeric_limits<size_t>::max(); --i){
                    if(remainder.number.getSign()) {
                        quotient.setBit(i, false);
                        (remainder <<= 1) += divisor; 
                    } else {
                        quotient.setBit(i, true);
                        (remainder <<= 1) -= divisor;                     
                    }
                }
                quotient -= ~quotient;

                if(remainder.number.getSign()) {
                    --quotient;
                    //remainder += divisor;
                }
                //remainder >>= bits_in_dividend;
                if(sign ^ other_sign) quotient.changeSign();
                (*this) = quotient;
            }
        } else {//sz > sizeof(T)
            assert(other != 0 && "division by zero!");
            
            bool sign = number.getSign();
            if(sign) changeSign();
            bool other_sign = (other < 0);
            constexpr size_t bits_in_dividend = sz * 8;

            bigInt<sz * 2> remainder(*this);
            bigInt<sz>& quotient = *this;
            //TODO: divisor is const, shifted by whole bytes => we can use add function, that skip first sz bytes(they are zeros)
            bigInt<sz * 2> divisor(other_sign ? -other : other);
            divisor <<= bits_in_dividend;

            for(size_t i = bits_in_dividend - 1; i < std::numeric_limits<size_t>::max(); --i){
                if(remainder.number.getSign()) {
                    quotient.setBit(i, false);
                    (remainder <<= 1) += divisor; 
                } else {
                    quotient.setBit(i, true);
                    (remainder <<= 1) -= divisor;                     
                }
            }
            quotient -= ~quotient;

            if(remainder.number.getSign()) {
                --quotient;
                //remainder += divisor;
            }
            //remainder >>= bits_in_dividend;
            if(sign ^ other_sign) quotient.changeSign();
            //(*this) = quotient;
        }

        return *this;
    }

    template<size_t other_sz>
    bigInt& operator/=(const bigInt<other_sz>& other)& {
        constexpr auto max_sz = std::max(sz, other_sz);
        assert(other != 0 && "division by zero!");

        bool sign = number.getSign();
        if(sign) changeSign();
        bool other_sign = (other.number.getSign());
        constexpr size_t bits_in_dividend = sz * 8;

        bigInt<sz * 2> remainder(*this);
        bigInt<sz>& quotient(*this);
        //TODO: divisor is const, shifted by whole bytes => we can use add function, that skip first sz bytes(they are zeros)
        bigInt<sz * 2> divisor(other_sign ? -other : other);
        divisor <<= bits_in_dividend;

        for(size_t i = bits_in_dividend - 1; i < std::numeric_limits<size_t>::max(); --i){
            if(remainder.number.getSign()) {
                quotient.setBit(i, false);
                (remainder <<= 1) += divisor; 
            } else {
                quotient.setBit(i, true);
                (remainder <<= 1) -= divisor;                     
            }
        }
        quotient -= ~quotient;

        if(remainder.number.getSign()) {
            --quotient;
            //remainder += divisor;
        }
        //remainder >>= bits_in_dividend;
        if(sign ^ other_sign) quotient.changeSign();
        //(*this) = quotient;

        return *this;
    }

    bigInt& operator/=(const bigInt& other)& {
        assert(other != 0 && "division by zero!");

        bool sign = number.getSign();
        if(sign) changeSign();
        bool other_sign = (other.number.getSign());
        constexpr size_t bits_in_dividend = sz * 8;

        bigInt<sz * 2> remainder(*this);
        bigInt<sz>& quotient(*this);
        //TODO: divisor is const, shifted by whole bytes => we can use add function, that skip first sz bytes(they are zeros)
        bigInt<sz * 2> divisor(other_sign ? -other : other);
        divisor <<= bits_in_dividend;

        for(size_t i = bits_in_dividend - 1; i < std::numeric_limits<size_t>::max(); --i){
            if(remainder.number.getSign()) {
                quotient.setBit(i, false);
                (remainder <<= 1) += divisor; 
            } else {
                quotient.setBit(i, true);
                (remainder <<= 1) -= divisor;                     
            }
        }
        quotient -= ~quotient;

        if(remainder.number.getSign()) {
            --quotient;
            //remainder += divisor;
        }
        //remainder >>= bits_in_dividend;
        if(sign ^ other_sign) quotient.changeSign();
        //(*this) = quotient;

        return *this;
    }



    bigInt& operator++()&{//++bi 
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator++()!\n";
        #endif
        if(++data[0] == 0){//OF
            for (size_t i = 1; (i < sz) && (++data[i] == 0); ++i);
        } 
        return *this;
    }

    bigInt operator++(int)&{//bi++
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator++(int)!\n";
        #endif
        bigInt res(*this);
        ++(*this);
        return res;
    }

    bigInt& operator--()&{//--bi
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator--()!\n";
        #endif
        if(--data[0] == 255) {
            for (size_t i = 1; (i < sz) && (--data[i] == 255); ++i);
        }
        return *this;
    }

    bigInt operator--(int)&{//bi--
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator--(int)!\n";
        #endif
        bigInt res(*this);
        --(*this);
        return res;
    }
    //---------------------------------------bitwise---------------------------------------
    bigInt operator~() const{//~bi
        #ifdef ENABLE_LOGGING
        std::cout << "operator~ is called!\n";
        #endif
        bigInt res(*this);
        for (size_t i = 0; i < sz; ++i) {
            res.data[i] = ~res.data[i];
        }
        return res;
    }

    bigInt& applyNot()&{    
        #ifdef ENABLE_LOGGING
        std::cout << "applyNot is called!\n";
        #endif
        for (size_t i = 0; i < sz; ++i) {
            data[i] = ~data[i];
        }
        return *this;
    }

    bigInt operator-() const{//-bi
        #ifdef ENABLE_LOGGING
        std::cout << "leftside operator- is called!\n";
        #endif
        bigInt res(~(*this));
        ++res;
        return res;
    }

    bigInt& changeSign()&{
        #ifdef ENABLE_LOGGING
        std::cout << "changeSign is called!\n";
        #endif
        ++(this->applyNot());
        return *this;
    }
    //bitIndex - from 0
    bigInt& setBit(size_t bitIndex, bool val) {
        assert(bitIndex < (sz * 8) && "can't acces bitIndex!\n");

        if(val) {
            data[bitIndex / 8] |= (1 << (bitIndex % 8));
        } else {
            data[bitIndex / 8] &= (~(1 << (bitIndex % 8)));
        }

        return *this;
    }

    bigInt& operator<<=(size_t shift) {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator<<=!\n";
        #endif
        shift %= (sz * 8);
        if(shift == 0) return *this;
        const auto byteShift = shift / 8;
        const auto bitShift = shift % 8;
        constexpr size_t cluster_sz = sizeof(data[0]) * 8;

        if(byteShift == 0) {//the most often
            for(size_t i = (sz - 1); i > 0; --i) {
                (data[i] <<= bitShift) |= (data[i - 1] >> (cluster_sz - bitShift));
            }
            data[0] <<= bitShift;
        } else if(bitShift == 0) {
            for(size_t i = (sz - 1); i >= byteShift; --i) {
                data[i] = (data[i - byteShift]);
            }
            memset(&data[0], 0, byteShift);
        } else {//arbitrary case
            for(size_t i = (sz - 1); i > byteShift; --i) {
                data[i] = (data[i - byteShift] << bitShift) | (data[i - byteShift - 1] >> (cluster_sz - bitShift));
            }
            data[byteShift] = data[0] << bitShift;
            memset(&data[0], 0, byteShift);
        }

        return *this;
    }

    bigInt& operator>>=(size_t shift) {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator>>=!\n";
        #endif
        
        constexpr size_t cluster_sz = sizeof(data[0]) * 8;
        shift %= (sz * 8);
        if(shift == 0) return *this;
        const auto byteShift = shift / 8;
        const auto bitShift = shift % 8;
        bool sign = number.getSign();
        if(byteShift == 0) {//the most often
            for(size_t i = 0; i < (sz - 1); ++i) {
                data[i] = (data[i] >> bitShift) | (data[i + 1] << (cluster_sz - bitShift));
            }
            reinterpret_cast<int8_t&>(data[sz - 1]) >>=  bitShift;
        } else if(bitShift == 0) {
            for(size_t i = 0; i < (sz - byteShift - 1); ++i) {
                data[i] = data[i + byteShift];
            }
            data[sz - byteShift - 1] = reinterpret_cast<int8_t&>(data[sz - 1]);
            memset(&data[sz - byteShift], sign ? 255 : 0, byteShift);
        } else {//arbitrary case
            for(size_t i = 0; i < (sz - byteShift - 1); ++i) {
                data[i] = (data[i + byteShift] >> bitShift) | (data[i + byteShift + 1] << (cluster_sz - bitShift));
            }
            data[sz - byteShift - 1] = reinterpret_cast<int8_t&>(data[sz - 1]) >> bitShift;
            memset(&data[sz - byteShift], sign ? 255 : 0, byteShift);
        }

        return *this;
    }

    //---------------------------------------IO--------------------------------------------
    template <size_t size>
    friend std::ostream& operator<<(std::ostream& os, const bigInt<size>& b);
    //---------------------------------------logical---------------------------------------
    template<typename T>
    requires (std::is_integral_v<T>)
    std::weak_ordering operator<=>(const T& other) const& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator<=> with integer!\n";
        #endif
        if constexpr (sz == sizeof(T)) {
            return std::weak_order(reinterpret_cast<std::make_signed_t<T>&>(data[0]), other);
        } else if (sz < sizeof(T)){//series of cringe optimization
            if constexpr(!std::is_same_v<size_to_int_t<sz>, void>){
                return std::weak_order(
                    static_cast<std::make_signed_t<T>>(
                        reinterpret_cast<const size_to_int_t<sz>&>(data[0]))
                    , other);
            } else {//creating full number considering the sign(if negative - extents with ones, else - zeros)
                using u_T =  std::make_unsigned_t<T>;
                constexpr u_T u_T_max = std::numeric_limits<u_T>::max();

                std::make_signed_t<T> extend = number.getSign() ? u_T_max : 0;//signed/unsigned extention
                memcpy(&extend, &data[0], sz);
                return std::weak_order(extend, other);
            } 
        } else {//sz > sizeof(T)
            if constexpr(std::is_unsigned_v<T>){
                if(number.getSign()) return std::weak_ordering::less;//bigInt is negative and other is positive
                //now bigInt is positive and other is positive -> check exceeding bytes considering the sign(if negative - extents with ones, else - zeros)
                //if any of bytes of data, exceeding other is > 0 (!= 0, bc unsigned), (*this) is less
                if(all_is_same<uint8_t, sz, sizeof(T), 0>(&data[0]) == false) return std::weak_ordering::greater;
                return unsigned_compare<sizeof(T)>(&data[0], reinterpret_cast<const uint8_t*>(&other));
            } else {//other is signed
                if(number.getSign()){//bigInt is negative
                    if(other >= 0) return std::weak_ordering::less;//if other is NOT negative return true
                    //other is negative(both same sign) => can use unsigned comparison
                    if(all_is_same<uint8_t, sz, sizeof(T), 255>(&data[0]) == false) return std::weak_ordering::less;
                    return unsigned_compare<sizeof(T)>(&data[0], reinterpret_cast<const uint8_t*>(&other));
                } else {//bigInt is NOT negative                    
                    if(other < 0) return std::weak_ordering::greater;//other is negative
                    //other is NOT negative(both same sign) => can use unsigned comparison
                    if(all_is_same<uint8_t, sz, sizeof(T), 0>(&data[0]) == false) return std::weak_ordering::greater;
                    return unsigned_compare<sizeof(T)>(&data[0], reinterpret_cast<const uint8_t*>(&other));
                }
            }
        }
    }

    template<size_t other_sz>
    std::weak_ordering operator<=>(const bigInt<other_sz>& other) const& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz <<">.operator<=> with bigInt<" << other_sz <<"> is called!\n";
        #endif        
        if constexpr (sz > other_sz){
            if(number.getSign()){//bigInt is negative
                if(other.number.getSign() == false) return std::weak_ordering::less;
                if(all_is_same<uint8_t, sz, other_sz, 255>(&data[0]) == false) return std::weak_ordering::less;
                return unsigned_compare<other_sz>(&data[0], &other.data[0]);
            } else {//bigInt is NOT negative
                if(other.number.getSign()) return std::weak_ordering::greater;
                if(all_is_same<uint8_t, sz, other_sz, 0>(&data[0]) == false) return std::weak_ordering::greater;
                return unsigned_compare<other_sz>(&data[0], &other.data[0]);
            }
        } else {//sz < other_sz
            if(number.getSign()){//bigInt is negative
                if(other.number.getSign() == false) return std::weak_ordering::less;
                if(all_is_same<uint8_t, other_sz, sz, 255>(&other.data[0]) == false) return std::weak_ordering::greater;
                return unsigned_compare<sz>(&data[0], &other.data[0]);
            } else {//bigInt is NOT negative
                if(other.number.getSign()) return std::weak_ordering::greater;
                if(all_is_same<uint8_t, other_sz, sz, 0>(&other.data[0]) == false) return std::weak_ordering::less;
                return unsigned_compare<sz>(&data[0], &other.data[0]);
            }
        }
    }

    std::weak_ordering operator<=>(const bigInt& other) const& {
        #ifdef ENABLE_LOGGING
        std::cout << "operator< with bigInt same size!\n";   
        #endif 
        if(number.getSign()){//this is negative
            if(other.number.getSign() == false) return std::weak_ordering::less;//if other is NOT negative return true
            return unsigned_compare<sz>(&data[0], &other.data[0]);
        } else {//bigInt is NOT negative
            if(other.number.getSign()) return std::weak_ordering::greater;//if other is negative return true
            return unsigned_compare<sz>(&data[0], &other.data[0]);
        }
    }



    template<typename T>
    requires (std::is_integral_v<T>)
    bool operator==(const T& other) const& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator== with integer sizeof(T) = " << sizeof(T) << "!\n";
        #endif

        if constexpr (sz == sizeof(T)) {
            return reinterpret_cast<T&>(data[0]) == other;
        } else if (sz < sizeof(T)){
            if constexpr(!std::is_same_v<size_to_int_t<sz>, void>){
                return (reinterpret_cast<size_to_int_t<sz>&>(data[0]) == other);
            } else {
                using u_T =  std::make_unsigned_t<T>;
                constexpr u_T u_T_max = std::numeric_limits<u_T>::max();

                std::make_signed_t<T> extend = number.getSign() ? u_T_max : 0;//signed/unsigned extention
                memcpy(reinterpret_cast<unsigned char*>(&extend), &data[0], sz);
                return (extend == other);
            } 
        } else {//sz > sizeof(T)
            if constexpr (std::is_unsigned_v<T>) {
                if(all_is_same<uint8_t, sz, sizeof(T), 0>(&data[0]) == false) return false;
                return all_is_same<sizeof(T), 0>(&data[0], reinterpret_cast<const uint8_t*>(&other));
            } else {
                if(other < 0){
                    if(all_is_same<uint8_t, sz, sizeof(T), 255>(&data[0]) == false) return false;
                    return all_is_same<sizeof(T), 0>(&data[0], reinterpret_cast<const uint8_t*>(&other));
                } else {
                    if(all_is_same<uint8_t, sz, sizeof(T), 0>(&data[0]) == false) return false;
                    return all_is_same<sizeof(T), 0>(&data[0], reinterpret_cast<const uint8_t*>(&other));
                }
            }
        }
    }

    template<size_t other_sz>
    bool operator==(const bigInt<other_sz>& other) const& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz <<">.operator== with bigInt<" << other_sz <<"> is called!\n";
        #endif     

        if constexpr (sz > other_sz){
            if(other.number.getSign()){
                if(all_is_same<uint8_t, sz, other_sz, 255>(&data[0]) == false) return false;
            } else {
                if(all_is_same<uint8_t, sz, other_sz, 0>(&data[0]) == false) return false;
            }

            return all_is_same<other_sz, 0>(&data[0], &other.data[0]);
        } else {//sz < other_sz
            if(number.getSign()){
                if(all_is_same<uint8_t, other_sz, sz, 255>(&other.data[0]) == false) return false;
            } else {
                if(all_is_same<uint8_t, other_sz, sz, 0>(&other.data[0]) == false) return false;
            }

            return all_is_same<sz, 0>(&data[0], &other.data[0]);
        }
    }

    bool operator==(const bigInt& other) const& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator== with bigInt same size!\n";
        #endif 

        return all_is_same<sz, 0>(&data[0], &other.data[0]);
    }



    template<typename T>
    requires (std::is_integral_v<T>)
    bool operator!=(const T& other) const& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator!= with integer sizeof(T) = " << sizeof(T) << "!\n";
        #endif
        
        return !(*this == other);
    }

    template<size_t other_sz>
    bool operator!=(const bigInt<other_sz>& other) const& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz <<">.operator!= with bigInt<" << other_sz <<"> is called!\n";
        #endif        
        
        return !(*this == other);
    }

    bool operator!=(const bigInt& other) const& {
        #ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator!= with bigInt same size!\n";
        #endif 

        return !(*this == other);
    }



    ~bigInt() {
        std::string str = std::to_string(reinterpret_cast<uint64_t&>(data[0]));
        while(str.size() < 20){
            str.push_back(' ');
        }
        #ifdef ENABLE_LOGGING
        std::cout << "| deleted with size: " << sz << " | with value: " << str << " |\n";
        #endif
    }
};

template <size_t sz>
std::ostream& operator<<(std::ostream& os, const bigInt<sz>& b){
    unsigned char (&data)[sz] = b.data;
    size_to_int_t<sz> res = data[0];
    for(size_t i = 1; i < sz; ++i){
        res += static_cast<size_to_int_t<sz>>(data[i]) << (8 * i);
    }
    os << res;
    return os;
}

template<size_t sz>
bigInt<sz> operator+(const bigInt<sz>& bi_1, const bigInt<sz>& bi_2){
    bigInt<sz> res(bi_1);
    res += bi_2;
    #ifdef ENABLE_LOGGING    
    std::cout << "plus between bigInt and bigInt with same size outside struct!\n";
    #endif 
    return res;//return value optimization
} 

template<size_t sz_1, size_t sz_2>
bigInt<sz_1> operator+(const bigInt<sz_1>& bi_1, const bigInt<sz_2>& bi_2){
    bigInt<sz_1> res(bi_1);
    res += bi_2;
    #ifdef ENABLE_LOGGING
    std::cout << "plus between bigInt and bigInt outside struct!\n";
    #endif 
    return res;
} 

template<typename T, size_t sz>
requires(std::is_integral_v<std::remove_cvref_t<T>>)
bigInt<sz> operator+(const bigInt<sz>& bi, T other){
    bigInt<sz> res(bi);
    res += other;
    #ifdef ENABLE_LOGGING
    std::cout << "plus between bigInt and integer outside struct!\n";
    #endif 
    return res;
} 

template<typename T, size_t sz>
requires(std::is_integral_v<std::remove_cvref_t<T>>)
bigInt<sz> operator+(T other, const bigInt<sz>& bi){
    bigInt<sz> res(bi);
    res += other;
    #ifdef ENABLE_LOGGING
    std::cout << "plus between integer and bigInt outside struct!\n";
    #endif 
    return res;
} 

template<size_t sz>
bigInt<sz> operator-(const bigInt<sz>& bi_1, const bigInt<sz>& bi_2){
    bigInt<sz>  res(bi_1);
    res -= bi_2;
    #ifdef ENABLE_LOGGING
    std::cout << "minus between bigInt and bigInt with same size outside struct!\n";
    #endif 
    return res;
} 

template<size_t sz_1, size_t sz_2>
bigInt<sz_1> operator-(const bigInt<sz_1>& bi_1, const bigInt<sz_2>& bi_2){
    bigInt<sz_1> res(bi_1);
    res -= bi_2;
    #ifdef ENABLE_LOGGING
    std::cout << "minus between bigInt and bigInt outside struct!\n";
    #endif 
    return res;
} 

template<typename T, size_t sz>
requires(std::is_integral_v<std::remove_cvref_t<T>>)
bigInt<sz> operator-(const bigInt<sz>& bi, T other){
    bigInt<sz> res(bi);
    res -= other;
    #ifdef ENABLE_LOGGING
    std::cout << "minus between bigInt and integer outside struct!\n";
    #endif 
    return res;
} 

template<typename T, size_t sz>
requires(std::is_integral_v<std::remove_cvref_t<T>>)
bigInt<sz> operator-(T other, const bigInt<sz>& bi){
    bigInt<sz> res(bi);
    res -= other;
    #ifdef ENABLE_LOGGING
    std::cout << "minus between integer and bigInt outside struct!\n";
    #endif 
    return res;
} 



template<typename T, size_t sz>
requires (std::is_integral_v<T>)
bool operator==(T other, const bigInt<sz>& bi) {
    #ifdef ENABLE_LOGGING
    std::cout << "operator== integer sizeof(T) = " << sizeof(T) << " with bigInt<" << sz << ">!\n";
    #endif

    return (bi == other);
}


template<typename T, size_t sz>
requires (std::is_integral_v<T>)
bool operator!=(T other, const bigInt<sz>& bi) {
    #ifdef ENABLE_LOGGING
    std::cout << "operator!= integer sizeof(T) = " << sizeof(T) << " with bigInt<" << sz << ">!\n";
    #endif
    
    return !(bi == other);
}