#ifndef OPERATORS_H
#define OPERATORS_H

#include <string>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_numeric.hpp>

#include "types.h"
#include "bytebuffer.h"

class UnaryOperator
{
public:
    virtual ~UnaryOperator() {}
    virtual ByteBuffer operation(ByteBuffer &value) = 0;
    virtual ByteBuffer operation(ViewByteBuffer &value) = 0;
};

template<typename T>
class FromStringCast: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        throw;
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        throw;
    }
};

template<>
class FromStringCast<Int8Type>: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        std::string str(value._data, value._size);
        int8_t cast_value = static_cast<int8_t>(std::stoi(str));

        return ByteBuffer(sizeof(int8_t), reinterpret_cast<char*>(&cast_value));
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        std::string str(value._data, value._size);
        int8_t cast_value = static_cast<int8_t>(std::stoi(str));

        return ByteBuffer(sizeof(int8_t), reinterpret_cast<char*>(&cast_value));
    }
};

template<>
class FromStringCast<Int16Type>: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        std::string str(value._data, value._size);
        int16_t cast_value = static_cast<int16_t>(std::stoi(str));

        return ByteBuffer(sizeof(int16_t), reinterpret_cast<char*>(&cast_value));
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        std::string str(value._data, value._size);
        int16_t cast_value = static_cast<int16_t>(std::stoi(str));

        return ByteBuffer(sizeof(int16_t), reinterpret_cast<char*>(&cast_value));
    }
};

template<>
class FromStringCast<Int32Type>: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        std::string str(value._data, value._size);
        int32_t cast_value = std::stoi(str);

        return ByteBuffer(sizeof(int32_t), reinterpret_cast<char*>(&cast_value));
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        std::string str(value._data, value._size);
        int32_t cast_value = std::stoi(str);

        return ByteBuffer(sizeof(int32_t), reinterpret_cast<char*>(&cast_value));
    }
};

template<>
class FromStringCast<Int64Type>: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        std::experimental::string_view str(value._data, value._size);
        int64_t cast_value = 0;
        boost::spirit::qi::parse(str.begin(), str.end(), boost::spirit::qi::long_, cast_value);

        return ByteBuffer(sizeof(int64_t), reinterpret_cast<char*>(&cast_value));
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        std::experimental::string_view str(value._data, value._size);
        int64_t cast_value = 0;
        boost::spirit::qi::parse(str.begin(), str.end(), boost::spirit::qi::long_, cast_value);

        return ByteBuffer(sizeof(int64_t), reinterpret_cast<char*>(&cast_value));
    }
};

template<>
class FromStringCast<FloatType>: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        std::string str(value._data, value._size);
        float cast_value = std::stof(str);

        return ByteBuffer(sizeof(float), reinterpret_cast<char*>(&cast_value));
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        std::string str(value._data, value._size);
        float cast_value = std::stof(str);

        return ByteBuffer(sizeof(float), reinterpret_cast<char*>(&cast_value));
    }
};

template<>
class FromStringCast<DoubleType>: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        std::string str(value._data, value._size);
        double cast_value = std::stod(str);

        return ByteBuffer(sizeof(double), reinterpret_cast<char*>(&cast_value));
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        std::string str(value._data, value._size);
        double cast_value = std::stod(str);

        return ByteBuffer(sizeof(double), reinterpret_cast<char*>(&cast_value));
    }
};

template<>
class FromStringCast<StringType>: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        return ByteBuffer(value._size, value._data);
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        return ByteBuffer(value._size, value._data);
    }
};

template<typename T>
class ToStringCast: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        throw;
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        throw;
    }
};

template<>
class ToStringCast<Int8Type>: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        int8_t cast_value = *reinterpret_cast<int8_t*>(value._data);
        std::string str = std::to_string(cast_value);

        return ByteBuffer(str.size(), str.data());
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        int8_t cast_value = *reinterpret_cast<int8_t*>(value._data);
        std::string str = std::to_string(cast_value);

        return ByteBuffer(str.size(), str.data());
    }
};

template<>
class ToStringCast<Int16Type>: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        int16_t cast_value = *reinterpret_cast<int16_t*>(value._data);
        std::string str = std::to_string(cast_value);

        return ByteBuffer(str.size(), str.data());
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        int16_t cast_value = *reinterpret_cast<int16_t*>(value._data);
        std::string str = std::to_string(cast_value);

        return ByteBuffer(str.size(), str.data());
    }
};

template<>
class ToStringCast<Int32Type>: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        int32_t cast_value = *reinterpret_cast<int32_t*>(value._data);
        std::string str = std::to_string(cast_value);

        return ByteBuffer(str.size(), str.data());
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        int32_t cast_value = *reinterpret_cast<int32_t*>(value._data);
        std::string str = std::to_string(cast_value);

        return ByteBuffer(str.size(), str.data());
    }
};

template<>
class ToStringCast<Int64Type>: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        int64_t cast_value = *reinterpret_cast<int64_t*>(value._data);
        std::string str = std::to_string(cast_value);

        return ByteBuffer(str.size(), str.data());
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        int64_t cast_value = *reinterpret_cast<int64_t*>(value._data);
        std::string str = std::to_string(cast_value);

        return ByteBuffer(str.size(), str.data());
    }
};

template<>
class ToStringCast<FloatType>: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        float cast_value = *reinterpret_cast<float*>(value._data);
        std::string str = std::to_string(cast_value);

        return ByteBuffer(str.size(), str.data());
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        float cast_value = *reinterpret_cast<float*>(value._data);
        std::string str = std::to_string(cast_value);

        return ByteBuffer(str.size(), str.data());
    }
};

template<>
class ToStringCast<DoubleType>: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        double cast_value = *reinterpret_cast<double*>(value._data);
        std::string str = std::to_string(cast_value);

        return ByteBuffer(str.size(), str.data());
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        double cast_value = *reinterpret_cast<double*>(value._data);
        std::string str = std::to_string(cast_value);

        return ByteBuffer(str.size(), str.data());
    }
};

template<>
class ToStringCast<StringType>: public UnaryOperator
{
public:
    ByteBuffer operation(ByteBuffer &value) override
    {
        return ByteBuffer(value._size, value._data);
    }
    ByteBuffer operation(ViewByteBuffer &value) override
    {
        return ByteBuffer(value._size, value._data);
    }
};

#endif // OPERATORS_H
