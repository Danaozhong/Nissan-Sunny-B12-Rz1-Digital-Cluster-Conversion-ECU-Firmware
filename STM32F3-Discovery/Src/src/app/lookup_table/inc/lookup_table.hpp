#ifndef _LOOKUP_TABLE_HPP_
#define _LOOKUP_TABLE_HPP_

#include <memory>
#include <vector>
#include <utility>

namespace app
{

	/* Class to store a characteristic curve */
	class CharacteristicCurve
	{
	public:
		CharacteristicCurve(const std::vector<std::pair<double, double>> &data_points);
		double get_y(double d_x) const;

		double get_x(double d_y) const;
	private:
		std::vector<std::pair<double, double>> m_a_lookup_table;


	};
}

#endif
