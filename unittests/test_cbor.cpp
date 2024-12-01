#include <gtest/gtest.h>

#include <array>

#include <cbor/Decoder.h>
#include <cbor/Encoder.h>
#include "Bytes.h"

using namespace Bytes::Literals;

TEST(CBOR, Decoder)
{
    // [1, [2, 3], [4, 5]]
    static constexpr auto TEST_DATA = 0x8301820203820405_bytes;

    CBOR::StaticDataModel<64, 256> model;
    const auto [error, length] = CBOR::decode(model, TEST_DATA);
    ASSERT_EQ(error, CBOR::Error::OK);
    EXPECT_EQ(length, TEST_DATA.size());

    auto root = model.root();
    ASSERT_TRUE(bool(root));

    ASSERT_EQ(root.type(), CBOR::Type::ARRAY);
    EXPECT_EQ(root.type(), CBOR::Type::ARRAY);
    ASSERT_EQ(root.size(), 3);

    // [1, [2, 3], [4, 5]]
    //  ^
    {
        auto first = root[0];
        ASSERT_TRUE(bool(first));
        ASSERT_EQ(first.type(), CBOR::Type::INTEGER);
        EXPECT_EQ(first.toInt(), 1);
    }

    // [1, [2, 3], [4, 5]]
    //     ^
    {
        auto second = root[1];
        ASSERT_TRUE(bool(second));
        ASSERT_EQ(second.type(), CBOR::Type::ARRAY);
        ASSERT_EQ(second.size(), 2);
    }
    
    // [1, [2, 3], [4, 5]]
    //             ^
    {
        auto third = root[2];
        ASSERT_TRUE(bool(third));
        ASSERT_EQ(third.type(), CBOR::Type::ARRAY);
        ASSERT_EQ(third.size(), 2);
    }
}

TEST(CBOR, Encoder)
{
    CBOR::StaticDataModel<64, 256> model;
    auto root = model.createEmpty(CBOR::Type::ARRAY);
    ASSERT_TRUE(bool(root));
    ASSERT_EQ(root.type(), CBOR::Type::ARRAY);

    auto first = root.addChild(CBOR::Type::INTEGER, CBOR::Integer(1));
    ASSERT_TRUE(bool(first));

    std::array<uint8_t, 2> data{};
    const auto [error, len] = CBOR::encode(model, data);
    ASSERT_EQ(error, CBOR::Error::OK);
    ASSERT_EQ(len, 2);

    const CBOR::InitByte arrayInit(data[0]);
    EXPECT_EQ(arrayInit.majorType(), CBOR::MajorType::ARRAY);
    EXPECT_EQ(arrayInit.argument(), 1);

    const CBOR::InitByte intInit(data[1]);
    EXPECT_EQ(intInit.majorType(), CBOR::MajorType::UNSIGNED_INT);
    EXPECT_EQ(intInit.argument(), 1);
}