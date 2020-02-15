#include "nonvolatile_data_handler.hpp"
#include "os_console.hpp"
#include "excp_Handler_if.h"
#include "util_algorithms.hpp" /* for CRC calculation */
#include "eeprom.h"

namespace
{
    /* Todo move this to configuration file */
    static uint32_t _eeprom_emulation_start __attribute__ ((section (".eeprom_emulation"))) __attribute__ ((__used__)) = 0xAABBCCDD; //0xABCDEF;
}
namespace midware
{
    NonvolatileDataHandler::NonvolatileDataHandler(uint16_t u32_num_of_flash_blocks, uint32_t u32_flash_block_size)
     : m_u16_num_of_flash_blocks(u32_num_of_flash_blocks), m_u32_flash_block_size(u32_flash_block_size)
    {
        // initialize the shadow with size for header + table
        m_au8_data_shadow.resize(cu32_header_size + cu32_block_information_size);
        //static_assert(cu32_header_size + cu32_block_information_size % 4 == 0);
    }

    int32_t NonvolatileDataHandler::register_data(const char* cac_name, uint32_t u32_size)
    {
        // make sure data is 4 - byte aligned
        if (u32_size % 4 != 0)
        {
            u32_size += (4 - u32_size % 4);
        }

        // make sure that everything still fits in the flash
        if (m_au8_data_shadow.size() + u32_size > m_u32_flash_block_size * m_u16_num_of_flash_blocks)
        {
            // new section will not fit into flash
            return OSServices::ERROR_CODE_NOT_ENOUGH_MEMORY;
        }


        // check if it also fits into the section table
        if ((m_flash_sections.size() + 1) * sizeof(FlashSection) > cu32_block_information_size)
        {
            // need to increase the block information size of the block (section) table.
            return OSServices::ERROR_CODE_NOT_ENOUGH_MEMORY;
        }


        // create the new flash section
        FlashSection o_new_section = { 0 };
        std::strncpy(o_new_section.m_ac_name, cac_name, 8);
        o_new_section.m_u32_size = u32_size;
        o_new_section.m_u32_position = m_au8_data_shadow.size();

        // check if the name already exists
        for (auto&& section : m_flash_sections)
        {
            if (std::strcmp(section.m_ac_name, o_new_section.m_ac_name) == 0)
            {
                return OSServices::ERROR_CODE_PARAMETER_WRONG;
            }
        }

        // all OK!
        m_au8_data_shadow.resize(m_au8_data_shadow.size() + u32_size);
        m_flash_sections.push_back(o_new_section);
        return OSServices::ERROR_CODE_SUCCESS;
    }


    /** Load all the registered data from nonvolatile memory */
    int32_t NonvolatileDataHandler::load()
    {
        int32_t i32_ret_val = try_to_load();

        // TODO check if there
        if (OSServices::ERROR_CODE_SUCCESS != i32_ret_val)
        {
            // restore empty default configuration
            uint32_t u32_minimum_buffer_size = cu32_block_information_size + cu32_header_size;
            m_flash_sections.clear();
            m_au8_data_shadow.resize(u32_minimum_buffer_size);

        }
        else
        {
            // TODO check if there were sections added or removed from the default ones, and if the sizes do match

        }
        return i32_ret_val;

    }
    int32_t NonvolatileDataHandler::try_to_load()
    {
        m_flash_sections.clear();

        m_au8_data_shadow.resize(m_u32_flash_block_size * m_u16_num_of_flash_blocks);
        for (uint16_t u16_block = 0; u16_block < m_u16_num_of_flash_blocks; u16_block++)
        {
            if (false == EE_Reads(0, m_u32_flash_block_size / 4, reinterpret_cast<uint32_t*>(m_au8_data_shadow.data() + u16_block * m_u32_flash_block_size))) /* _EEPROM_FLASH_PAGE_SIZE is missing here TODO */
            {
                return OSServices::ERROR_CODE_INTERNAL_ERROR;
            }
        }

        // check magic pattern
        if (m_au8_data_shadow[12] != 'N' ||
            m_au8_data_shadow[13] != 'O' ||
            m_au8_data_shadow[14] != 'N' ||
            m_au8_data_shadow[15] != 'V')
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        // check if the version info it OK
        uint32_t u32_version_information = 0u;
        std::memcpy(&u32_version_information, m_au8_data_shadow.data() + 16, 4);
        if (u32_version_information != 100u)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        // read how many sections we have
        uint32_t u32_num_of_sections = 0u;
        std::memcpy(&u32_num_of_sections, m_au8_data_shadow.data() + 20, 4);
        if (u32_num_of_sections * sizeof(FlashSection) > cu32_block_information_size)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        // read the total size
        uint32_t u32_data_size = 0u; // static_cast<uint32_t>(m_au8_data_shadow.size());
        std::memcpy(&u32_data_size, m_au8_data_shadow.data() + 24, 4);
        if (u32_data_size > m_au8_data_shadow.size() || u32_data_size < cu32_header_size + cu32_block_information_size)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        // make a checksum test
        uint32_t u32_total_checksum_from_buffer = 0u;
        std::memcpy(&u32_total_checksum_from_buffer, m_au8_data_shadow.data() + 4, 4);
        const uint32_t u32_total_checksum = midware::Algorithms::calculate_crc8(
                m_au8_data_shadow.data() + 8,
                m_au8_data_shadow.size() - 8u);

        if (u32_total_checksum_from_buffer != u32_total_checksum)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        // parse the block information, and make the buffer as small as possible
        uint32_t u32_minimum_buffer_size = cu32_block_information_size + cu32_header_size;
        std::vector<FlashSection> ao_flash_sections(u32_num_of_sections);
        for (uint16_t u16_section = 0; u16_section != u32_num_of_sections; ++u16_section)
        {
            FlashSection o_current_section;
            std::memcpy(&o_current_section,
                    m_au8_data_shadow.data() + cu32_header_size + u16_section*sizeof(FlashSection),
                    sizeof(FlashSection));
            // sanity check for data that might be too large
            if (o_current_section.m_u32_position + o_current_section.m_u32_size > m_au8_data_shadow.size())
            {
                return OSServices::ERROR_CODE_INTERNAL_ERROR;
            }

            // ignore empty section
            if (o_current_section.m_u32_size == 0)
            {
                continue;
            }
            ao_flash_sections.push_back(o_current_section);
            // remember the largest block of data.
            u32_minimum_buffer_size = std::max(u32_minimum_buffer_size, o_current_section.m_u32_position + o_current_section.m_u32_size);

        }

        // make sure the buffer size is aligned to 4 bytes
        while (u32_minimum_buffer_size % 4 != 0)
        {
            u32_minimum_buffer_size++;
        }

        // throw away everything that was excessively read
        m_au8_data_shadow.resize(u32_minimum_buffer_size);
        // only now, take over the flash sections
        m_flash_sections = ao_flash_sections;
        return OSServices::ERROR_CODE_SUCCESS;
    }

    /** */
    int32_t NonvolatileDataHandler::store()
    {
        // write the header information
        std::fill(m_au8_data_shadow.begin(), m_au8_data_shadow.begin() + cu32_header_size + cu32_block_information_size, 0u);

        // first 4 bytes are just a marker in memory, completely ignore them

        // second 4 bytes are used to accomodate the total checksum

        // some magic pattern
        m_au8_data_shadow[12] = 'N';
        m_au8_data_shadow[13] = 'O';
        m_au8_data_shadow[14] = 'N';
        m_au8_data_shadow[15] = 'V';

        // version_information
        const uint32_t u32_version_information = 100u;
        std::memcpy(m_au8_data_shadow.data() + 16, &u32_version_information, 4);

        // write how many sections we have
        const uint32_t u32_num_of_sections = static_cast<uint32_t>(m_flash_sections.size());
        std::memcpy(m_au8_data_shadow.data() + 20, &u32_num_of_sections, 4);

        // write the total size
        const uint32_t u32_data_size = static_cast<uint32_t>(m_au8_data_shadow.size());
        std::memcpy(m_au8_data_shadow.data() + 24, &u32_data_size, 4);

        // remaining bytes in the header remain zero (reserved)

        // write the flash section table
        uint32_t u32_buffer_offset = cu32_header_size;
        for (auto&& section : m_flash_sections)
        {
            std::memcpy(m_au8_data_shadow.data() + u32_buffer_offset, &section, sizeof(FlashSection));
            u32_buffer_offset += sizeof(FlashSection);

            // sanity check to make sure we don't overwrite data
            if (u32_buffer_offset + sizeof(FlashSection) > cu32_header_size + cu32_block_information_size)
            {
                /* cu32_block_information_size buffer is not large enough to allocate all
                sections, increase cu32_block_information_size */
                return OSServices::ERROR_CODE_NOT_ENOUGH_MEMORY;
            }
        }

        // data should already be there, nothing to do

        // write the checksum at the begin of the data
        const uint32_t u32_total_checksum = midware::Algorithms::calculate_crc8(
                m_au8_data_shadow.data() + 8,
                m_au8_data_shadow.size() - 8u);

        std::memcpy(m_au8_data_shadow.data() + 4, &u32_total_checksum, 4);


        // Step 2 - write everything into EEPROM
        uint16_t u16_num_of_blocks_to_be_written = static_cast<uint16_t>(m_au8_data_shadow.size() / m_u32_flash_block_size);
        if (0 != m_au8_data_shadow.size() % m_u32_flash_block_size)
        {
            u16_num_of_blocks_to_be_written++;
        }

        if (u16_num_of_blocks_to_be_written > m_u16_num_of_flash_blocks)
        {
            return OSServices::ERROR_CODE_NOT_ENOUGH_MEMORY;
        }

        uint32_t remaining_size_to_write = m_au8_data_shadow.size();
        uint32_t current_read_index = 0;
        for(uint16_t u16_block = 0; u16_block != u16_num_of_blocks_to_be_written; ++u16_block)
        {
            const uint32_t cu32_current_block_size = std::min(remaining_size_to_write, m_u32_flash_block_size);
            remaining_size_to_write =- cu32_current_block_size;

            // read the current block from EEPROM
            uint32_t au32_buffer[m_u32_flash_block_size / 4];
            /* make sure to always read sufficient data */
            uint32_t u32_block_size_in_words = cu32_current_block_size / 4;
            if (0 != cu32_current_block_size % 4)
            {
                u32_block_size_in_words++;
            }

           if (false == EE_Reads(0, u32_block_size_in_words, au32_buffer)) /* _EEPROM_FLASH_PAGE_SIZE is missing here TODO */
           {
               return OSServices::ERROR_CODE_INTERNAL_ERROR;
           }

           // make a comparison to check if the flash block really needs to be rewritten
           if (0 != std::memcmp(au32_buffer, m_au8_data_shadow.data() + current_read_index, cu32_current_block_size))
           {
               // data of this flash block is different to what we see, rewrite
               if (false == EE_ErasePage(u16_block))
               {
                   return OSServices::ERROR_CODE_INTERNAL_ERROR;
               }
               // write the block
               if (false == EE_Writes(0, cu32_current_block_size / 4, reinterpret_cast<uint32_t*>(m_au8_data_shadow.data() + current_read_index)))
               {
                   return OSServices::ERROR_CODE_INTERNAL_ERROR;
               }
           }
            current_read_index += cu32_current_block_size;
        }
        return OSServices::ERROR_CODE_SUCCESS;
    }

    int32_t NonvolatileDataHandler::read_section(const char *ac_name, std::vector<uint8_t>& au8_output_buffer) const
    {
        auto itr = find_section(ac_name);
        if (itr == m_flash_sections.end())
        {
            return OSServices::ERROR_CODE_PARAMETER_WRONG;
        }

        auto begin_buffer_itr = m_au8_data_shadow.begin()
                        + itr->m_u32_position;
        auto end_buffer_itr = begin_buffer_itr + itr->m_u32_size;

        au8_output_buffer = std::vector<uint8_t>(begin_buffer_itr, end_buffer_itr);
        return OSServices::ERROR_CODE_SUCCESS;
    }

    int32_t NonvolatileDataHandler::write_section(const char *cac_name, uint32_t u32_start, const std::vector<uint8_t> &au8_buffer)
    {
        auto itr = find_section(cac_name);

        // Does a section with this name exist?
        if (itr == m_flash_sections.end())
        {
            return OSServices::ERROR_CODE_PARAMETER_WRONG;
        }

        // Is the data we want to write larger than what is allowed?
        if (au8_buffer.size() + u32_start > itr->m_u32_size)
        {
            return OSServices::ERROR_CODE_PARAMETER_WRONG;
        }

        std::copy(au8_buffer.begin(), au8_buffer.end(), m_au8_data_shadow.begin() + itr->m_u32_position + u32_start);
        return OSServices::ERROR_CODE_SUCCESS;
    }

    bool NonvolatileDataHandler::section_exist(const char *cac_name) const
    {
        return (find_section(cac_name) != m_flash_sections.end());
    }
    std::vector<FlashSection>::const_iterator NonvolatileDataHandler::find_section(const char* ac_name) const
    {
        if (nullptr == ac_name)
        {
            return m_flash_sections.end();
        }
        for (auto itr = m_flash_sections.begin(); itr != m_flash_sections.end(); ++itr)
        {
            if (strcmp(itr->m_ac_name, ac_name) == 0)
            {
                return itr;
            }
        }
        return m_flash_sections.end();
    }
}
