#include <gtest/gtest.h>

#include <cstdint>

#include <array>
#include <string_view>

#include <cbor/Encoding.h>
#include <cbor/Decoding.h>

using namespace std::literals;
using namespace Bytes::Literals;

TEST(CBOR_Encoding, Encode_Decode_Unsigned)
{
    constexpr auto INT = INT64_C(23);

    std::array<uint8_t, 16> data;

    // Encode
    {
        SpanOutputBuffer buffer(data);
        ASSERT_EQ(CBOR::Encoding::encode(buffer, INT), CBOR::Error::OK);
    }

    // Decode
    {
        SpanInputBuffer buffer(data);
        const auto [error, header] = CBOR::Decoding::decode(buffer);
        ASSERT_EQ(error, CBOR::Error::OK);

        EXPECT_EQ(header.majorType(), CBOR::MajorType::UNSIGNED_INT);
        EXPECT_EQ(header.argument(), INT);
    }
}

TEST(CBOR_Encoding, Encode_Decode_Signed)
{
    constexpr auto INT = INT64_C(-23);

    std::array<uint8_t, 16> data;

    // Encode
    {
        SpanOutputBuffer buffer(data);
        ASSERT_EQ(CBOR::Encoding::encode(buffer, INT), CBOR::Error::OK);
    }

    // Decode
    {
        SpanInputBuffer buffer(data);
        const auto [error, header] = CBOR::Decoding::decode(buffer);
        ASSERT_EQ(error, CBOR::Error::OK);

        EXPECT_EQ(header.majorType(), CBOR::MajorType::SIGNED_INT);
        EXPECT_EQ(header.argument(), INT64_C(-1) - INT);
    }
}

TEST(CBOR_Encoding, Encode_Decode_MultiByteInteger)
{
    constexpr auto INT = INT64_C(2345);

    std::array<uint8_t, 16> data{0};

    // Encode
    {
        SpanOutputBuffer buffer(data);
        ASSERT_EQ(CBOR::Encoding::encode(buffer, INT), CBOR::Error::OK);
        ASSERT_EQ(buffer.size(), 3);
    }

    ASSERT_EQ(data[0], 0x19);
    ASSERT_EQ(data[1], 0x09);
    ASSERT_EQ(data[2], 0x29);

    // Decode
    {
        SpanInputBuffer buffer(data);
        const auto [error, header] = CBOR::Decoding::decode(buffer);
        ASSERT_EQ(error, CBOR::Error::OK);

        EXPECT_EQ(header.majorType(), CBOR::MajorType::UNSIGNED_INT);
        EXPECT_EQ(header.argument(), INT);
        EXPECT_TRUE(header.payload().empty());
    }
}

TEST(CBOR_Encoding, Encode_Decode_ByteString)
{
    constexpr auto STRING = 0x1234567890_bytes;
    constexpr auto LENGTH = STRING.size();

    std::array<uint8_t, 16> data;

    // Encode
    {
        SpanOutputBuffer buffer(data);
        ASSERT_EQ(CBOR::Encoding::encode(buffer, STRING), CBOR::Error::OK);
        ASSERT_EQ(buffer.size(), LENGTH + 1);
    }

    // Decode
    {
        SpanInputBuffer buffer(data);
        const auto [error, header] = CBOR::Decoding::decode(buffer);
        ASSERT_EQ(error, CBOR::Error::OK);

        EXPECT_EQ(header.majorType(), CBOR::MajorType::BYTE_STRING);
        EXPECT_EQ(header.argument(), LENGTH);
        EXPECT_EQ(header.payload().size(), LENGTH);

        for (size_t i = 0; i < LENGTH; ++i)
        {
            EXPECT_EQ(header.payload()[i], STRING[i]);
        }
    }
}

TEST(CBOR_Encoding, Encode_Decode_TextString)
{
    constexpr auto STRING = "Hello World"sv;
    constexpr auto LENGTH = STRING.size();

    std::array<uint8_t, 16> data;

    // Encode
    {
        SpanOutputBuffer buffer(data);
        ASSERT_EQ(CBOR::Encoding::encode(buffer, STRING), CBOR::Error::OK);
        ASSERT_EQ(buffer.size(), LENGTH + 1);
    }

    // Decode
    {
        SpanInputBuffer buffer(data);
        const auto [error, header] = CBOR::Decoding::decode(buffer);
        ASSERT_EQ(error, CBOR::Error::OK);

        EXPECT_EQ(header.majorType(), CBOR::MajorType::TEXT_STRING);
        EXPECT_EQ(header.argument(), LENGTH);
        EXPECT_EQ(header.payload().size(), LENGTH);

        for (size_t i = 0; i < LENGTH; ++i)
        {
            EXPECT_EQ(header.payload()[i], STRING[i]);
        }
    }
}

TEST(CBOR_Encoding, Encode_Decode_Array)
{
    constexpr auto LENGTH = INT64_C(3);

    std::array<uint8_t, 16> data;

    // Encode
    {
        SpanOutputBuffer buffer(data);
        ASSERT_EQ(CBOR::Encoding::encode(buffer, CBOR::MajorType::ARRAY, 3), CBOR::Error::OK);
        for (int64_t i = 0; i < LENGTH; ++i)
        {
            ASSERT_EQ(CBOR::Encoding::encode(buffer, i), CBOR::Error::OK);
        }
    }

    // Decode
    {
        SpanInputBuffer buffer(data);
        // Array header
        {
            const auto [error, header] = CBOR::Decoding::decode(buffer);
            ASSERT_EQ(error, CBOR::Error::OK);

            EXPECT_EQ(header.majorType(), CBOR::MajorType::ARRAY);
            EXPECT_EQ(header.argument(), LENGTH);
            EXPECT_TRUE(header.payload().empty());
        }

        // Array elements
        for (uint64_t i = 0; i < LENGTH; ++i)
        {
            const auto [error, header] = CBOR::Decoding::decode(buffer);
            ASSERT_EQ(error, CBOR::Error::OK);

            EXPECT_EQ(header.majorType(), CBOR::MajorType::UNSIGNED_INT);
            EXPECT_EQ(header.argument(), i);
            EXPECT_TRUE(header.payload().empty());
        }
    }
}

TEST(CBOR_Encoding, Encode_Decode_Map)
{
    constexpr auto LENGTH = INT64_C(3);
    constexpr auto TEXT = "Hello"sv;

    std::array<uint8_t, 256> data;

    // Encode
    {
        SpanOutputBuffer buffer(data);
        ASSERT_EQ(CBOR::Encoding::encode(buffer, CBOR::MajorType::MAP, 3), CBOR::Error::OK);
        for (int64_t i = 0; i < LENGTH; ++i)
        {
            ASSERT_EQ(CBOR::Encoding::encode(buffer, i), CBOR::Error::OK); // key
            ASSERT_EQ(CBOR::Encoding::encode(buffer, TEXT), CBOR::Error::OK); // value
        }
    }

    // Decode
    {
        SpanInputBuffer buffer(data);
        // Array header
        {
            const auto [error, header] = CBOR::Decoding::decode(buffer);
            ASSERT_EQ(error, CBOR::Error::OK);

            EXPECT_EQ(header.majorType(), CBOR::MajorType::MAP);
            EXPECT_EQ(header.argument(), LENGTH);
            EXPECT_TRUE(header.payload().empty());
        }

        // Array elements
        for (uint64_t i = 0; i < LENGTH; ++i)
        {
            // key
            {
                const auto [error, header] = CBOR::Decoding::decode(buffer);
                ASSERT_EQ(error, CBOR::Error::OK);

                EXPECT_EQ(header.majorType(), CBOR::MajorType::UNSIGNED_INT);
                EXPECT_EQ(header.argument(), i);
                EXPECT_TRUE(header.payload().empty());
            }

            // value
            {
                const auto [error, header] = CBOR::Decoding::decode(buffer);
                ASSERT_EQ(error, CBOR::Error::OK);

                EXPECT_EQ(header.majorType(), CBOR::MajorType::TEXT_STRING);
                EXPECT_EQ(header.argument(), TEXT.size());
                EXPECT_EQ(header.payload().size(), TEXT.size());

                for (size_t i = 0; i < LENGTH; ++i)
                {
                    EXPECT_EQ(header.payload()[i], TEXT[i]);
                }
            }
        }
    }
}

TEST(CBOR_Encoding, Encode_Decode_Tagged)
{
    constexpr auto TAG = CBOR::Tag::DATE_TIME_STRING;
    constexpr auto INT = INT64_C(23);

    std::array<uint8_t, 16> data;

    // Encode
    {
        SpanOutputBuffer buffer(data);
        ASSERT_EQ(CBOR::Encoding::encode(buffer, TAG), CBOR::Error::OK);
        ASSERT_EQ(CBOR::Encoding::encode(buffer, INT), CBOR::Error::OK);
        ASSERT_EQ(buffer.size(), 2);
    }

    // Decode
    {
        SpanInputBuffer buffer(data);
        // Tag
        {
            const auto [error, header] = CBOR::Decoding::decode(buffer);
            ASSERT_EQ(error, CBOR::Error::OK);

            EXPECT_EQ(header.majorType(), CBOR::MajorType::TAGGED);
            EXPECT_EQ(header.argument(), (uint64_t)TAG);
            EXPECT_TRUE(header.payload().empty());
        }

        // Integer
        {
            const auto [error, header] = CBOR::Decoding::decode(buffer);
            ASSERT_EQ(error, CBOR::Error::OK);

            EXPECT_EQ(header.majorType(), CBOR::MajorType::UNSIGNED_INT);
            EXPECT_EQ(header.argument(), (uint64_t)INT);
            EXPECT_TRUE(header.payload().empty());
        }
    }
}