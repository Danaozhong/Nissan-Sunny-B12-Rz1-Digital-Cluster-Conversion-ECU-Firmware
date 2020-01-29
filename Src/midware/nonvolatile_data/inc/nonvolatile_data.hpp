#ifndef _NONVOLATILE_DATA_HPP_
#define _NONVOLATILE_DATA_HPP_

#include <cstddef>

namespace midware
{
    class BufferStorableObject
    {
    public:
        virtual ~BufferStorableObject() = 0;

        /** Stores the object into a buffer. Return code is the actual size needed */
        virtual int32_t store_to_buffer(uint8_t* pu8_buffer, size_t u_buffer_size, size_t &u_used_buffer_size) = 0;

        virtual int32_t load_from_buffer(const uint8_t* pcu8_buffer, size_t u_buffer_size) = 0;
    };

    class NonvolatileData : public BufferStorableObject
    {
    public:
        virtual int32_t load_default_values();
    };
}

#endif /* _NONVOLATILE_DATA_HPP_ */
