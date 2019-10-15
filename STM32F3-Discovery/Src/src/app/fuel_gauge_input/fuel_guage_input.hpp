#ifndef _FUEL_GAUGE_INPUT_HPP_
#define _FUEL_GAUGE_INPUT_HPP_

#include <memory>
#include "generic_adc.hpp"


namespace app
{

	class LookupTable
	{
	public:
		double get_y(double d_x) const;

		double get_x(double d_y) const
		{
			if (m_a_lookup_table.size() == 0)
			{
				return 0.0;
			}

			// check boundaries (min)
			if (m_a_lookup_table[0][1] > d_y)
			{
				return m_a_lookup_table[0][0];
			}

			// within the valid range of the lookup table, do a linear interpolation
			for (auto itr = m_a_lookup_table.begin(); itr != m_a_lookup_table.end() - 1; ++itr)
			{
				// check if the current value is within the current y range
				if ((itr+1)->get<1>() > d_y)
				{
					const double d_y_span = (itr+1)->get<1>() - itr->get<1>();
					const double d_x_span = (itr+1)->get<0>() - itr->get<0>();
					const double d_delta = (d_y - itr->get<1>()) / d_y_span;
					return itr->get<0>() + d_delta * d_x_span;
				}
			}

			// value must be outside (to the right) of the LUT - return the last data point.
			return m_a_lookup_table.back()[0];
		}
	private:

		std::vector<std::tuple<double, double>> m_a_lookup_table;


	};


	class FuelGaugeInputFromADC
	{
	public:
		FuelGaugeInputFromADC(std::shared_ptr<drivers::GenericADC> p_adc);


	private:
		void thread_main(void)
		{

			// never terminate this thread
			while(true)
			{
				std_ex::sleep_for(std::chrono::miliseconds(500));
			}

		}

		/// The ADC used to
		std::shared_ptr<drivers::GenericADC> m_p_adc;


	};


}

#endif /* _FUEL_GAUGE_INPUT_HPP_ */
