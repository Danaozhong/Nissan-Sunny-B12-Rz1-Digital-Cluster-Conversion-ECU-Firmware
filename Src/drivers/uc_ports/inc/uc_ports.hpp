#ifndef _UC_PORTS_HPP_
#define _UC_PORTS_HPP_

#include <stdint.h>

namespace drivers
{
    /** Class to store a uC port configuration */
    class UcPorts
    {
    public:
        UcPorts();
        virtual ~UcPorts() {}

        virtual int32_t init_ports_can() { return 0; }
        virtual int32_t deinit_ports_can() { return 0; }
        virtual int32_t init_ports_adc() { return 0; }
        virtual int32_t deinit_ports_adc() { return 0; }
        virtual int32_t init_ports_dac() { return 0; }
        virtual int32_t deinit_ports_dac() { return 0; }
        virtual int32_t init_ports_tim() { return 0; }
        virtual int32_t deinit_ports_tim() { return 0; }
    };

    class STM32F303CCT6UcPorts : public UcPorts
    {
    public:
        STM32F303CCT6UcPorts();
        virtual int32_t init_ports_can();
        virtual int32_t deinit_ports_can();
        virtual int32_t init_ports_adc();
        virtual int32_t deinit_ports_adc();
        virtual int32_t init_ports_dac();
        virtual int32_t deinit_ports_dac();
        virtual int32_t init_ports_tim();
        virtual int32_t deinit_ports_tim();


    };
}

#endif /* _UC_PORTS_HPP_ */
