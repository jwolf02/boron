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

        x = cvt.x;

        return true;
    }

    virtual size_t size() const = 0;
};

class OutputBuffer
{
public:
    virtual bool write(uint8_t x) = 0;

    virtual bool write(std::span<const uint8_t> data, Bytes::Endianess endianess = Bytes::Endianess::NATIVE)
    {
        for (size_t i = 0; i < data.size(); ++i)
        {
            const auto index = endianess == Bytes::Endianess::NATIVE ? i : data.size() - i - 1;
            if (write(data[index]) == false)
            {
                return false;
            }
        }

        return true;
    }

    template <typename T>
        requires(std::is_fundamental_v<T>)
    bool write(const T& x, Bytes::Endianess endianess = Bytes::Endianess::NATIVE)
    {
        auto bytes = Bytes::getBytes(x);

        if (endianess != Bytes::Endianess::NATIVE)
        {
            std::reverse(bytes.begin(), bytes.end());
        }

        return write(bytes);
    }

    virtual size_t size() const = 0;
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

    constexpr size_t capacity() const
    {
        return _data.size();
    }

private:
    std::span<const uint8_t> _data;

    size_t _size = 0;
};

class SpanOutputBuffer : public OutputBuffer 
{
public:
    constexpr SpanOutputBuffer() = default;

    constexpr SpanOutputBuffer(std::span<uint8_t> data) :
        _data(data) {}

    bool write(uint8_t x) override
    {
        if (size() >= capacity())
        {  
            return false;
        }

        _data[_size++] = x;

        return true;
    }

    bool write(std::span<const uint8_t> data, Bytes::Endianess endianness) override
    {
        if ((size() + data.size()) >= capacity())
        {
            return false;
        }

        std::copy(data.begin(), data.end(), _data.begin() + size());
        if (endianness != Bytes::Endianess::NATIVE)
        {
            std::reverse(_data.begin() + size(), _data.begin() + size() + data.size());
        }

        _size += data.size();

        return true;
    }

    constexpr size_t size() const override
    {
        return _size;
    }

    constexpr size_t capacity() const 
    {
        return _data.size();
    }

private:
    std::span<uint8_t> _data;
    
    size_t _size = 0;
};

class DynamicOutputBuffer : public OutputBuffer
{
public:
    DynamicOutputBuffer() = default;

    bool write(uint8_t x) override
    {
        _data.push_back(x);
        return true;
    }

    uint8_t* data()
    {
        return _data.data();
    }

    const uint8_t* data() const
    {
        return _data.data();
    }

    size_t size() const override
    {
        return _data.size();
    }

private:
    std::vector<uint8_t> _data;
};

#endif // BORON_BUFFERS_H_