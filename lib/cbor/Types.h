#ifndef BORON_CBOR_TYPES_H_
#define BORON_CBOR_TYPES_H_

#include <cstdint>
#include <cstddef>
#include <climits>

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

#include "Tags.h"

namespace CBOR
{
using Integer = uint64_t;
using Float = double;
using Boolean = bool;

enum class Error
{
    OK = 0,
    ALLOC_FAILED,
    UNEXPECTED_EOF,
    UNSUPPORTED_DATATYPE,
    MALFORMED_MESSAGE
};

enum class MajorType : uint8_t
{
    UNSIGNED_INT = 0,
    SIGNED_INT = 1,
    BYTE_STRING = 2,
    TEXT_STRING = 3,
    ARRAY = 4,
    MAP = 5,
    TAGGED = 6,
    FLOAT_OR_SIMPLE
};

enum class Type : uint8_t
{
    INTEGER,
    BYTES,
    STRING,
    ARRAY,
    MAP,
    FLOAT,
    BOOL,
    NULLVAL,
    UNDEFINED
};

enum class Simple
{
    TRUE,
    FALSE,
    NULLVAL,
    UNDEFINED
};

constexpr uint8_t MAX_ARGUMENT_VALUE_IN_REMAINDER = 23;

enum class ArgumentType : uint8_t
{
    NEXT_1_BYTE = 24,
    NEXT_2_BYTES = 25,
    NEXT_4_BYTES = 26,
    NEXT_8_BYTES = 27,
    NONE = 31
};

enum class FloatOrSimpleArgumentType : uint8_t
{
    FALSE = 20,
    TRUE = 21,
    NULLVAL = 22,
    UNDEFINED = 23,
    FLOAT16 = 25,
    FLOAT32 = 26,
    FLOAT64 = 27
};

class InitByte
{
public:
    constexpr InitByte(uint8_t x) :
        _majorType((MajorType)(x >> 5)), _remainder(x & 0x1f) {}

    constexpr MajorType majorType() const
    {
        return _majorType;
    }

    constexpr uint8_t argument() const
    {
        return _remainder;
    }

private:
    MajorType _majorType;

    uint8_t _remainder;
};

struct item_t
{
public:
    struct Value
    {
        constexpr Value(Integer i) :
            i(i) {}

        constexpr Value(Float f) :
            f(f) {}

        constexpr Value(Boolean b) :
            s(b ? Simple::TRUE : Simple::FALSE) {}

        constexpr Value(std::span<char> text) :
            text(text) {}

        constexpr Value(std::span<uint8_t> blob) :
            blob(blob) {}

        constexpr Value(Simple s) :
            s(s) {}

        union
        {
            Integer i;

            Float f;

            Simple s;

            std::span<char> text;

            std::span<uint8_t> blob;
        };
    };

    struct Members
    {
        constexpr Members(item_t* child) :
            children({child, child}) {}

        constexpr Members(Value value) :
            value(value) {}

        union 
        {
            struct
            {
                // this struct may only be accessed if the type == OBJECT || type == ARRAY
                item_t* first;

                item_t* last;
            } children;

            // this union's members may only be accessed if type != OBJECT && type != ARRAY
            Value value;
        };
    };

    constexpr item_t() = default;

    constexpr item_t(Type type, item_t* parent, const Members& members) :
        parent(parent), type(type), members(members) {}

    void addToChildren(item_t* child)
    {
        child->parent = this;

        if (members.children.first == nullptr)
        {
            members.children.first = child;
            members.children.last = child;
        }
        else
        {
            members.children.last->sibling = child;
            members.children.last = child;
        }
    }

    Type type = Type::UNDEFINED;

    item_t* parent = nullptr;

    item_t* key = nullptr;

    item_t* sibling = nullptr;

    Members members{nullptr};

    Tag tag = Tag::INVALID;
};
} // namespace CBOR

#endif // BORON_CBOR_TYPES_H_