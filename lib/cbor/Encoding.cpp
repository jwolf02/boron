#include "Encoding.h"

namespace
{
CBOR::Error encode(OutputBuffer& buffer, CBOR::InitByte initByte, std::span<const uint8_t> argument, std::span<const uint8_t> payload)
{
    if (buffer.write((uint8_t)initByte) == false)
    {
        return CBOR::Error::UNEXPECTED_EOF;
    }

    if (argument.empty() == false)
    {
        if (buffer.write(argument, Bytes::Endianess::NETWORK) == false)
        {
            return CBOR::Error::UNEXPECTED_EOF;
        }
    }

    if (payload.empty() == false)
    {
        if (buffer.write(payload) == false)
        {
            return CBOR::Error::UNEXPECTED_EOF;
        }
    }

    return CBOR::Error::OK;
}

CBOR::Error encodeIntegerWithPayload(OutputBuffer& buffer, CBOR::MajorType majorType, uint64_t argument, std::span<const uint8_t> payload = {})
{
    if (argument <= CBOR::MAX_ARGUMENT_VALUE_IN_REMAINDER)
    {
        return encode(buffer, CBOR::InitByte(majorType, (uint8_t)argument), {}, payload);
    }
    else if (argument <= std::numeric_limits<uint8_t>::max())
    {
        const auto arg = (uint8_t)argument;
        return encode(buffer, CBOR::InitByte(majorType, (uint8_t)CBOR::ArgumentType::NEXT_1_BYTE), Bytes::asBytes(arg), payload);
    }
    else if (argument <= std::numeric_limits<uint16_t>::max())
    {
        const auto arg = (uint16_t)argument;
        return encode(buffer, CBOR::InitByte(majorType, (uint8_t)CBOR::ArgumentType::NEXT_2_BYTES), Bytes::asBytes(arg), payload);
    }
    else if (argument <= std::numeric_limits<uint32_t>::max())
    {
        const auto arg = (uint32_t)argument;
        return encode(buffer, CBOR::InitByte(majorType, (uint8_t)CBOR::ArgumentType::NEXT_4_BYTES), Bytes::asBytes(arg), payload);
    }
    else
    {
        return encode(buffer, CBOR::InitByte(majorType, (uint8_t)argument), Bytes::asBytes(argument), payload);
    }
}

CBOR::Error encodeFloat(OutputBuffer& buffer,CBOR::FloatOrSimpleArgumentType type, std::span<const uint8_t> payload)
{
    return encode(buffer, CBOR::InitByte(CBOR::MajorType::FLOAT_OR_SIMPLE, (uint8_t)type), {}, payload);
}
} // namespace

CBOR::Error CBOR::Encoding::encode(OutputBuffer& buffer, MajorType majorType, uint64_t argument, std::span<const uint8_t> payload)
{
    return encodeIntegerWithPayload(buffer, majorType, argument, payload);
}

CBOR::Error CBOR::Encoding::encode(OutputBuffer& buffer, int64_t argument)
{
    if (argument >= 0)
    {
        return encodeIntegerWithPayload(buffer, MajorType::UNSIGNED_INT, (uint64_t)argument);
    }
    else
    {
        return encodeIntegerWithPayload(buffer, MajorType::SIGNED_INT, (uint64_t)(INT64_C(-1) - argument));
    }
}

CBOR::Error CBOR::Encoding::encode(OutputBuffer& buffer, float argument)
{
    return encodeFloat(buffer, FloatOrSimpleArgumentType::FLOAT32, Bytes::asBytes(argument));
}

CBOR::Error CBOR::Encoding::encode(OutputBuffer& buffer, double argument)
{
    return encodeFloat(buffer, FloatOrSimpleArgumentType::FLOAT64, Bytes::asBytes(argument));
}

CBOR::Error CBOR::Encoding::encode(OutputBuffer& buffer, std::span<const uint8_t> argument)
{
    return encodeIntegerWithPayload(buffer, MajorType::BYTE_STRING, argument.size(), argument);
}

CBOR::Error CBOR::Encoding::encode(OutputBuffer& buffer, std::string_view argument)
{
    return encodeIntegerWithPayload(buffer, MajorType::TEXT_STRING, argument.size(), {(const uint8_t*)argument.data(), argument.size()});
}

CBOR::Error CBOR::Encoding::encode(OutputBuffer& buffer, Tag tag)
{
    return encodeIntegerWithPayload(buffer, MajorType::TAGGED, (uint64_t)tag);
}

CBOR::Error CBOR::Encoding::encode(OutputBuffer& buffer, Simple simple)
{
    return encodeIntegerWithPayload(buffer, MajorType::FLOAT_OR_SIMPLE, (uint64_t)simple);
}