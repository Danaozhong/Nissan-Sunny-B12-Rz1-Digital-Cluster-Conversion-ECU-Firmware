#include "main_application.hpp"
#include "console_commands.hpp"
#ifdef USE_CAN
#include "Can.h"
#include "CanIf.h"

#include "can_diagnostics.hpp"
#endif

#include "excp_handler.hpp"
#include "excp_handler_console_commands.hpp"
#include "eol_command.hpp"
#include "trace_if.h"

#define MAIN_APPLICATION_MEASURE_STARTUP_TIME

#ifdef MAIN_APPLICATION_MEASURE_STARTUP_TIME
#include "libtable.h"
#endif /* MAIN_APPLICATION_MEASURE_STARTUP_TIME */

namespace app
{

	MainApplication::MainApplication()
	    : m_en_fuel_gauge_output_mode(FUEL_GAUGE_OUTPUT_MODE_CONVERSION),
	      m_i32_fuel_gauge_output_manual_value(0)
	{}

#ifdef MAIN_APPLICATION_MEASURE_STARTUP_TIME
	enum MainApplicationStartupTimePosition
	{
	    StartupTimeBegin = 0,
	    StartupTimeUARTInitComplete,
	    StartupTimeConsoleInitComplete,
	    StartupTimeCANInitComplete,
	    StartupTimeTraceInitComplete,
	    StartupTimeNVDHInitComplete,
	    StartupTimeExcpHandlerInitComplete,
	    StartupTimeDatasetInitComplete,
	    StartupTimeApplInitComplete,
	    StartupTimeStartupCompleted,
	    StartupTimeNumOfElements
	};


	const char* get_timestamp_name(MainApplicationStartupTimePosition position)
	{
	    switch (position)
	    {
	    case StartupTimeUARTInitComplete:
	        return "UART initalized";
        case StartupTimeConsoleInitComplete:
            return "console initialized";
        case StartupTimeCANInitComplete:
            return "CAN initialized";
        case StartupTimeTraceInitComplete:
            return "Trace initialized";
        case StartupTimeNVDHInitComplete:
            return "NVDH initialized";
        case StartupTimeExcpHandlerInitComplete:
            return "excp handler initialized";
        case StartupTimeDatasetInitComplete:
            return "dataset loaded";
        case StartupTimeApplInitComplete:
            return "application initialized";
        case StartupTimeStartupCompleted:
            return "startup finished";

	    default:
	        return "unknown";
	    }
	}
#endif /* MAIN_APPLICATION_MEASURE_STARTUP_TIME */

	void MainApplication::startup_from_reset()
	{
#ifdef MAIN_APPLICATION_MEASURE_STARTUP_TIME
	    uint32_t au32_startup_times[StartupTimeNumOfElements] = { 0 };
	    au32_startup_times[StartupTimeBegin] = std_ex::get_timestamp_in_ms();
#endif /* MAIN_APPLICATION_MEASURE_STARTUP_TIME */
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
	    m_p_uart->connect(115200, drivers::UART_WORD_LENGTH_8BIT, drivers::UART_STOP_BITS_1, drivers::UART_FLOW_CONTROL_NONE);
#ifdef MAIN_APPLICATION_MEASURE_STARTUP_TIME
	    au32_startup_times[StartupTimeUARTInitComplete] = std_ex::get_timestamp_in_ms();
#endif /* MAIN_APPLICATION_MEASURE_STARTUP_TIME */
	    // Create the debug console
	    m_po_os_io_interface = new OSServices::OSConsoleUartIOInterface(m_p_uart);
	    m_po_os_console = new OSServices::OSConsole(*m_po_os_io_interface);
#ifdef MAIN_APPLICATION_MEASURE_STARTUP_TIME
	    au32_startup_times[StartupTimeConsoleInitComplete] = std_ex::get_timestamp_in_ms();
#endif /* MAIN_APPLICATION_MEASURE_STARTUP_TIME */

#ifdef USE_CAN
        /* Init the CAN interface (AUTOSAR conform) */
        Can_Init(&CanConfigData);
        CanIf_Init(&CanIf_Config);

        CanIf_SetControllerMode(0, CAN_CS_STARTED);
        CanIf_SetPduMode(0, CANIF_ONLINE);

        /* Register CAN diagnostics commands on the UART */
        m_po_os_console->register_command(new app::CommandCAN());
#ifdef MAIN_APPLICATION_MEASURE_STARTUP_TIME
        au32_startup_times[StartupTimeCANInitComplete] = std_ex::get_timestamp_in_ms();
#endif /* MAIN_APPLICATION_MEASURE_STARTUP_TIME */
#endif


#ifdef USE_TRACE
	    // Initialize the trace module
	    m_po_trace = new midware::Trace();
        m_po_trace->init();
	    // Enable trace logging via the OS console
	    m_po_trace->add_trace_io_interface(m_po_os_console);

	    // and set the trace module as the default system tracer
	    m_po_trace->set_as_default_trace();

#ifdef MAIN_APPLICATION_MEASURE_STARTUP_TIME
	    au32_startup_times[StartupTimeTraceInitComplete] = std_ex::get_timestamp_in_ms();
#endif /* MAIN_APPLICATION_MEASURE_STARTUP_TIME */
#endif

        m_po_exception_handler = new midware::ExceptionHandler();
        m_po_exception_handler->set_as_default_exception_handler();

#ifdef USE_NVDH
        // initialize non-volatile memory (uses 1 block of size 1024) TODO move config to CMake
        m_po_nonvolatile_data_handler = std::make_shared<midware::NonvolatileDataHandler>(1u, 1024u);

        // Configure the default sections for the module. Size must be < total size (header
        std::vector<midware::FlashSection> default_flash_sections =
        {
            midware::FlashSection{"EOL", 64u},
            midware::FlashSection{"EXCP", 300u},
            midware::FlashSection{"DATASET", 300u}
        };

        if (OSServices::ERROR_CODE_SUCCESS != m_po_nonvolatile_data_handler->set_default_sections(default_flash_sections))
        {
            ExceptionHandler_handle_exception(EXCP_MODULE_NONVOLATILE_DATA, EXCP_TYPE_NONVOLATILE_DATA_SETTING_DEFAULT_SECTIONS_FAILED, true, __FILE__, __LINE__, 0u);
        }

        // afterwards, have a look what is in memory and read it
        if (OSServices::ERROR_CODE_SUCCESS != m_po_nonvolatile_data_handler->load())
        {
            ExceptionHandler_handle_exception(EXCP_MODULE_NONVOLATILE_DATA, EXCP_TYPE_NONVOLATILE_DATA_LOADING_FAILED, false, __FILE__, __LINE__, 0u);
        }

        m_po_exception_handler->set_nonvolatile_data_handler(m_po_nonvolatile_data_handler, "EXCP");

#ifdef MAIN_APPLICATION_MEASURE_STARTUP_TIME
        au32_startup_times[StartupTimeNVDHInitComplete] = std_ex::get_timestamp_in_ms();
#endif /* MAIN_APPLICATION_MEASURE_STARTUP_TIME */
#endif /* USE_NVDH */

        // Initialize the exception storage module, to be able to log / debug exceptions
        m_po_exception_handler->init();

#ifdef MAIN_APPLICATION_MEASURE_STARTUP_TIME
        au32_startup_times[StartupTimeExcpHandlerInitComplete] = std_ex::get_timestamp_in_ms();
#endif /* MAIN_APPLICATION_MEASURE_STARTUP_TIME */

	    // register the command to debug the exception handler on the os console
	    m_po_os_console->register_command(new midware::CommandListExceptions());


        m_o_eol_data.set_nonvolatile_data_handler(m_po_nonvolatile_data_handler, "EOL");
        if (OSServices::ERROR_CODE_SUCCESS != m_o_eol_data.eol_init())
        {
            ExceptionHandler_handle_exception(EXCP_MODULE_EOL, EXCP_TYPE_NONVOLATILE_DATA_LOADING_FAILED, false, __FILE__, __LINE__, 0u);
        }

#ifdef USE_NVDH
	    // read the dataset
	    if (OSServices::ERROR_CODE_SUCCESS != m_o_dataset.load_dataset(*m_po_nonvolatile_data_handler))
	    {
	        ExceptionHandler_handle_exception(EXCP_MODULE_DATASET, EXCP_TYPE_DATASET_LOADING_FAILED, false, __FILE__, __LINE__, 0u);
	        // load default dataset
	        m_o_dataset.load_default_dataset();

	        // overwrite corrupted dataset in flash
	        m_o_dataset.write_dataset(*m_po_nonvolatile_data_handler);
	    }

#else
	    // load default dataset
        m_o_dataset.load_default_dataset();
#endif

#ifdef MAIN_APPLICATION_MEASURE_STARTUP_TIME
        au32_startup_times[StartupTimeDatasetInitComplete] = std_ex::get_timestamp_in_ms();
#endif /* MAIN_APPLICATION_MEASURE_STARTUP_TIME */

        // initialization below is for the application
        init_fuel_level_converter();
        init_speed_converter();

#ifdef MAIN_APPLICATION_MEASURE_STARTUP_TIME
        au32_startup_times[StartupTimeApplInitComplete] = std_ex::get_timestamp_in_ms();
#endif /* MAIN_APPLICATION_MEASURE_STARTUP_TIME */

        // print further bootscreen info (SW version, EOL data, etc.)
        auto p_o_command_version = new app::CommandVersion();
        this->m_po_os_console->register_command(p_o_command_version);
        p_o_command_version->command_main(nullptr, 0, this->get_stdio());

        // register the debug commands in the os console, so that debugging of the speed signals is possible.
        this->m_po_os_console->register_command(new app::LookupTableEditor());
        this->m_po_os_console->register_command(new app::CommandSpeed());
        this->m_po_os_console->register_command(new app::CommandFuel());
        this->m_po_os_console->register_command(new app::CommandDataset());
        this->m_po_os_console->register_command(new app::EOLCommand(get_eol_data()));
        this->m_po_os_console->register_command(new app::CommandTrace());

#ifdef MAIN_APPLICATION_MEASURE_STARTUP_TIME
        au32_startup_times[StartupTimeStartupCompleted] = std_ex::get_timestamp_in_ms();

        /* Print out the startup times on the UART interface */

        tst_lib_table table;
        i32_lib_table_initialize_table(&table);

        i32_lib_table_add_row(&table, 2, "Function", "Time");

        for (uint32_t i = StartupTimeUARTInitComplete; i < StartupTimeNumOfElements; ++i)
        {
            char timestamp[20] = "";
            const unsigned int current_delta_in_ms = static_cast<unsigned int>(au32_startup_times[i] - au32_startup_times[StartupTimeBegin]);

            snprintf(timestamp, 20, "%u ms", current_delta_in_ms);
            i32_lib_table_add_row(&table, 2, get_timestamp_name(static_cast<MainApplicationStartupTimePosition>(i)), timestamp);
        }

        // give the system some time to print out the current trace buffer
        std_ex::sleep_for(std::chrono::milliseconds(100));

        {
            char buffer[128];
            int32_t i32_ret_val = 1;
            uint32_t u32_offet = 0u;
            while(1 == i32_ret_val)
            {

                i32_ret_val = i32_lib_table_draw_table(&table, buffer, 128, u32_offet);
                u32_offet += 127;
                DEBUG_PRINTF(buffer);

                // again, give the trace task some time to push the data to the UART
                std_ex::sleep_for(std::chrono::milliseconds(10));
            }
        }
#endif /* MAIN_APPLICATION_MEASURE_STARTUP_TIME */
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

	void MainApplication::cycle_10ms()
	{
        // check for debug input
	    auto p_console = get_os_console();
	    if (nullptr != p_console)
	    {
	        p_console->run();
	    }
	}

    void MainApplication::cycle_100ms()
    {
        if (nullptr != m_p_o_fuel_gauge_input)
        {
            m_p_o_fuel_gauge_input->cycle_100ms();
        }

        if (nullptr != m_po_speed_sensor_converter)
        {
            m_po_speed_sensor_converter->cycle();
        }

    }

    void MainApplication::cycle_1000ms()
    {
        // backup the shadow to flash every second
        get_nonvolatile_data_handler()->store();
    }



#ifdef USE_NVDH
        std::shared_ptr<midware::NonvolatileDataHandler> MainApplication::get_nonvolatile_data_handler() const
        {
            return m_po_nonvolatile_data_handler;
        }
#endif

    OSServices::OSConsole* MainApplication::get_os_console()
	{
	    return this->m_po_os_console;
	}

    OSServices::OSConsoleGenericIOInterface& MainApplication::get_stdio()
    {
        return get_os_console()->get_io_interface();
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

    app::EOLData& MainApplication::get_eol_data()
    {
        return m_o_eol_data;
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
            int32_t i32_ret_val = m_p_o_fuel_gauge_output->set_fuel_level(i32_output_value);
            if (0 != i32_ret_val)
            {
                ExceptionHandler_handle_exception(EXCP_MODULE_FUEL_SIGNAL_CONVERTER, EXCP_TYPE_FUEL_SIGNAL_CONVERTER_SET_OUTPUT_FAILED,
                        false, __FILE__, __LINE__, static_cast<uint32_t>(i32_ret_val));
            }
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
        m_p_pwm_ic = new drivers::STM32PWM_IC(TIM4, TIM_CHANNEL_1, TIM_CHANNEL_2, 1023u, 65536u); // Pins PA11 PA12 1023u prescaler?? TODO
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
        if (nullptr == m_p_o_fuel_gauge_input)
        {
            ExceptionHandler_handle_exception(EXCP_MODULE_FUEL_SIGNAL_CONVERTER, EXCP_TYPE_NULLPOINTER, false, __FILE__, __LINE__, 0u);
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }
        else
        {
            m_p_o_fuel_gauge_input->m_sig_fuel_level_changed.connect(event_handler);

            // trigger one reading cycle, to immediately display a fuel value after startup.
            m_p_o_fuel_gauge_input->cycle_100ms();
            return OSServices::ERROR_CODE_SUCCESS;
        }
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
