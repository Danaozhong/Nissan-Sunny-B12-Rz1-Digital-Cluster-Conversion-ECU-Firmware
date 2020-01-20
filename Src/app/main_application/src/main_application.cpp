#include "main_application.hpp"
#include "console_commands.hpp"
#ifdef USE_CAN
#include "Can.h"
#include "CanIf.h"
#endif

#include "excp_handler.hpp"
#include "excp_handler_console_commands.hpp"


namespace app
{

	MainApplication::MainApplication()
	{}

	void MainApplication::startup_from_reset()
	{
	    // create the UART interface to be able to log low-level debug messages
	#ifdef USE_STM32_F3_DISCO
	    m_p_uart = std::make_shared<drivers::STM32HardwareUART>(GPIOD, GPIO_PIN_6, GPIOD, GPIO_PIN_5);
	#elif defined USE_STM32F3XX_NUCLEO_32
	    m_p_uart = new drivers::STM32HardwareUART(GPIOA, GPIO_PIN_3, GPIOA, GPIO_PIN_2);
	#elif defined STM32F303xC
	    m_p_uart = new drivers::STM32HardwareUART(GPIOA, GPIO_PIN_3, GPIOA, GPIO_PIN_2);
#elif defined STM32F429xx
	    m_p_uart = new drivers::STM32HardwareUART(GPIOA, GPIO_PIN_10, GPIOA, GPIO_PIN_9);
#else
#error "No valid pin configuration for UART this MCU set!"
	#endif

	    // ...open UART connection.
	    m_p_uart->connect(9600, drivers::UART_WORD_LENGTH_8BIT, drivers::UART_STOP_BITS_1, drivers::UART_FLOW_CONTROL_NONE);

	    // Create the debug console
	    m_po_os_console = new OSServices::OSConsole(m_p_uart);

#ifdef USE_TRACE
	    // Initialize the trace module
	    m_po_trace = new midware::Trace();

	    // Enable trace logging via UART
	    auto po_trace_io_interface = std::make_shared<midware::UARTTraceIOInterface>(m_p_uart);
	    m_po_trace->add_trace_io_interface(po_trace_io_interface);

	    // and set the trace moduel as the default system tracer
	    m_po_trace->set_as_default_trace();
#endif

        // Initialize the exception storage module, to be able to log / debug exceptions
        m_po_exception_handler = new midware::ExceptionHandler();
        m_po_exception_handler->init();
        m_po_exception_handler->set_as_default_exception_handler();

	    // register the command to debug the exception handler on the os console
	    m_po_os_console->register_command(new midware::CommandListExceptions());

#ifdef USE_CAN
	    /* Init the CAN interface (AUTOSAR conform) */
	    Can_Init(&CanConfigData);
	    CanIf_Init(&CanIf_Config);
#endif

	    // initialization below is for the application
        init_fuel_level_converter();
        init_speed_converter();

        // register the debug commands in the os console, so that debugging of the speed signals is possible.
        this->m_po_os_console->register_command(new app::CommandSpeed());
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

	OSServices::OSConsole* MainApplication::get_os_console()
	{
	    return this->m_po_os_console;
	}

	void MainApplication::fuel_sensor_input_received(int32_t i32_value)
	{
		if (m_p_o_fuel_gauge_output != nullptr)
		{
			m_p_o_fuel_gauge_output->set_fuel_level(i32_value);
		}
	}

	int32_t MainApplication::init_speed_converter()
    {
        // TODO this is the configuration for the STM32 discovery, change to support the small board
#ifdef STM32F429xx
	    m_p_pwm = std::make_shared<drivers::STM32PWM>(TIM5, TIM_CHANNEL_4, GPIOA, GPIO_PIN_3);
	    m_p_pwm_ic = std::make_shared<drivers::STM32PWM_IC>(TIM2, TIM_CHANNEL_2, TIM_CHANNEL_3, 1u, 65536u);

	    if (OSServices::ERROR_CODE_SUCCESS != m_p_pwm_ic->init())
	    {
	        ExceptionHandler_handle_exception(EXCP_MODULE_PWM_IC, EXCP_TYPE_PWM_IC_INIT_FAILED,
	                false, __FILE__, __LINE__, 0u);
	    }
#else
        m_p_pwm = std::make_shared<drivers::STM32PWM>(TIM3, TIM_CHANNEL_1, GPIOC, GPIO_PIN_6);
#endif
        m_po_speed_sensor_converter = std::make_shared<SpeedSensorConverter>(m_p_pwm, m_p_pwm_ic, 1u, 4200u);
        return OSServices::ERROR_CODE_SUCCESS;
    }

    int32_t MainApplication::deinit_speed_converter()
    {
        return OSServices::ERROR_CODE_SUCCESS;
    }

	int32_t MainApplication::init_fuel_level_converter()
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

    int32_t MainApplication::deinit_fuel_level_converter()
    {
        // explicitly delete the application objects
        delete m_p_o_fuel_gauge_input;

        m_p_o_fuel_gauge_input = nullptr;
        m_p_o_fuel_gauge_output = nullptr;
        m_p_o_fuel_gauge_output_characteristic = nullptr;
        m_p_o_fuel_gauge_input_characteristic = nullptr;
        m_p_adc = nullptr;
        m_p_dac = nullptr;

        return OSServices::ERROR_CODE_SUCCESS;
    }


}
