#include "Decoding.h"

namespace
{
template <typename T>
std::pair<CBOR::Error, CBOR::Header> decodeArgumentFromNextBytes(InputBuffer& buffer, CBOR::MajorType majorType)
{
    T arg = 0;
    if (buffer.read<T>(arg, Bytes::Endianess::NETWORK) == false)
    {
        return std::make_pair(CBOR::Error::UNEXPECTED_EOF, CBOR::Header());
    }

    if (arg <= CBOR::MAX_ARGUMENT_VALUE_IN_REMAINDER)
    {
        return std::make_pair(CBOR::Error::MALFORMED_ARGUMENT, CBOR::Header());
    }

    return std::make_pair(CBOR::Error::OK, CBOR::Header(majorType, (uint64_t)arg));
}

std::pair<CBOR::Error, CBOR::Header> decodeArgument(InputBuffer& buffer, CBOR::InitByte initByte)
{
    const uint8_t argument = initByte.argument();
    switch ((CBOR::ArgumentType)argument)
    {
        case CBOR::ArgumentType::NEXT_1_BYTE:
        {
            return decodeArgumentFromNextBytes<uint8_t>(buffer, initByte.majorType());
        }
        case CBOR::ArgumentType::NEXT_2_BYTES:
        {
            return decodeArgumentFromNextBytes<uint16_t>(buffer, initByte.majorType());
        }
        case CBOR::ArgumentType::NEXT_4_BYTES:
        {
            return decodeArgumentFromNextBytes<uint32_t>(buffer, initByte.majorType());
        }
        case CBOR::ArgumentType::NEXT_8_BYTES:
        {
            return decodeArgumentFromNextBytes<uint64_t>(buffer, initByte.majorType());
        }
        default:
        {
            return std::make_pair(CBOR::Error::OK, CBOR::Header(initByte.majorType(), argument));
        }
    }
}

std::pair<CBOR::Error, CBOR::Header> decodeWithPayload(InputBuffer& buffer, CBOR::InitByte initByte)
{
    const auto arg = decodeArgument(buffer, initByte);
    const auto length = arg.second.argument();

    const auto payload = buffer.readSpan((size_t)length);
    if (payload.size() != length)
    {
        return std::make_pair(CBOR::Error::UNEXPECTED_EOF, CBOR::Header());
    }

    return std::make_pair(arg.first, CBOR::Header(arg.second.majorType(), arg.second.argument(), payload));
}

std::pair<CBOR::Error, CBOR::Header> decodeFloatOrSimple(InputBuffer& buffer, CBOR::InitByte initByte)
{
    const auto type = initByte.argument();
    if (type < (uint8_t)CBOR::FloatOrSimpleArgumentType::FLOAT16 || type > (uint8_t)CBOR::FloatOrSimpleArgumentType::FLOAT64)
    {
        return std::make_pair(CBOR::Error::OK, CBOR::Header(initByte.majorType(), initByte.argument()));
    }

    const auto length = 1 << ((type - (uint8_t)CBOR::FloatOrSimpleArgumentType::FLOAT16) + 1);
    const auto payload = buffer.readSpan((size_t)length);
    if (payload.size() != length)
    {
        return std::make_pair(CBOR::Error::UNEXPECTED_EOF, CBOR::Header());
    }

    return std::make_pair(CBOR::Error::OK, CBOR::Header(initByte.majorType(), initByte.argument(), payload));
}
} // namespace

std::pair<CBOR::Error, CBOR::Header> CBOR::Decoding::decode(InputBuffer& buffer)
{
    uint8_t x = 0;
    if (buffer.read(x) == false)
    {
        return std::make_pair(Error::UNEXPECTED_EOF, Header());
    }

    const InitByte initByte(x);
    switch (initByte.majorType())
    {
        case MajorType::UNSIGNED_INT:
        case MajorType::SIGNED_INT:
        case MajorType::TAGGED:
        case MajorType::ARRAY:
        case MajorType::MAP:
        {
            return decodeArgument(buffer, initByte);
        }
        case MajorType::BYTE_STRING:
        case MajorType::TEXT_STRING:
        {
            return decodeWithPayload(buffer, initByte);
        }
        case MajorType::FLOAT_OR_SIMPLE:
        {
            return decodeFloatOrSimple(buffer, initByte);
        }
        default:
        {
            break;
        }
    }
}