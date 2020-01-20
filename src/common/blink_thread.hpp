#pragma once

#include <modm/processing/protothread.hpp>
#include <modm/processing/timer/timeout.hpp>
#include <common/board.hpp>

#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

template<typename LedGpio, uint16_t OnTime, uint16_t OffTime>
class BlinkThread : public modm::pt::Protothread
{
public:
    inline bool
    run() {
        PT_BEGIN();
        while (true)
        {
            timeout.restart(OffTime);
            LedGpio::set();
            PT_WAIT_UNTIL(timeout.isExpired());

            timeout.restart(OnTime);
            LedGpio::reset();
            PT_WAIT_UNTIL(timeout.isExpired());
        }
        PT_END();
    }
private:
    modm::ShortTimeout timeout;
};