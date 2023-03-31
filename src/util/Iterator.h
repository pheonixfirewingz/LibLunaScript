#pragma once
namespace LunaScript::stl
{
template<typename T> struct iiterator
{
    virtual constexpr T *begin() noexcept = 0;
    virtual constexpr const T *begin() const noexcept = 0;
    virtual constexpr const T *cbegin() const noexcept = 0;
    virtual constexpr T *end() noexcept = 0;
    virtual constexpr const T *end() const noexcept = 0;
    virtual constexpr const T *cend() const noexcept = 0;
};
} // namespace LunaScript::stl