#ifndef BORON_CBOR_ENCODING_H_
#define BORON_CBOR_ENCODING_H_

#include <cstdint>

#include <span>
#include <string_view>

#include "Types.h"
#include "Tags.h"
#include "../Buffers.h"

namespace CBOR::Encoding
{
CBOR::Error encode(OutputBuffer& buffer, InitByte initByte, std::span<const uint8_t> argument, std::span<const uint8_t> payload = {});

CBOR::Error encode(OutputBuffer& buffer, int64_t argument);

CBOR::Error encode(OutputBuffer& buffer, float argument);

CBOR::Error encode(OutputBuffer& buffer, double argument);

CBOR::Error encode(OutputBuffer& buffer, std::span<const uint8_t> argument);

CBOR::Error encode(OutputBuffer& buffer, std::string_view argument);

CBOR::Error encode(OutputBuffer& buffer, Tag tag);

CBOR::Error encode(OutputBuffer& buffer, Simple simple);
} // namespace CBOR::Encoding

#endif // BORON_CBOR_ENCODING_H_