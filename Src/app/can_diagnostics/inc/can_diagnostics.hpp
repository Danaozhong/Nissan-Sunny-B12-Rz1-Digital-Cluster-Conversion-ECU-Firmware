#ifndef _CAN_DIAGNOSTICS_HPP_
#define _CAN_DIAGNOSTICS_HPP_


#include "os_console.hpp"

namespace app
{
    /**
     * This command is used to manipulate the speed signal configuration, e.g. set to manual mode, change conversion rate, etc.
     */
    class CommandCAN : public OSServices::Command
    {
    public:
        CommandCAN() : Command("can") {}

        virtual ~CommandCAN() {}

        void display_can_status(std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface);
        void display_usage(std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface);

        virtual int32_t command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface);
    };
}

#endif
