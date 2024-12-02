#ifndef BORON_CBOR_ENCODER_H_
#define BORON_CBOR_ENCODER_H_

#include <utility>

#include "DataModelBase.h"

namespace CBOR
{
class Encoder
{
public:
    constexpr Encoder(DataModelBase& model) :
        _model(model) {}

    std::pair<Error, size_t> encode(std::span<uint8_t> data);

private:
    Error encodeAnything(Item item);

    Error encodeArgument(MajorType majorType, uint64_t argument);

    Error encodeInteger(Item item);

    Error encodeByteString(Item item);

    Error encodeTextString(Item item);

    Error encodeArray(Item item);

    Error encodeMap(Item item);

    Error encodeTagged(Item item);

    Error encodeFloat(Item item);

    Error encodeBool(Item item);

    Error encodeSimple(Item item);

    Error write(std::span<const uint8_t> data);

    DataModelBase& _model;

    std::span<uint8_t> _data;

    size_t _encodedBytes = 0;
};

inline auto encode(DataModelBase& model, std::span<uint8_t> data)
{
    Encoder encoder(model);
    return encoder.encode(data);
}
} // namespace CBOR

#endif // NOSCHAME_CBOR_ENCODER_H_