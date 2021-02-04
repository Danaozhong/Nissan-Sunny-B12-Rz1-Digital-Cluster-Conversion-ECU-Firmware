#include "ima_data_provider.hpp"
#include "trace_if.h"

#include <cstring>
#include <cstdlib>
extern "C"
{
#if 0
    void IMACanControllerModeIndication(uint8 controllerId, CanIf_ControllerModeType controllerMode)
    {
        
    }
#endif

    uint16_t u16_swap_endianess(uint16_t input)
    {
        return (input >> 8 & 0x00FF) | (input << 8 % 0xFF00);
    }

    int16_t i16_swap_endianess(int16_t input)
    {
        return (input >> 8 & 0x00FF) | (input << 8 % 0xFF00);
    }

    void IMACanRxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr)
    {
        if (nullptr == PduInfoPtr)
        {
            return;
        }

        app::HondaCRZIMAStatus& o_ima_data = app::get_ima_data_provider().get_ima_data();

        switch (RxPduId)
        {
        case 3: /* CAN ID 111 */
            std::memcpy(&o_ima_data.u16_engine_rpm, PduInfoPtr->SduDataPtr + 0, 2);
            std::memcpy(&o_ima_data.u16_battery_voltage, PduInfoPtr->SduDataPtr + 2, 2);

            o_ima_data.u16_engine_rpm = u16_swap_endianess(o_ima_data.u16_engine_rpm);
            o_ima_data.u16_battery_voltage = u16_swap_endianess(o_ima_data.u16_battery_voltage);

            break;
        case 2: /* CAN ID 169 */
            std::memcpy(&o_ima_data.i16_ima_motor_current, PduInfoPtr->SduDataPtr + 0, 2);
            o_ima_data.i16_ima_motor_current = i16_swap_endianess(o_ima_data.i16_ima_motor_current);
            break;
        case 1: /* CAN ID 231 */
            std::memcpy(&o_ima_data.u16_SOC, PduInfoPtr->SduDataPtr + 2, 2);
            o_ima_data.u16_SOC = u16_swap_endianess(o_ima_data.u16_SOC);
            break;
        case 0: /* CAN ID 318 */
            std::memcpy(&o_ima_data.u8_some_temperature, PduInfoPtr->SduDataPtr + 4, 2);
            break;
        }
    }
}

namespace app
{

    HondaCRZIMAStatus& IMADataProvider::get_ima_data()
    {
        return m_ima_status;
    }

    const HondaCRZIMAStatus& IMADataProvider::get_ima_data() const
    {
        return m_ima_status;
    }

    void IMADataProvider::print_ima_data() const
    {
        DEBUG_PRINTF("CURRENT IMA STATUS\n\r");

        unsigned int u_rpm = static_cast<unsigned int>(get_ima_data().u16_engine_rpm);
        unsigned int u_battery_voltage_int = static_cast<unsigned int>(get_ima_data().u16_battery_voltage) / 100;
        unsigned int u_battery_voltage_frac = static_cast<unsigned int>(get_ima_data().u16_battery_voltage) % 100;

        unsigned int u_soc_int = static_cast<unsigned int>(get_ima_data().u16_SOC) / 100;
        unsigned int u_soc_frac = static_cast<unsigned int>(get_ima_data().u16_SOC) % 100;



        int i_ima_current_int = static_cast<int>(get_ima_data().i16_ima_motor_current) /  100;
        unsigned int ima_current_frac = std::abs(static_cast<int>(get_ima_data().i16_ima_motor_current)) % 100;
        unsigned int u_temperature = static_cast<unsigned int>(get_ima_data().u8_some_temperature);

        DEBUG_PRINTF("   Engine RPM: %u rpm\n\r", u_rpm);
        DEBUG_PRINTF("   Battery Voltage: %u.%uV\n\r", u_battery_voltage_int, u_battery_voltage_frac);
        DEBUG_PRINTF("   Battery SOC: %u.%u%%\n\r", u_soc_int, u_soc_frac);
        DEBUG_PRINTF("   Battery Current: %i.%uA\n\r", i_ima_current_int, ima_current_frac);
        DEBUG_PRINTF("   Battery temperature: %uF\n\r", u_temperature);
        DEBUG_PRINTF("\n\r\n\r");
    }


    IMADataProvider& get_ima_data_provider()
    {
        static IMADataProvider o_instance;
        return o_instance;
    }
}
