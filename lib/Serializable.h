#ifndef BORON_SERIALIZABLE_H_
#define BORON_SERIALIZABLE_H_

#include "cbor/Types.h"
#include "cbor/Item.h"

namespace Boron
{
class Serializable
{
public:
    virtual CBOR::Error serialize(CBOR::Item item) const = 0;
};
} // namespace Boron

#endif // BORON_SERIALIZABLE_H_