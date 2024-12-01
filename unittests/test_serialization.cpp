#include <gtest/gtest.h>

#include <cstdint>

#include <string>
#include <array>
#include <string_view>

#include "cbor/Encoder.h"
#include "cbor/Decoder.h"
#include "Serializable.h"
#include "Deserializable.h"

using namespace Boron;

namespace
{
class TestClass : public Serializable, public Deserializable
{
public:
    TestClass() = default;
    
    TestClass(uint32_t i, std::string_view s) :
        _i(i), _s(s) {}

    CBOR::Error serialize(CBOR::Item item) const override
    {
        return CBOR::Error::OK;
    }

    CBOR::Error deserialize(CBOR::Item item) override
    {
        return CBOR::Error::OK;
    }

private:
    int32_t _i = 0;

    std::string _s;
};
} // namespace 

TEST(Boron_Serialization, De_Serialization)
{
}