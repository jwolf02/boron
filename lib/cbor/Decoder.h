#ifndef BORON_CBOR_DECODER_H_
#define BORON_CBOR_DECODER_H_

#include <cstdint>

#include <utility>

#include "Types.h"
#include "Item.h"
#include "DataModel.h"
#include "Bytes.h"
#include "../Buffers.h"

namespace CBOR
{
class Decoder
{
public:
    constexpr Decoder(DataModelBase& model) :
        _model(model) {}

    std::pair<Error, size_t> decode(std::span<const uint8_t> data);

private:
    constexpr uint8_t peek() const
    {
        return _data.empty() == false ? _data.front() : 0x00;
    }

    constexpr uint8_t pop()
    {
        if (_data.empty())
        {
            return 0x00;
        }

        _bytesUsed++;

        const uint8_t x = _data.front();
        _data = _data.subspan(1);
        return x;
    }

    constexpr std::span<const uint8_t> pop(size_t length)
    {
        if (_data.size() < length)
        {
            return {};
        }

        _bytesUsed += length;

        const auto x = _data.subspan(0, length);
        _data = _data.subspan(length);
        return x;
    }

    template <typename T>
    std::optional<T> popArgument(ArgumentType type)
    {
        std::array<uint8_t, 8> tmp{0};
        size_t size = 0;

        if ((uint8_t)type < MAX_ARGUMENT_VALUE_IN_REMAINDER)
        {
            return (T)type;
        }
        else
        {
            switch (type)
            {
                case ArgumentType::NEXT_1_BYTE:
                {
                    if (_data.size() >= 1)
                    {
                        return {};
                    }

                    return (T)pop();
                }
                case ArgumentType::NEXT_2_BYTES:
                {
                    if (_data.size() >= 2)
                    {
                        return {};
                    }

                    return (T)Bytes::fromBytes<uint16_t>(pop(2), Bytes::Endianess::NETWORK);
                }
                case ArgumentType::NEXT_4_BYTES:
                {
                    if (_data.size() >= 4)
                    {
                        return {};
                    }

                    return (T)Bytes::fromBytes<uint32_t>(pop(4), Bytes::Endianess::NETWORK);
                }
                case ArgumentType::NEXT_8_BYTES:
                {
                    if (_data.size() >= 8)
                    {
                        return {};
                    }

                    return (T)Bytes::fromBytes<uint64_t>(pop(8), Bytes::Endianess::NETWORK);
                }
                default:
                {
                    return T();
                }
            }
        }
    }

    item_t* decodeAnything();

    item_t* decodeInteger(InitByte init);

    item_t* decodeByteString(InitByte init);

    item_t* decodeTextString(InitByte init);

    item_t* decodeArray(InitByte init);

    item_t* decodeMap(InitByte init);

    item_t* decodeTagged(InitByte init);

    item_t* decodeFloatOrSimple(InitByte init);

    item_t* decodeFloat(InitByte init);

    item_t* decodeSimple(InitByte init);

    DataModelBase& _model;

    std::span<const uint8_t> _data;

    size_t _bytesUsed = 0;

    item_t* _current = nullptr;

    Error _error = Error::OK;
};

inline auto decode(DataModelBase& model, std::span<const uint8_t> data)
{
    Decoder decoder(model);
    return decoder.decode(data);
}
} // namespace CBOR

#endif // BORON_CBOR_DECODER_H_