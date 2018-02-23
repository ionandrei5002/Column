#ifndef VALUE_H
#define VALUE_H

#include "types.h"
#include "bytebuffer.h"

class Value
{
protected:
    ViewByteBuffer _buffer;
    Type::type _type;
public:
    virtual ~Value() {}
    virtual ViewByteBuffer get() = 0;
    virtual void set(ViewByteBuffer &value) = 0;
    virtual Type::type getType() = 0;
    virtual bool operator <(const Value &value) = 0;
    virtual bool operator >(const Value &value) = 0;
};

template<typename T>
class TypedValue: public Value
{
public:
    TypedValue(ViewByteBuffer &value)
    {
        this->_buffer = value;
    }
    ViewByteBuffer get() override
    {
        return this->_buffer;
    }
    void set(ViewByteBuffer &value) override
    {
        this->_buffer = value;
    }
    Type::type getType() override
    {
        return T::type_num;
    }
    bool operator <(const Value &value) override
    {
        typedef typename T::c_type _val;
        return *reinterpret_cast<_val*>(this->_buffer._data) < *reinterpret_cast<_val*>(value._buffer._data);
    }
    bool operator <(const TypedValue<T> &value)
    {
        typedef typename T::c_type _val;
        return *reinterpret_cast<_val*>(this->_buffer._data) < *reinterpret_cast<_val*>(value._buffer._data);
    }
    bool operator >(const Value &value) override
    {
        typedef typename T::c_type _val;
        return *reinterpret_cast<_val*>(this->_buffer._data) > *reinterpret_cast<_val*>(value._buffer._data);
    }
    bool operator >(const TypedValue<T> &value)
    {
        typedef typename T::c_type _val;
        return *reinterpret_cast<_val*>(this->_buffer._data) > *reinterpret_cast<_val*>(value._buffer._data);
    }
};

#endif // VALUE_H
