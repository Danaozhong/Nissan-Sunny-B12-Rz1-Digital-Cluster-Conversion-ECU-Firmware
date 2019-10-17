#include "lookup_table.hpp"


namespace app
{
	CharacteristicCurve::CharacteristicCurve(const std::vector<std::pair<double, double>> &data_points)
	:m_a_lookup_table(data_points)
	{}

	double CharacteristicCurve::get_y(double d_x) const
	{
		using namespace std;
		if (m_a_lookup_table.size() == 0)
		{
			return 0.0;
		}

		if (d_x < get<0>(m_a_lookup_table.front()))
		{
			return get<1>(m_a_lookup_table.front());
		}

		for (auto itr = m_a_lookup_table.begin(); itr != m_a_lookup_table.end() - 1; ++itr)
		{
			double current_x = get<0>(*itr);
			double next_x = get<0>(*(itr + 1));
			double current_y = get<1>(*itr);
			double next_y = get<1>(*(itr + 1));

			if(d_x < next_x)
			{
				if ((next_x - current_x) > 0.0001)
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

	double CharacteristicCurve::get_x(double d_y) const
	{
		if (m_a_lookup_table.size() == 0)
		{
			return 0.0;
		}

		// within the valid range of the lookup table, do a linear interpolation
		for (auto itr = m_a_lookup_table.begin(); itr != m_a_lookup_table.end() - 1; ++itr)
		{
			double current_y = std::get<1>(*itr);
			double next_y = std::get<1>(*(itr+1));
			if (std::min(current_y, next_y) <= d_y && std::max(current_y, next_y) > d_y)
			{
				const double d_y_span = next_y - current_y;
				const double d_x_span = std::get<0>(*(itr+1)) - std::get<0>(*itr);
				if (std::abs(d_y_span) > 0.0001)
				{
					const double d_delta = (d_y - current_y) / d_y_span;
					return std::get<0>(*itr) + d_delta * d_x_span;
				}
				return std::get<0>(*itr);
			}
		}

		// value must be outside of the LUT.
		return 0.0;
	}
}
