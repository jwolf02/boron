#ifndef BORON_CBOR_HEADER_H_
#define BORON_CBOR_HEADER_H_

#include <cstdint>

#include <span>

#include "Types.h"

namespace CBOR
{
class Header
{
public:
    constexpr Header() = default;

    constexpr Header(MajorType majorType, uint64_t argument, std::span<const uint8_t> payload = {}) :
        _majorType(majorType), _argument(argument), _payload(payload) {}

    constexpr MajorType majorType() const
    {
        return _majorType;
    }

    constexpr uint64_t argument() const
    {
        return _argument;
    }

    constexpr std::span<const uint8_t> payload() const
    {
        return _payload;
    }

private:
    MajorType _majorType = MajorType::UNSIGNED_INT;

    uint64_t _argument = 0;

    std::span<const uint8_t> _payload;
};
} // namespace CBOR

#endif // BORON_CBOR_HEADER_H_