#ifndef BORON_BUFFERS_H_
#define BORON_BUFFERS_H_

#include <cstddef>

#include <algorithm>
#include <type_traits>

#include "cbor/Types.h"
#include "Bytes.h"

class InputBuffer
{
public:
    virtual bool read(uint8_t& x) = 0;

    virtual std::span<const uint8_t> readSpan(size_t n) = 0;

    template <typename T>
        requires(std::is_fundamental_v<T>)
    bool read(T& x, Bytes::Endianess endianness = Bytes::Endianess::NATIVE)
    {
        union
        {
            T x;

            std::array<uint8_t, sizeof(T)> bytes;
        } cvt;

        for (int i = 0; i < sizeof(T); ++i)
        {
            if (read(cvt.bytes[i]) == false)
            {
                return false;
            }
        }

        if (endianness != Bytes::Endianess::NATIVE)
        {
            std::reverse(cvt.bytes.begin(), cvt.bytes.end());
        }

        return cvt.x;
    }

    virtual size_t size() const = 0;

    virtual size_t capacity() const = 0;
};

class OutputBuffer
{
public:
    virtual bool write(uint8_t x) = 0;

    virtual bool write(std::span<const uint8_t> data) = 0;

    virtual size_t size() const = 0;

    virtual size_t capacity() const = 0;
};

class SpanInputBuffer : public InputBuffer
{
public:
    constexpr SpanInputBuffer(std::span<const uint8_t> data) :
        _data(data) {}

    constexpr bool read(uint8_t& x) override
    {
        if ((capacity() - size()) == 0)
        {
            return false;
        }

        x = _data[_size++];
        return true;
    }

    constexpr std::span<const uint8_t> readSpan(size_t length) override
    {
        if ((capacity() - size()) < length)
        {
            return {};
        }

        const std::span<const uint8_t> x = {_data.data() + _size, length};
        _size += length;
        return x;
    }

    constexpr size_t size() const override
    {
        return _size;
    }

    constexpr size_t capacity() const override
    {
        return _data.size();
    }

private:
    std::span<const uint8_t> _data;

    size_t _size = 0;
};

#endif // BORON_BUFFERS_H_