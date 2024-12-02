#include "Decoder.h"

#include <cstdio>

#include "Bytes.h"

namespace
{
constexpr CBOR::item_t createInteger(uint64_t value, CBOR::item_t *parent)
{
    return CBOR::item_t(CBOR::Type::INTEGER, parent, CBOR::item_t::Members(value));
}

constexpr CBOR::item_t createByteString(std::span<uint8_t> bytes, CBOR::item_t *parent)
{
    return CBOR::item_t(CBOR::Type::BYTES, parent, CBOR::item_t::Members(bytes));
}

constexpr CBOR::item_t createTextString(std::span<char> text, CBOR::item_t *parent)
{
    return CBOR::item_t(CBOR::Type::STRING, parent, CBOR::item_t::Members(text));
}

constexpr CBOR::item_t createArray(CBOR::item_t *parent)
{
    return CBOR::item_t(CBOR::Type::ARRAY, parent, CBOR::item_t::Members(nullptr));
}

constexpr CBOR::item_t createMap(CBOR::item_t *parent)
{
    return CBOR::item_t(CBOR::Type::MAP, parent, CBOR::item_t::Members(nullptr));
}

constexpr CBOR::item_t createFloat(CBOR::Float value, CBOR::item_t* parent)
{
    return CBOR::item_t(CBOR::Type::FLOAT, parent, CBOR::item_t::Members(value));
}

constexpr CBOR::item_t createBool(CBOR::Boolean value, CBOR::item_t* parent)
{
    return CBOR::item_t(CBOR::Type::BOOL, parent, CBOR::item_t::Members(value));
}

constexpr CBOR::item_t createSimple(CBOR::Simple value, CBOR::item_t* parent)
{
    return CBOR::item_t(value == CBOR::Simple::NULLVAL ? CBOR::Type::NULLVAL : CBOR::Type::UNDEFINED, parent, CBOR::item_t::Members(nullptr));
}
} // namespace

std::pair<CBOR::Error, size_t> CBOR::Decoder::decode(std::span<const uint8_t> data)
{
    if (data.empty())
    {
        return std::make_pair(Error::UNEXPECTED_EOF, 0);
    }

    _data = data;
    
    _model._root = decodeAnything();

    return std::make_pair(_error, _bytesUsed);
}

CBOR::item_t* CBOR::Decoder::decodeAnything()
{
    if (_data.empty())
    {
        _error = Error::UNEXPECTED_EOF;
        return nullptr;
    }

    const auto init = InitByte(pop());
    switch (init.majorType())
    {
        case MajorType::UNSIGNED_INT:
        case MajorType::SIGNED_INT:
        {
            return decodeInteger(init);
        }
        case MajorType::BYTE_STRING:
        {
            return decodeByteString(init);
        }
        case MajorType::TEXT_STRING:
        {
            return decodeTextString(init);
        }
        case MajorType::ARRAY:
        {
            return decodeArray(init);
        }
        case MajorType::MAP:
        {
            return decodeMap(init);
        }
        case MajorType::TAGGED:
        {
            return decodeTagged(init);
        }
        case MajorType::FLOAT_OR_SIMPLE:
        {
            return decodeFloatOrSimple(init);
        }
    }

    return nullptr;
}

CBOR::item_t* CBOR::Decoder::decodeInteger(InitByte init)
{
    int64_t value = 0;
    if (init.argument() <= MAX_ARGUMENT_VALUE_IN_REMAINDER)
    {
        value = (int64_t)init.argument();
    }
    else
    {
        auto tmp = popArgument<int64_t>((ArgumentType)init.argument());
        if (tmp.has_value() == false)
        {
            _error = Error::UNEXPECTED_EOF;
            return nullptr;
        }

        value = *tmp;
    }

    auto* item = _model.itemAllocator().allocate();
    if (item == nullptr)
    {
        _error = Error::ITEM_ALLOC_FAILED;
        return nullptr;
    }

    if (init.majorType() == MajorType::SIGNED_INT)
    {
        value = INT16_C(-1) - value;
    }

    *item = createInteger(value, _current);

    return item;
}

CBOR::item_t* CBOR::Decoder::decodeByteString(InitByte init)
{
    const auto length = popArgument<int64_t>((ArgumentType)init.argument());
    if (length.has_value() == false)
    {
        _error = Error::UNEXPECTED_EOF;
        return nullptr;
    }

    auto* item = _model.itemAllocator().allocate();
    if (item == nullptr)
    {
        _error = Error::ITEM_ALLOC_FAILED;
        return nullptr;
    }

    if (_data.size() < length)
    {
        _error = Error::UNEXPECTED_EOF;
        return nullptr;
    }

    auto* blob = _model.blobAllocator().allocate(*length, pop(*length));
    if (blob == nullptr)
    {
        _error = Error::BLOB_ALLOC_FAILED;
        return nullptr;
    }

    *item = createByteString({blob, (size_t)*length}, _current);

    return item;
}

CBOR::item_t* CBOR::Decoder::decodeTextString(InitByte init)
{
    const auto length = popArgument<int64_t>((ArgumentType)init.argument());
    if (length.has_value() == false)
    {
        _error = Error::UNEXPECTED_EOF;
        return nullptr;
    }

    auto* item = _model.itemAllocator().allocate();
    if (item == nullptr)
    {
        _error = Error::ITEM_ALLOC_FAILED;
        return nullptr;
    }

    if (_data.size() < *length)
    {
        _error = Error::UNEXPECTED_EOF;
        return nullptr;
    }

    auto* blob = _model.blobAllocator().allocate(*length, pop(*length));
    if (blob == nullptr)
    {
        _error = Error::BLOB_ALLOC_FAILED;
        return nullptr;
    }

    *item = createTextString({(char*)blob, (size_t)*length}, _current);

    return item;
}

CBOR::item_t* CBOR::Decoder::decodeArray(InitByte init)
{
    const auto length = popArgument<uint64_t>((ArgumentType)init.argument());
    if (length.has_value() == false)
    {
        _error = Error::UNEXPECTED_EOF;
        return nullptr;
    }

    auto* array = _model.itemAllocator().allocate();
    if (array == nullptr)
    {
       _error = Error::ITEM_ALLOC_FAILED;
        return nullptr;
    }

    *array = createArray(_current);
    _current = array;

    for (size_t i = 0; i < (size_t)*length; ++i)
    {
        auto* item = decodeAnything();
        if (item == nullptr)
        {
            return nullptr;
        }

        _current->addToChildren(item);
    }

    _current = array->parent;

    return array;
}

CBOR::item_t* CBOR::Decoder::decodeMap(InitByte init)
{
    const auto numPairs = popArgument<int64_t>((ArgumentType)init.argument());
    if (numPairs.has_value() == false)
    {
        _error = Error::UNEXPECTED_EOF;
        return nullptr;
    }

    auto* map = _model.itemAllocator().allocate();
    if (map == nullptr)
    {
        _error = Error::ITEM_ALLOC_FAILED;
        return nullptr;
    }

    *map = createMap(_current);
    _current = map;

    for (size_t i = 0; i < (size_t)*numPairs; ++i)
    {
        auto* key = decodeAnything();
        if (key == nullptr)
        {
            break;
        }

        if (key->type != Type::INTEGER && key->type != Type::STRING)
        {
            _error = Error::UNSUPPORTED_KEY_TYPE;
            return nullptr;
        }

        auto* value = decodeAnything();
        if (value == nullptr)
        {
            break;
        }

        value->key = key;

        map->addToChildren(value);
    }
    _current = map->parent;
    
    return map;
}

CBOR::item_t* CBOR::Decoder::decodeTagged(InitByte init)
{
    const auto tag = popArgument<uint64_t>((ArgumentType)init.argument());
    if (tag.has_value() == false)
    {
        _error = Error::UNEXPECTED_EOF;
        return nullptr;
    }

    auto* tagged = decodeAnything();
    if (tagged == nullptr)
    {
        return nullptr;
    }

    tagged->tag = (Tag)*tag;

    return tagged;
}

CBOR::item_t* CBOR::Decoder::decodeFloatOrSimple(InitByte init)
{
    const auto argument = popArgument<uint64_t>((ArgumentType)init.argument());
    if (argument.has_value() == false)
    {
        _error = Error::UNEXPECTED_EOF;
        return nullptr;
    }

    switch ((FloatOrSimpleArgumentType)*argument)
    {
        case FloatOrSimpleArgumentType::FALSE:
        case FloatOrSimpleArgumentType::TRUE:
        case FloatOrSimpleArgumentType::NULLVAL:
        case FloatOrSimpleArgumentType::UNDEFINED:
        {
            return decodeSimple(init);
        }
        case FloatOrSimpleArgumentType::FLOAT32:
        case FloatOrSimpleArgumentType::FLOAT64:
        {
            return decodeFloat(init);
        }
        case FloatOrSimpleArgumentType::BREAK:
        default:
        {
            _error = Error::MALFORMED_MESSAGE;
            return nullptr;
        }
    }

    return nullptr;
}

CBOR::item_t* CBOR::Decoder::decodeFloat(InitByte init)
{
    auto* item = _model.itemAllocator().allocate();
    if (item == nullptr)
    {
        _error = Error::ITEM_ALLOC_FAILED;
        return nullptr;
    }

    Float value = 0;
    switch ((FloatOrSimpleArgumentType)init.argument())
    {
        case FloatOrSimpleArgumentType::FLOAT32:
        {
            value = (double)Bytes::fromBytes<float>(pop(sizeof(float)));
            break;
        }
        case FloatOrSimpleArgumentType::FLOAT64:
        {
            value = Bytes::fromBytes<double>(pop(sizeof(double)));
            break;
        }
        default:
        {
            break;
        }
    }

    *item = createFloat(value, _current);

    return item;
}

CBOR::item_t* CBOR::Decoder::decodeSimple(InitByte init)
{
    auto* item = _model.itemAllocator().allocate();
    if (item == nullptr)
    {
        _error = Error::ITEM_ALLOC_FAILED;
        return nullptr;
    }

    switch ((FloatOrSimpleArgumentType)init.argument())
    {
        case FloatOrSimpleArgumentType::FALSE:
        {
            *item = createBool(false, _current);
            break;
        }
        case FloatOrSimpleArgumentType::TRUE:
        {
            *item = createBool(true, _current);
            break;
        }
        case FloatOrSimpleArgumentType::NULLVAL:
        {
            *item = createSimple(Simple::NULLVAL, _current);
            break;
        }
        case FloatOrSimpleArgumentType::UNDEFINED:
        {
            *item = createSimple(Simple::UNDEFINED, _current);
            break;
        }
        default:
        {
            _error = Error::UNSUPPORTED_SIMPLE;
            return nullptr;
        }
    }

    return item;
}