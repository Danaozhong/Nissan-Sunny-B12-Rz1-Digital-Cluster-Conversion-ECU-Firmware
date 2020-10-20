#include "can_diagnostics.hpp"
#include "main_application.hpp"
#include <cstring>


#include "Can.h"
#include "CanIf.h"

using namespace OSServices;

namespace app
{
    const char* CanControllerModeToString(Can_ControllerStateType controller_type)
    {
        switch (controller_type)
        {
        case CAN_CS_UNINIT:
            return "uninitialized";
        case CAN_CS_STARTED:
            return "started";
        case CAN_CS_STOPPED:
            return "stopped";
        case CAN_CS_SLEEP:
            return "sleep";
        default:
            return "invalid";
        }
    }

    Can_ControllerStateType StringToCanControllerState(const char* controller_state)
    {
        if (nullptr == controller_state)
        {
            return CAN_CS_UNINIT;
        }


        if (0 == strcmp(controller_state, "uninitialized"))
        {
            return CAN_CS_UNINIT;
        }
        else if (0 == strcmp(controller_state, "start"))
        {
            return CAN_CS_STARTED;
        }
        else if (0 == strcmp(controller_state, "stop"))
        {
            return CAN_CS_STOPPED;
        }
        else if (0 == strcmp(controller_state, "sleep"))
        {
            return CAN_CS_SLEEP;
        }
        return CAN_CS_UNINIT;

    }
    const char* CanErrorStatePtrToString(Can_ErrorStateType error_state)
    {
        switch (error_state)
        {
        case CAN_ERRORSTATE_ACTIVE:
            return "active";
        case CAN_ERRORSTATE_PASSIVE:
            return "passive";
        case CAN_ERRORSTATE_BUSOFF:
            return "bus off";
        default:
            return "invalid";
        }
    }

    void CommandCAN::display_usage(std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface)
    {
        p_o_io_interface<< "Wrong usage command, or wrong parameters.";
    }

    void CommandCAN::display_can_status(std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface)
    {
        /* display status of the CAN transceiver(s) */
        for (int32_t i32_configId=0; i32_configId < CAN_ARC_CTRL_CONFIG_CNT; i32_configId++)
        {
            p_o_io_interface << "CAN CONTROLLER #" << i32_configId << " STATUS\n";
            Can_ControllerStateType ControllerModePtr;
            if (E_OK == CanIf_GetControllerMode(i32_configId, &ControllerModePtr))
            {
                p_o_io_interface << "   Controller mode: " << CanControllerModeToString(ControllerModePtr) << "\n";
            }
            else
            {
                p_o_io_interface << "   Error reading controller mode." << "\n";
            }


            Can_ErrorStateType ErrorStatePtr;
            if (E_OK == CanIf_GetControllerErrorState(i32_configId, &ErrorStatePtr))
            {
                p_o_io_interface << "   Controller error state: " << CanErrorStatePtrToString(ErrorStatePtr) << "\n";
            }
            else
            {
                p_o_io_interface << "   Error reading controller error state." << "\n";
            }
        }
    }

    int32_t CommandCAN::command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface)
    {
        if (u32_num_of_params == 0)
        {
            // parameter error, no parameter provided
            display_usage(p_o_io_interface);
            return OSServices::ERROR_CODE_NUM_OF_PARAMETERS;
        }

        if (0 == strcmp(params[0], "status"))
        {
            display_can_status(p_o_io_interface);
        }
        else if (0 == strcmp(params[0], "mode"))
        {
            /* Change the mode of the CAN transceiver */
            if (u32_num_of_params < 2)
            {
                // parameter error, no parameter provided
                p_o_io_interface << "Please provide a mode setting (....)";
                return OSServices::ERROR_CODE_NUM_OF_PARAMETERS;
            }

            if (E_OK == CanIf_SetControllerMode(0, StringToCanControllerState(params[1])))
            {
                p_o_io_interface << "Setting mode " << params[1] << " on CAN controller 0 successful." << "\n";
            }
            else
            {
                p_o_io_interface << "Error setting CAN controller mode!" << "\n";
            }

            display_can_status(p_o_io_interface);

        }


        // if no early return, the command was executed successfully.
        return OSServices::ERROR_CODE_SUCCESS;
    }


}

