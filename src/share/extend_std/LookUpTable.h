#pragma once
#include "Iterator.h"
#include <initializer_list>
#include <memory>
#include <new>
namespace std
{
template<typename Key, typename Value> struct TableEntry
{
    Key key;
    Value value;
    constexpr TableEntry() noexcept
    {
    }
    constexpr TableEntry(const Key key, const Value value) noexcept
        : key(key)
        , value(value)
    {
    }

    constexpr TableEntry &operator=(const TableEntry &entry) noexcept
    {
        this->key = entry.key;
        this->value = entry.value;
        return *this;
    }

    bool operator==(const Key &key_in) const noexcept
    {
        return key == key_in;
    };
};

template<typename Key, typename Value>
struct ReadOnlyLookupTable : public ForwardIterator<TableEntry<Key, Value>>
{
  private:
    TableEntry<Key, Value> *table;
    size_t size_of_table = 0;

  public:
    ReadOnlyLookupTable() noexcept = default;
    ReadOnlyLookupTable(initializer_list<TableEntry<Key, Value>> entries) noexcept
    {
        table = new (nothrow) TableEntry<Key, Value>[(size_of_table = entries.size())];
        size_t index = 0;
        for (auto &entry : entries)
            table[index++] = entry;
    }
    ~ReadOnlyLookupTable() noexcept
    {
        if (table != nullptr)
            delete[] table;
    }

    virtual size_t i_size() const noexcept override final
    {
        return size_of_table;
    }

    constexpr size_t size() const noexcept
    {
        return size_of_table;
    }

    virtual constexpr TableEntry<Key, Value> *begin() noexcept override final
    {
        return table;
    }

    virtual constexpr const TableEntry<Key, Value> *begin() const noexcept override final
    {
        return table;
    }

    virtual constexpr TableEntry<Key, Value> *cbegin() const noexcept override final
    {
        return table;
    }

    constexpr Value *find(const Key &key) const noexcept
    {
        for (size_t i = 0; i < size_of_table; i++)
        {
            if (table[i] == key)
                return &table[i].value;
        }
        return nullptr;
    }
};
} // namespace std