#include "os_console.hpp"
#include "stm32fxxx.h"
namespace OSServices
{


int32_t CommandListTasks::command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface)
{
	if (nullptr == p_o_io_interface)
	{
		return -1;
	}

    const uint32_t u32_buffer_size = 1024;
    char p_i8_output_buffer[1024] = "";

	//char ai8_buffer[400] = { 0 };
#ifdef USE_FREERTOS_TASK_LIST
	vTaskList(p_i8_output_buffer);
#else


		UBaseType_t uxArraySize;
		char* pcWriteBuffer = &p_i8_output_buffer[0];

		/* Make sure the write buffer does not contain a string. */
		*pcWriteBuffer = ( char ) 0x00;

		/* Take a snapshot of the number of tasks in case it changes while this
		function is executing. */
		uxArraySize = uxTaskGetNumberOfTasks();;

		/* Allocate an array index for each task.  NOTE!  if
		configSUPPORT_DYNAMIC_ALLOCATION is set to 0 then pvPortMalloc() will
		equate to NULL. */
		TaskStatus_t pxTaskStatusArray[uxArraySize];

		/* Generate the (binary) data. */
		uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, NULL );

		/* Create a human readable table from the binary data. */

		/* Print the table header */
		snprintf(pcWriteBuffer, u32_buffer_size - strlen(p_i8_output_buffer) - 1,
				"Name        Status      Prio  Stack                    ID\r\n"
				"---------------------------------------------------------\r\n");
		pcWriteBuffer += strlen(pcWriteBuffer);

		for(uint32_t x = 0; x < uxArraySize; x++ )
		{
			char ai8_status_str[100] = "invalid";
			switch( pxTaskStatusArray[ x ].eCurrentState )
			{
				case eRunning:
					strcpy(ai8_status_str, "running");
					break;
				case eReady:
					strcpy(ai8_status_str, "ready");
					break;
				case eBlocked:
					strcpy(ai8_status_str, "blocked");
					break;
				case eSuspended:
					strcpy(ai8_status_str, "SUSPENDED");
					break;
				case eDeleted:
					strcpy(ai8_status_str, "deleted");
					break;
				case eInvalid:
				default:
					strcpy(ai8_status_str, "INVALID");
					break;
			}

			/* Write the task name to the string, padding with spaces so it
			can be printed in tabular form more easily. */
			//pcWriteBuffer = prvWriteNameToBuffer( pcWriteBuffer,  );
			uint32_t u32_stack_size = 0;
#ifdef configRECORD_STACK_HIGH_ADDRESS
			u32_stack_size = uxTaskGetStackSize(pxTaskStatusArray[x].xHandle);
#endif
			uint32_t u32_stack_usage = 100;
			if (0 != u32_stack_size)
			{
				u32_stack_usage = (pxTaskStatusArray[x].usStackHighWaterMark * 100) / u32_stack_size;
			}


			/* Write the rest of the string. */

			snprintf( pcWriteBuffer, u32_buffer_size - strlen(p_i8_output_buffer) - 1, "%-10s  %-10s  %-4u  %08x/%08x %3u%%  %-2u\r\n",
					pxTaskStatusArray[ x ].pcTaskName, ai8_status_str,
					static_cast<unsigned int>(pxTaskStatusArray[ x ].uxCurrentPriority),
					static_cast<unsigned int>(pxTaskStatusArray[ x ].usStackHighWaterMark),
					static_cast<unsigned int>(u32_stack_size),
					static_cast<unsigned int>(u32_stack_usage),
					( unsigned int ) pxTaskStatusArray[ x ].xTaskNumber ); /*lint !e586 sprintf() allowed as this is compiled with many compilers and this is a utility function only - not part of the core kernel implementation. */
			pcWriteBuffer += strlen( pcWriteBuffer ); /*lint !e9016 Pointer arithmetic ok on char pointers especially as in this case where it best denotes the intent of the code. */
		}
#endif

        p_o_io_interface << p_i8_output_buffer;

		return 0;
	}

    int32_t CommandMemory::command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface)
    {
		int i_free_heap = xPortGetFreeHeapSize();
	    int i_min_heap = xPortGetMinimumEverFreeHeapSize();

	    char ac_output_buffer[1024] = "";

	    snprintf(ac_output_buffer, 1024,
	    		"Current heap size: %u bytes\r\n"
	    		"Minimum ever heap size: %u bytes\r\n",
				i_free_heap, i_min_heap
	    );

	    p_o_io_interface << ac_output_buffer;



#if 0

		typedef struct FREE FREE;

		struct FREE {                          // Heap free block list structure:
		   uint32_t sulSize;                   //    Free block size
		   FREE *spsNext;                      //    Pointer to the next free block
		};

		extern FREE __data_Aldata;             // Heap information structure
		/* sample code to walk the heap */
		   FREE *xpsFree;                      // Free heap block
		   long *xplStack;                     // Stack pointer
		   long xlRomSize;                     // ROM size
		   long xlRamSize;                     // RAM size
		   long xlMemory;
		   long xlHeapTotal;
		   long xlHeapFree;
		   long xlHeapUsed;

		// The heap is walked to find the amount of free memory available.

		   xlHeapFree = 0;
		   xpsFree = &__data_Aldata;
		   while (xpsFree->spsNext) {
		      xpsFree = xpsFree->spsNext;
		      xlHeapFree += xpsFree->sulSize;
		   }
		   xlHeapTotal = (long) __sfe ("HEAP") – (long) __sfb ("HEAP");
		   xlHeapUsed = xlHeapTotal – xlHeapFree;

		// Header

		   printf ("Heap             | Free stack         | Free memoryrn");
		   xlRomSize = (long) __sfb ("ROM0_END") – (long) __sfb ("ROM0_START") + 1;
		   xlRamSize = (long) __sfb ("RAM_END") – (long) __sfb ("RAM_START") + 1;

		// Used heap, FIQ stack, and free flash

		   for (xplStack = (long *) &Stack_FIQ;
		         xplStack < (long *) &StackTop_FIQ && *xplStack == RAM_TEST;
		         ++xplStack);
		   xlMemory = (long) __sfb ("ROM0_END") – (long) __sfb ("ROM0_TOP") + 1;
		   printf ("   Used:  %5d  |    FIQ: %5d      |    Flash:    %6d (%d%%)rn",
		         xlHeapUsed, 4 * (xplStack – (long *) &Stack_FIQ), xlMemory,
		         100 * xlMemory / xlRomSize);

		// Free heap, IRQ stack, and free RAM

		   for (xplStack = (long *) &Stack_IRQ;
		         xplStack < (long *) &StackTop_IRQ && *xplStack == RAM_TEST;
		         ++xplStack);
		   xlMemory = (int) __sfe ("RAM_END") – (int) __sfb ("RAM_TOP") + 1;
		   printf ("   Free:  %5d  |    IRQ: %5d      |    RAM:      %6d (%d%%)rn",
		         xlHeapFree, 4 * (xplStack – (long *) &Stack_IRQ), xlMemory,
		         100 * xlMemory / xlRamSize);

		// Total heap, SVC stack, and free heap + RAM

		   for (xplStack = (long *) &Stack_SVC;
		         xplStack < (long *) &StackTop_SVC && *xplStack == RAM_TEST;
		         ++xplStack);
		   printf ("   TOTAL: %5d  |    SVC: %5d      |    Heap+RAM: %6d (%d%%)rn",
		         xlHeapTotal, 4 * (xplStack – (long *) &Stack_SVC),
		         xlMemory + xlHeapFree, 100 * (xlMemory + xlHeapFree) / xlRamSize);
		}
#endif
		return 0;
	}

    int32_t CommandReset::command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface)
    {
        NVIC_SystemReset();
        return OSServices::ERROR_CODE_SUCCESS;
    }

    OSConsoleUartIOInterface::OSConsoleUartIOInterface(drivers::GenericUART* po_io_interface)
        : m_po_io_interface(po_io_interface)
    {
    }

    void OSConsoleUartIOInterface::write_data(const char* pc_data, size_t s_data_size)
    {
        if (nullptr != m_po_io_interface)
        {
            m_po_io_interface->write(reinterpret_cast<const uint8_t*>(pc_data), s_data_size);
        }
    }

    int OSConsoleUartIOInterface::available() const
    {
        return m_po_io_interface->available();
    }

    int OSConsoleUartIOInterface::read()
    {
        return m_po_io_interface->read();
    }

    int32_t OSConsoleUartIOInterface::wait_for_input_to_be_available(const std::chrono::milliseconds& waiting_time)
    {
        if (m_po_io_interface->available() > 0)
        {
            // data already available
            return 0;
        }
        std::mutex mtx;
        std::unique_lock<std::mutex> lck(mtx);
        if (std::cv_status::timeout == m_po_io_interface->m_cv_input_available.wait_for(lck, waiting_time))
        {
            return -1;
        }
        return 0;
    }


    OSConsole::OSConsole(std::shared_ptr<OSConsoleGenericIOInterface> po_io_interface)
	: m_po_io_interface(po_io_interface), m_u32_num_of_registered_commands(0u)
	{
		this->register_command(new CommandListTasks());
		this->register_command(new CommandMemory());
		this->register_command(new CommandReset());
		print_bootscreen();
	}

	void OSConsole::process_input(const char* ai8_input_command)
	{
		bool bo_program_executed = false;

		// split the string by spaces, and only check for the first command part
		char ai8_command_copy[COMMAND_MAXIMUM_LENGTH] = { 0 };
		const char delimiter[] = " ";

		// first copy it so that we can run strtok() on it.
		strncpy(ai8_command_copy, ai8_input_command, COMMAND_MAXIMUM_LENGTH);

		// process all backspace commands, if the user has mistyped something
		erase_backspaces(ai8_command_copy);

        const char* api8_delimiters[100]  = { nullptr };
        uint32_t u32_num_of_delimiters = 0u;
		// then search for all spaces and split the lines
		char* p_i8_token = strtok(ai8_command_copy, delimiter);
		while (nullptr != p_i8_token)
		{
		    api8_delimiters[u32_num_of_delimiters] = p_i8_token;
		    ++u32_num_of_delimiters;
		    p_i8_token = strtok(nullptr, delimiter);
		}

		const char* pi8_command = api8_delimiters[0];
		if (nullptr != pi8_command)
		{
            for (uint32_t u32_i = 0; u32_i < m_u32_num_of_registered_commands; ++u32_i)
            {
                auto p_command = this->m_apo_commands[u32_i];

                if (strcmp(pi8_command, p_command->get_command()) == 0)
                {
                    /* u32_num_of_delimiters -1 because the first delimiter is the command,
                     * which does not count as a delimiter */
                    int32_t i32_return_code = p_command->command_main(api8_delimiters + 1,
                            u32_num_of_delimiters - 1, m_po_io_interface);

                    // and print the return code.
                    char ai8_print_str[LINE_LENGTH] = { 0 };
                    snprintf(ai8_print_str, LINE_LENGTH - 1, "\r\nProgram \'%s\' has terminated with return code %i.\r\n", \
                            p_command->get_command(), static_cast<int>(i32_return_code));
                    m_po_io_interface->write_data(ai8_print_str, strlen(ai8_print_str));

                    bo_program_executed = true;
                }
            }
		}
		if (false == bo_program_executed)
		{
			if (strlen(ai8_input_command) > 0)
			{
				char ai8_print_str[LINE_LENGTH] = { 0 };
				snprintf(ai8_print_str, LINE_LENGTH - 1, "No command found with name \'%s\'.\r\n", ai8_command_copy);
				m_po_io_interface->write_data(ai8_print_str, strlen(ai8_print_str));
			}
		}
	}

	void OSConsole::run()
	{
		if (this->m_po_io_interface != nullptr)
		{
			int i_read_char = 0;
			if(this->m_po_io_interface->available() > 0)
			{
			    // show the prompt
			    m_po_io_interface << "\r\n\r\nFreeRTOS> ";
			    std::vector<char> ai8_input = read_input_line(m_po_io_interface);

                // command finished, new line
                m_po_io_interface << "\r\n\r\n";

                // command end, process
                this->process_input(ai8_input.data());
			}
		}
	}

	int32_t OSConsole::register_command(Command* p_command)
	{
		if (nullptr == p_command || this->m_u32_num_of_registered_commands + 1 >= OS_CONSOLE_MAX_NUM_OF_COMMANDS)
		{
			return -1;
		}

		this->m_apo_commands[this->m_u32_num_of_registered_commands] = p_command;
		this->m_u32_num_of_registered_commands++;
		return 0;
	}


	void OSConsole::print_bootscreen() const
	{
		char ai8_bootscreen[] = "FreeRTOS Platform V0.1\n\r(c) 2019 \n\r";
		m_po_io_interface << ai8_bootscreen;
	}

	std::vector<char> read_input_line(std::shared_ptr<OSConsoleGenericIOInterface> po_io_interface)
    {
	    std::vector<char> ai8_output = {'\0'};
        char input = '\0';

	    while ('\r' != input)
        {
	        const size_t s_previous_size = strlen(ai8_output.data());

            po_io_interface->wait_for_input_to_be_available(std::chrono::milliseconds(100000));
            input = static_cast<char>(po_io_interface->read());
            if (input != '\r')
            {
                *ai8_output.rbegin() = input; // insert at second last position to keep the 0 termination
                ai8_output.push_back('\0');

                // filter backspaces, if needed
                erase_backspaces(ai8_output.data());
                ai8_output.resize(strlen(ai8_output.data()) + 1);

                if ('\b' != input || s_previous_size > 0) // only write if we don't delete already existing things
                {
                    if ('\b' == input)
                    {
                        char ai8_delete_last_char[] = "\b \b"; // go back one char, space, and back again.
                        po_io_interface->write_data(ai8_delete_last_char, 3);
                    }
                    else
                    {
                        // print out the character to the serial (like in a terminal)
                        po_io_interface->write_data(&input, 1);
                    }
                }
            }
        }
        return ai8_output;
    }
}


//OSConsoleGenericIOInterface* operator<< (OSConsoleGenericIOInterface* po_console_io_interface, char* c_string)
std::shared_ptr<OSServices::OSConsoleGenericIOInterface> operator<< (std::shared_ptr<OSServices::OSConsoleGenericIOInterface> po_console_io_interface, char* pc_string)
{
    if (nullptr != po_console_io_interface)
    {
        po_console_io_interface->write_data(pc_string, strlen(pc_string));
    }
    return po_console_io_interface;
}

std::shared_ptr<OSServices::OSConsoleGenericIOInterface> operator<< (std::shared_ptr<OSServices::OSConsoleGenericIOInterface> po_console_io_interface, int32_t i32_value)
{
    char ac_buffer[64] = "";
    snprintf(ac_buffer, 64, "%i", static_cast<int>(i32_value));
    po_console_io_interface << ac_buffer;
    return po_console_io_interface;

}

size_t erase_backspaces(char* pc_string)
{
    const size_t string_len = strlen(pc_string);
    char* write_ptr = pc_string;
    char* read_ptr = pc_string; // how many backspaces have already been erased

    while (read_ptr != pc_string + string_len) // check if end of string reached
    {
        if (*read_ptr == '\b')
        {
            // when reading a backspace, ignore this character and jump to the one after it
            read_ptr++;
            if (write_ptr != pc_string) // only erase a character if this is not start of string
            {
                // remove one character (will be overwritten in the next loop)
                write_ptr--;
            }
            // execute while condition statement, to make sure we don't overrun the buffer
            continue;
        }
        *write_ptr = *read_ptr;
        write_ptr++;
        read_ptr++;
    }
    // append a null terminator
    *write_ptr = '\0';
    return read_ptr - write_ptr;
}
