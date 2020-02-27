#ifndef _DATASET_HPP_
#define _DATASET_HPP_

#include "lookup_Table.hpp"
#include "nonvolatile_data_handler.hpp"

namespace app
{
    class Dataset
    {
    public:

        int32_t load_dataset(midware::NonvolatileDataHandler &po_nonvolatile_data_handler);

        void load_default_dataset();

        int32_t write_dataset(midware::NonvolatileDataHandler &po_nonvolatile_data_handler);

        const app::CharacteristicCurve<int32_t, int32_t>& get_fuel_input_lookup_table() const;

        void set_fuel_input_lookup_table(const app::CharacteristicCurve<int32_t, int32_t>& o_fuel_input_lookup_table);

        const app::CharacteristicCurve<int32_t, int32_t>& get_fuel_output_lookup_table() const;

        void set_fuel_output_lookup_table(const app::CharacteristicCurve<int32_t, int32_t>& o_fuel_output_lookup_table);

        uint32_t get_input_pulses_per_kmph_mHz() const;

        void set_input_pulses_per_kmph_mHz(uint32_t u32_pulses);

        uint32_t get_output_pulses_per_kmph_mHz() const;

        void set_output_pulses_per_kmph_mHz(uint32_t u32_pulses);

        uint32_t get_dac_out_amplifying_factor() const;

        void set_dac_out_amplifying_factor(uint32_t u32_amplifying_factor);
    private:
        static const uint32_t cu32_version_number = 100u;

        bool m_bo_initialized;
        app::CharacteristicCurve<int32_t, int32_t> m_o_fuel_gauge_input_characteristic;
        app::CharacteristicCurve<int32_t, int32_t> m_o_fuel_gauge_output_characteristic;
        uint32_t m_u32_input_pulses_per_kmph_mHz;
        uint32_t m_u32_output_pulses_per_kmph_mHz;

        uint32_t m_u32_dac_out_amplifying_factor;

    };
}
#endif /* _DATASET_HPP_ */
