/*
* Configuration of module: CanIf (CanIf_Cfg.c)
*
* Created by:              
* Copyright:               
*
* Configured for (MCU):    STM32_F107
*
* Module vendor:           ArcCore
* Generator version:       2.0.6
*
* Generated by Arctic Studio (http://arccore.com) 
*/


 
#include "CanIf.h"
#if defined(USE_CANTP)
#include "CanTp.h"
#include "CanTp_Cbk.h"
#endif
#if defined(USE_J1939TP)
#include "J1939Tp.h"
#include "J1939Tp_Cbk.h"
#endif
#if defined(USE_PDUR)
#include "PduR.h"
#endif
#if defined(USE_CANNM)
#include "CanNm_Cbk.h"
#endif
#include <stdlib.h>
#include "CanIf_Types.h"


// Imported structs from Can_Lcfg.c
extern const Can_ControllerConfigType CanControllerConfigData[];
extern const Can_ConfigSetType CanConfigSetData;

/* Application callbacks */
//void IMACanControllerModeIndication(uint8 controllerId, CanIf_ControllerModeType controllerMode);
void IMACanRxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);



// Contains the mapping from CanIf-specific Channels to Can Controllers
const CanControllerIdType CanIf_Arc_ChannelToControllerMap[CANIF_CHANNEL_CNT] = {
	CAN_CTRL_1, // Controller_A
};

const uint8 CanIf_Arc_ChannelDefaultConfIndex[CANIF_CHANNEL_CNT] = {
	CANIF_Controller_A_CONFIG_0,
};

// Container that gets slamed into CanIf_InitController()
// Inits ALL controllers
// Multiplicity 1..*

/* Stubs Clemens */
#define CANIF_WAKEUP_SUPPORT_NO_WAKEUP 0
/* Stubs end */
const CanIf_ControllerConfigType CanIfControllerConfig[] = {
	// This is the ConfigurationIndex in CanIf_InitController()
	{
		.WakeupSupport = CANIF_WAKEUP_SUPPORT_NO_WAKEUP,
		.CanIfControllerIdRef = CANIF_Controller_A,
		.CanIfDriverNameRef = "FLEXCAN",  // Not used
		.CanIfInitControllerRef = &CanControllerConfigData[0],
	},
	
};


// Function callbacks for higher layers
const CanIf_DispatchConfigType CanIfDispatchConfig =
{
  .CanIfBusOffNotification = NULL,
  .CanIfWakeUpNotification = NULL,        // Not used
  .CanIfWakeupValidNotification = NULL,   // Not used
  .CanIfErrorNotificaton = NULL,
};

#if 0
// Data for init configuration CanIfInitConfiguration

const CanIf_HthConfigType CanIfHthConfigData_Hoh[] =
{

  {
    .CanIfHthType = CAN_ARC_HANDLE_TYPE_BASIC,
    .CanIfCanControllerIdRef = CANIF_Controller_A,
    .CanIfHthIdSymRef = HWObj_2,
    .CanIf_Arc_EOL = 1,
  },
};

#endif



#if 0


const CanIf_InitHohConfigType CanIfHohConfigData[] = { 
	{
		.CanConfigSet = &CanConfigSetData,
		.CanIfHrhConfig = CanIfHrhConfigData_Hoh,
	    .CanIfHthConfig = CanIfHthConfigData_Hoh,
    	.CanIf_Arc_EOL = 1,
	},
};


#define PDUR_REVERSE_PDU_ID_CanDB_Message_2  0
#define PDUR_PDU_ID_CanDB_Message_1 1

#define PduR_CanIfTxConfirmation NULL


const CanIf_RxPduConfigType CanIfRxPduConfigData[] = {		
  {
    .CanIfCanRxPduId = PDUR_PDU_ID_CanDB_Message_1,
    .CanIfCanRxPduCanId = 256,
    .CanIfCanRxPduDlc = 8,
#if ( CANIF_CANPDUID_READDATA_API == STD_ON )    
    .CanIfReadRxPduData = false,
#endif    
#if ( CANIF_READTXPDU_NOTIFY_STATUS_API == STD_ON )
    .CanIfReadRxPduNotifyStatus = false, 
#endif
	.CanIfRxUserType = CANIF_USER_TYPE_CAN_PDUR,
    .CanIfCanRxPduHrhRef = &CanIfHrhConfigData_Hoh[0],
    .CanIfRxPduIdCanIdType = CANIF_CAN_ID_TYPE_11,
    .CanIfUserRxIndication = NULL,
    .CanIfSoftwareFilterType = CANIF_SOFTFILTER_TYPE_MASK,
    .CanIfCanRxPduCanIdMask = 0xFFF,
    .PduIdRef = NULL,
  },  
};

// This container contains the init parameters of the CAN
// Multiplicity 1..*
const CanIf_InitConfigType CanIfInitConfig =
{
  .CanIfConfigSet = 0, // Not used  
  .CanIfNumberOfCanRxPduIds = 1,
  .CanIfNumberOfCanTXPduIds = 1,
  .CanIfNumberOfDynamicCanTXPduIds = 0, // Not used

  // Containers
  .CanIfHohConfigPtr = CanIfHohConfigData,
  .CanIfRxPduConfigPtr = CanIfRxPduConfigData,
  .CanIfTxPduConfigPtr = CanIfTxPduConfigData,
};
#endif

/* List of PDUs that will be processed by controller 0 */
PduIdType LPDU_Controller0[] =
{
    CANIF_PDU_ID_PDU_IMA_ID111,
    CANIF_PDU_ID_PDU_IMA_ID169,
    CANIF_PDU_ID_PDU_IMA_ID231,
    CANIF_PDU_ID_PDU_IMA_ID318,

};

const CanIf_HrHConfigType CanIfHrhConfigData_HohController0[] =
{
    {
        .pduInfo.array = LPDU_Controller0,
        .arrayLen = 4,
    },
};



const CanIf_HrHConfigType* CanIfHrhConfigData_Hoh[] =
{
    CanIfHrhConfigData_HohController0,
};

const CanIf_TxPduConfigType CanIfTxPduConfigData[CANIF_NUM_TX_PDU_ID] =
{
  {

    .id = 512,
    .dlc = 8,
    .controller = 0,
    .hth = 0, //&CanIfHthConfigData_Hoh[0],
    .user_TxConfirmation = NULL,
    .ulPduId = 0
  },
  {
      .id = 0x13F, /* throttle position parameters */
      .dlc = 8,
      .controller = 0,
      .hth = 0,
      .user_TxConfirmation = NULL,
      .ulPduId = 1
  },
};

const CanIf_RxLPduConfigType CanIfRxPduConfigData[CANIF_NUM_RX_LPDU_ID] =
{ /* CAN ID must be sorted from high to low! */
    { /* Contains throttle body position sensor, and some temperature values */
        .id = 0x318,
        .dlc = 8,
        .controller = 0,
        .user_RxIndication = &IMACanRxIndication,
        .ulPduId = 0
    },
    { /* contains the SOC */
        .id = 0x231,
        .dlc = 7,
        .controller = 0,
        .user_RxIndication = &IMACanRxIndication,
        .ulPduId = 1
    },
    { /* contains the motor current */
        .id = 0x169,
        .dlc = 8,
        .controller = 0,
        .user_RxIndication = &IMACanRxIndication,
        .ulPduId = 2
    },
    {
        .id = 0x111, /* contains engine RPM, and battery voltage */
        .dlc = 7,
        .controller = 0,
        .user_RxIndication = &IMACanRxIndication,
        .ulPduId = 3
    },



};



	// This container includes all necessary configuration sub-containers
// according the CAN Interface configuration structure.
const CanIf_ConfigType CanIf_Config =
{
  .ControllerConfig = CanIfControllerConfig,
  .DispatchConfig = &CanIfDispatchConfig,
  .TxPduCfg = CanIfTxPduConfigData,
  .RxLpduCfg = CanIfRxPduConfigData,
  .canIfHrhCfg = CanIfHrhConfigData_Hoh,
  //.InitConfig = &CanIfInitConfig,
  //.TransceiverConfig = NULL, // Not used
  //.Arc_ChannelToControllerMap = CanIf_Arc_ChannelToControllerMap,
  //.Arc_ChannelDefaultConfIndex = CanIf_Arc_ChannelDefaultConfIndex,
};

