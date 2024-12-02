#ifndef BORON_BUFFERS_H_
#define BORON_BUFFERS_H_

#include <cstddef>

#include <algorithm>

#include "cbor/Types.h"

class InputBuffer
{
public:
    virtual bool read(uint8_t& x) = 0;

    virtual bool read(std::span<uint8_t> data) = 0;

    virtual size_t size() const = 0;

    virtual size_t capacity() const = 0;
};

class OutputBuffer
{
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

    constexpr bool read(std::span<uint8_t> data) override
    {
        if ((capacity() - size()) < data.size())
        {
            return false;
        }

        std::copy(_data.begin() + size(), _data.begin() + size() + data.size(), data.begin());
        _size += data.size();
        return true;
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