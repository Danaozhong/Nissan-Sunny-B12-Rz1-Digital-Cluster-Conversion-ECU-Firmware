#ifndef _UTIL_ALGORITHMS_HPP_
#define _UTIL_ALGORITHMS_HPP_

#include <stdint.h>
#include <cstddef>
#include <utility>
#include <vector>
#include <cmath>

namespace midware
{
    namespace Algorithms
    {
        uint32_t calculate_crc8(const uint8_t* pu8_data, size_t u_num_of_elements);
        uint32_t calculate_crc32(const uint32_t* pu32_data, size_t u_num_of_elements);

        /** Calculates an average value of data by removing outliers.
         * The values furthest away from the average are removed, until the the average is less than cu32_min_max_allowed_gap, or
         * the remaining number of data elements is equal or less to cu32_minimum_readings.
         * \returns A pair of the average value, and the remaining gap of the data.
         */

        template <typename T>
        typename std::iterator_traits<T>::value_type get_average(T begin_itr, T end_itr)
        {
            typename std::iterator_traits<T>::value_type avg = 0;
            for (auto itr = begin_itr; itr != end_itr; ++itr)
            {
                avg += *itr;
            }
            const typename std::iterator_traits<T>::value_type dist = std::distance(begin_itr, end_itr);
            if (dist == 0)
            {
                return avg;
            }
            return avg / dist;
        }

        template<typename T, typename U>
        U get_weighted_average(T begin, T end)
        {
            if (begin == end)
            {
                return 0;
            }

            U sum_of_weighted_values = 0;
            U sum_of_weights = 0;
            for (auto itr = begin; itr != end; ++itr)
            {
                const U weight = static_cast<U>(std::get<1>(*itr));
                sum_of_weighted_values += weight * static_cast<U>(std::get<0>(*itr));
                sum_of_weights += weight;
            }
            if (sum_of_weights == 0)
            {
                return sum_of_weighted_values;
            }
            return sum_of_weighted_values / sum_of_weights;
        }

        template<typename T>
        std::pair<T, T> filter_outliers_by_max_deviation(std::vector<T> &input_data, const uint32_t cu32_minimum_readings, const T min_max_allowed_gap)
        {
            T u32_avg = get_average(input_data.begin(), input_data.end());
            T u32_gap = 0;

            while (input_data.size() > cu32_minimum_readings) // take at least three readings into account, if filtering is employed
            {
                // calculate min, max, and the data point furthest away from the average
                T u32_min = input_data[0];
                T u32_max = input_data[0];
                T u32_max_deviation = 0u;
                uint32_t u8_max_deviation_index = 0u;

                for (uint32_t u8_i = 0; u8_i < input_data.size(); u8_i++)
                {
                    u32_max = std::max(u32_max, input_data[u8_i]);
                    u32_min = std::min(u32_max, input_data[u8_i]);

                    // check if the current point is further away from the average than the previous ones
                    T u32_current_deviation = std::abs(static_cast<int32_t>(u32_avg) - static_cast<int32_t>(input_data[u8_i]));
                    if (u32_current_deviation > u32_max_deviation)
                    {
                        u32_max_deviation = u32_current_deviation;
                        u8_max_deviation_index = u8_i;
                    }
                }

                u32_gap = u32_max - u32_min;
                // check if the average value satisfies the termination criteria.
                if (u32_gap < min_max_allowed_gap)
                {
                    // no more filtering needed
                    break;
                }

                // remove the item with the largest deviation from the list...
                std::swap(input_data.back(), input_data[u8_max_deviation_index]);
                input_data.pop_back();

                // ...and recalculate average
                u32_avg = get_average(input_data.begin(), input_data.end());
            }
            return std::pair<T, T>(u32_avg, u32_gap);
        }
    }
}

#endif
