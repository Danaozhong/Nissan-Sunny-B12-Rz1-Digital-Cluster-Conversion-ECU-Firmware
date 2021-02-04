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

    enum NonvolatileDataReadingState
    {
        DATA_READING_STATE_NOT_YET_READ,
        DATA_READING_STATE_READ_SUCCESSFUL,
        DATA_READING_STATE_DEFAULT_VALUES_LOADED

    };

    /** Class for storing nonvolatile data */
    class NonvolatileData : public BufferStorableObject
    {
    public:
        /** In case the data in non volatile memory got corrupted, this function is used
         * to load the default values */
        virtual int32_t load_default_values() = 0;

        NonvolatileDataReadingState get_data_reading_state();

        void set_reading_state(NonvolatileDataReadingState en_reading_state);
    };
}

#endif /* _NONVOLATILE_DATA_HPP_ */
