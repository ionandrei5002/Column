#ifndef COLUMN_H
#define COLUMN_H

#include <vector>
#include <set>
#include <string>

#include "types.h"
#include "bytebuffer.h"
#include "array.h"

struct Encoding
{
    enum type
    {
        PLAIN = 0,
        DICTIONARY = 1
    };
};

template<Encoding::type TYPE>
struct Store {};

typedef Store<Encoding::PLAIN> PlainStore;
typedef Store<Encoding::DICTIONARY> DictStore;

class Column
{
public:
    virtual ~Column() {}
    virtual void put(ByteBuffer& value) = 0;
    virtual ByteBuffer get(uint64_t position) = 0;
    virtual void put(ViewByteBuffer& value) = 0;
    virtual ViewByteBuffer getView(uint64_t position) = 0;
};

class IsNullable
{
protected:
    std::vector<char> _nulls;
public:
    virtual ~IsNullable() {}
    virtual void putNull(bool value) = 0;
    virtual bool getNull(uint64_t position) = 0;
};

class Storage
{
public:
    virtual ~Storage() {}
    virtual uint64_t put(ByteBuffer& value) = 0;
    virtual ByteBuffer get(uint64_t offset, uint64_t type_size) = 0;
    virtual uint64_t put(ViewByteBuffer& value) = 0;
    virtual ViewByteBuffer getView(uint64_t offset, uint64_t type_size) = 0;
};

template<typename T>
class TypeStore: public Storage {};

template<>
class TypeStore<PlainStore>: public Storage
{
private:
    Array _data;
public:
    uint64_t put(ByteBuffer& value) override
    {
        uint64_t offset = _data.size();

        _data.emplace_back(value._size, value._data);

        return offset;
    }
    ByteBuffer get(uint64_t offset, uint64_t type_size) override
    {
        ByteBuffer value(type_size, _data.get(offset));
        return value;
    }
    uint64_t put(ViewByteBuffer& value) override
    {
        uint64_t offset = _data.size();

        _data.emplace_back(value._size, value._data);

        return offset;
    }
    ViewByteBuffer getView(uint64_t offset, uint64_t type_size) override
    {
        ViewByteBuffer value(type_size, _data.get(offset));
        return value;
    }
};

template<>
class TypeStore<DictStore>: public Storage
{
private:
    std::vector<ByteBuffer*> _position;
    std::set<ByteBuffer> _set;
public:
    uint64_t put(ByteBuffer& value) override
    {
        uint64_t offset = _position.size();

        auto find = _set.find(value);
        if (_set.end() == find)
        {
            _set.insert(value);
            find = _set.find(value);
        }
        _position.emplace_back(const_cast<ByteBuffer*>(&*find));

        return offset;
    }
    ByteBuffer get(uint64_t offset, uint64_t type_size) override
    {
        return *_position.at(offset);
    }
    uint64_t put(ViewByteBuffer& value) override
    {
        ByteBuffer buffer(value);
        uint64_t offset = _position.size();

        auto find = _set.find(buffer);
        if (_set.end() == find)
        {
            _set.insert(buffer);
            find = _set.find(buffer);
        }
        _position.emplace_back(const_cast<ByteBuffer*>(&*find));

        return offset;
    }
    ViewByteBuffer getView(uint64_t offset, uint64_t type_size) override
    {
        ViewByteBuffer buffer(*_position.at(offset));
        return buffer;
    }
};

template<typename T, typename U>
class TypedColumn: public Column
{
private:
    T _encoding;
    typename U::c_type _type;
    TypeStore<T> _store;
public:
    explicit TypedColumn() {}
    ~TypedColumn() {}
    void put(ByteBuffer& value) override
    {
        _store.put(value);
    }
    void put(ViewByteBuffer& value) override
    {
        _store.put(value);
    }
    ByteBuffer get(uint64_t position) override
    {
        uint64_t offset = position * sizeof(_type);
        return _store.get(offset, sizeof(_type));
    }
    ViewByteBuffer getView(uint64_t position) override
    {
        uint64_t offset = position * sizeof(_type);
        return _store.getView(offset, sizeof(_type));
    }
};

template<typename T>
class TypedColumn<T, StringType>: public Column
{
private:
    T _encoding;
    typename StringType::c_type _type;
    TypeStore<T> _store;
    std::vector<uint64_t> _offsets;
public:
    explicit TypedColumn() {}
    ~TypedColumn() {}
    void put(ByteBuffer& value) override
    {
        ViewByteBuffer value_size(sizeof(uint64_t), reinterpret_cast<char*>(&value._size));
        uint64_t offset = _store.put(value_size);
        _store.put(value);

        _offsets.emplace_back(offset);
    }
    void put(ViewByteBuffer& value) override
    {
        ViewByteBuffer value_size(sizeof(uint64_t), reinterpret_cast<char*>(&value._size));
        uint64_t offset = _store.put(value_size);
        _store.put(value);

        _offsets.emplace_back(offset);
    }
    ByteBuffer get(uint64_t position) override
    {
        uint64_t offset = _offsets[position];

        ViewByteBuffer value_size = _store.get(offset, sizeof(uint64_t));
        uint64_t size = *reinterpret_cast<uint64_t*>(value_size._data);

        ByteBuffer value = _store.get(offset + sizeof(uint64_t), size);

        return value;
    }
    ViewByteBuffer getView(uint64_t position) override
    {
        uint64_t offset = _offsets[position];

        ViewByteBuffer value_size = _store.getView(offset, sizeof(uint64_t));
        uint64_t size = *reinterpret_cast<uint64_t*>(value_size._data);

        ViewByteBuffer value = _store.getView(offset + sizeof(uint64_t), size);

        return value;
    }
};

template<>
class TypedColumn<DictStore, StringType>: public Column
{
private:
    DictStore _encoding;
    typename StringType::c_type _type;
    TypeStore<DictStore> _store;
    std::vector<uint64_t> _offsets;
public:
    explicit TypedColumn() {}
    ~TypedColumn() {}
    void put(ByteBuffer& value) override
    {
        uint64_t offset = _store.put(value);

        _offsets.emplace_back(offset);
    }
    void put(ViewByteBuffer& value) override
    {
        uint64_t offset = _store.put(value);

        _offsets.emplace_back(offset);
    }
    ByteBuffer get(uint64_t position) override
    {
        uint64_t offset = _offsets[position];

        ByteBuffer value = _store.get(offset, sizeof(ByteBuffer));

        return value;
    }
    ViewByteBuffer getView(uint64_t position) override
    {
        uint64_t offset = _offsets[position];

        ViewByteBuffer value = _store.getView(offset, sizeof(ByteBuffer));

        return value;
    }
};

template<typename T, typename U>
class NullableTypedColumn: public Column, IsNullable
{
private:
    T _encoding;
    typename U::c_type _type;
    TypeStore<T> _store;
public:
    explicit NullableTypedColumn() {}
    ~NullableTypedColumn() {}
    void put(ByteBuffer& value) override
    {
        _store.put(value);
    }
    void put(ViewByteBuffer& value) override
    {
        _store.put(value);
    }
    ByteBuffer get(uint64_t position) override
    {
        uint64_t offset = position * sizeof(_type);
        return _store.get(offset, sizeof(_type));
    }
    ViewByteBuffer getView(uint64_t position) override
    {
        uint64_t offset = position * sizeof(_type);
        return _store.getView(offset, sizeof(_type));
    }
    void putNull(bool value) override
    {
        _nulls.push_back(value);
    }
    bool getNull(uint64_t position) override
    {
        return _nulls.at(position);
    }
};

template<typename T>
class NullableTypedColumn<T, StringType>: public Column, IsNullable
{
private:
    T _encoding;
    typename StringType::c_type _type;
    TypeStore<T> _store;
    std::vector<uint64_t> _offsets;
public:
    explicit NullableTypedColumn() {}
    ~NullableTypedColumn() {}
    void put(ByteBuffer& value) override
    {
        ViewByteBuffer value_size(sizeof(uint64_t), reinterpret_cast<char*>(&value._size));
        uint64_t offset = _store.put(value_size);
        _store.put(value);

        _offsets.emplace_back(offset);
    }
    void put(ViewByteBuffer& value) override
    {
        ViewByteBuffer value_size(sizeof(uint64_t), reinterpret_cast<char*>(&value._size));
        uint64_t offset = _store.put(value_size);
        _store.put(value);

        _offsets.emplace_back(offset);
    }
    ByteBuffer get(uint64_t position) override
    {
        uint64_t offset = _offsets[position];

        ViewByteBuffer value_size = _store.get(offset, sizeof(uint64_t));
        uint64_t size = *reinterpret_cast<uint64_t*>(value_size._data);

        ByteBuffer value = _store.get(offset + sizeof(uint64_t), size);

        return value;
    }
    ViewByteBuffer getView(uint64_t position) override
    {
        uint64_t offset = _offsets[position];

        ViewByteBuffer value_size = _store.getView(offset, sizeof(uint64_t));
        uint64_t size = *reinterpret_cast<uint64_t*>(value_size._data);

        ViewByteBuffer value = _store.getView(offset + sizeof(uint64_t), size);

        return value;
    }
    void putNull(bool value) override
    {
        _nulls.push_back(value);
    }
    bool getNull(uint64_t position) override
    {
        return _nulls.at(position);
    }
};

template<>
class NullableTypedColumn<DictStore, StringType>: public Column, IsNullable
{
private:
    DictStore _encoding;
    typename StringType::c_type _type;
    TypeStore<DictStore> _store;
    std::vector<uint64_t> _offsets;
public:
    explicit NullableTypedColumn() {}
    ~NullableTypedColumn() {}
    void put(ByteBuffer& value) override
    {
        uint64_t offset = _store.put(value);

        _offsets.emplace_back(offset);
    }
    void put(ViewByteBuffer& value) override
    {
        uint64_t offset = _store.put(value);

        _offsets.emplace_back(offset);
    }
    ByteBuffer get(uint64_t position) override
    {
        uint64_t offset = _offsets[position];

        ByteBuffer value = _store.get(offset, sizeof(ByteBuffer));

        return value;
    }
    ViewByteBuffer getView(uint64_t position) override
    {
        uint64_t offset = _offsets[position];

        ViewByteBuffer value = _store.getView(offset, sizeof(ByteBuffer));

        return value;
    }
    void putNull(bool value) override
    {
        _nulls.push_back(value);
    }
    bool getNull(uint64_t position) override
    {
        return _nulls.at(position);
    }
};

#endif // COLUMN_H
