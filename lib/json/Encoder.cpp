#include "Encoder.h"

#include <cstdint>
#include <climits>
#include <cfloat>

#include <array>
#include <string_view>

using namespace std::literals;

namespace
{
CBOR::Error encode(std::string_view simple, OutputBuffer& buffer)
{
    return buffer.write(std::span<const uint8_t>((const uint8_t*)simple.data(), simple.size())) ? CBOR::Error::OK : CBOR::Error::UNEXPECTED_EOF;
}

CBOR::Error encodeTag(CBOR::Tag tag, OutputBuffer& buffer, JSON::Encoding encoding)
{
    if (encoding != JSON::Encoding::EXTENDED)
    {
        return CBOR::Error::UNSUPPORTED_DATATYPE;
    }

    std::array<char, 20> tmp{0};
    const auto len = snprintf(tmp.data(), tmp.size(), "%llu", (uint64_t)tag);
    return encode(std::string_view(tmp.data(), len), buffer);
}

CBOR::Error encodeInteger(CBOR::Item item, OutputBuffer& buffer)
{
    std::array<char, 20> tmp{0};
    const auto len = snprintf(tmp.data(), tmp.size(), "%lld", item.toInt());
    return encode(std::string_view(tmp.data(), len), buffer);
}

CBOR::Error encodeBytes(CBOR::Item item, OutputBuffer& buffer, JSON::Encoding encoding)
{
    switch (encoding)
    {
        case JSON::Encoding::STRICT:
        {
            return CBOR::Error::UNSUPPORTED_DATATYPE;
        }
        case JSON::Encoding::COMPAT:
        {
            if (buffer.write('[') == false)
            {
                return CBOR::Error::UNEXPECTED_EOF;
            }

            const auto bytes = item.toByteString();
            for (size_t i = 0; i < bytes.size(); ++i)
            {
                if (buffer.write('0') == false || buffer.write('x') == false)
                {
                    return CBOR::Error::UNEXPECTED_EOF;
                }

                const auto tmp = Bytes::toHex(bytes[i]);
                if (buffer.write(tmp.first) == false || buffer.write(tmp.second) == false)
                {
                    return CBOR::Error::UNEXPECTED_EOF;
                }

                if (i + 1 < bytes.size() && buffer.write(',') == false)
                {
                    return CBOR::Error::UNEXPECTED_EOF;
                }
            }

            if (buffer.write(']') == false)
            {
                return CBOR::Error::UNEXPECTED_EOF;
            }

            return CBOR::Error::OK;
        }
        case JSON::Encoding::EXTENDED:
        {
            if (buffer.write('0') == false || buffer.write('x') == false)
            {
                return CBOR::Error::UNEXPECTED_EOF;
            }

            const auto bytes = item.toByteString();
            for (auto byte : bytes)
            {
                const auto tmp = Bytes::toHex(byte);
                if (buffer.write(tmp.first) == false || buffer.write(tmp.second) == false)
                {
                    return CBOR::Error::UNEXPECTED_EOF;
                }
            }

            return CBOR::Error::OK;
        }
    }
}

CBOR::Error encodeString(CBOR::Item item, OutputBuffer& buffer)
{
    if (buffer.write('"') == false)
    {
        return CBOR::Error::UNEXPECTED_EOF;
    }

    const auto text = item.toTextString();
    if (const auto error = encode(std::string_view(text.data(), text.size()), buffer); error != CBOR::Error::OK)
    {
        return error;
    }

    if (buffer.write('"') == false)
    {
        return CBOR::Error::UNEXPECTED_EOF;
    }

    return CBOR::Error::OK;
}

CBOR::Error encodeArray(CBOR::Item item, OutputBuffer& buffer, JSON::Encoding encoding)
{
    if (buffer.write('[') == false)
    {
        return CBOR::Error::UNEXPECTED_EOF;
    }

    for (auto child = item.begin(); bool(child); child = child.sibling())
    {
        if (const auto error = encode(child, buffer, encoding); error != CBOR::Error::OK)
        {
            return error;
        }

        if (bool(child.sibling()) && buffer.write(',') == false)
        {
            return CBOR::Error::UNEXPECTED_EOF;
        }
    }

    if (buffer.write(']') == false)
    {
        return CBOR::Error::UNEXPECTED_EOF;
    }

    return CBOR::Error::OK;
}

CBOR::Error encodeMap(CBOR::Item item, OutputBuffer& buffer, JSON::Encoding encoding)
{
    if (buffer.write('{') == false)
    {
        return CBOR::Error::UNEXPECTED_EOF;
    }

    for (auto child = item.begin(); bool(child); child = child.sibling())
    {
        auto key = child.key();
        if (key.type() != CBOR::Type::STRING && encoding != JSON::Encoding::EXTENDED)
        {
            return CBOR::Error::UNSUPPORTED_DATATYPE;
        }

        if (const auto error = encode(key, buffer, encoding); error != CBOR::Error::OK)
        {
            return error;
        }

        if (buffer.write(':') == false)
        {
            return CBOR::Error::UNEXPECTED_EOF;
        }

        if (const auto error = encode(child, buffer, encoding); error != CBOR::Error::OK)
        {
            return error;
        }

        if (bool(child.sibling()) && buffer.write(',') == false)
        {
            return CBOR::Error::UNEXPECTED_EOF;
        }
    }

    if (buffer.write('}') == false)
    {
        return CBOR::Error::UNEXPECTED_EOF;
    }

    return CBOR::Error::OK;
}

CBOR::Error encodeFloat(CBOR::Item item, OutputBuffer& buffer)
{
    std::array<char, DBL_MAX_10_EXP> tmp{0};
    const auto len = snprintf(tmp.data(), tmp.size(), "%f", item.toFloat());
    return encode(std::string_view(tmp.data(), len), buffer);
}

CBOR::Error encodeBool(CBOR::Item item, OutputBuffer& buffer)
{
    return encode(item.toBool() ? "true"sv : "false"sv, buffer);
}

CBOR::Error encodeSimple(CBOR::Item item, OutputBuffer& buffer, JSON::Encoding encoding)
{
    switch (encoding)
    {
        case JSON::Encoding::STRICT:
        {
            return CBOR::Error::UNSUPPORTED_DATATYPE;
        }
        case JSON::Encoding::COMPAT:
        {
            return encode("null"sv, buffer);
        }
        case JSON::Encoding::EXTENDED:
        {
            return encode(item.isNull() ? "null"sv : "undefined"sv, buffer);
        }
    }
}
} // namespace

CBOR::Error JSON::encode(CBOR::Item root, OutputBuffer& buffer, Encoding encoding)
{
    if (root.tag() != CBOR::Tag::INVALID)
    {
        if (buffer.write('<') == false)
        {
            return CBOR::Error::UNEXPECTED_EOF;
        }

        if (const auto error = encodeTag(root.tag(), buffer, encoding); error != CBOR::Error::OK)
        {
            return error;
        }

        if (buffer.write(':') == false)
        {
            return CBOR::Error::UNEXPECTED_EOF;
        }
    }

    CBOR::Error error = CBOR::Error::OK;
    switch (root.type())
    {
        case CBOR::Type::INTEGER:
        {
            error = encodeInteger(root, buffer);
            break;
        }
        case CBOR::Type::BYTES:
        {
            error = encodeBytes(root, buffer, encoding);
            break;
        }
        case CBOR::Type::STRING:
        {
            error = encodeString(root, buffer);
            break;
        }
        case CBOR::Type::ARRAY:
        {
            error = encodeArray(root, buffer, encoding);
            break;
        }
        case CBOR::Type::MAP:
        {
            error = encodeMap(root, buffer, encoding);
            break;
        }
        case CBOR::Type::FLOAT:
        {
            error = encodeFloat(root, buffer);
            break;
        }
        case CBOR::Type::BOOL:
        {
            error = encodeBool(root, buffer);
            break;
        }
        case CBOR::Type::NULLVAL:
        case CBOR::Type::UNDEFINED:
        {
            error = encodeSimple(root, buffer, encoding);
            break;
        }
    }

    if (root.tag() != CBOR::Tag::INVALID && buffer.write('>') == false)
    {
        return CBOR::Error::UNEXPECTED_EOF;
    }

    return error;
}