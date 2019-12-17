#ifndef _LOOKUP_TABLE_HPP_
#define _LOOKUP_TABLE_HPP_

#include <memory>
#include <vector>
#include <utility>

namespace app
{
	/* Class to store a characteristic curve */
	template<typename Tx, typename Ty>
	class CharacteristicCurve
	{
	public:
		CharacteristicCurve(std::pair<Tx, Ty> data_points[], uint32_t u32_num_of_data_points)
			//:m_a_lookup_table(data_points)
		{
			m_u32_num_of_data_points = u32_num_of_data_points;
			m_p_lookup_table = new std::pair<Tx, Ty>[m_u32_num_of_data_points];
			for (uint32_t u32_i = 0u; u32_i < m_u32_num_of_data_points; ++u32_i)
			{
				m_p_lookup_table[u32_i] = data_points[u32_i];

			}
		}

		~CharacteristicCurve()
		{
			delete[] m_p_lookup_table;
		}


		Ty get_y(Tx d_x) const
		{
			using namespace std;
			if (m_u32_num_of_data_points == 0)
			{
				return 0;
			}

			if (d_x < get<0>(m_p_lookup_table[0]))
			{
				return get<1>(m_p_lookup_table[0]);
			}

			for (uint32_t u32_i = 0; u32_i < m_u32_num_of_data_points - 1; ++u32_i)
			{
				Tx current_x = get<0>(m_p_lookup_table[u32_i]);
				Tx next_x = get<0>(m_p_lookup_table[u32_i + 1]);
				Ty current_y = get<1>(m_p_lookup_table[u32_i]);
				Ty next_y = get<1>(m_p_lookup_table[u32_i + 1]);

				if(d_x < next_x)
				{
					if (static_cast<int32_t>(next_x - current_x) != 0)
					{
						// do linear interpolation
						return current_y + (next_y - current_y) / (next_x - current_x) * (d_x - current_x);
					}
					return current_y;
				}
			}

			// we are out too the right of the characteristic. Return the last given value
			return get<1>(m_p_lookup_table[m_u32_num_of_data_points - 1]);
		}


		Tx get_x(Ty d_y) const
		{
			if (m_u32_num_of_data_points == 0)
			{
				return 0;
			}

			// within the valid range of the lookup table, do a linear interpolation
			for (uint32_t u32_i = 0; u32_i < m_u32_num_of_data_points - 1; ++u32_i)
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
			return 0;
		}

	private:
		std::pair<Tx, Ty>* m_p_lookup_table;
		uint32_t m_u32_num_of_data_points;


	};
}

#endif
