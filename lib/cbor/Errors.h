#ifndef BORON_CBOR_ERRORS_H_
#define BORON_CBOR_ERRORS_H_

#include <cstdint>

namespace CBOR
{
enum class Error : uint32_t
{
    OK = 0, /**< no error */
    ITEM_ALLOC_FAILED, /**< allocation of an item failed */
    BLOB_ALLOC_FAILED, /**< allocation of a blob (for binary or text string) failed */
    UNEXPECTED_EOF, /**< the input data was insufficient */
    UNSUPPORTED_DATATYPE, /**< unsupported datatype */
    MALFORMED_MESSAGE, /** */
    DOUBLE_TAGGED, /**< a tagged item was used to tag another tagged item */
    UNSUPPORTED_KEY_TYPE, /**< the key used inside a map was neither a string or an integer */
    MALFORMED_ARGUMENT, /**< the arguments was encoded in an invalid way (e.g. argument value <=23 in another byte) */
    UNSUPPORTED_SIMPLE /**< a simple was not recognized */
};

inline constexpr const char* toString(Error error)
{
    switch (error)
    {
        case Error::OK:
        {
            return "OK";
        }
        case Error::ITEM_ALLOC_FAILED:
        {
            return "Item allocation failed";
        }
        case Error::BLOB_ALLOC_FAILED:
        {
            return "Blob allocation failed";
        }
        case Error::UNEXPECTED_EOF:
        {
            return "Unexpected EOF";
        }
        case Error::UNSUPPORTED_DATATYPE:
        {
            return "Unsupported datatype";
        }
        case Error::MALFORMED_MESSAGE:
        {
            return "Malformed message";
        }
        case Error::DOUBLE_TAGGED:
        {
            return "Double tagged";
        }
        case Error::UNSUPPORTED_KEY_TYPE:
        {
            return "Unsupported key type";
        }
        case Error::MALFORMED_ARGUMENT:
        {
            return "Malformed argument";
        }
        case Error::UNSUPPORTED_SIMPLE:
        {
            return "Unsupported simple";
        }
        default:
        {
            return "Error";
        }
    }
}
} // namespace CBOR

#endif // BORON_CBOR_ERRORS_H_