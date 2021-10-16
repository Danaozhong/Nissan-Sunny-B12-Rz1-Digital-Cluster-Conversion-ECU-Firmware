#include "dataset.hpp"
#include "os_console.hpp"
namespace app
{
    const uint32_t Dataset::cu32_version_number = 101u;

    int32_t Dataset::write_dataset(midware::NonvolatileDataHandler &o_nonvolatile_data_handler)
    {
        const char ai8_section_name[] = "DATASET";
        if (false == o_nonvolatile_data_handler.section_exist(ai8_section_name)
                || o_nonvolatile_data_handler.get_section_size(ai8_section_name) < 300)
        {
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }

        // write everything into the buffer

        // start with the header
        std::vector<uint8_t> au8_data_buffer(4);
        au8_data_buffer[0] = 'D';
        au8_data_buffer[1] = 'A';
        au8_data_buffer[2] = 'T';
        au8_data_buffer[3] = 'A';
        o_nonvolatile_data_handler.write_section(ai8_section_name, 0, au8_data_buffer);

        // write the version information
        au8_data_buffer.resize(4);
        std::memcpy(au8_data_buffer.data(), &Dataset::cu32_version_number, 4);
        o_nonvolatile_data_handler.write_section(ai8_section_name, 4, au8_data_buffer);

        // write all the smaller data
        au8_data_buffer.resize(4);
        std::memcpy(au8_data_buffer.data(), &m_u32_input_pulses_per_kmph_mHz, 4);
        o_nonvolatile_data_handler.write_section(ai8_section_name, 32, au8_data_buffer);

        au8_data_buffer.resize(4);
        std::memcpy(au8_data_buffer.data(), &m_u32_output_pulses_per_kmph_mHz, 4);
        o_nonvolatile_data_handler.write_section(ai8_section_name, 36, au8_data_buffer);

        au8_data_buffer.resize(4);
        std::memcpy(au8_data_buffer.data(), &m_u32_dac_out_amplifying_factor, 4);
        o_nonvolatile_data_handler.write_section(ai8_section_name, 40, au8_data_buffer);


        auto lb_store_characteristic_curve = [&o_nonvolatile_data_handler, &ai8_section_name, &au8_data_buffer](const app::CharacteristicCurve<int32_t, int32_t> &o_curve, const uint32_t u32_position) -> int32_t
        {
            int32_t i32_ret_val = OSServices::ERROR_CODE_INTERNAL_ERROR;
            std::vector<uint8_t> au8_data_buffer;
            au8_data_buffer = app::CharacteristicCurveHelper::to_bytes(o_curve);

            if (au8_data_buffer.size() + sizeof(uint32_t) <= 100)
            {
                std::vector<uint8_t> au8_size_data(4);
                uint32_t u32_vector_size = static_cast<uint32_t>(au8_data_buffer.size());
                std::memcpy(au8_size_data.data(), &u32_vector_size, sizeof(uint32_t));
                o_nonvolatile_data_handler.write_section(ai8_section_name, u32_position, au8_size_data);
                o_nonvolatile_data_handler.write_section(ai8_section_name, u32_position + sizeof(uint32_t), au8_data_buffer);
                i32_ret_val = OSServices::ERROR_CODE_SUCCESS;
            }
            return i32_ret_val;
        };


        if (OSServices::ERROR_CODE_SUCCESS != lb_store_characteristic_curve(m_o_fuel_gauge_output_characteristic, 100))
        {
            // don't write the new dataset - it is too large.
            return OSServices::ERROR_CODE_NOT_ENOUGH_MEMORY;
        }
        if (OSServices::ERROR_CODE_SUCCESS != lb_store_characteristic_curve(m_o_fuel_gauge_input_characteristic, 200))
        {
            // don't write the new dataset - it is too large.
            return OSServices::ERROR_CODE_NOT_ENOUGH_MEMORY;
        }

        return OSServices::ERROR_CODE_SUCCESS;
    }


    int32_t Dataset::load_dataset(midware::NonvolatileDataHandler &o_nonvolatile_data_handler)
    {
        const char ai8_section_name[] = "DATASET";
        if (false == o_nonvolatile_data_handler.section_exist(ai8_section_name)
                || false == o_nonvolatile_data_handler.section_data_valid(ai8_section_name))
        {
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }

        std::vector<uint8_t> au8_buffer;
        int32_t i32_ret_val = o_nonvolatile_data_handler.read_section(ai8_section_name, au8_buffer);
        if (OSServices::ERROR_CODE_SUCCESS != i32_ret_val)
        {
            return i32_ret_val;
        }

        if (au8_buffer.size() < 300)
        {
            return OSServices::ERROR_CODE_NOT_ENOUGH_MEMORY;
        }

        // check if the header is there
        if (au8_buffer[0] != 'D'
            || au8_buffer[1] != 'A'
            || au8_buffer[2] != 'T'
            || au8_buffer[3] != 'A')
        {
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }

        // make sure the dataset has a version number which we can understand
        m_u32_read_dataset_version_no = 0u;
        std::memcpy(&m_u32_read_dataset_version_no,  au8_buffer.data() + 4, 4);
        if (m_u32_read_dataset_version_no > Dataset::cu32_version_number)
        {
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }

        std::memcpy(&m_u32_input_pulses_per_kmph_mHz,  au8_buffer.data() + 32, 4);
        std::memcpy(&m_u32_output_pulses_per_kmph_mHz, au8_buffer.data() + 36, 4);
        std::memcpy(&m_u32_dac_out_amplifying_factor,  au8_buffer.data() + 40, 4);

        auto lb_load_characteristic_curve = [&au8_buffer](const uint32_t u32_position, app::CharacteristicCurve<int32_t, int32_t> &o_curve) -> int32_t
        {
            int32_t i32_ret_val = OSServices::ERROR_CODE_INTERNAL_ERROR;

            if (u32_position + sizeof(uint32_t) < au8_buffer.size())
            {
                uint32_t u32_vector_size = 0u;
                std::memcpy(&u32_vector_size, au8_buffer.data() + u32_position, sizeof(uint32_t));
                if (u32_position + sizeof(uint32_t) + u32_vector_size < au8_buffer.size())
                {
                    std::vector<uint8_t>::const_iterator itr_start = au8_buffer.begin() + u32_position + sizeof(uint32_t);
                    std::vector<uint8_t>::const_iterator itr_end = au8_buffer.begin() + u32_position + sizeof(uint32_t) + u32_vector_size;
                    o_curve = app::CharacteristicCurveHelper::from_bytes<int32_t, int32_t>(itr_start, itr_end);
                    i32_ret_val = OSServices::ERROR_CODE_SUCCESS;
                }
            }
            return i32_ret_val;
        };

        if (OSServices::ERROR_CODE_SUCCESS != lb_load_characteristic_curve(100, m_o_fuel_gauge_output_characteristic))
        {
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }
        if (OSServices::ERROR_CODE_SUCCESS != lb_load_characteristic_curve(200, m_o_fuel_gauge_input_characteristic))
        {
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }

        return OSServices::ERROR_CODE_SUCCESS;

    }

    void Dataset::load_default_dataset()
    {
        /* Characteristics of the Nissan Sunny EUDM fuel sensor. 0% = 100Ohm (empty), 100% = 10Ohm (full). See
         * http://texelography.com/2019/06/21/nissan-rz1-digital-cluster-conversion/ for the full dataset */
        std::pair<int32_t, int32_t> a_input_lut[] =
        {
                /* x = Fuel level (% * 100), y = Resistor value in mOhm */
                std::make_pair(-1000, 120000), /* this is below empty */
                std::make_pair(-100, 87000),
                std::make_pair(500, 80600),
                std::make_pair(2500, 61800),
                std::make_pair(4800, 35700),
                std::make_pair(7700, 21000),
                std::make_pair(10000, 11800),
                std::make_pair(11000, 2400),
                std::make_pair(11500, 0000), /* fuel value supports more than 100% - if the tank is really filled to the maximum */
        };

        m_o_fuel_gauge_input_characteristic = app::CharacteristicCurve<int32_t, int32_t>(a_input_lut, sizeof(a_input_lut) / sizeof(a_input_lut[0]));

        /* Characteristics of the digital cluster fuel gauge
         * x = percentage, y = output voltage (mV) */
        std::pair<int32_t, int32_t> a_output_lut[] =
        {
                std::make_pair(-1000, 5000),
                std::make_pair(0, 5000),
                std::make_pair(100, 4800),
                std::make_pair(714, 4500),
                std::make_pair(2143, 4100),
                std::make_pair(4286, 3300),
                std::make_pair(6429, 2240),
                std::make_pair(9286, 1100),
                std::make_pair(10000, 700), /* display all bars */
                std::make_pair(11000, 700)
        };


        m_o_fuel_gauge_output_characteristic = app::CharacteristicCurve<int32_t, int32_t>(a_output_lut, sizeof(a_output_lut) / sizeof(a_output_lut[0]));

        // for speed conversion
        //m_u32_input_pulses_per_kmph_mHz = 4200u; // for testing direct feedback only (input pin connected to output pin)
        m_u32_input_pulses_per_kmph_mHz = 700u;
        m_u32_output_pulses_per_kmph_mHz = 2800u; // Use this value for the 9000rpm digital cluster
        //m_u32_output_pulses_per_kmph_mHz = 4200u; // Use this value for the 8000rpm digital cluster
        m_u32_dac_out_amplifying_factor = 2000u;

        m_u32_read_dataset_version_no = Dataset::cu32_version_number;
    }

    const app::CharacteristicCurve<int32_t, int32_t>& Dataset::get_fuel_input_lookup_table() const
    {
        return m_o_fuel_gauge_input_characteristic;
    }

    void Dataset::set_fuel_input_lookup_table(const app::CharacteristicCurve<int32_t, int32_t>& o_fuel_input_lookup_table)
    {
        m_o_fuel_gauge_input_characteristic = o_fuel_input_lookup_table;
    }

    const app::CharacteristicCurve<int32_t, int32_t>& Dataset::get_fuel_output_lookup_table() const
    {
        return m_o_fuel_gauge_output_characteristic;
    }

    void Dataset::set_fuel_output_lookup_table(const app::CharacteristicCurve<int32_t, int32_t>& o_fuel_output_lookup_table)
    {
        m_o_fuel_gauge_output_characteristic = o_fuel_output_lookup_table;
    }

    uint32_t Dataset::get_input_pulses_per_kmph_mHz() const
    {
        return m_u32_input_pulses_per_kmph_mHz;
    }

    void Dataset::set_input_pulses_per_kmph_mHz(uint32_t u32_pulses)
    {
        m_u32_input_pulses_per_kmph_mHz = u32_pulses;
    }

    uint32_t Dataset::get_output_pulses_per_kmph_mHz() const
    {
        return m_u32_output_pulses_per_kmph_mHz;
    }

    void Dataset::set_output_pulses_per_kmph_mHz(uint32_t u32_pulses)
    {
        m_u32_output_pulses_per_kmph_mHz = u32_pulses;
    }

    uint32_t Dataset::get_dac_out_amplifying_factor() const
    {
        return m_u32_dac_out_amplifying_factor;
    }

    void Dataset::set_dac_out_amplifying_factor(uint32_t u32_amplifying_factor)
    {
        m_u32_dac_out_amplifying_factor = u32_amplifying_factor;
    }

    uint32_t Dataset::get_read_dataset_version_no() const
    {
        return m_u32_read_dataset_version_no;
    }

    uint32_t Dataset::get_default_dataset_version_no() const
    {
        return Dataset::cu32_version_number;
    }
}
