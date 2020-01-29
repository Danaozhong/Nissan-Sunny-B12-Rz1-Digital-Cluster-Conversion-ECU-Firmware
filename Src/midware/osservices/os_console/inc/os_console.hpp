#ifndef _OS_CONSOLE_HPP_
#define _OS_CONSOLE_HPP_

/* Standard Library */
#include <memory>
//#include <vector>
#include <cstring>

/* FreeRTOS OS calls */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "generic_uart.hpp"

#define COMMAND_MAXIMUM_LENGTH (30u)
#define LINE_LENGTH  (100u)
#define OS_CONSOLE_MAX_NUM_OF_COMMANDS   (10u)
namespace OSServices
{
    const int32_t ERROR_CODE_SUCCESS = 0;
    const int32_t ERROR_CODE_NULLPTR = -1;
    const int32_t ERROR_CODE_NUM_OF_PARAMETERS = -2;
    const int32_t ERROR_CODE_PARAMETER_WRONG = -3;
    const int32_t ERROR_CODE_UNINITIALIZED = -4;
    const int32_t ERROR_CODE_INTERNAL_ERROR = -20;
    const int32_t ERROR_CODE_NOT_ENOUGH_MEMORY = -21;
    const int32_t ERROR_CODE_UNEXPECTED_VALUE = -22;


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

		virtual int32_t execute(const char** params, uint32_t u32_num_of_params, char* p_i8_output_buffer, uint32_t u32_buffer_size)
		{
			return 0;
		}
	protected:
		char m_ai8_command_str[COMMAND_MAXIMUM_LENGTH];

	};

	/** This command is used to list all tasks, their priority, stack size, name, state.
	 */
	class CommandListTasks : public Command
	{
	public:
		CommandListTasks() : Command("lt") {}

		virtual ~CommandListTasks() {}

		virtual int32_t execute(const char** params, uint32_t u32_num_of_params, char* p_i8_output_buffer, uint32_t u32_buffer_size);
	};

	/** This command prints out the heap memory size */
	class CommandMemory : public Command
	{
	public:
		CommandMemory() : Command("mem") {}

		virtual ~CommandMemory() {}

		virtual int32_t execute(const char** params, uint32_t u32_num_of_params, char* p_i8_output_buffer, uint32_t u32_buffer_size);
	};

	class OSConsole
	{
	public:
		OSConsole(drivers::GenericUART* po_io_interface);
		~OSConsole() {}

		void run();

		int32_t register_command(Command* p_command);
	private:
		void print_bootscreen() const;

		void process_input(const char* ai8_input_command);


		/// The input/output interface which this console is running on.
		drivers::GenericUART* m_po_io_interface;

		/// the executable commands that can be run in this console context
		Command* m_apo_commands[OS_CONSOLE_MAX_NUM_OF_COMMANDS];
		uint32_t m_u32_num_of_registered_commands;

		char m_ai8_command_buffer[LINE_LENGTH];

		bool m_bo_entering_command;
	};
}


/** This function takes a C string, and processes all the backspaces in it. */
size_t erase_backspaces(char* pc_string);

#endif
