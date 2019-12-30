#include "main_application.hpp"

namespace app
{

	MainApplication::MainApplication()
	{
	    initialize_fuel_level_converter();
	    initilialize_speed_converter();
	}

	MainApplication& MainApplication::get()
	{
		// singleton will be instatiated when first called.
		static MainApplication o_main_application;
		return o_main_application;
	}

	std::shared_ptr<SpeedSensorConverter> MainApplication::get_speed_sensor_converter() const
    {
	    return m_po_speed_sensor_converter;
    }

	void MainApplication::fuel_sensor_input_received(int32_t i32_value)
	{
		if (m_p_o_fuel_gauge_output != nullptr)
		{
			m_p_o_fuel_gauge_output->set_fuel_level(i32_value);
		}
	}

	int32_t MainApplication::initilialize_speed_converter()
    {
        // TODO this is the configuration for the STM32 discovery, change to support the small board
        m_p_pwm = std::make_shared<drivers::STM32PWM>(TIM3, TIM_CHANNEL_1, GPIOC, GPIO_PIN_6);
        m_po_speed_sensor_converter = std::make_shared<SpeedSensorConverter>(m_p_pwm, 1u, 4200u);
        return OSServices::ERROR_CODE_SUCCESS;
    }

	int32_t MainApplication::initialize_fuel_level_converter()
    {
        // create the low-level hardware interfaces
        m_p_adc = std::make_shared<drivers::STM32ADC>(drivers::ADCResolution::ADC_RESOLUTION_12BIT, ADC2, ADC_CHANNEL_2, GPIOA, GPIO_PIN_5);
        m_p_dac = std::make_shared<drivers::STM32DAC>(DAC1, GPIOA, GPIO_PIN_4);

        /* Characteristics of the Nissan Sunny EUDM fuel sensor. 0% = 100Ohm (empty), 100% = 10Ohm (full). See
         * http://texelography.com/2019/06/21/nissan-rz1-digital-cluster-conversion/ for the full dataset */
        std::pair<int32_t, int32_t> a_input_lut[] =
        {
                /* Fuel level (% * 100)  Resistor value in mOhm */
                std::make_pair(-1000, 120000),
                std::make_pair(-100, 87000),
                std::make_pair(500, 80600),
                std::make_pair(2500, 61800),
                std::make_pair(4800, 35700),
                std::make_pair(7700, 21000),
                std::make_pair(10000, 11800),
                std::make_pair(11000, 2400),
                std::make_pair(11500, 0000),
        };

        m_p_o_fuel_gauge_input_characteristic = std::make_shared<app::CharacteristicCurve<int32_t, int32_t>>(a_input_lut, sizeof(a_input_lut) / sizeof(a_input_lut[0]));

        /* Characteristics of the digital cluster fuel gauge */
        std::pair<int32_t, int32_t> a_output_lut[] =
        {
                std::make_pair(-1000, 5000),
                std::make_pair(0, 5000),
                std::make_pair(100, 4900),
                //std::make_pair(0, 4700),
                std::make_pair(714, 4340),
                std::make_pair(2143, 4040),
                std::make_pair(4286, 3300),
                std::make_pair(6429, 2240),
                std::make_pair(9286, 1100),
                std::make_pair(10000, 700),
                std::make_pair(11000, 700)
        };

        m_p_o_fuel_gauge_output_characteristic = std::make_shared<app::CharacteristicCurve<int32_t, int32_t>>(a_output_lut, sizeof(a_output_lut) / sizeof(a_output_lut[0]));

        // start the data output thread
        m_p_o_fuel_gauge_output = std::make_shared<app::FuelGaugeOutput>(m_p_dac, m_p_o_fuel_gauge_output_characteristic, 1500, 0);
        // start the data acquisition thread
        m_p_o_fuel_gauge_input = new app::FuelGaugeInputFromADC(m_p_adc, m_p_o_fuel_gauge_input_characteristic);

        // attach to the signal of the fuel sensor input
        auto event_handler = std::bind(&MainApplication::fuel_sensor_input_received, this, std::placeholders::_1);
        m_p_o_fuel_gauge_input->m_sig_fuel_level_changed.connect(event_handler);

        return OSServices::ERROR_CODE_SUCCESS;
    }


}
