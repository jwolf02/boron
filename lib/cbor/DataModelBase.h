#ifndef BORON_CBOR_DATAMODELBASE_H_
#define BORON_CBOR_DATAMODELBASE_H_

#include "Types.h"
#include "Item.h"
#include "Allocators.h"

namespace CBOR
{
class Decoder;

class DataModelBase
{
public:
    friend Decoder;

    constexpr DataModelBase(ItemAllocator& itemAllocator, BlobAllocator& blobAllocator) :
        _itemAllocator(itemAllocator), _blobAllocator(blobAllocator) {}

    ~DataModelBase();

    Item createEmpty(Type type);

    constexpr Item root() const
    {
        return _root;
    }

    constexpr ItemAllocator& itemAllocator()
    {
        return _itemAllocator;
    }
    
    constexpr BlobAllocator& blobAllocator()
    {
        return _blobAllocator;
    }

    void clear()
    {
        _itemAllocator.clear();
        _blobAllocator.clear();
    }

private:
    Item _root;

    ItemAllocator& _itemAllocator;

    BlobAllocator& _blobAllocator;
};
} // namespace CBOR

#endif // BORON_CBOR_DATAMODELBASE_H_