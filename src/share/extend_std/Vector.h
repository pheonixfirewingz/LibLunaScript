#pragma once
#include "Iterator.h"
#include <cassert>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <vector>
namespace std
{
template<typename T> struct ReadOnlyVector : public ForwardIterator<T>
{
  private:
    T *ptr_;
    data_size_t size_;

  public:
    constexpr ReadOnlyVector(T *data_in, data_size_t size) noexcept
        : size_(size)
    {
        ptr_ = new T[size_];
        for (size_t i = 0; i < size; ++i)
            ptr_[i] = data_in[i];
    }
    ReadOnlyVector(initializer_list<T> entries) noexcept
        : size_(entries.size())
    {
        ptr_ = new T[entries.size()];
        size_t index = 0;
        for (auto &entry : entries)
            ptr_[index++] = entry;
    }

    constexpr ~ReadOnlyVector() noexcept
    {
        if (ptr_ && !is_trivially_destructible<T>())
            delete[] ptr_;
    }

    constexpr T *data() const noexcept
    {
        return ptr_;
    }

    constexpr T operator[](const data_size_t i) const
    {
        assert(i < size_);
        return *(begin() + i);
    }

    constexpr T &operator[](const data_size_t i)
    {
        assert(i < size_);
        return begin() + i;
    }

    constexpr const T &at(const data_size_t i) const
    {
        assert(i < size_);
        return *(begin() + i);
    }

    virtual data_size_t i_size() const noexcept override final
    {
        return size_;
    }

    constexpr data_size_t size() const noexcept
    {
        return size_;
    }

    virtual constexpr T *begin() noexcept override final
    {
        return ptr_;
    }

    virtual constexpr const T *begin() const noexcept override final
    {
        return ptr_;
    }

    virtual constexpr const T *cbegin() const noexcept override final
    {
        return ptr_;
    }

    constexpr uint64_t findIndex(const T index) const noexcept
    {
        for (uint64_t i = 0; i < size_; i++)
        {
            if (at(i) == index)
                return i;
        }
        return size_;
    }

    constexpr bool empty() const noexcept
    {
        return size_ == 0;
    }

    constexpr T *release() const noexcept
    {
        return std::move(ptr_);
    }

    constexpr void copy(const std::ReadOnlyVector<T> data_in)
    {
        if (data_in.empty())
            return;
        size_ = data_in.size();
        ptr_ = new T[size_];
        for (size_t i = 0; i < size_; ++i)
            ptr_[i] = data_in[i];
    }

    static ReadOnlyVector<T> lock(vector<T> data)
    {
        return *new ReadOnlyVector(data.data(), data.size());
    }
};
} // namespace std