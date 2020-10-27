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

		virtual int32_t command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface) = 0;
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

		int32_t register_command(Command* p_command);
	private:
		void print_bootscreen() const;

		void process_input(const char* ai8_input_command);


		/// The input/output interface which this console is running on.
		std::shared_ptr<OSConsoleGenericIOInterface> m_po_io_interface;

		/// the executable commands that can be run in this console context
		Command* m_apo_commands[OS_CONSOLE_MAX_NUM_OF_COMMANDS];
		uint32_t m_u32_num_of_registered_commands;
	};

	std::vector<char> read_input_line(std::shared_ptr<OSConsoleGenericIOInterface> po_io_interface);
}


/** Allow comfortable c++ like stream outputs for C strings */
std::shared_ptr<OSServices::OSConsoleGenericIOInterface> operator<< (std::shared_ptr<OSServices::OSConsoleGenericIOInterface> po_console_io_interface, const char* pc_string);

std::shared_ptr<OSServices::OSConsoleGenericIOInterface> operator<< (std::shared_ptr<OSServices::OSConsoleGenericIOInterface> po_console_io_interface, int32_t i32_value);


/** This function takes a C string, and processes all the backspaces in it. */
size_t erase_backspaces(char* pc_string);

#endif
