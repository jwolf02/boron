#ifndef BORON_FUNCTIONS_H_
#define BORON_FUNCTIONS_H_

#include <cstdint>

#include <span>
#include <string_view>
#include <vector>
#include <utility>

#include <cbor/Errors.h>

namespace Boron
{
/***
 * Inspect a CBOR message. Showing the internal structure and data types.
 * 
 * @param input The binary input.
 * 
 * @return A pair containing the status of the inspection and the inspected message if successful.
 */
std::pair<CBOR::Error, std::string> inspect(std::span<const uint8_t> input);

/***
 * Encode a CBOR message from an Extended JSON string.
 * 
 * @param input The (Extended) JSON string.
 * 
 * @output A pair containing the status of the encoding and the encoded message if successful.
 */
std::pair<CBOR::Error, std::vector<uint8_t>> encode(std::string_view input);

/***
 * Decode a CBOR message to an Extended JSON string.
 * 
 * @param input The CBOR message.
 * 
 * @output A pair containing the status of the decoding and the (Extended) JSON string if successful.
 */
std::pair<CBOR::Error, std::string> decode(std::span<const uint8_t> input);
} // namespace Boron

#endif // BORON_FUNCTIONS_H_