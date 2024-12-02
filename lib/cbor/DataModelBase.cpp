#include "DataModelBase.h"

CBOR::DataModelBase::~DataModelBase()
{
    clear();
}

CBOR::Item CBOR::DataModelBase::createEmpty(Type type)
{
    clear();

    auto* root = _itemAllocator.allocate();
    if (root == nullptr)
    {
        return Item(nullptr, this);
    }

    root->type = type;
    _root = Item(root, this);
    return _root;
}