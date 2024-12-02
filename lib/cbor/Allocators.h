#ifndef BORON_CBOR_ALLOCATORS_H_
#define BORON_CBOR_ALLOCATORS_H_

#include <cstddef>
#include <cstdint>

#include <array>
#include <vector>
#include <memory>
#include <algorithm>

#include "Types.h"

namespace CBOR
{
class AllocatorBase
{
public:
    /***
     * Clear all allocations.
     */
    virtual void clear() = 0;

    /***
     * Get the number of allocations.
     * 
     * @return The number of allocations.
     */
    virtual size_t size() const = 0;

    /***
     * Get the allocators capacity.
     * 
     * @return The capacity.
     */
    virtual size_t capacity() const = 0;
};

class ItemAllocator : public AllocatorBase
{
public:
    /***
     * Allocate an item.
     * 
     * @return The pointer to the item, nullptr if allocation failed.
     */
    virtual item_t* allocate() = 0;
};

class BlobAllocator : public AllocatorBase
{
public:
    /***
     * Allocate a binary large object (a byte array).
     * 
     * @param n The number of bytes to allocate
     * @param init An init value for the array
     * 
     * @return A pointer to the byte array, nullptr if allocation failed.
     */
    virtual uint8_t* allocate(size_t n, std::span<const uint8_t> init = {}) = 0;
};

/***
 * Allocated item from a static, fixed-size pool of items.
 */
template <size_t N>
class StaticItemAllocator : public ItemAllocator
{
public:
    constexpr StaticItemAllocator() = default;

    constexpr void clear() override
    {
        _size = 0;
    }

    constexpr size_t size() const override
    {
        return _size;
    }

    constexpr size_t capacity() const override
    {
        return N;
    }

    item_t* allocate() override
    {
        if (size() >= capacity())
        {
            return nullptr;
        }

        return &_items[_size++];
    }

private:
    std::array<item_t, N> _items{};

    size_t _size = 0;
};

/***
 * Allocates items on the heap.
 */
class DynamicItemAllocator : public ItemAllocator
{
public:
    DynamicItemAllocator() = default;

    void clear() override
    {
        _items.clear();
    }

    size_t size() const override
    {
        return _items.size();
    }

    constexpr size_t capacity() const override
    {
        return 0;
    }

    item_t* allocate() override
    {
       auto& item = _items.emplace_back(std::make_unique<item_t>());
       return item.get();
    }

private:
    std::vector<std::unique_ptr<item_t>> _items;
};

/***
 * Pseudo-allocator that just returns a pointer to the init array.
 */
class InlineBlobAllocator : public BlobAllocator
{
public:
    constexpr InlineBlobAllocator() = default;

    constexpr void clear() override
    {
        return;
    }

    constexpr size_t size() const override
    {
        return _size;
    }

    constexpr size_t capacity() const override
    {
        return 0;
    }

    constexpr uint8_t* allocate(size_t n, std::span<const uint8_t> init) override
    {
        return const_cast<uint8_t*>(init.data());
    }

private:
    size_t _size = 0;
};

/***
 * Allocated bytes from a static, fixed-size pool of memory.
 */
template <size_t N>
class StaticBlobAllocator : public BlobAllocator
{
public:
    constexpr StaticBlobAllocator() = default;

    constexpr void clear() override
    {
        _size = 0;
    }

    constexpr size_t size() const override
    {
        return _size;
    }

    constexpr size_t capacity() const override
    {
        return N;
    }

    uint8_t* allocate(size_t n, std::span<const uint8_t> init) override
    {
        if (size() + n >= capacity())
        {
            return nullptr;
        }

        auto* blob = &_data[_size];
        _size += n;
        
        if (init.empty() == false)
        {
            memcpy(blob, init.data(), init.size());
        }

        return blob;
    }

private:
    std::array<uint8_t, N> _data{};

    size_t _size = 0;
};

class DynamicBlobAllocator : public BlobAllocator
{
public:
    DynamicBlobAllocator() = default;

    void clear() override
    {
        _blobs.clear();
    }

    size_t size() const override
    {
        return std::reduce(_blobs.begin(), _blobs.end(), 0, [](size_t init, const auto& blob) -> size_t
        {
            return init + blob.second;
        });
    }

    constexpr size_t capacity() const override
    {
        return 0;
    }

    uint8_t* allocate(size_t n, std::span<const uint8_t> init) override
    {
        auto ptr = std::make_unique<uint8_t[]>(n);

        if (init.empty() == false)
        {
            std::copy(init.begin(), init.end(), ptr.get());
        }

        auto& blob = _blobs.emplace_back(std::make_pair(std::move(ptr), n));
        return blob.first.get();
    }

private:
    std::vector<std::pair<std::unique_ptr<uint8_t[]>, size_t>> _blobs;
};
} // namespace CBOR

#endif // BORON_CBOR_ALLOCATORS_H_