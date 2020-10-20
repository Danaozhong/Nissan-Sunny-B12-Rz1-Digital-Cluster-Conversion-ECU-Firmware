#ifndef _IMA_DATA_PROVIDER_HPP_
#define _IMA_DATA_PROVIDER_HPP_

#include "CanIf.h"

extern "C"
{
    
    //void IMACanControllerModeIndication(uint8 controllerId, CanIf_ControllerModeType controllerMode);
    void IMACanRxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);
}


#endif /* _IMA_DATA_PROVIDER_HPP_ */
