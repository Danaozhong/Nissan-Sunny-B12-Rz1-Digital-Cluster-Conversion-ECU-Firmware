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
		CharacteristicCurve(const std::vector<std::pair<Tx, Ty>> &data_points)
			:m_a_lookup_table(data_points)
		{}


		Ty get_y(Tx d_x) const
		{
			using namespace std;
			if (m_a_lookup_table.size() == 0)
			{
				return 0;
			}

			if (d_x < get<0>(m_a_lookup_table.front()))
			{
				return get<1>(m_a_lookup_table.front());
			}

			for (auto itr = m_a_lookup_table.begin(); itr != m_a_lookup_table.end() - 1; ++itr)
			{
				Tx current_x = get<0>(*itr);
				Tx next_x = get<0>(*(itr + 1));
				Ty current_y = get<1>(*itr);
				Ty next_y = get<1>(*(itr + 1));

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
			return get<1>(m_a_lookup_table.back());
		}


		Tx get_x(Ty d_y) const
		{
			if (m_a_lookup_table.size() == 0)
			{
				return 0;
			}

			// within the valid range of the lookup table, do a linear interpolation
			for (auto itr = m_a_lookup_table.begin(); itr != m_a_lookup_table.end() - 1; ++itr)
			{
				Ty current_y = std::get<1>(*itr);
				Ty next_y = std::get<1>(*(itr+1));
				if (std::min(current_y, next_y) <= d_y && std::max(current_y, next_y) > d_y)
				{
					const Ty d_y_span = next_y - current_y;
					const Tx d_x_span = std::get<0>(*(itr+1)) - std::get<0>(*itr);
					if (static_cast<int32_t>(d_y_span) != 0)
					{
						return std::get<0>(*itr) + (d_y - current_y) * d_x_span / d_y_span;
					}
					return std::get<0>(*itr);
				}
			}

			// value must be outside of the LUT.
			return 0;
		}

	private:
		std::vector<std::pair<Tx, Ty>> m_a_lookup_table;


	};
}

#endif
