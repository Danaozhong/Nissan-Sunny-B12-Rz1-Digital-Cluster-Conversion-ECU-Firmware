#include "main_application.hpp"
#include "console_commands.hpp"
#ifdef USE_CAN
#include "Can.h"
#include "CanIf.h"

#include "can_diagnostics.hpp"
#endif

#include "excp_handler.hpp"
#include "excp_handler_console_commands.hpp"


namespace app
{

	MainApplication::MainApplication()
	    : m_en_fuel_gauge_output_mode(FUEL_GAUGE_OUTPUT_MODE_CONVERSION),
	      m_i32_fuel_gauge_output_manual_value(0)
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
	    m_po_os_io_interface = std::make_shared<OSServices::OSConsoleUartIOInterface>(m_p_uart);
	    m_po_os_console = std::make_shared<OSServices::OSConsole>(m_po_os_io_interface);

#ifdef USE_CAN
        /* Init the CAN interface (AUTOSAR conform) */
        Can_Init(&CanConfigData);
        CanIf_Init(&CanIf_Config);

        CanIf_SetControllerMode(0, CAN_CS_STARTED);
        CanIf_SetPduMode(0, CANIF_ONLINE);

        /* Register CAN diagnostics commands on the UART */
        m_po_os_console->register_command(new app::CommandCAN());
#endif


#ifdef USE_TRACE
	    // Initialize the trace module
	    m_po_trace = new midware::Trace();
        m_po_trace->init();
	    // Enable trace logging via the OS console
	    m_po_trace->add_trace_io_interface(m_po_os_console);

	    // and set the trace module as the default system tracer
	    m_po_trace->set_as_default_trace();
#endif

        m_po_exception_handler = new midware::ExceptionHandler();
        m_po_exception_handler->set_as_default_exception_handler();


#ifdef USE_NVDH
        // initialize non-volatile memory (uses 1 block of size 1024) TODO move config to CMake
        m_po_nonvolatile_data_handler = std::make_shared<midware::NonvolatileDataHandler>(1u, 1024u);

        // Configure the default sections for the module
        std::vector<midware::FlashSection> default_flash_sections =
                {
                    midware::FlashSection{"EXCP", 256},
                    midware::FlashSection{"DATASET", 512}
                };

        if (OSServices::ERROR_CODE_SUCCESS != m_po_nonvolatile_data_handler->set_default_sections(default_flash_sections))
        {
            ExceptionHandler_handle_exception(EXCP_MODULE_NONVOLATILE_DATA, EXCP_TYPE_NONVOLATILE_DATA_SETTING_DEFAULT_SECTIONS_FAILED, false, __FILE__, __LINE__, 0u);
        }

        // afterwards, have a look what is in memory and read it
        if (OSServices::ERROR_CODE_SUCCESS != m_po_nonvolatile_data_handler->load())
        {
            ExceptionHandler_handle_exception(EXCP_MODULE_NONVOLATILE_DATA, EXCP_TYPE_NONVOLATILE_DATA_LOADING_FAILED, false, __FILE__, __LINE__, 0u);
        }



        m_po_exception_handler->set_nonvolatile_data_handler(m_po_nonvolatile_data_handler, "EXCP");
#endif /* USE_NVDH */

        // Initialize the exception storage module, to be able to log / debug exceptions
        m_po_exception_handler->init();

	    // register the command to debug the exception handler on the os console
	    m_po_os_console->register_command(new midware::CommandListExceptions());


#ifdef USE_NVDH
	    // read the dataset
	    if (OSServices::ERROR_CODE_SUCCESS != m_o_dataset.load_dataset(*m_po_nonvolatile_data_handler))
	    {
	        ExceptionHandler_handle_exception(EXCP_MODULE_DATASET, EXCP_TYPE_DATASET_LOADING_FAILED, false, __FILE__, __LINE__, 0u);
	        // load default dataset
	        m_o_dataset.load_default_dataset();
	    }

#else
	    // load default dataset
        m_o_dataset.load_default_dataset();
#endif


        // initialization below is for the application
        init_fuel_level_converter();
        init_speed_converter();

        // register the debug commands in the os console, so that debugging of the speed signals is possible.
        this->m_po_os_console->register_command(new app::LookupTableEditor());
        this->m_po_os_console->register_command(new app::CommandSpeed());
        this->m_po_os_console->register_command(new app::CommandFuel());
        this->m_po_os_console->register_command(new app::CommandDataset());
        this->m_po_os_console->register_command(new app::CommandVersion());
	}

	MainApplication& MainApplication::get()
	{
		// singleton will be instantiated when first called.
		static MainApplication o_main_application;
		return o_main_application;
	}

	SpeedSensorConverter* MainApplication::get_speed_sensor_converter() const
    {
	    return m_po_speed_sensor_converter;
    }

#ifdef USE_NVDH
        std::shared_ptr<midware::NonvolatileDataHandler> MainApplication::get_nonvolatile_data_handler() const
        {
            return m_po_nonvolatile_data_handler;
        }
#endif

    std::shared_ptr<OSServices::OSConsole> MainApplication::get_os_console()
	{
	    return this->m_po_os_console;
	}

    void MainApplication::set_fuel_gauge_output_mode(FuelGaugeOutputMode en_fuel_output_mode)
    {
        m_en_fuel_gauge_output_mode = en_fuel_output_mode;
        update_fuel_sensor_output();
    }

    void MainApplication::set_manual_fuel_gauge_output_value(int32_t _i32_fuel_gauge_output_value)
    {
        m_i32_fuel_gauge_output_manual_value = _i32_fuel_gauge_output_value;
        update_fuel_sensor_output();
    }

    const app::Dataset& MainApplication::get_dataset() const
    {
        return m_o_dataset;
    }

    app::Dataset& MainApplication::get_dataset()
    {
        return m_o_dataset;
    }


	void MainApplication::fuel_sensor_input_received(int32_t i32_value)
	{
	    m_i32_fuel_sensor_read_value = i32_value;
	    update_fuel_sensor_output();
	}

	void MainApplication::update_fuel_sensor_output()
	{
	    int32_t i32_output_value = m_i32_fuel_sensor_read_value;
        if (FUEL_GAUGE_OUTPUT_MODE_MANUAL == m_en_fuel_gauge_output_mode)
        {
            i32_output_value = m_i32_fuel_gauge_output_manual_value;
        }
        if (m_p_o_fuel_gauge_output != nullptr)
        {
            m_p_o_fuel_gauge_output->set_fuel_level(i32_output_value);
        }
	}

	int32_t MainApplication::init_speed_converter()
    {
        // TODO this is the configuration for the STM32 discovery, change to support the small board
#ifdef STM32F429xx
	    m_p_pwm = new drivers::STM32PWM(TIM5, TIM_CHANNEL_4, GPIOA, GPIO_PIN_3);
	    m_p_pwm_ic = new drivers::STM32PWM_IC(TIM2, TIM_CHANNEL_2, TIM_CHANNEL_3, 1u, 65536u);
#else
        m_p_pwm = new drivers::STM32PWM(TIM3, TIM_CHANNEL_1, GPIOA, GPIO_PIN_6);
        m_p_pwm_ic = new drivers::STM32PWM_IC(TIM4, TIM_CHANNEL_1, TIM_CHANNEL_2, 1023u, 65536u); // Pins PA11 PA12
#endif

        int32_t i32_ret_val = m_p_pwm_ic->init();
        if (OSServices::ERROR_CODE_SUCCESS != i32_ret_val)
        {
            ExceptionHandler_handle_exception(EXCP_MODULE_PWM_IC, EXCP_TYPE_PWM_IC_INIT_FAILED,
                    false, __FILE__, __LINE__, static_cast<uint32_t>(i32_ret_val));
        }




        m_po_speed_sensor_converter = new SpeedSensorConverter(m_p_pwm, m_p_pwm_ic,
                get_dataset().get_input_pulses_per_kmph_mHz(),
                get_dataset().get_output_pulses_per_kmph_mHz());



        return OSServices::ERROR_CODE_SUCCESS;
    }

    int32_t MainApplication::deinit_speed_converter()
    {
        return OSServices::ERROR_CODE_SUCCESS;
    }

	int32_t MainApplication::init_fuel_level_converter()
    {
        // create the low-level hardware interfaces
        m_p_adc = new drivers::STM32ADC(drivers::ADCResolution::ADC_RESOLUTION_12BIT, ADC2, ADC_CHANNEL_2, GPIOA, GPIO_PIN_5);
#ifdef STM32F303xC
        m_p_dac = new drivers::STM32DAC(DAC1, GPIOA, GPIO_PIN_4);
#else
        m_p_dac = new drivers::STM32DAC(DAC1, GPIOA, GPIO_PIN_4);
#endif

        // start the data output thread

        m_p_o_fuel_gauge_output = new app::FuelGaugeOutput(m_p_dac,
                get_dataset().get_fuel_output_lookup_table(),
                get_dataset().get_dac_out_amplifying_factor(),
                0);
        // start the data acquisition thread
        m_p_o_fuel_gauge_input = new app::FuelGaugeInputFromADC(m_p_adc,
                get_dataset().get_fuel_input_lookup_table());

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

        delete m_p_o_fuel_gauge_output;
        m_p_o_fuel_gauge_output = nullptr;

        delete m_p_adc;
        m_p_adc = nullptr;

        delete m_p_dac;
        m_p_dac = nullptr;

        return OSServices::ERROR_CODE_SUCCESS;
    }
}
