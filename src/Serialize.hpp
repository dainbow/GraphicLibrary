#pragma once

#include <cstdint>
#include <cstdio>

class Serializeable {
    public:
        virtual void Serialize(FILE* outStream, uint32_t depth) const = 0;

        virtual ~Serializeable();
};
