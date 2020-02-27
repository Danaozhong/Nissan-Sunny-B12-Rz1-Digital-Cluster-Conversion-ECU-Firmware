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

        void display_usage(std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface);

        virtual int32_t command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface);
    };


    class CommandFuel : public OSServices::Command
    {
    public:
        CommandFuel() : Command("fuel") {}

        virtual ~CommandFuel() {}

        void display_usage(std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface);

        virtual int32_t command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface);
    };

    class CommandDataset : public OSServices::Command
    {
    public:
        CommandDataset() : Command("dataset") {}

        virtual ~CommandDataset() {};

        virtual int32_t command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface);
    };
}

#endif
