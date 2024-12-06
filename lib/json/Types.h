#ifndef BORON_LIB_JSON_TYPES_H_
#define BORON_LIB_JSON_TYPES_H_

namespace JSON
{
enum class Encoding
{
    STRICT, /**< create JSON strictly conforming to JSON standard (i.e. byte strings, tags and non-string map key cannot be encoded) */
    COMPAT, /**< create JSON that conforms to the JSON standard by compatibility (byte string become array and tags are dropped) */
    EXTENDED /**< create extended JSON that allows to completely encode CBOR */
};
} // namespace JSON

#endif // BORON_LIB_JSON_TYPES_H_