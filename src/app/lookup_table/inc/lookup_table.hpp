#ifndef _LOOKUP_TABLE_HPP_
#define _LOOKUP_TABLE_HPP_

#include <memory>
#include <vector>
#include <utility>
#include <cstring>

namespace app
{
	/* Class to store a characteristic curve */
	template<typename Tx, typename Ty>
	class CharacteristicCurve
	{
	public:
	    CharacteristicCurve()  {}

	    /** Paramter constructor */
		CharacteristicCurve(std::pair<Tx, Ty> data_points[], uint32_t u32_num_of_data_points)
			//:m_a_lookup_table(data_points)
		{
		    m_p_lookup_table.reserve(u32_num_of_data_points);
		    for (uint32_t u32_i = 0u; u32_i < u32_num_of_data_points; ++u32_i)
			{
				m_p_lookup_table.push_back(data_points[u32_i]);

			}
		}

        CharacteristicCurve(const std::vector<std::pair<Tx, Ty>> &data_points)
            //:m_a_lookup_table(data_points)
        {
            m_p_lookup_table = data_points;
        }

		/** copy constructor */
		CharacteristicCurve(const CharacteristicCurve &other)
		{
		    m_p_lookup_table = other.m_p_lookup_table;
		}

		/** Move constructor */
		CharacteristicCurve(CharacteristicCurve&& other)
		{
		    m_p_lookup_table = std::move(other.m_p_lookup_table);
		}

		CharacteristicCurve& operator=(const CharacteristicCurve &other)
		{
		    m_p_lookup_table = other.m_p_lookup_table;
		    return *this;
		}

		~CharacteristicCurve()
		{
		}


		Ty get_y(Tx d_x) const
		{
			using namespace std;
			if (m_p_lookup_table.size() == 0)
			{
				return Ty(0);
			}

			if (d_x < get<0>(m_p_lookup_table[0]))
			{
				return get<1>(m_p_lookup_table[0]);
			}

			for (uint32_t u32_i = 0; u32_i < m_p_lookup_table.size() - 1; ++u32_i)
			{
				Tx current_x = get<0>(m_p_lookup_table[u32_i]);
				Tx next_x = get<0>(m_p_lookup_table[u32_i + 1]);
				Ty current_y = get<1>(m_p_lookup_table[u32_i]);
				Ty next_y = get<1>(m_p_lookup_table[u32_i + 1]);

				if(d_x < next_x)
				{
					if ((next_x - current_x) != Tx(0))
					{
						// do linear interpolation
						return current_y + ((next_y - current_y) * static_cast<Ty>(d_x - current_x)) / static_cast<Ty>(next_x - current_x);
					}
					return current_y;
				}
			}

			// we are out too the right of the characteristic. Return the last given value
			return get<1>(m_p_lookup_table[m_p_lookup_table.size() - 1]);
		}


		Tx get_x(Ty d_y) const
		{
			if (m_p_lookup_table.size() == 0)
			{
				return Tx(0);
			}

			// within the valid range of the lookup table, do a linear interpolation
			for (uint32_t u32_i = 0; u32_i < m_p_lookup_table.size() - 1; ++u32_i)
			{
				Ty current_y = std::get<1>(m_p_lookup_table[u32_i]);
				Ty next_y = std::get<1>(m_p_lookup_table[u32_i + 1]);
				if (std::min(current_y, next_y) <= d_y && std::max(current_y, next_y) > d_y)
				{
					const Ty d_y_span = next_y - current_y;
					const Tx d_x_span = std::get<0>(m_p_lookup_table[u32_i + 1]) - std::get<0>(m_p_lookup_table[u32_i]);
					if (static_cast<int32_t>(d_y_span) != 0)
					{
						return std::get<0>(m_p_lookup_table[u32_i]) + (d_y - current_y) * d_x_span / d_y_span;
					}
					return std::get<0>(m_p_lookup_table[u32_i]);
				}
			}

			// value must be outside of the LUT.
			return Tx(0);
		}

		Tx get_first_x() const
		{
            if (m_p_lookup_table.size() == 0)
            {
                return Tx(0);
            }
            return std::get<0>(m_p_lookup_table[0]);
		}

        Tx get_last_x() const
        {
            if (m_p_lookup_table.size() == 0)
            {
                return Tx(0);
            }
            return std::get<0>(m_p_lookup_table[m_p_lookup_table.size() - 1]);
        }

        Ty get_max_y() const
        {
            auto found_element = std::max_element(m_p_lookup_table.begin(), m_p_lookup_table.end(),
                    [](const std::pair<Tx, Ty>& e1, const std::pair<Tx, Ty>& e2) -> bool
            {
                return std::get<1>(e1) < std::get<1>(e2);
            });
            return std::get<1>(*found_element);
        }

        Ty get_min_y() const
        {
            auto found_element = std::min_element(m_p_lookup_table.begin(), m_p_lookup_table.end(),
                    [](const std::pair<Tx, Ty>& e1, const std::pair<Tx, Ty>& e2) -> bool
            {
                return std::get<1>(e1) < std::get<1>(e2);
            });
            return std::get<1>(*found_element);
        }

        std::vector<std::pair<Tx, Ty>>& get_data_points()
        {
            return m_p_lookup_table;
        }

        const std::vector<std::pair<Tx, Ty>>& get_data_points() const
        {
            return m_p_lookup_table;
        }

        size_t get_num_of_data_points() const
        {
            return m_p_lookup_table.size();
        }

	private:
		std::vector<std::pair<Tx, Ty>> m_p_lookup_table;
	};

	namespace CharacteristicCurveHelper
	{
	    template<typename Tx, typename Ty>
	    std::vector<uint8_t> to_bytes(const CharacteristicCurve<Tx, Ty> &o_characteristic)
	    {
	        auto data_points = o_characteristic.get_data_points();

	        // 4 bytes for the size, the rest is the table data
	        std::vector<uint8_t> ai8_result(4 + data_points.size() * (sizeof(Tx) + sizeof(Ty)));
	        const uint32_t u32_num_of_elements = static_cast<uint32_t>(data_points.size());
	        std::memcpy(ai8_result.data(), &u32_num_of_elements, 4);

	        uint8_t* write_pointer = ai8_result.data() + 4;
	        for (uint32_t u32_i = 0u; u32_i < u32_num_of_elements; ++u32_i)
	        {
	            std::memcpy(write_pointer, &std::get<0>(data_points[u32_i]), sizeof(Tx));
	            write_pointer += sizeof(Tx);
                std::memcpy(write_pointer, &std::get<1>(data_points[u32_i]), sizeof(Ty));
                write_pointer += sizeof(Ty);
	        }
	        return ai8_result;
	    }

	    template<typename Tx, typename Ty>
	    CharacteristicCurve<Tx, Ty> from_bytes(
	            const std::vector<uint8_t>::const_iterator &begin_itr,
	            const std::vector<uint8_t>::const_iterator &end_itr)
        {
	        size_t s_buffer_length = end_itr - begin_itr;
	        CharacteristicCurve<Tx, Ty> o_ret_val;

	        if (s_buffer_length >= 2)
	        {
	            uint32_t u32_num_of_elements = 0u;
	            std::memcpy(&u32_num_of_elements, &(*begin_itr), sizeof(uint32_t));

	            if (u32_num_of_elements * (sizeof(Tx) + sizeof(Ty)) + sizeof(uint32_t) <= s_buffer_length)
	            {
	                std::vector<std::pair<Tx, Ty>> ao_data(u32_num_of_elements);
	                const uint8_t* read_pointer= &(*begin_itr) + sizeof(uint32_t);
	                for (uint32_t u32_i = 0u; u32_i < u32_num_of_elements; ++u32_i)
	                {
	                    Tx o_x;
	                    Ty o_y;
	                    std::memcpy(&o_x, read_pointer, sizeof(Tx));
	                    read_pointer += sizeof(Tx);
	                    std::memcpy(&o_y, read_pointer, sizeof(Ty));
	                    read_pointer += sizeof(Ty);
	                    auto o_new_pair = std::pair<Tx, Ty>(o_x, o_y);
	                    ao_data[u32_i] = o_new_pair;
	                }
	                o_ret_val = CharacteristicCurve<Tx, Ty>(ao_data);
	            }
	        }
	        return o_ret_val;
        }
	}
}
#endif
