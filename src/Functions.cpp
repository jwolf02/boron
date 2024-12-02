#include "Functions.h"

#include <cbor/CBOR.h>

namespace
{
void indent(std::string& str, uint32_t indent)
{
    for (uint32_t i = 0; i < indent; ++i)
    {
        str += "  ";
    }
}

void inspect(std::string& str, CBOR::Item item, uint32_t ind = 0)
{
    if (item.tag() != CBOR::Tag::INVALID)
    {
        str += "< TAGGED ";
        str += CBOR::toString(item.tag());
        str += " (";
        str += (uint64_t)item.tag();
        str += "): ";
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
            str += "[\n";

            for (auto child = item.begin(); bool(child); child = child.sibling())
            {
                indent(str, ind + 1);

                inspect(str, child, ind);
                
                if (bool(child.sibling()))
                {
                    str += ", \n";
                }
            }

            str += "\n]\n";
            break;
        }
        case CBOR::Type::MAP:
        {
            str += "{ ";
            for (auto child = item.begin(); bool(child); child = child.sibling())
            {
                str += child.key().toString();
                str += ": ";

                inspect(str, child, ind + 1);

                if (bool(child.sibling()))
                {
                    str += ", ";
                }
            }

            str += " }";
            break;
        }
    }

    if (item.tag() != CBOR::Tag::INVALID)
    {
        str += " >";
    }
}

void decode(std::string& str, CBOR::Item item, uint32_t ind = 0)
{
    if (item.tag() != CBOR::Tag::INVALID)
    {
        str += '<';
        str += (uint64_t)item.tag();
        str += ": ";
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
            str += "[ ";

            for (auto child = item.begin(); bool(child); child = child.sibling())
            {
                inspect(str, child, ind);
                
                if (bool(child.sibling()))
                {
                    str += ", ";
                }
            }

            str += " ]";
            break;
        }
        case CBOR::Type::MAP:
        {
            str += "{ ";
            for (auto child = item.begin(); bool(child); child = child.sibling())
            {
                str += child.key().toString();
                str += ": ";

                inspect(str, child, ind + 1);

                if (bool(child.sibling()))
                {
                    str += ", ";
                }
            }

            str += " }";
            break;
        }
    }

    if (item.tag() != CBOR::Tag::INVALID)
    {
        str += '>';
    }
}
} // namespace

std::pair<CBOR::Error, std::string> Boron::inspect(std::span<const uint8_t> input)
{
    CBOR::DynamicDataModel model;
    const auto [error, length] = CBOR::decode(model, input);
    if (error != CBOR::Error::OK)
    {
        printf("Error: %s\n", CBOR::toString(error));
        return std::make_pair(error, "");
    }

    std::string str;
    ::inspect(str, model.root());

    return std::make_pair(CBOR::Error::OK, str);
}

std::pair<CBOR::Error, std::vector<uint8_t>> encode(std::string_view input)
{
    return std::make_pair(CBOR::Error::MALFORMED_MESSAGE, std::vector<uint8_t>());
}

std::pair<CBOR::Error, std::string> decode(std::span<const uint8_t> input)
{
    return std::make_pair(CBOR::Error::MALFORMED_MESSAGE, "");
}