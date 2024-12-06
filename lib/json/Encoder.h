#ifndef BORON_LIB_JSON_ENCODER_H_
#define BORON_LIB_JSON_ENCODER_H_

#include "Types.h"
#include "../cbor/Types.h"
#include "../cbor/Item.h"
#include "../Buffers.h"

namespace JSON
{
enum class Indentation
{
    NONE = 0,
    INDENTED
};

CBOR::Error encode(CBOR::Item root, OutputBuffer& buffer, Encoding encoding = Encoding::STRICT);
} // namespace JSON

#endif // BORON_LIB_JSON_ENCODER_H_