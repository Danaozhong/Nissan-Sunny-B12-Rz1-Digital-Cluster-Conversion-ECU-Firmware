#ifndef _UTIL_ALGORITHMS_HPP_
#define _UTIL_ALGORITHMS_HPP_

#include <stdint.h>
#include <cstddef>

namespace midware
{
    namespace Algorithms
    {
        uint32_t calculate_crc8(const uint8_t* pu8_data, size_t u_num_of_elements);
        uint32_t calculate_crc32(const uint32_t* pu32_data, size_t u_num_of_elements);

    }
}

#endif
