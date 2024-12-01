#ifndef BORON_BYTES_H_
#define BORON_BYTES_H_

#include <cstdint>
#include <cstddef>

#include <array>
#include <type_traits>

#if __cplusplus >= 202000L
#include <span>
#else
#include <etl/span.h>

namespace std
{
template <typename T>
using span = etl::span;
} // namespace std
#endif

namespace Bytes
{
enum class Endianess
{
    LITTLE,
    BIG,
    NETWORK = BIG,
    #if BYTE_ORDER == LITTLE_ENDIAN
    NATIVE = LITTLE
    #else
    NATIVE = BIG
    #endif // BYTE_ORDER == LITTLE_ENDIAN
};

template <typename T, size_t N = sizeof(T)>
    requires(std::is_fundamental_v<T>)
inline std::array<uint8_t, N> getBytes(const T& x, Endianess endianess = Endianess::NATIVE)
{
    union
    {
        T x;

        std::array<uint8_t, N> bytes;
    } cvt;
    
    cvt.x = x;

    if (endianess != Endianess::NATIVE)
    {
        std::reverse(cvt.bytes.begin(), cvt.bytes.end());
    }

    return cvt.bytes;
}

template <typename T, size_t N = sizeof(T)>
    requires(sizeof(T) >= N)
inline T fromBytes(std::span<const uint8_t> bytes, Endianess endianess = Endianess::NATIVE)
{
    union 
    {
        T x;

        std::array<uint8_t, N> bytes;
    } cvt;

    std::copy(bytes.begin(), bytes.end(), cvt.bytes.begin());

    if (endianess != Endianess::NATIVE)
    {
        std::reverse(cvt.bytes.begin(), cvt.bytes.end());
    }

    return cvt.x;
}

inline constexpr bool isHexDigit(char c)
{
    return (c >= 'a' && c <= 'f') || (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

/***
 * Parse a 4 bit nibble from a char and store it in the lower half of a byte.
 * 
 * @param nibble Single hex nibble
 * 
 * @return Byte with the lower half set to the nibble
 */
constexpr inline uint8_t parseNibble(char nibble)
{
    if (nibble >= '0' && nibble <= '9')
    {
        return nibble - '0';
    }
    else if (nibble >= 'a' && nibble <= 'f')
    {
        return 10 + (nibble - 'a');
    }
    else if (nibble >= 'A' && nibble <= 'F')
    {
        return 10 + (nibble - 'A');
    }
    else
    {
        return 0;
    }
}

/***
 * Parse a 8 bit byte from two nibbles and store it in a byte.
 * 
 * @param upper The upper half of the byte
 * @param lower The lower half of the byte
 * 
 * @return Byte containing the two parsed nibbles
 */
constexpr inline uint8_t parseByte(char upper, char lower)
{
    return (uint8_t)(parseNibble(upper) << 4) | parseNibble(lower);
}

namespace Literals
{
template <typename InputIterator, typename OutputIterator>
constexpr void __parseHexDigitsToBytes(InputIterator begin, InputIterator end, OutputIterator out)
{
    for (auto it = begin; it != end; ++it)
    {
        auto a = *(it++);
        auto b = *it;
        uint8_t x = parseByte(a, b);
        *(out++) = x;
    }
}

template <char c0, char c1, char ...cs>
constexpr inline std::array<uint8_t, sizeof...(cs) / 2> __parseArrayFromHexChars()
{
    static_assert(c0 == '0' && c1 == 'x', "can only create byte array with preceeding hex chars '0x'");
    static_assert(sizeof...(cs) % 2 == 0, "must be even number of characters");
    static_assert((isHexDigit(cs) && ...), "non-hex character are not allowed");

    const std::array<char, sizeof...(cs)> chars = { cs... };
    std::array<uint8_t, sizeof...(cs) / 2> bytes{};
    __parseHexDigitsToBytes(chars.begin(), chars.end(), bytes.begin());

    return bytes;
}

template <char ...cs>
constexpr inline std::array<uint8_t, (sizeof...(cs) / 2) - 1> operator""_bytes()
{
    return __parseArrayFromHexChars<cs...>();
}
} // namepace Literals
} // namespace Bytes

#endif // BORON_BYTES_H_