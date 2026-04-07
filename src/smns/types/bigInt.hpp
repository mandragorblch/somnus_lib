#pragma once
#include <iostream>
#include <cstdint>
#include <limits>
#include <compare>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <cstring>

template<typename T, typename U>
struct is_same_template_v : std::is_same<T, U>
{
};

template<template<typename> typename T, typename Args1, typename Args2>
struct is_same_template_v<T<Args1>, T<Args2>> : std::true_type
{
};

//sz - size in bytes
template<size_t sz>
struct Number {
    unsigned char val[sz] {};

    template<typename T>
        requires(std::is_integral_v<T>)
    Number(const T& other) {
        static_assert(sz >= sizeof(T), "u don't wanna shrink fundamental type (yeah, i'm pretty sure u don't :3)");

        if constexpr (sz == sizeof(T)) {
            reinterpret_cast<T&>(val[0]) = other;
        } else if (sz > sizeof(T)) {
            std::memset(&val[sizeof(T)], other < 0 ? ~0 : 0, sz - sizeof(T));
            reinterpret_cast<T&>(val[0]) = other;
        }
    }

    template<size_t other_sz>
    Number(const Number<other_sz>& other) {
        if constexpr (sz > other_sz) std::memset(&val[other_sz], other.getSign() ? 0xff : 0x00, sz - other_sz);
        std::memcpy(&val[0], &other.val[0], std::min(sz, other_sz));
    }

    bool getSign() const {
        return reinterpret_cast<const char&>(val[sz - 1]) < 0;
    }
};

template<typename T>
    requires(std::is_integral_v<T> && std::is_unsigned_v<T>)
bool add_overflow(T a, T b, T* dst) {
    return (dst = a + b) < b;
    //return __builtin_add_overflow(a, b, dst);
}

template<typename T, typename U>
    requires(std::is_integral_v<T> && std::is_unsigned_v<T> &&
             std::is_integral_v<U> && std::is_unsigned_v<U> &&
             sizeof(T) >= sizeof(U))
bool add_overflow(T& dst, const U& val) {
    return (dst += val) < (val);
    //return __builtin_add_overflow(dst, val, &dst);
}

template<typename T>
    requires(std::is_integral_v<T> && std::is_unsigned_v<T>)
bool add_with_carry(T& dst, const T val, const bool carry) {
    return add_overflow(dst, carry) | add_overflow(dst, val);
}

template<typename T, typename U>
    requires(std::is_integral_v<T> && std::is_unsigned_v<T> &&
             std::is_integral_v<U> && std::is_unsigned_v<U> &&
             sizeof(T) >= sizeof(U))
bool sub_overflow(T& dst, const U& val) {
    bool carry = dst < val;
    dst -= val;
    return carry;
}

template<typename T>
    requires(std::is_integral_v<T>&& std::is_unsigned_v<T>)
bool sub_with_borrow(T& dst, const T val, const bool carry) {
    return sub_overflow(dst, carry) | sub_overflow(dst, val);
}

template<size_t sz>
std::weak_ordering unsigned_compare(const uint8_t* data, const uint8_t* other_data) {
    for (size_t i = (sz - 1); i < std::numeric_limits<size_t>::max(); --i) {
        if (data[i] == other_data[i]) continue;
        return (data[i] < other_data[i]) ? std::weak_ordering::less : std::weak_ordering::greater;
    }
    return std::weak_ordering::equivalent;//is equal
}

template<size_t end, size_t begin>
bool all_is_same(const void* data, const void* other_data) {

    for (size_t i = begin; i < end; ++i) {
        if (reinterpret_cast<const uint8_t*>(data)[i] != reinterpret_cast<const uint8_t*>(other_data)[i]) return false;//0 - signed extension. if data[i] == 0 for will continue automaticaly
    }
    return true;
}

template<typename T, size_t end, size_t begin, T val>
bool all_is_same(const void* data) {
    for (size_t i = begin; i < end; ++i) {
        if (reinterpret_cast<const uint8_t*>(data)[i] != val) return false;//0 - signed extension. if data[i] == 0 for will continue automaticaly
    }
    return true;
}

//void if there is no fitting int
template<size_t sz>
struct size_to_int {
    using type = void;
};

template<>
struct size_to_int<1> {
    using type = int8_t;
};

template<>
struct size_to_int<2> {
    using type = int16_t;
};

template<>
struct size_to_int<4> {
    using type = int32_t;
};

template<>
struct size_to_int<8> {
    using type = int64_t;
};

template<size_t sz>
using size_to_int_t = typename size_to_int<sz>::type;



//chunk_sz - amount of elements of type chunk_t
//chunk_t - type(size) of data chunks
//sz - size in bytes
template<size_t chunk_sz, typename chunk_t = uint8_t, size_t sz = chunk_sz * sizeof(chunk_t)>
    requires(std::is_unsigned_v<chunk_t> && sz > 0 && sz % sizeof(chunk_t) == 0)
struct bigInt {
    Number<sz> number;
    chunk_t* const data = &number.val[0];
    //tells if last op cause overflow
    bool overflow = false;

private:
    static constexpr chunk_t chunk_max = std::numeric_limits<chunk_t>::max();

public:

    //explicit static conversion to type T
    template<typename T>
        requires (std::is_integral_v<std::remove_cvref_t<T>>)
    explicit operator T() {
#ifdef ENABLE_LOGGING
        std::cout << "explicitly casted!\n";
#endif
        static_assert(sz <= sizeof(T), "u don't wanna shrink ur number (yeah, i'm pretty sure u don't :3)");
        bigInt<sizeof(T)> res = *this;
        return reinterpret_cast<T&>(res.data[0]);
    }



    bigInt() = delete;

    template<typename T>
        requires (std::is_integral_v<T>)
    bigInt(const T& other) : number(other) {
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << "> constructor from integer!\n";
#endif
    }

    template<size_t other_sz>
    bigInt(const bigInt<other_sz>& other) : number(other.number) {
#ifdef ENABLE_LOGGING
        std::cout << "constructed from bigInt!\n";
#endif
    }

    //copy constructor
    bigInt(const bigInt& other) : number(other.number) {
#ifdef ENABLE_LOGGING
        std::cout << "constructed from bigInt with same size!\n";
#endif
    }



    template<typename T>
        requires (std::is_integral_v<T>)
    bigInt& operator=(const T& other)& {
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator= with integer sizeof(T) = " << sizeof(T) << "!\n";
#endif    
        static_assert(sz >= sizeof(T), "u don't wanna shrink fundamental type (yeah, i'm pretty sure u don't :3)");
        //sizeof(T) + 1 to make difference between signed/unsigned T clear
        return *this = bigInt<sizeof(T) + 1>(other);
    }

    template<size_t other_sz>
    bigInt& operator=(const bigInt<other_sz>& other)& {
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator= with bigInt<" << other_sz << ">!\n";
#endif 
        static_assert(sz >= other_sz, "u don't wanna shrink other (yeah, i'm pretty sure u don't :3)");

        //signed extension
        std::memset(&data[other_sz], other.signbit() ? chunk_max : 0, sz - other_sz);
        std::memcpy(&data[0], &other.data[0], other_sz);
        return *this;
    }

    //copy assignment 
    bigInt& operator=(const bigInt& other)& {
#ifdef ENABLE_LOGGING
        std::cout << "equaled to bigInt with same size!\n";
#endif  

        if (this == &other)
            return *this;

        std::memcpy(&data[0], &other.data[0], sz);
        return *this;
    }

    //------------------------------------arithmetic---------------------------------------

    template<typename T>
        requires (std::is_integral_v<T>)
    bigInt& operator+=(const T& other)& {
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator+= with integer sizeof(T) = " << sizeof(T) << "!\n";
#endif  
        static_assert(sz >= sizeof(T), "u don't wanna add bigger or the same size fundamental type (yeah, i'm pretty sure u don't :3)");
        //sizeof(T) + 1 to make difference between signed/unsigned T clear
        return *this += bigInt<sizeof(T) + 1>(other);
    }

    template<size_t other_chunk_sz, typename other_chunk_t, size_t other_sz>
    bigInt& operator+=(const bigInt<other_chunk_sz, other_chunk_t, other_sz>& other)& {
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator+= with bigInt<" << other_sz << ">!\n";
#endif
        static_assert(sz >= other_sz, "u don't wanna add bigger or the same size bigInt (yeah, i'm pretty sure u don't :3)");

        overflow = false;
        //carry flag
        bool carry = add_overflow(data[0], other.data[0]);
        //TODO - rewrite that for arbitrary chunk_t
        for (size_t i = 1; i < std::min(sz, other_sz); ++i) {
            carry = add_with_carry(data[i], other.data[i], carry);
        }

        if constexpr (sz > other_sz) {
            //bringing carry block-to-block
            if (other.signbit()) {
                //signed extension
                //if we have carry = 1, it will overflow ALL signed extension
                //and all further addition will be with 0
                for (size_t i = other_sz; !carry && (i < sz); ++i) {
                    carry = add_with_carry(data[i], chunk_max, carry);
                }
            } else {
                //propagate carry
                for (size_t i = other_sz; carry && (i < sz); ++i) {
                    carry = (++data[i] == 0);
                }
            }
        }

        overflow = carry;

        return *this;
    }


    template<typename T>
        requires (std::is_integral_v<T>)
    bigInt& operator-=(const T& other)& {
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator-= with integer sizeof(T) = " << sizeof(T) << "!\n";
#endif  

        if constexpr (sz >= sizeof(T)) {
            //sizeof(T) + 1 to make difference between signed/unsigned T clear
            return *this -= bigInt<sizeof(T) + 1>(other);
        }
        static_assert(sz >= sizeof(T), "u don't wanna sub bigger fundamental type (yeah, i'm pretty sure u don't :3)");
        return *this;
    }

    template<size_t other_sz>
    bigInt& operator-=(const bigInt<other_sz>& other)& {
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator-= with bigInt<" << other_sz << ">!\n";
#endif
        static_assert(sz >= other_sz, "u don't wanna sub bigger number (yeah, i'm pretty sure u don't :3)");

        //ofc there can be a DRY idiom, but there is a copy
        //return (*this += -other);

        overflow = false;
        //borrow flag
        bool borrow = sub_overflow(data[0], other.data[0]);
        for (size_t i = 1; i < std::min(sz, other_sz); ++i) {
            borrow = sub_with_borrow(data[i], other.data[i], borrow);
        }

        if constexpr (sz > other_sz) {
            //bringing carry block-to-block
            if (other.signbit()) {
                //signed extension
                //that optimization is a tricky one! sooo...
                //we have (data[i] -= chunk_max + borrow) here, so if we had borrow = 1
                //then we overflow chunk_max and then we have (data[i] -= 0)
                //and (borrow = 1) for next data block, and that borrow underflows
                //ALL further blocks and it ended up with (data[i] -= 0) till the end
                //so no need to continue
                for (size_t i = other_sz; !borrow && i < sz; ++i) {
                    borrow = sub_with_borrow(data[i], chunk_max, borrow);
                }
            } else {
                //propagate borrow
                for (size_t i = other_sz; borrow && (i < sz); ++i) {
                    borrow = (--data[i] == chunk_max);
                }
            }
        }

        overflow = borrow;

        return *this;
    }



    template<typename T>
        requires (std::is_integral_v<T>)
    bigInt& operator/=(const T& other)& {
        if constexpr (sz == sizeof(T)) {
            reinterpret_cast<std::make_signed_t<T>&>(data[0]) /= other;
        } else if (sz < sizeof(T)) {
            if constexpr (!std::is_same_v<size_to_int_t<sz>, void>) {
                reinterpret_cast<size_to_int_t<sz>&>(data[0]) /= other;
            } else {
                assert(other != 0 && "division by zero!");

                bool sign = number.getSign();
                if (sign) changeSign();
                bool other_sign = (other < 0);
                constexpr size_t bits_in_dividend = sizeof(T) * 8;

                bigInt<sizeof(T) * 2> remainder(*this);
                bigInt<sizeof(T)> quotient = *this;
                //TODO: divisor is const, shifted by whole bytes => we can use add function, that skip first sz bytes(they are zeros)
                bigInt<sizeof(T) * 2> divisor = (other_sign ? -other : other);
                divisor <<= (sizeof(T) * 8);

                for (size_t i = bits_in_dividend - 1; i < std::numeric_limits<size_t>::max(); --i) {
                    if (remainder.number.getSign()) {
                        quotient.setBit(i, false);
                        (remainder <<= 1) += divisor;
                    } else {
                        quotient.setBit(i, true);
                        (remainder <<= 1) -= divisor;
                    }
                }
                quotient -= ~quotient;

                if (remainder.number.getSign()) {
                    --quotient;
                    //remainder += divisor;
                }
                //remainder >>= bits_in_dividend;
                if (sign ^ other_sign) quotient.changeSign();
                (*this) = quotient;
            }
        } else {//sz > sizeof(T)
            assert(other != 0 && "division by zero!");

            bool sign = number.getSign();
            if (sign) changeSign();
            bool other_sign = (other < 0);
            constexpr size_t bits_in_dividend = sz * 8;

            bigInt<sz * 2> remainder(*this);
            bigInt<sz>& quotient = *this;
            //TODO: divisor is const, shifted by whole bytes => we can use add function, that skip first sz bytes(they are zeros)
            bigInt<sz * 2> divisor(other_sign ? -other : other);
            divisor <<= bits_in_dividend;

            for (size_t i = bits_in_dividend - 1; i < std::numeric_limits<size_t>::max(); --i) {
                if (remainder.number.getSign()) {
                    quotient.setBit(i, false);
                    (remainder <<= 1) += divisor;
                } else {
                    quotient.setBit(i, true);
                    (remainder <<= 1) -= divisor;
                }
            }
            quotient -= ~quotient;

            if (remainder.number.getSign()) {
                --quotient;
                //remainder += divisor;
            }
            //remainder >>= bits_in_dividend;
            if (sign ^ other_sign) quotient.changeSign();
            //(*this) = quotient;
        }

        return *this;
    }

    template<size_t other_sz>
    bigInt& operator/=(const bigInt<other_sz>& other)& {
        constexpr auto max_sz = std::max(sz, other_sz);
        assert(other != 0 && "division by zero!");

        bool sign = number.getSign();
        if (sign) changeSign();
        bool other_sign = (other.number.getSign());
        constexpr size_t bits_in_dividend = sz * 8;

        bigInt<sz * 2> remainder(*this);
        bigInt<sz>& quotient(*this);
        //TODO: divisor is const, shifted by whole bytes => we can use add function, that skip first sz bytes(they are zeros)
        bigInt<sz * 2> divisor(other_sign ? -other : other);
        divisor <<= bits_in_dividend;

        for (size_t i = bits_in_dividend - 1; i < std::numeric_limits<size_t>::max(); --i) {
            if (remainder.number.getSign()) {
                quotient.setBit(i, false);
                (remainder <<= 1) += divisor;
            } else {
                quotient.setBit(i, true);
                (remainder <<= 1) -= divisor;
            }
        }
        quotient -= ~quotient;

        if (remainder.number.getSign()) {
            --quotient;
            //remainder += divisor;
        }
        //remainder >>= bits_in_dividend;
        if (sign ^ other_sign) quotient.changeSign();
        //(*this) = quotient;

        return *this;
    }

    bigInt& operator/=(const bigInt& other)& {
        assert(other != 0 && "division by zero!");

        bool sign = number.getSign();
        if (sign) changeSign();
        bool other_sign = (other.number.getSign());
        constexpr size_t bits_in_dividend = sz * 8;

        bigInt<sz * 2> remainder(*this);
        bigInt<sz>& quotient(*this);
        //TODO: divisor is const, shifted by whole bytes => we can use add function, that skip first sz bytes(they are zeros)
        bigInt<sz * 2> divisor(other_sign ? -other : other);
        divisor <<= bits_in_dividend;

        for (size_t i = bits_in_dividend - 1; i < std::numeric_limits<size_t>::max(); --i) {
            if (remainder.number.getSign()) {
                quotient.setBit(i, false);
                (remainder <<= 1) += divisor;
            } else {
                quotient.setBit(i, true);
                (remainder <<= 1) -= divisor;
            }
        }
        quotient -= ~quotient;

        if (remainder.number.getSign()) {
            --quotient;
            //remainder += divisor;
        }
        //remainder >>= bits_in_dividend;
        if (sign ^ other_sign) quotient.changeSign();
        //(*this) = quotient;

        return *this;
    }



    bigInt& operator++()& {//++bi 
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator++()!\n";
#endif
        return (*this += bigInt<1>(true));
    }

    bigInt operator++(int)& {//bi++
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator++(int)!\n";
#endif
        bigInt res(*this);
        ++(*this);
        return res;
    }

    bigInt& operator--()& {//--bi
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator--()!\n";
#endif
        return (*this -= bigInt<1>(true));
    }

    bigInt operator--(int)& {//bi--
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator--(int)!\n";
#endif
        bigInt res(*this);
        --(*this);
        return res;
    }
    //---------------------------------------bitwise---------------------------------------
    bigInt operator~() const {//~bi
#ifdef ENABLE_LOGGING
        std::cout << "operator~ is called!\n";
#endif
        bigInt res(*this);
        for (size_t i = 0; i < chunk_sz; ++i) {
            res.data[i] = ~res.data[i];
        }
        return res;
    }

    bigInt& applyNot()& {
#ifdef ENABLE_LOGGING
        std::cout << "applyNot is called!\n";
#endif
        for (size_t i = 0; i < chunk_sz; ++i) {
            data[i] = ~data[i];
        }
        return *this;
    }

    bigInt operator-() const {//-bi
#ifdef ENABLE_LOGGING
        std::cout << "leftside operator- is called!\n";
#endif
        bigInt res(~(*this));
        ++res;
        return res;
    }

    bigInt& changeSign()& {
#ifdef ENABLE_LOGGING
        std::cout << "changeSign is called!\n";
#endif
        return ++(this->applyNot());
    }
    
    //bitIndex - from 0
    bigInt& setBit(size_t bitIndex, bool val) {
        assert(bitIndex < (sz * 8) && "can't acces bitIndex!\n");

        constexpr size_t bits_per_chunk = sizeof(chunk_t) * 8;
        const size_t chunkIndex = bitIndex / bits_per_chunk;
        const size_t bitInChunk = bitIndex % bits_per_chunk;
        const chunk_t mask = static_cast<chunk_t>(1) << bitInChunk;
        if (val) {
            data[chunkIndex] |= mask;
        } else {
            data[chunkIndex] &= ~mask;
        }

        return *this;
    }

    bigInt& operator<<=(size_t shift) {
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator<<=!\n";
#endif
        shift %= (sz * 8);
        if (shift == 0) return *this;
        const auto byteShift = shift / 8;
        const auto bitShift = shift % 8;
        constexpr size_t cluster_sz = sizeof(data[0]) * 8;

        if (byteShift == 0) {//the most often
            for (size_t i = (sz - 1); i > 0; --i) {
                (data[i] <<= bitShift) |= (data[i - 1] >> (cluster_sz - bitShift));
            }
            data[0] <<= bitShift;
        } else if (bitShift == 0) {
            for (size_t i = (sz - 1); i >= byteShift; --i) {
                data[i] = (data[i - byteShift]);
            }
            std::memset(&data[0], 0, byteShift);
        } else {//arbitrary case
            for (size_t i = (sz - 1); i > byteShift; --i) {
                data[i] = (data[i - byteShift] << bitShift) | (data[i - byteShift - 1] >> (cluster_sz - bitShift));
            }
            data[byteShift] = data[0] << bitShift;
            std::memset(&data[0], 0, byteShift);
        }

        return *this;
    }

    bigInt& operator>>=(size_t shift) {
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator>>=!\n";
#endif

        constexpr size_t cluster_sz = sizeof(data[0]) * 8;
        shift %= (sz * 8);
        if (shift == 0) return *this;
        const auto byteShift = shift / 8;
        const auto bitShift = shift % 8;
        bool sign = number.getSign();
        if (byteShift == 0) {//the most often
            for (size_t i = 0; i < (sz - 1); ++i) {
                data[i] = (data[i] >> bitShift) | (data[i + 1] << (cluster_sz - bitShift));
            }
            reinterpret_cast<int8_t&>(data[sz - 1]) >>= bitShift;
        } else if (bitShift == 0) {
            for (size_t i = 0; i < (sz - byteShift - 1); ++i) {
                data[i] = data[i + byteShift];
            }
            data[sz - byteShift - 1] = reinterpret_cast<int8_t&>(data[sz - 1]);
            std::memset(&data[sz - byteShift], sign ? 255 : 0, byteShift);
        } else {//arbitrary case
            for (size_t i = 0; i < (sz - byteShift - 1); ++i) {
                data[i] = (data[i + byteShift] >> bitShift) | (data[i + byteShift + 1] << (cluster_sz - bitShift));
            }
            data[sz - byteShift - 1] = reinterpret_cast<int8_t&>(data[sz - 1]) >> bitShift;
            std::memset(&data[sz - byteShift], sign ? 255 : 0, byteShift);
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
        } else if (sz < sizeof(T)) {//series of cringe optimization
            if constexpr (!std::is_same_v<size_to_int_t<sz>, void>) {
                return std::weak_order(
                    static_cast<std::make_signed_t<T>>(
                        reinterpret_cast<const size_to_int_t<sz>&>(data[0]))
                    , other);
            } else {//creating full number considering the sign(if negative - extents with ones, else - zeros)
                using u_T = std::make_unsigned_t<T>;
                constexpr u_T u_T_max = std::numeric_limits<u_T>::max();

                std::make_signed_t<T> extend = number.getSign() ? u_T_max : 0;//signed/unsigned extention
                std::memcpy(&extend, &data[0], sz);
                return std::weak_order(extend, other);
            }
        } else {//sz > sizeof(T)
            if constexpr (std::is_unsigned_v<T>) {
                if (number.getSign()) return std::weak_ordering::less;//bigInt is negative and other is positive
                //now bigInt is positive and other is positive -> check exceeding bytes considering the sign(if negative - extents with ones, else - zeros)
                //if any of bytes of data, exceeding other is > 0 (!= 0, bc unsigned), (*this) is less
                if (all_is_same<uint8_t, sz, sizeof(T), 0>(&data[0]) == false) return std::weak_ordering::greater;
                return unsigned_compare<sizeof(T)>(&data[0], reinterpret_cast<const uint8_t*>(&other));
            } else {//other is signed
                if (number.getSign()) {//bigInt is negative
                    if (other >= 0) return std::weak_ordering::less;//if other is NOT negative return true
                    //other is negative(both same sign) => can use unsigned comparison
                    if (all_is_same<uint8_t, sz, sizeof(T), 255>(&data[0]) == false) return std::weak_ordering::less;
                    return unsigned_compare<sizeof(T)>(&data[0], reinterpret_cast<const uint8_t*>(&other));
                } else {//bigInt is NOT negative                    
                    if (other < 0) return std::weak_ordering::greater;//other is negative
                    //other is NOT negative(both same sign) => can use unsigned comparison
                    if (all_is_same<uint8_t, sz, sizeof(T), 0>(&data[0]) == false) return std::weak_ordering::greater;
                    return unsigned_compare<sizeof(T)>(&data[0], reinterpret_cast<const uint8_t*>(&other));
                }
            }
        }
    }

    template<size_t other_sz>
    std::weak_ordering operator<=>(const bigInt<other_sz>& other) const& {
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">.operator<=> with bigInt<" << other_sz << "> is called!\n";
#endif        
        if constexpr (sz > other_sz) {
            if (number.getSign()) {//bigInt is negative
                if (other.number.getSign() == false) return std::weak_ordering::less;
                if (all_is_same<uint8_t, sz, other_sz, 255>(&data[0]) == false) return std::weak_ordering::less;
                return unsigned_compare<other_sz>(&data[0], &other.data[0]);
            } else {//bigInt is NOT negative
                if (other.number.getSign()) return std::weak_ordering::greater;
                if (all_is_same<uint8_t, sz, other_sz, 0>(&data[0]) == false) return std::weak_ordering::greater;
                return unsigned_compare<other_sz>(&data[0], &other.data[0]);
            }
        } else {//sz < other_sz
            if (number.getSign()) {//bigInt is negative
                if (other.number.getSign() == false) return std::weak_ordering::less;
                if (all_is_same<uint8_t, other_sz, sz, 255>(&other.data[0]) == false) return std::weak_ordering::greater;
                return unsigned_compare<sz>(&data[0], &other.data[0]);
            } else {//bigInt is NOT negative
                if (other.number.getSign()) return std::weak_ordering::greater;
                if (all_is_same<uint8_t, other_sz, sz, 0>(&other.data[0]) == false) return std::weak_ordering::less;
                return unsigned_compare<sz>(&data[0], &other.data[0]);
            }
        }
    }

    std::weak_ordering operator<=>(const bigInt& other) const& {
#ifdef ENABLE_LOGGING
        std::cout << "operator< with bigInt same size!\n";
#endif 
        if (number.getSign()) {//this is negative
            if (other.number.getSign() == false) return std::weak_ordering::less;//if other is NOT negative return true
            return unsigned_compare<sz>(&data[0], &other.data[0]);
        } else {//bigInt is NOT negative
            if (other.number.getSign()) return std::weak_ordering::greater;//if other is negative return true
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
        } else if (sz < sizeof(T)) {
            if constexpr (!std::is_same_v<size_to_int_t<sz>, void>) {
                return (reinterpret_cast<size_to_int_t<sz>&>(data[0]) == reinterpret_cast<const std::make_signed_t<T>&>(other));
            } else {
                using u_T = std::make_unsigned_t<T>;
                constexpr u_T u_T_max = std::numeric_limits<u_T>::max();

                std::make_signed_t<T> extend = number.getSign() ? u_T_max : 0;//signed/unsigned extention
                std::memcpy(reinterpret_cast<unsigned char*>(&extend), &data[0], sz);
                return (extend == other);
            }
        } else {//sz > sizeof(T)
            if constexpr (std::is_unsigned_v<T>) {
                if (all_is_same<uint8_t, sz, sizeof(T), 0>(&data[0]) == false) return false;
                return all_is_same<sizeof(T), 0>(&data[0], reinterpret_cast<const uint8_t*>(&other));
            } else {
                if (other < 0) {
                    if (all_is_same<uint8_t, sz, sizeof(T), 255>(&data[0]) == false) return false;
                    return all_is_same<sizeof(T), 0>(&data[0], reinterpret_cast<const uint8_t*>(&other));
                } else {
                    if (all_is_same<uint8_t, sz, sizeof(T), 0>(&data[0]) == false) return false;
                    return all_is_same<sizeof(T), 0>(&data[0], reinterpret_cast<const uint8_t*>(&other));
                }
            }
        }
    }

    template<size_t other_sz>
    bool operator==(const bigInt<other_sz>& other) const& {
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">.operator== with bigInt<" << other_sz << "> is called!\n";
#endif     

        if constexpr (sz > other_sz) {
            if (other.number.getSign()) {
                if (all_is_same<uint8_t, sz, other_sz, 255>(&data[0]) == false) return false;
            } else {
                if (all_is_same<uint8_t, sz, other_sz, 0>(&data[0]) == false) return false;
            }

            return all_is_same<other_sz, 0>(&data[0], &other.data[0]);
        } else {//sz < other_sz
            if (number.getSign()) {
                if (all_is_same<uint8_t, other_sz, sz, 255>(&other.data[0]) == false) return false;
            } else {
                if (all_is_same<uint8_t, other_sz, sz, 0>(&other.data[0]) == false) return false;
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
        std::cout << "bigInt<" << sz << ">.operator!= with bigInt<" << other_sz << "> is called!\n";
#endif        

        return !(*this == other);
    }

    bool operator!=(const bigInt& other) const& {
#ifdef ENABLE_LOGGING
        std::cout << "bigInt<" << sz << ">::operator!= with bigInt same size!\n";
#endif 

        return !(*this == other);
    }


    
    bool signbit() const& {
        return number.getSign();
    }


    ~bigInt() {
        std::string str = std::to_string(reinterpret_cast<uint64_t&>(data[0]));
        while (str.size() < 20) {
            str.push_back(' ');
        }
#ifdef ENABLE_LOGGING
        std::cout << "| deleted with size: " << sz << " | with value: " << str << " |\n";
#endif
    }
};

template <size_t sz>
std::ostream& operator<<(std::ostream& os, const bigInt<sz>& b) {
    auto data = b.data;
    int64_t res = data[0];
    for (size_t i = 1; i < sz; ++i) {
        res += static_cast<int64_t>(data[i]) << (8 * i);
    }
    os << res;
    return os;
}

template<size_t sz>
bigInt<sz> operator+(const bigInt<sz>& bi_1, const bigInt<sz>& bi_2) {
    bigInt<sz> res(bi_1);
    res += bi_2;
#ifdef ENABLE_LOGGING    
    std::cout << "plus between bigInt and bigInt with same size outside struct!\n";
#endif 
    return res;//return value optimization
}

template<size_t sz_1, size_t sz_2>
bigInt<sz_1> operator+(const bigInt<sz_1>& bi_1, const bigInt<sz_2>& bi_2) {
    bigInt<sz_1> res(bi_1);
    res += bi_2;
#ifdef ENABLE_LOGGING
    std::cout << "plus between bigInt and bigInt outside struct!\n";
#endif 
    return res;
}

template<typename T, size_t sz>
    requires(std::is_integral_v<std::remove_cvref_t<T>>)
bigInt<sz> operator+(const bigInt<sz>& bi, T other) {
    bigInt<sz> res(bi);
    res += other;
#ifdef ENABLE_LOGGING
    std::cout << "plus between bigInt and integer outside struct!\n";
#endif 
    return res;
}

template<typename T, size_t sz>
    requires(std::is_integral_v<std::remove_cvref_t<T>>)
bigInt<sz> operator+(T other, const bigInt<sz>& bi) {
    bigInt<sz> res(bi);
    res += other;
#ifdef ENABLE_LOGGING
    std::cout << "plus between integer and bigInt outside struct!\n";
#endif 
    return res;
}



template<size_t sz>
bigInt<sz> operator-(const bigInt<sz>& bi_1, const bigInt<sz>& bi_2) {
    bigInt<sz>  res(bi_1);
    res -= bi_2;
#ifdef ENABLE_LOGGING
    std::cout << "minus between bigInt and bigInt with same size outside struct!\n";
#endif 
    return res;
}

template<size_t sz_1, size_t sz_2>
bigInt<sz_1> operator-(const bigInt<sz_1>& bi_1, const bigInt<sz_2>& bi_2) {
    bigInt<sz_1> res(bi_1);
    res -= bi_2;
#ifdef ENABLE_LOGGING
    std::cout << "minus between bigInt and bigInt outside struct!\n";
#endif 
    return res;
}

template<typename T, size_t sz>
    requires(std::is_integral_v<std::remove_cvref_t<T>>)
bigInt<sz> operator-(const bigInt<sz>& bi, T other) {
    bigInt<sz> res(bi);
    res -= other;
#ifdef ENABLE_LOGGING
    std::cout << "minus between bigInt and integer outside struct!\n";
#endif 
    return res;
}

template<typename T, size_t sz>
    requires(std::is_integral_v<std::remove_cvref_t<T>>)
bigInt<sz> operator-(T other, const bigInt<sz>& bi) {
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