#ifndef BORON_CBOR_DECODING_H_
#define BORON_CBOR_DECODING_H_

#include <utility>

#include "Types.h"
#include "Header.h"
#include "../Buffers.h"

namespace CBOR::Decoding
{
/***
 * Decode the header of a CBOR item from the buffer.
 * 
 * @param buffer The input buffer.
 * 
 * @return A pair with the error and the CBOR::Header if successful.
 */
std::pair<Error, Header> decode(InputBuffer& buffer);
} // namespace CBOR::Decoding

#endif // BORON_CBOR_DECODING_H_