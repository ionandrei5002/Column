#ifndef VALUE_H
#define VALUE_H

#include "types.h"
#include "bytebuffer.h"

class Value
{
private:
    ByteBuffer _buffer;
public:
    virtual ~Value() {}
};

#endif // VALUE_H
