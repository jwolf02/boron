#include "Functions.h"

#include <cbor/CBOR.h>
#include <json/Encoder.h>

namespace
{
void indent(std::string& str, uint32_t indent)
{
    for (uint32_t i = 0; i < indent; ++i)
    {
        str += "  ";
    }
}

void decode(std::string& str, CBOR::Item item, uint32_t ind = 0, bool indented = true)
{
    if (item.tag() != CBOR::Tag::INVALID)
    {
        str += '<';
        str += std::to_string((uint64_t)item.tag());
        str += indented ? ": " : ":";
    }

    switch (item.type())
    {
        case CBOR::Type::INTEGER:
        case CBOR::Type::FLOAT:
        case CBOR::Type::BOOL:
        case CBOR::Type::NULLVAL:
        case CBOR::Type::UNDEFINED:
        {
            str += item.toString(false);
            break;
        }
        case CBOR::Type::BYTES:
        {
            str += "0x";
            str += item.toString(false);
            break;
        }
        case CBOR::Type::STRING:
        {
            str += '"';
            str += item.toString(false);
            str += '"';
            break;
        }
        case CBOR::Type::ARRAY:
        {
            str += indented ? "[ " : "[";

            for (auto child = item.begin(); bool(child); child = child.sibling())
            {
                decode(str, child, ind, indented);
                
                if (bool(child.sibling()))
                {
                    str += indented ? ", " : ",";
                }
            }

            str += indented ? " ]" : "]";
            break;
        }
        case CBOR::Type::MAP:
        {
            str += indented ? "{\n" : "{";
            for (auto child = item.begin(); bool(child); child = child.sibling())
            {
                str += child.key().toString();
                str += indented ? ": " : ":";

                decode(str, child, ind + 1, indented);

                if (bool(child.sibling()))
                {
                    str += indented ? ", " : ",";
                }
            }

            str += indented ? "\n}" : "}";
            break;
        }
    }

    if (item.tag() != CBOR::Tag::INVALID)
    {
        str += '>';
    }
}
} // namespace

std::pair<CBOR::Error, std::vector<uint8_t>> Boron::encode(std::string_view input)
{
    return std::make_pair(CBOR::Error::MALFORMED_MESSAGE, std::vector<uint8_t>());
}

std::pair<CBOR::Error, std::string> Boron::decode(std::span<const uint8_t> input, StringFormat format)
{
    CBOR::DynamicDataModel model;
    const auto [error, length] = CBOR::decode(model, input);
    if (error != CBOR::Error::OK)
    {
        printf("Error: %s\n", CBOR::toString(error));
        return std::make_pair(error, "");
    }

    DynamicOutputBuffer buffer;
    return std::make_pair(JSON::encode(model.root(), buffer, JSON::Encoding::EXTENDED), std::string((const char*)buffer.data(), buffer.size()));
}