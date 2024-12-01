#ifndef BORON_CBOR_DATAMODEL_H_
#define BORON_CBOR_DATAMODEL_H_

#include <type_traits>

#include "Types.h"
#include "Item.h"
#include "DataModelBase.h"

namespace CBOR
{
template <typename ItemAllocatorType, typename BlobAllocatorType>
    requires(std::is_base_of_v<ItemAllocator, ItemAllocatorType> && std::is_base_of_v<BlobAllocator, BlobAllocatorType>)
class DataModel : public DataModelBase
{
public:
    constexpr DataModel() :
        DataModelBase(_itemAllocator, _blobAllocator) {}

private:
    ItemAllocatorType _itemAllocator;

    BlobAllocatorType _blobAllocator;
};

template <size_t NumItems, size_t NumBytesForBlobs>
using StaticDataModel = DataModel<StaticItemAllocator<NumItems>, StaticBlobAllocator<NumBytesForBlobs>>;

using DynamicDataModel = DataModel<DynamicItemAllocator, DynamicBlobAllocator>;
} // namespace CBOR

#endif // BORON_CBOR_DATAMODEL_H_