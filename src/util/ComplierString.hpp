#pragma once
#include "Iterator.h"
#include <cstring>
#include <string>
namespace LunaScript::stl
{
class unicodestring;
struct unicodespan : public iiterator<char16_t>
{
    char16_t *start_span;
    char16_t *end_span;
    constexpr char16_t *begin() noexcept final override
    {
        return start_span;
    }

    constexpr const char16_t *begin() const noexcept final override
    {
        return start_span;
    }

    constexpr const char16_t *cbegin() const noexcept final override
    {
        return start_span;
    }

    constexpr char16_t *end() noexcept final override
    {
        return end_span;
    }

    constexpr const char16_t *end() const noexcept final override
    {
        return end_span;
    }

    constexpr const char16_t *cend() const noexcept final override
    {
        return end_span;
    }

  private:
    friend unicodestring;
    unicodespan(char16_t *start_in, const size_t lenght)
        : start_span(start_in)
        , end_span(start_in + lenght)
    {
    }
};

template<typename T> static constexpr T *copy(T *const dst, const T *const src, const size_t size) noexcept
{
#if IS_CLANG || IS_GCC
    __builtin_memcpy(dst, src, size * sizeof(T));
#else
#    if __cplusplus >= 202002L
    if constexpr (std::is_constant_evaluated())
    {
        for (size_t _Idx = 0; _Idx != size; ++_Idx)
            dst[_Idx] = src[_Idx];
        return dst;
    }
#    endif
    std::memcpy(dst, src, size * sizeof(T));
#endif
    return dst;
}

class unicodestring : public iiterator<char16_t>
{
    char16_t *data;
    size_t length;

  public:
    unicodestring()
        : data(nullptr)
        , length(0)
    {
    }

    unicodestring(const char16_t *str)
    {
        length = std::char_traits<char16_t>::length(str);
        data = new char16_t[length + 1];
        copy(data, str, length);
        data[length] = u'\0';
    }

    unicodestring(const char16_t *str, const size_t start, const size_t length_in)
        : length(length_in)
    {
        data = new char16_t[length + 1];
        copy(data, str + start, length);
        data[length] = u'\0';
    }

    unicodestring(const unicodestring &other)
    {
        length = other.length;
        data = new char16_t[length + 1];
        std::char_traits<char16_t>::copy(data, other.data, length);
        data[length] = u'\0';
    }

    constexpr char16_t *begin() noexcept final override
    {
        return data;
    }

    constexpr const char16_t *begin() const noexcept final override
    {
        return data;
    }

    constexpr const char16_t *cbegin() const noexcept final override
    {
        return data;
    }

    constexpr char16_t *end() noexcept final override
    {
        return data + length;
    }

    constexpr const char16_t *end() const noexcept final override
    {
        return data + length;
    }

    constexpr const char16_t *cend() const noexcept final override
    {
        return data + length;
    }

    unicodestring &operator=(const unicodestring &other)
    {
        if (this != &other)
        {
            delete[] data;
            length = other.length;
            data = new char16_t[length + 1];
            copy(data, other.data, length);
            data[length] = u'\0';
        }
        return *this;
    }

    ~unicodestring()
    {
        delete[] data;
    }

    size_t getLength() const
    {
        return length;
    }

    unicodestring substr(size_t start, size_t len) const
    {
        return unicodestring(data, start, length);
    }

    unicodespan span(size_t start, size_t len) const
    {
        return unicodespan(data + start, length);
    }

    unicodestring operator+(const unicodestring &other) const
    {
        unicodestring result;
        result.length = length + other.length;
        result.data = new char16_t[result.length + 1];
        copy(result.data, data, length);
        copy(result.data + length, other.data, other.length);
        result.data[result.length] = u'\0';
        return result;
    }

    bool operator==(const unicodestring &other) const
    {
        if (length != other.length)
        {
            return false;
        }
        for (size_t i = 0; i < length; i++)
        {
            if (data[i] != other.data[i])
            {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const unicodestring &other) const
    {
        return !(*this == other);
    }

    std::size_t hash() const
    {
        std::size_t result = 0;
        for (size_t i = 0; i < length; i++)
        {
            result = result * 31 + std::hash<char16_t>()(data[i]);
        }
        return result;
    }
};
} // namespace LunaScript::stl