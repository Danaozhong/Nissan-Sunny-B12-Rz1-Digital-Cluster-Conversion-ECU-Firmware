#ifndef _SPEED_SENSOR_CONVERTER_HPP_
#define _SPEED_SENSOR_CONVERTER_HPP_

#include "ex_thread.hpp"
#include "generic_pwm.hpp"
#include <atomic>

namespace app
{
    enum SpeedOutputMode
    {
        OUTPUT_MODE_CONVERSION,
        OUTPUT_MODE_MANUAL
    };

    class SpeedSensorConverter
    {
    public:
        SpeedSensorConverter(std::shared_ptr<drivers::GenericPWM> &p_output_pwm,
                uint32_t u32_input_pulses_per_kmph_mHz,
                uint32_t u32_output_pulses_per_kmph_mHz);

        ~SpeedSensorConverter();


        /** Use this to select the mode in which the speed signal is sent out to the cluster.
         * use OUTPUT_MODE_CONVERSION to derive the speed signal from the input speed sensor,
         * alternatively; use OUTPUT_MODE_MANUAL to manually configure a speed value. */
        void set_speed_output_mode(SpeedOutputMode en_speed_output_mode);

        /** When the speed sensor conversion is in manual mode, use this function to set the manual
         * speed value.
         * \param[in] i32_speed_in_mph  The velocity in meters / hour.
         */
        int32_t set_manual_speed(int32_t i32_speed_in_mph);

    private:
        /** one single data processing cycle. Called cyclically from speed_sensor_converter_main */
        void cycle();
        void speed_sensor_converter_main();

        std::shared_ptr<drivers::GenericPWM> m_p_output_pwm;

        std::unique_ptr<std_ex::thread> m_p_data_conversion_thread;

        std::atomic<SpeedOutputMode> m_en_current_speed_output_mode;

        /**
         * The manually set vehicle speed in meter/hour. Only valid if OUTPUT_MODE_MANUAL is set.
         */
        int32_t m_i32_manual_speed;

        int32_t m_i32_current_vehicle_speed_kmph;

        /** Unit is mili Hertz mHz */
        const uint32_t m_u32_input_pulses_per_kmph_mHz;

        /** Unit is mili Hertz mHz */
        const uint32_t m_u32_output_pulses_per_kmph_mHz;

        std::atomic<bool> m_bo_terminate_thread;
    };
}
#endif /* _SPEED_SENSOR_CONVERTER_HPP_ */
