#include "Encoding.h"

namespace
{
CBOR::Error encodeIntegerWithPayload(OutputBuffer& buffer, CBOR::MajorType majorType, uint64_t argument, std::span<const uint8_t> payload = {})
{
    if (argument <= CBOR::MAX_ARGUMENT_VALUE_IN_REMAINDER)
    {
        return CBOR::Encoding::encode(buffer, CBOR::InitByte(majorType, (uint8_t)argument), {}, payload);
    }
    else if (argument < std::numeric_limits<uint8_t>::max())
    {
        const auto arg = (uint8_t)argument;
        return CBOR::Encoding::encode(buffer, CBOR::InitByte(majorType, (uint8_t)CBOR::ArgumentType::NEXT_1_BYTE), Bytes::asBytes(arg), payload);
    }
    else if (argument < std::numeric_limits<uint16_t>::max())
    {
        const auto arg = (uint16_t)argument;
        return CBOR::Encoding::encode(buffer, CBOR::InitByte(majorType, (uint8_t)CBOR::ArgumentType::NEXT_2_BYTES), Bytes::asBytes(arg), payload);
    }
    else if (argument < std::numeric_limits<uint32_t>::max())
    {
        const auto arg = (uint32_t)argument;
        return CBOR::Encoding::encode(buffer, CBOR::InitByte(majorType, (uint8_t)CBOR::ArgumentType::NEXT_4_BYTES), Bytes::asBytes(arg), payload);
    }
    else
    {
        return CBOR::Encoding::encode(buffer, CBOR::InitByte(majorType, (uint8_t)argument), Bytes::asBytes(argument), payload);
    }
}

CBOR::Error encodeFloat(OutputBuffer& buffer,CBOR::FloatOrSimpleArgumentType type, std::span<const uint8_t> payload)
{
    return CBOR::Encoding::encode(buffer, CBOR::InitByte(CBOR::MajorType::FLOAT_OR_SIMPLE, (uint8_t)type), {}, payload);
}
} // namespace

CBOR::Error CBOR::Encoding::encode(OutputBuffer& buffer, InitByte initByte, std::span<const uint8_t> argument, std::span<const uint8_t> payload)
{
    if (buffer.write((uint8_t)initByte) == false)
    {
        return Error::UNEXPECTED_EOF;
    }

    if (argument.empty() == false)
    {
        if (buffer.write(argument) == false)
        {
            return Error::UNEXPECTED_EOF;
        }
    }

    if (payload.empty() == false)
    {
        if (buffer.write(payload) == false)
        {
            return Error::UNEXPECTED_EOF;
        }
    }

    return Error::OK;
}

CBOR::Error CBOR::Encoding::encode(OutputBuffer& buffer, int64_t argument)
{
    return encodeIntegerWithPayload(buffer, argument >= 0 ? MajorType::UNSIGNED_INT : MajorType::SIGNED_INT, (uint64_t)argument);
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