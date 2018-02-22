#ifndef ARRAY_H
#define ARRAY_H

#include <memory>
#include <memory.h>

class Array
{
private:
    char* _data = nullptr;
    uint64_t _capacity = 0;
    uint64_t _size = 0;
public:
    Array()
    {
        _data = new char[1024*1024];
        _capacity = 1024*1024;
        _size = 0;
    }
    inline void emplace_back(uint64_t size, const char* data)
    {
        uint64_t bytesLeft = _capacity - _size;

        if (bytesLeft < size)
        {
            resize();
        }

        memcpy(&_data[_size], data, size);
        _size += size;
    }
    inline char* get(uint64_t offset)
    {
        return &_data[offset];
    }
    inline uint64_t size()
    {
        return _size;
    }
    inline uint64_t capacity()
    {
        return _capacity;
    }
    ~Array()
    {
        delete[] _data;
        _data = nullptr;
        _capacity = 0;
        _size = 0;
    }
private:
    inline void resize()
    {
        uint64_t newCapacity = _capacity * 2;
        char* newData = new char[newCapacity];
        memcpy(newData, _data, _size);

        delete[] _data;
        _data = nullptr;

        _data = newData;
        _capacity = newCapacity;
    }
};

#endif // ARRAY_H
