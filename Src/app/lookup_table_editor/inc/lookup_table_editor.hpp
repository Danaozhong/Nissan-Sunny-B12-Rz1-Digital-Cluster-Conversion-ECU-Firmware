#ifndef _LOOKUP_TABLE_EDITOR_HPP_
#define _LOOKUP_TABLE_EDITOR_HPP_


#include "lookup_table.hpp"
#include "os_console.hpp"
#include <vector>

namespace app
{
    class LookupTableEditor : public OSServices::Command
    {
    public:
        LookupTableEditor() : Command("elut") {}

       virtual ~LookupTableEditor() {}

       //void display_usage(std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface);

       virtual int32_t command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface);
    private:
       void print_commands() const;

       void print_current_luk() const;

       void process_input();

       /** retrieves a certain set of characters as input, until the user presses enter */
       std::vector<char> collect_input(std::shared_ptr<OSServices::OSConsoleGenericIOInterface>) const;

       std::shared_ptr<OSServices::OSConsoleGenericIOInterface> m_p_o_io_interface;

       app::CharacteristicCurve<int32_t, int32_t>* m_loaded_lookup_table;

       bool m_bo_program_running;
    };
}

#endif
