#include "excp_handler_console_commands.hpp"
#include "excp_handler.hpp"
#include "os_console.hpp"

namespace midware
{

    void CommandListExceptions::display_usage(std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface) const
    {
        p_o_io_interface << "Wrong parameter.\r\n";
    }

    int32_t CommandListExceptions::execute(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface)
    {
        ExceptionHandler* p_exception_handler = ExceptionHandler::get_default_exception_handler();

        if (u32_num_of_params == 0 || nullptr == p_exception_handler)
        {
            // parameter error, no parameter provided
            display_usage(p_o_io_interface);
            return OSServices::ERROR_CODE_NUM_OF_PARAMETERS;
        }

        if (0 == strcmp(params[0], "list"))
        {
            // List all exceptions
            p_exception_handler->print(p_o_io_interface);
            //p_o_io_interface << ac_buffer;
            return OSServices::ERROR_CODE_SUCCESS;
        }
        else if (0 == strcmp(params[0], "clear"))
        {
            p_exception_handler->clear_exceptions();
            p_o_io_interface << "All exceptions in RAM cleared.\r\n";
            return OSServices::ERROR_CODE_SUCCESS;
        }
        else if (0 == strcmp(params[0], "write_flash"))
        {
            return p_exception_handler->store_into_data_flash();
        }
        else if (0 == strcmp(params[0], "read_flash"))
        {
            return p_exception_handler->read_from_data_flash();
        }

        return OSServices::ERROR_CODE_PARAMETER_WRONG;
    }
}

