#ifndef _CONSOLE_COMMANDS_HPP_
#define _CONSOLE_COMMANDS_HPP_


#include "os_console.hpp"

namespace app
{
    /**
     * This command is used to manipulate the speed signal configuration, e.g. set to manual mode, change conversion rate, etc.
     */
    class CommandSpeed : public OSServices::Command
    {
    public:
        CommandSpeed() : Command("speed") {}

        virtual ~CommandSpeed() {}

        void display_usage();

        virtual int32_t execute(const char** params, uint32_t u32_num_of_params, char* p_i8_output_buffer, uint32_t u32_buffer_size);

    private:

        uint32_t u32_get_output_buffer(char* &p_output_buffer);

        char* m_pi8_output_buffer;
        uint32_t m_u32_buffer_size;
    };

}

#endif
