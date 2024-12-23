#ifndef BORON_CBOR_TAGS_H_
#define BORON_CBOR_TAGS_H_

#include <cstdint>

#include <array>
#include <utility>

namespace CBOR
{
// see https://www.iana.org/assignments/cbor-tags/cbor-tags.xhtml
enum class Tag : uint64_t
{
    DATE_TIME_STRING = 0, // text string
    EPOCH_BASED_DATE_TIME = 1, // integer or float
    UNSIGNED_BIGNUM = 2, // byte string
    NEGATIVE_BIGNUM = 3, // byte string
    DECIMAL_FRACTION = 4, // array
    BIGFLOAT = 5, // array
    // Unassigned [6-15]
    COSE_ENCRYPT0 = 16, // COSE_Encrypt0
    COSE_MAC0 = 17, // COSE_Mac0
    COSE_SIGN1 = 18, // COSE_Sign1
    COSE_COUNTERSIGNATURE = 19, // COSE_Countersignature
    // Unassigned [29]
    EXPECTED_CONVERSION_TO_BASE64URL = 21, // any
    EXPECTED_CONVERSION_TO_BASE64 = 22, // any
    EXPECTED_CONVERSION_TO_BASE16 = 23, // any
    ENCODED_CBOR_ITEM = 24, // byte string
    REFERENCE_THE_NTH_PREVIOUSLY_SEEN_STRINGS = 25, // unsigned integer
    SERIALIZED_PERL_OBJECT = 26, // array
    SERIALIZED_LANGUAGE_INDEPENDENT_OBJECT = 27, // array
    SHARED = 28, // any
    REFERENCE_NTH_MARKED_VALUE = 29, // unsigned integer
    RATIONAL_NUMBER = 30, // multiple
    ABSENT_VALUE_IN_CBOR_ARRAY = 31, // Undefined
    URI = 32, // text string
    BASE64URL = 33, // text string
    BASE64 = 34, // text string
    REGULAR_EXPRESSION = 35, // UTF-8 string
    MIME_MESSAGE = 36, // text string
    BINARY_UUID = 37, // byte string
    LANGUAGE_TAGGED_STRING = 38, // array
    IDENTIFIER = 39, // multiple
    MULTI_DIMENSIONAL_ARRAY = 40, // array of two arrays
    HOMOGENEOUS_ARRAY = 41, // array
    IPLD_CONTENT_IDENTIFIER = 42, // byte string
    YANG_BITS_DATATYPE = 43, // text string
    YANG_ENUMERATION_DATATYPE = 44, // text string
    YANG_IDENTITYREF_DATATYPE = 45, // unsigned integer or text string
    YANG_INSTANCE_IDENTIFIER_DATATYPE = 46, // unsigned integer  or text string or array
    YANG_SCMEMA_ITEM_IDENTIFIER = 47, // unsigned integer
    IEEE_MAC_ADDRESS = 48, // byte string
    // Unassigned [49-51]
    IPV4 = 52, // byte string or array
    // Unassigned [53]
    IPV6 = 54, // byte string or array
    // Unassigned [55-60]
    CBOR_WEB_TOKEN = 61, // CBOR Web Token (CWT)
    // Unassigned [62]
    ENCODED_CBOR_SEQUENCE = 63, // byte string
    TYPED_ARRAY_UINT8 = 64, // byte string
    TYPED_ARRAY_UINT16_BIG = 65, // byte string
    TYPED_ARRAY_UINT32_BIG = 66, // byte string
    TYPED_ARRAY_UINT64_BIG = 67, // byte string
    TYPED_ARRAY_UINT8_CLAMPED = 68, // byte string
    TYPED_ARRAY_UINT16_LITTLE = 69, // byte string
    TYPED_ARRAY_UINT32_LITTLE = 70, // byte string
    TYPED_ARRAY_UINT64_LITTLE = 71, // byte string
    TYPED_ARRAY_INT8 = 72, // byte string
    TYPED_ARRAY_INT16_BIG = 73, // byte string
    TYPED_ARRAY_INT32_BIG = 74, // byte string
    TYPED_ARRAY_INT64_BIG = 75, // byte string
    // Unassigned [76]
    TYPED_ARRAY_INT16_LITTLE = 77, // byte string
    TYPED_ARRAY_INT32_LITTLE = 78, // byte string
    TYPED_ARRAY_INT64_LITTLE = 79, // byte string
    TYPED_ARRAY_FLOAT16_BIG = 80, // byte string
    TYPED_ARRAY_FLOAT32_BIG = 81, // byte string
    TYPED_ARRAY_FLOAT64_BIG = 82, // byte string
    TYPED_ARRAY_FLOAT128_BIG = 83, // byte string
    TYPED_ARRAY_FLOAT16_LITTLE = 80, // byte string
    TYPED_ARRAY_FLOAT32_LITTLE = 81, // byte string
    TYPED_ARRAY_FLOAT64_LITTLE = 82, // byte string
    TYPED_ARRAY_FLOAT128_LITTLE = 83, // byte string
    EMBEDDED_JSON_OBJECT = 262, // text string
    HEXADECIMAL_STRING = 263, // text string
    EXTENDED_TIME = 1001, // map
    DURATION = 1002, // map
    PERIOD = 1003, // map
    SHA256_DIGEST = 40001, // byte string
    // Unassigned [4294967297-18446744073709551614]
    INVALID = 0xffffffffffffffff // (none valid)
};

inline constexpr const char* toString(Tag tag)
{
    switch (tag)
    {
        case Tag::DATE_TIME_STRING:
        {
            return "Data-Time String";
        }
        case Tag::EPOCH_BASED_DATE_TIME:
        {
            return "Epoch-based Date-Time";
        }
        case Tag::UNSIGNED_BIGNUM:
        {
            return "Unsigned Bignum";
        }
        case Tag::NEGATIVE_BIGNUM:
        {
            return "Negative Bignum";
        }
        case Tag::DECIMAL_FRACTION:
        {
            return "Decimal Fraction";
        }
        case Tag::BIGFLOAT:
        {
            return "Bigfloat";
        }
        case Tag::COSE_ENCRYPT0:
        {
            return "COSE_Encrypt0";
        }
        case Tag::COSE_MAC0:
        {
            return "COSE_Mac0";
        }
        case Tag::COSE_SIGN1:
        {
            return "COSE_Sign1";
        }
        case Tag::COSE_COUNTERSIGNATURE:
        {
            return "COSE_Countersignature";
        }
        case Tag::EXPECTED_CONVERSION_TO_BASE64URL:
        {
            return "Expected Conversion to Base64 URL";
        }
        case Tag::EXPECTED_CONVERSION_TO_BASE64:
        {
            return "Expected Conversion to Base64";
        }
        case Tag::EXPECTED_CONVERSION_TO_BASE16:
        {
            return "Expected Conversion to Base16";
        }
        case Tag::ENCODED_CBOR_ITEM:
        {
            return "Encoded CBOR Item";
        }
        case Tag::REFERENCE_THE_NTH_PREVIOUSLY_SEEN_STRINGS:
        {
            return "Reference the Nth Previously Seen Strings";
        }
        case Tag::SERIALIZED_PERL_OBJECT:
        {
            return "Serialized Perl Object";
        }
        case Tag::SERIALIZED_LANGUAGE_INDEPENDENT_OBJECT:
        {
            return "Serialized Language Independent Object";
        }
        case Tag::SHARED:
        {
            return "(Potentially) Shared";
        }
        case Tag::REFERENCE_NTH_MARKED_VALUE:
        {
            return "Reference Nth Marked Value";
        }
        case Tag::RATIONAL_NUMBER:
        {
            return "Rational Number";
        }
        case Tag::ABSENT_VALUE_IN_CBOR_ARRAY:
        {
            return "Absent Value in CBOR Array";
        }
        case Tag::URI:
        {
            return "URI";
        }
        case Tag::BASE64URL:
        {
            return "Base64 URL";
        }
        case Tag::BASE64:
        {
            return "Base64";
        }
        case Tag::REGULAR_EXPRESSION:
        {
            return "Regular Expression";
        }
        case Tag::MIME_MESSAGE:
        {
            return "MIME Message";
        }
        case Tag::BINARY_UUID:
        {
            return "Binary UUID";
        }
        case Tag::LANGUAGE_TAGGED_STRING:
        {
            return "Language Tagged String";
        }
        case Tag::IDENTIFIER:
        {
            return "Identifier";
        }
        case Tag::MULTI_DIMENSIONAL_ARRAY:
        {
            return "Multi-Dimensional Array";
        }
        case Tag::HOMOGENEOUS_ARRAY:
        {
            return "Homogeneous Array";
        }
        case Tag::IPLD_CONTENT_IDENTIFIER:
        {
            return "IPLD Content Identifier";
        }
        case Tag::IPV4:
        {
            return "IPv4";
        }
        case Tag::IPV6:
        {
            return "IPv6";
        }
        case Tag::CBOR_WEB_TOKEN:
        {
            return "CBOR Web Token";
        }
        case Tag::TYPED_ARRAY_UINT8:
        {
            return "Typed Array (uint8)";
        }
        case Tag::EMBEDDED_JSON_OBJECT:
        {
            return "Embedded JSON Object";
        }
        case Tag::HEXADECIMAL_STRING:
        {
            return "Hexadecimal String";
        }
        case Tag::EXTENDED_TIME:
        {
            return "Extended Time";
        }
        case Tag::DURATION:
        {
            return "Duration";
        }
        case Tag::PERIOD:
        {
            return "Period";
        }
        case Tag::SHA256_DIGEST:
        {
            return "SHA-256 Digest";
        }
        case Tag::INVALID:
        {
            return "Invalid";
        }
        default:
        {
            return "Unassigned";
        }
    }
}
} // namespace CBOR

#endif // BORON_CBOR_TAGS_H_