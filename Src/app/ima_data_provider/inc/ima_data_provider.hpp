#ifndef _IMA_DATA_PROVIDER_HPP_
#define _IMA_DATA_PROVIDER_HPP_


#include <cstdint>
#include "CanIf.h"

extern "C"
{
    
    //void IMACanControllerModeIndication(uint8 controllerId, CanIf_ControllerModeType controllerMode);
    void IMACanRxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);
}

namespace app
{
    struct HondaCRZIMAStatus
    {
        uint16_t u16_engine_rpm;
        uint16_t u16_battery_voltage /* in V */;
        int16_t i16_ima_motor_current; /* in mV */
        uint16_t u16_SOC; /* SOC of the IMA battery */
        uint8_t u8_some_temperature; /* no idea what temperature, but it looks cool */
    };


    class IMADataProvider
    {
    public:
        HondaCRZIMAStatus& get_ima_data();
        const HondaCRZIMAStatus& get_ima_data() const;

        void print_ima_data() const;

    private:
        HondaCRZIMAStatus m_ima_status;
    };

    IMADataProvider& get_ima_data_provider();
}
#endif /* _IMA_DATA_PROVIDER_HPP_ */
