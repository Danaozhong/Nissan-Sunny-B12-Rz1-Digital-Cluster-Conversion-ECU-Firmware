#include "ima_data_provider.hpp"
#include "trace_if.h"
extern "C"
{
#if 0
    void IMACanControllerModeIndication(uint8 controllerId, CanIf_ControllerModeType controllerMode)
    {
        
    }
#endif
    void IMACanRxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr)
    {
        DEBUG_PRINTF("CAN message received, PDU ID: %u!\n\r", static_cast<unsigned int>(RxPduId));
    }
}
