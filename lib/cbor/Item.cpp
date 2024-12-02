#include "Item.h"

#include "DataModelBase.h"

size_t CBOR::Item::size() const
{
    switch (type())
    {
        case Type::BYTES:
        {
            return _item->members.value.blob.size();
        }
        case Type::STRING:
        {
            return _item->members.value.text.size();
        }
        case Type::ARRAY:
        case Type::MAP:
        {
            size_t size = 0;
            for (auto child = _item->members.children.first; child != nullptr; child = child->sibling)
            {
                size++;
            }

            return size;
        }
        default:
        {
            break;
        }
    }

    return 0;
}

CBOR::Item CBOR::Item::operator[](uint32_t index)
{
    const auto type = Item::type();
    if (type != Type::ARRAY && type != Type::MAP)
    {
        return nullptr;
    }

    uint32_t i = 0;
    for (auto child = begin(); bool(child); child = child.sibling())
    {
        if (i == index)
        {
            return child;
        }

        i++;
    }

    return Item(nullptr, _model);
}

CBOR::Item CBOR::Item::addChild(Type type, std::optional<ValueBuilder> value)
{
    if (_model == nullptr)
    {
        return Item(nullptr, _model);
    }

    auto* child = _model->itemAllocator().allocate();
    if (child == nullptr)
    {
        return Item(nullptr, _model);
    }

    _item->addToChildren(child);

    child->type = type;
    Item item(child, _model);

    if (value.has_value())
    {
        item.setValue(*value);
    }

    return item;
}

void CBOR::Item::setValue(ValueBuilder value)
{
    if (value.type() != type() || _item == nullptr)
    {
        return;
    }

    switch (type())
    {
        case Type::INTEGER:
        {
            _item->members.value.i = value.toInt();
            break;
        }
        default:
        {
            break;
        }
    }
}