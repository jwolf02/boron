#include "Encoder.h"

#include <type_traits>

#include "Bytes.h"

std::pair<CBOR::Error, size_t> CBOR::Encoder::encode(std::span<uint8_t> data)
{
    auto root = _model.root();
    if (bool(root) == false)
    {
        return std::make_pair(Error::OK, 0);
    }

    if (data.empty())
    {
        return std::make_pair(Error::UNEXPECTED_EOF, 0);
    }

    _data = data;

    return std::make_pair(encodeAnything(root), _encodedBytes);
}

CBOR::Error CBOR::Encoder::encodeAnything(Item item)
{
    if (item.tag() != Tag::INVALID)
    {
        return encodeTagged(item);
    }

    switch (item.type())
    {
        case Type::INTEGER:
        {
            return encodeInteger(item);
        }
        case Type::BYTES:
        {
            return encodeByteString(item);
        }
        case Type::STRING:
        {
            return encodeTextString(item);
        }
        case Type::ARRAY:
        {
            return encodeArray(item);
        }
        case Type::MAP:
        {
            return encodeMap(item);
        }
        case Type::FLOAT:
        {
            return encodeFloat(item);
        }
        case Type::BOOL:
        {
            return encodeBool(item);
        }
        case Type::NULLVAL:
        case Type::UNDEFINED:
        {
            return encodeSimple(item);
        }
        default:
        {
            break;
        }
    }

    return Error::OK;
}

CBOR::Error CBOR::Encoder::encodeArgument(MajorType majorType, uint64_t argument)
{
    if (argument <= MAX_ARGUMENT_VALUE_IN_REMAINDER)
    {
        const uint8_t x = uint8_t(majorType) << 5 | uint8_t(argument);
        return write({&x, sizeof(x)});
    }
    else if (argument < 0xff) // 1-byte argument
    {
        const uint8_t init = uint8_t(majorType) << 5 | uint8_t(ArgumentType::NEXT_1_BYTE);
        if (const auto error = write({&init, sizeof(init)}); error != Error::OK)
        {
            return error;
        }

        const auto bytes = Bytes::getBytes((uint8_t)argument);
        return write(bytes);
    }
    else if (argument < 0xffff) // 2-byte argument
    {
        const uint8_t init = uint8_t(majorType) << 5 | uint8_t(ArgumentType::NEXT_2_BYTES);
        if (const auto error = write({&init, sizeof(init)}); error != Error::OK)
        {
            return error;
        }

        const auto bytes = Bytes::getBytes((uint16_t)argument);
        return write(bytes);
    }
    else if (argument < 0xffffffff) // 4-byte argument
    {
        const uint8_t init = uint8_t(majorType) << 5 | uint8_t(ArgumentType::NEXT_4_BYTES);
        if (const auto error = write({&init, sizeof(init)}); error != Error::OK)
        {
            return error;
        }

        const auto bytes = Bytes::getBytes((uint32_t)argument);
        return write(bytes);
    }
    else // 8-byte argument
    {
        const uint8_t init = uint8_t(majorType) << 5 | uint8_t(ArgumentType::NEXT_8_BYTES);
        if (const auto error = write({&init, sizeof(init)}); error != Error::OK)
        {
            return error;
        }

        const auto bytes = Bytes::getBytes((uint64_t)argument);
        return write(bytes);
    }
}

CBOR::Error CBOR::Encoder::encodeInteger(Item item)
{
    const int64_t value = item.toInt();
    const uint64_t argument = value > 0 ? value : uint64_t(UINT64_C(-1) - value);

    return encodeArgument(value > 0 ? MajorType::UNSIGNED_INT : MajorType::SIGNED_INT, argument);
}

CBOR::Error CBOR::Encoder::encodeByteString(Item item)
{
    if (const auto error = encodeArgument(MajorType::BYTE_STRING, item.size()); error != Error::OK)
    {
        return error;
    }

    return write(item.toByteString());
}

CBOR::Error CBOR::Encoder::encodeTextString(Item item)
{
    if (const auto error = encodeArgument(MajorType::TEXT_STRING, item.size()); error != Error::OK)
    {
        return error;
    }

    const auto str = item.toTextString();
    return write({(const uint8_t*)str.data(), str.size()});
}

CBOR::Error CBOR::Encoder::encodeArray(Item item)
{
    if (const auto error = encodeArgument(MajorType::ARRAY, item.size()); error != Error::OK)
    {
        return error;
    }

    for (auto child = item.begin(); bool(child); child = child.sibling())
    {
        if (const auto error = encodeAnything(child); error != Error::OK)
        {
            return error;
        }
    }

    return Error::OK;
}

CBOR::Error CBOR::Encoder::encodeMap(Item item)
{
    if (const auto error = encodeArgument(MajorType::MAP, item.size()); error != Error::OK)
    {
        return error;
    }

    for (auto child = item.begin(); bool(child); child = child.sibling())
    {
        if (const auto error = encodeAnything(child.key()); error != Error::OK)
        {
            return error;
        }

        if (const auto error = encodeAnything(child); error != Error::OK)
        {
            return error;
        }
    }

    return Error::OK;
}

CBOR::Error CBOR::Encoder::encodeTagged(Item item)
{
    if (const auto error = encodeArgument(MajorType::TAGGED, (uint64_t)item.tag()); error != Error::OK)
    {
        return error;
    }

    return encodeAnything(item.begin());
}

CBOR::Error CBOR::Encoder::encodeFloat(Item item)
{
    if (const auto error = encodeArgument(MajorType::FLOAT_OR_SIMPLE, uint64_t(FloatOrSimpleArgumentType::FLOAT64)); error != Error::OK)
    {
        return error;
    }

    return write(Bytes::getBytes(item.toFloat()));
}

CBOR::Error CBOR::Encoder::encodeBool(Item item)
{
    const auto argument = item.toBool() ? FloatOrSimpleArgumentType::NULLVAL : FloatOrSimpleArgumentType::UNDEFINED;
    return encodeArgument(MajorType::FLOAT_OR_SIMPLE, uint64_t(argument));
}

CBOR::Error CBOR::Encoder::encodeSimple(Item item)
{
    const auto argument = item.type() == Type::NULLVAL ? FloatOrSimpleArgumentType::NULLVAL : FloatOrSimpleArgumentType::UNDEFINED;
    return encodeArgument(MajorType::FLOAT_OR_SIMPLE, uint64_t(argument));
}

CBOR::Error CBOR::Encoder::write(std::span<const uint8_t> data)
{
    if (data.size() > _data.size())
    {
        return Error::UNEXPECTED_EOF;
    }

    for (size_t i = 0; i < data.size(); ++i)
    {
        _data[i] = data[i];
    }

    _data = _data.subspan(data.size());
    _encodedBytes += data.size();

    return Error::OK;
}