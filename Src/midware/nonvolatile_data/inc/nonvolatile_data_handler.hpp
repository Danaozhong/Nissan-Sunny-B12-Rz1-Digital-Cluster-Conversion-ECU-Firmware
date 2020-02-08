#ifndef _NONVOLATILE_DATA_HANDLER_HPP_
#define _NONVOLATILE_DATA_HANDLER_HPP_

#include <vector>
#include <memory>

#include "nonvolatile_data.hpp"

namespace midware
{
    struct FlashSection
    {
        char m_ac_name[8];
        uint32_t m_u32_size;
        uint16_t m_u32_position;
    };

    class NonvolatileDataHandler
    {
    public:
        NonvolatileDataHandler(uint16_t u32_num_of_flash_blocks, uint32_t u32_flash_block_size);

        int32_t register_data(const char* cac_name, uint32_t u32_size);

        /** Load all the registered data from nonvolatile memory */
        int32_t load();

        /** */
        int32_t store();


        int32_t read_section(const char *ac_name, std::vector<uint8_t>& au8_output_buffer) const;

        int32_t write_section(const char *cac_name, uint32_t u32_start, const std::vector<uint8_t> &au8_buffer);

        bool section_exist(const char *cac_name) const;
    private:
        const uint32_t cu32_header_size = 32u;

        ///
        const uint32_t cu32_block_information_size = 192u;
        std::vector<FlashSection>::const_iterator find_section(const char* ac_name) const;

        std::vector<std::shared_ptr<NonvolatileData>> m_ao_data_objects;

        /// the flash shadow
        std::vector<uint8_t> m_au8_data_shadow;

        /// the sections in flash
        std::vector<FlashSection> m_flash_sections;

        const uint16_t m_u16_num_of_flash_blocks;
        const uint32_t m_u32_flash_block_size;
    };
}
#endif
