#ifndef BORON_CBOR_ITEM_H_
#define BORON_CBOR_ITEM_H_

#include <cstdint>

#include <string>

#include "Types.h"
#include "ValueBuilder.h"
#include "Tags.h"

#define IF_VALID(__expression, __default) bool(*this) ? (__expression) : __default

namespace CBOR
{
class Decoder;
class DataModelBase;

class Item
{
public:
    friend Decoder;
    friend DataModelBase;

    constexpr Item() = default;

    constexpr Item(item_t* item, DataModelBase* model = nullptr) :
        _item(item), _model(model) {}

    constexpr operator bool() const
    {
        return _item != nullptr;
    }

    constexpr bool operator!() const
    {
        return _item == nullptr;
    }

    constexpr Item parent()
    {
        return Item(_item->parent, _model);
    }

    constexpr Item sibling()
    {
        return Item(_item->sibling, _model);
    }

    constexpr Item key()
    {
        return Item(_item->key, _model);
    }

    constexpr Tag tag() const
    {
        return IF_VALID(_item->tag, Tag::INVALID);
    }

    constexpr Type type() const
    {
        return IF_VALID(_item->type, Type::UNDEFINED);
    }

    size_t size() const;

    Item operator[](uint32_t index);

    constexpr int64_t toInt() const
    {
        return IF_VALID(_item->members.value.i, 0);
    }

    constexpr Float toFloat() const
    {
        return IF_VALID(_item->members.value.f, 0.0);
    }

    constexpr Boolean toBool() const
    {
        return IF_VALID(type() == Type::BOOL && _item->members.value.s == Simple::TRUE, false);
    }

    constexpr bool isNull() const
    {
        return IF_VALID(type() == Type::NULLVAL, false);
    }

    constexpr bool isUndefined() const
    {
        return IF_VALID(type() == Type::UNDEFINED, false);
    }

    constexpr std::span<const uint8_t> toByteString() const
    {
        return IF_VALID(_item->members.value.blob, std::span<uint8_t>());
    }

    constexpr std::span<const char> toTextString() const
    {
        return _item->members.value.text;
    }

    constexpr Item getTaggedItem()
    {
        return begin();
    }

    constexpr Item begin()
    {
        return Item(_item->members.children.first, _model);
    }

    constexpr Item end()
    {
        return Item(nullptr, _model);
    }

    Item addChild(Type type, std::optional<ValueBuilder> value = {});

    void removeChild(Item child);

    void setValue(ValueBuilder value);

    std::string toString(bool withTag = true);

private:
    std::string toStringInternal();

    item_t* _item = nullptr;

    DataModelBase* _model = nullptr;
};
} // namespace CBOR

#undef IF_VALID

#endif // BORON_CBOR_ITEM_H_