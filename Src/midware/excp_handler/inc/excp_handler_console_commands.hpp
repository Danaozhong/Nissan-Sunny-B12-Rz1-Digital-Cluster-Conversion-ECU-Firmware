#ifndef _EXCP_HANDLER_CONSOLE_COMMANDS_HPP_
#define _EXCP_HANDLER_CONSOLE_COMMANDS_HPP_


#include "os_console.hpp"

namespace midware
{
    /**
     * This command is used to manipulate the speed signal configuration, e.g. set to manual mode, change conversion rate, etc.
     */
    class CommandListExceptions : public OSServices::Command
    {
    public:
        CommandListExceptions() : OSServices::Command("excp") {}

        virtual ~CommandListExceptions() {}

        virtual int32_t execute(const char** params, uint32_t u32_num_of_params, char* p_i8_output_buffer, uint32_t u32_buffer_size);

        void display_usage() const;
    };

}



#endif
