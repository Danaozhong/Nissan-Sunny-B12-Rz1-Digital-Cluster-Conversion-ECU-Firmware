#ifndef _OS_CONSOLE_HPP_
#define _OS_CONSOLE_HPP_

/* Standard Library */
#include <memory>
#include <vector>
#include <chrono>
#include <cstring>

/* FreeRTOS OS calls */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "generic_uart.hpp"
#include "os_console.hpp"

#define COMMAND_MAXIMUM_LENGTH (30u)
#define LINE_LENGTH  (100u)
#define OS_CONSOLE_MAX_NUM_OF_COMMANDS   (20u)
namespace OSServices
{
    const int32_t ERROR_CODE_SUCCESS = 0;
    const int32_t ERROR_CODE_NULLPTR = -1;
    const int32_t ERROR_CODE_NUM_OF_PARAMETERS = -2;
    const int32_t ERROR_CODE_PARAMETER_WRONG = -3;
    const int32_t ERROR_CODE_UNINITIALIZED = -4;
    const int32_t ERROR_CODE_NOT_SUPPORTED = -5;
    const int32_t ERROR_CODE_INTERNAL_ERROR = -20;
    const int32_t ERROR_CODE_NOT_ENOUGH_MEMORY = -21;
    const int32_t ERROR_CODE_UNEXPECTED_VALUE = -22;

    /* forward declaration */
    class OSConsoleGenericIOInterface;

	class Command
	{
	public:
		Command(const char* ai8_command)
	    {
			strncpy(m_ai8_command_str, ai8_command, COMMAND_MAXIMUM_LENGTH - 1);
		}

		virtual ~Command() {}

		const char* get_command() const
		{
			return m_ai8_command_str;
		}

		/** Main processing function when a command is called.
		 * \param[in] params C string array of params. Each param is separated by a space.
		 * \param[in] u32_num_of_params Number of parameters.
		 * \param p_o_io_interface The IO interface from where to retrieve data (yes/no)
		 */
		virtual int32_t command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface) = 0;

		/**
		 * Prints the usage of this command. The default implementation will do nothing.
		 */
		virtual void print_usage(std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface) const {}
		char m_ai8_command_str[COMMAND_MAXIMUM_LENGTH];

	};

	/** This command is used to list all tasks, their priority, stack size, name, state.
	 */
	class CommandListTasks : public Command
	{
	public:
		CommandListTasks() : Command("lt") {}

		virtual ~CommandListTasks() {}

		virtual int32_t command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface);
	};

	/** This command prints out the heap memory size */
	class CommandMemory : public Command
	{
	public:
		CommandMemory() : Command("mem") {}

		virtual ~CommandMemory() {}

		virtual int32_t command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface);
	};

    class CommandReset : public Command
    {
    public:
	    CommandReset() : Command("reset") {}

        virtual ~CommandReset() {}

        virtual int32_t command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface);
    };

	/** Something like a generic IO stream object */
	class OSConsoleGenericIOInterface
	{
	public:
	    virtual ~OSConsoleGenericIOInterface() {}

	    virtual void write_data(const char* pc_data, size_t s_data_size) = 0;
	    virtual int available() const = 0;
	    virtual int read() = 0;

        virtual int32_t wait_for_input_to_be_available(const std::chrono::milliseconds& waiting_time) = 0;
	};

	class OSConsoleUartIOInterface : public OSConsoleGenericIOInterface
	{
	public:
	    OSConsoleUartIOInterface(drivers::GenericUART* po_io_interface);
	    virtual ~OSConsoleUartIOInterface() {}

	    virtual void write_data(const char* pc_data, size_t s_data_size);
        virtual int available() const;
        virtual int read();
        virtual int32_t wait_for_input_to_be_available(const std::chrono::milliseconds& waiting_time);
	private:
	    drivers::GenericUART* m_po_io_interface;
	};

	class OSConsole
	{
	public:
		OSConsole(std::shared_ptr<OSConsoleGenericIOInterface> po_io_interface);
		~OSConsole() {}

		void run();

		/** Adds a new command to the console. */
		int32_t register_command(Command* p_command);

		/** This will indicate to other users if the console is currently blocked (e.g. by entering a command).
		 * While this is true, other applications are not supposed to write anything on the UART, and instead
		 * should buffer their output. */
		bool console_blocked() const;

		/* returns the IO object */
		std::shared_ptr<OSConsoleGenericIOInterface> get_io_interface() const;
	private:
		void print_bootscreen() const;

		void process_input(const char* ai8_input_command);


		/// The input/output interface which this console is running on.
		std::shared_ptr<OSConsoleGenericIOInterface> m_po_io_interface;

		/// the executable commands that can be run in this console context
		Command* m_apo_commands[OS_CONSOLE_MAX_NUM_OF_COMMANDS];
		uint32_t m_u32_num_of_registered_commands;

		/// Flag to indicate whether the OS console currently requests full control over the UART
		bool m_bo_blocked;
	};

	int32_t read_bool_input(std::shared_ptr<OSConsoleGenericIOInterface> po_io_interface, bool &ret_val);
	int32_t read_timestamp(std::shared_ptr<OSConsoleGenericIOInterface> po_io_interface, std::time_t& timestamp);
	int32_t read_int32(std::shared_ptr<OSConsoleGenericIOInterface> po_io_interface, int32_t& value);
	std::vector<char> read_input_line(std::shared_ptr<OSConsoleGenericIOInterface> po_io_interface);
}


/** Allow comfortable c++ like stream outputs for C strings */
std::shared_ptr<OSServices::OSConsoleGenericIOInterface> operator<< (std::shared_ptr<OSServices::OSConsoleGenericIOInterface> po_console_io_interface, const char* pc_string);

std::shared_ptr<OSServices::OSConsoleGenericIOInterface> operator<< (std::shared_ptr<OSServices::OSConsoleGenericIOInterface> po_console_io_interface, int32_t i32_value);


/** This function takes a C string, and processes all the backspaces in it. */
size_t erase_backspaces(char* pc_string);

#endif
