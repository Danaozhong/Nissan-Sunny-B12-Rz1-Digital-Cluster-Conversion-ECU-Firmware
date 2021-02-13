#ifndef _FUEL_GAUGE_INPUT_HPP_
#define _FUEL_GAUGE_INPUT_HPP_

#include "generic_adc.hpp"
#include "lookup_table.hpp"
#include "event_handler.hpp"

#ifndef FUEL_GAUGE_INPUT_AVERAGING_SIZE
#error "FUEL_GAUGE_INPUT_AVERAGING_SIZE is not defined. verify correct Cmake generation."
#endif

#ifndef FUEL_GAUGE_INPUT_NUM_OF_AVERAGES
#error "FUEL_GAUGE_INPUT_NUM_OF_AVERAGES is not defined!"
#endif

namespace app
{
    /** Helper class to calculate the voltage / resistor values at a voltage divider */
    class VoltageDivider
    {
    public:
        VoltageDivider(uint32_t u32_resistor_1, int32_t i32_supply_voltage);

        uint32_t get_resistor_2_value(int32_t i32_resistor_2_voltage) const;
        int32_t get_resistor_2_voltage(uint32_t u32_resistor_2_value) const;

        /** Returns the supply voltage in mV */
        int32_t get_supply_voltage() const;
    private:
        uint32_t m_u32_resistor_1;
        int32_t m_i32_supply_voltage;
    };

    class ParallelVoltageDivider
    {
    public:
        ParallelVoltageDivider(uint32_t d_resistor_1, uint32_t d_resistor_2_parallel, int32_t d_supply_voltage);

        uint32_t get_resistor_2_value(int32_t d_resistor_2_voltage) const;

    private:
        VoltageDivider m_voltage_divider;
        uint32_t m_u32_resistor_1;
        uint32_t m_u32_resistor_2_parallel;
        int32_t m_i32_supply_voltage;
    };

    /** These are the states of the input fuel sensor state machine.
     * FuelGaugeInputSMStarting: The fuel sensor is initializing, and will make a first estimation on the fuel level
     * FuelGaugeInputSMNormalOperation: In this mode, only new fuel readings less than the current one will be accepted.
     * FuelGaugeInputSMRefillDetected: Triggered when the sensor value is suddenly higher than before, i.e. a refill
     * has been detected.
     * FuelGaugeInputSMErrorRecovery: Triggered when the sensor value is suddenly less than before. That would indicate
     * an error, and a reinitialization is triggered.
     */
    enum FuelGaugeInputSMState
    {
        FuelGaugeInputSMStarting,
        FuelGaugeInputSMNormalOperation,
        FuelGaugeInputSMRefillDetected,
        FuelGaugeInputSMErrorRecovery
    };

    class FuelGaugeInputFromADC
    {
    public:
        FuelGaugeInputFromADC(drivers::GenericADC* p_adc,
                const app::CharacteristicCurve<int32_t, int32_t>& o_fuel_input_characteristic);

        ~FuelGaugeInputFromADC();
        /// Signal triggered when a new value from the fuel sensor was retrieved
        boost::signals2::signal<int32_t> m_sig_fuel_level_changed;

        /** Every 100ms, a new fuel sensor value is read. It is also used at startup to find the average value for the fuel sensor */
        void cycle_100ms();

        int32_t get_fuel_sensor_value() const;

        int32_t get_current_averaged_fuel_sensor_value() const;

        int32_t get_current_raw_fuel_sensor_value() const;

        /** Returns the voltage read at the ADC */
        int32_t get_adc_voltage() const;

        /** Returns the calculated fuel resistor value */
        int32_t get_fuel_sensor_resistor_value() const;

    private:
        void set_fuel_sensor_value(int32_t i23_fuel_value);

        /// The ADC used to retrieve data
        drivers::GenericADC* m_p_adc;

        /// The actual last readings from the fuel sensor
        std::vector<int32_t> m_ai32_raw_last_read_fuel_percentages; /* in percent * 100 */

        /// The last averaged input values
        std::vector<std::pair<int32_t, int32_t>> m_ai32_averaged_fuel_readings_buffer; /* in percent * 100, min/max variation (%*100) */

        /// in which state the state machine is.
        FuelGaugeInputSMState m_en_state;

        uint32_t m_u32_input_unexpected_higher_error_counter;
        uint32_t m_u32_input_unexpected_lower_error_counter;
        uint32_t m_u32_input_slightly_lower_counter;

        int32_t m_i32_current_fuel_value;

        /// lookup table used to convert from the ADC value to a percentage
        const app::CharacteristicCurve<int32_t, int32_t>& m_o_fuel_input_characteristic;

        /// the voltage divider used at the hardware side on the ADC to measure the fuel sensor.
        VoltageDivider m_o_voltage_divider;

        /// the actual ADC voltage.
        int32_t m_i32_adc_pin_voltage;

        /// The resistor value of the fuel sensor.
        int32_t m_i32_fuel_sensor_resistor_value;
    };
}

#endif /* _FUEL_GAUGE_INPUT_HPP_ */
