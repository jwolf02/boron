#ifndef BORON_DESERIALIZABLE_H_
#define BORON_DESERIALIZABLE_H_

#include "cbor/Types.h"
#include "cbor/Item.h"

namespace Boron
{
class Deserializable
{
public:
    virtual CBOR::Error deserialize(CBOR::Item item) = 0;
};
} // namespace Boron

#endif // BORON_DESERIALIZABLE_H_