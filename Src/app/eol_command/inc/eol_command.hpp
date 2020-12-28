#ifndef _EOL_COMMAND_HPP_
#define _EOL_COMMAND_HPP_

#include "os_console.hpp"
#include "eol.hpp"
namespace app
{
    /**
     * This command is used to manipulate the speed signal configuration, e.g. set to manual mode, change conversion rate, etc.
     */
    class EOLCommand : public OSServices::Command
    {
    public:
        EOLCommand(app::EOLData& eol_data) : Command("eol"), m_o_eol_data(eol_data) {}

        virtual ~EOLCommand() {}

        void display_usage(OSServices::OSConsoleGenericIOInterface& p_o_io_interface);

        virtual int32_t command_main(const char** params, uint32_t u32_num_of_params, OSServices::OSConsoleGenericIOInterface& p_o_io_interface);
    private:
        app::EOLData& m_o_eol_data;
    };
}
#endif /* _EOL_COMMAND_HPP_ */
