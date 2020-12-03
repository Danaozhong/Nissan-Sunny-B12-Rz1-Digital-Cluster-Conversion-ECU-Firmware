#ifndef _SPEED_SENSOR_CONVERTER_HPP_
#define _SPEED_SENSOR_CONVERTER_HPP_

#include <atomic>
#include <mutex>

#include "ex_thread.hpp"
#include "generic_pwm.hpp"
#include "generic_pwm_ic.hpp"
#include "replay_curve.hpp"


/* Number of PWM Input Capture readings to be buffered */
#define SPEED_SENSOR_READINGS_BUFFER_LENGTH 10

//#define SPEED_CONVERTER_USE_OWN_TASK
namespace app
{
    enum SpeedOutputMode
    {
        OUTPUT_MODE_CONVERSION,
        OUTPUT_MODE_MANUAL,
        OUTPUT_MODE_REPLAY
    };

    class SpeedSensorConverter
    {
    public:
        SpeedSensorConverter(drivers::GenericPWM* p_output_pwm,
                drivers::GenericPWM_IC* p_output_pwm_input_capture,
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

        /** Returns the currently displayed speed on the cluster. */
        int32_t get_current_displayed_speed() const;

        uint32_t get_current_output_frequency() const;

        /** Returns the current vehicle speed in m/h, as read from the sensor */
        uint32_t get_current_vehicle_speed() const;


        /** polls the PWM for the last read value */
        void poll_vehicle_speed();

        /** callback executed when the PWM IC reads a frequency */
        void pwm_input_capture_callback(int32_t i32_read_frequency_in_mHz);


        /** one single data processing cycle. Called cyclically from speed_sensor_converter_main */
        void cycle();
    private:

        bool check_if_speed_is_valid(int32_t i32_speed_value_in_kmph);

        drivers::GenericPWM* m_p_output_pwm;

        drivers::GenericPWM_IC* m_p_output_pwm_input_capture;

        std::atomic<SpeedOutputMode> m_en_current_speed_output_mode;

        /**
         * The manually set vehicle speed in meter/hour. Only valid if OUTPUT_MODE_MANUAL is set.
         */
        int32_t m_i32_manual_speed;

        /**
         * A data curve that will be replayed when the output is in mode OUTPUT_MODE_REPLAY */
        ReplayCurve m_o_replay_curve;

        /** the currently calculated vehicle speed in m/h */
        uint32_t m_u32_current_vehicle_speed_mph;

        /** Array of frequencies read from the PWM IC pin */
        uint32_t m_au32_input_frequency_mHz[SPEED_SENSOR_READINGS_BUFFER_LENGTH];

        /** Specifies which position is currently being written to in the m_au32_input_frequency_mHz array */
        uint8_t m_u8_input_array_position;

        uint32_t m_u32_new_output_frequency_mHz;

        /** Unit is mili Hertz mHz */
        const uint32_t m_u32_input_pulses_per_kmph_mHz;

        /** Unit is mili Hertz mHz */
        const uint32_t m_u32_output_pulses_per_kmph_mHz;

        /// This mutex protects the m_au32_input_frequency_mHz and m_u8_input_array_position objects
        //std::mutex m_pwm_capture_data_mutex;

        /// count how many times the PWM capture has been triggered */
        uint32_t m_u32_num_of_pwm_captures;

        /// how many captures of the PWM were already processed
        uint32_t m_u32_num_of_processed_pwm_captures;

#ifdef SPEED_CONVERTER_USE_OWN_TASK
        void speed_sensor_converter_main();
        std_ex::thread* m_p_data_conversion_thread;
        std::atomic<bool> m_bo_terminate_thread;
#endif
    };
}
#endif /* _SPEED_SENSOR_CONVERTER_HPP_ */
