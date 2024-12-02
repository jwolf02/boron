#ifndef BORON_CBOR_VALUEBUILDER_H_
#define BORON_CBOR_VALUEBUILDER_H_

#include <cstdint>
#include <cstddef>
#include <cstring>

#include "Types.h"

namespace CBOR
{
class ValueBuilder
{
public:
    ValueBuilder() = default;

    constexpr ValueBuilder(Integer x) :
        _type(Type::INTEGER), _i(x) {}

    constexpr ValueBuilder(Float x) :
        _type(Type::FLOAT), _f(x) {}

    ValueBuilder(const char* x) :
        _type(Type::STRING), _t(x, strlen(x)) {}

    template <typename T>
        requires(std::is_fundamental_v<T> && std::is_arithmetic_v<T>)
    ValueBuilder(std::span<const T> array) :
        _type(Type::BYTES), _b((const uint8_t*)array.data(), array.size() * sizeof(T)) {}

    constexpr ValueBuilder(int8_t x) :
        ValueBuilder((Integer)x) {}
    
    constexpr ValueBuilder(int16_t x) :
        ValueBuilder((Integer)x) {}

    constexpr ValueBuilder(int64_t x) :
        ValueBuilder((Integer)x) {}

    constexpr ValueBuilder(uint8_t x) :
        ValueBuilder((Integer)x) {}

    constexpr ValueBuilder(uint16_t x) :
        ValueBuilder((Integer)x) {}

    constexpr ValueBuilder(uint32_t x) :
        ValueBuilder((Integer)x) {}

    constexpr ValueBuilder(const ValueBuilder& builder) = default;

    constexpr ValueBuilder& operator=(const ValueBuilder& builder) = default;

    bool operator==(const ValueBuilder& builder) const 
    { 
        return equals(builder); 
    }

    bool operator!=(const ValueBuilder& builder) const 
    { 
        return !equals(builder); 
    }

    constexpr Type type() const 
    { 
        return _type; 
    }

    constexpr Integer toInt() const 
    { 
        return _i; 
    }

    constexpr Float toFloat() const 
    { 
        return _f; 
    }

    constexpr std::span<const uint8_t> toByteString() const
    {
        return _b;
    }

    constexpr std::span<const char> toTextString() const 
    { 
       return _t; 
    }

    constexpr Simple toSimple() const
    {
        return _s;
    }

private:
    bool equals(const ValueBuilder& builder) const
    {
        if (type() != builder.type())
        {
            return false;
        }

        switch (type())
        {
            case Type::INTEGER:
            {
                return _i == builder._i;
            }
            case Type::BYTES:
            {
                return memcmp(_b.data(), builder._b.data(), _b.size());
            }
            case Type::STRING:
            {
                return memcmp(_t.data(), builder._t.data(), _t.size());
            }
            case Type::FLOAT:
            {
                return _f == builder._f;
            }
            case Type::BOOL:
            {
                return _s == builder._s;
            }
            default:
            {
                return true;
            }
        }
    }

    Type _type = Type::UNDEFINED;

    Integer _i = 0;

    Float _f = 0.0f;

    std::span<const uint8_t> _b;

    std::span<const char> _t;

    Simple _s = Simple::UNDEFINED;
};
} // namespace CBOR

#endif // BORON_CBOR_VALUEBUILDER_H_