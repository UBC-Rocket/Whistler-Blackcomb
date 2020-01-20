#pragma once

#include <modm/processing/protothread.hpp>
#include <modm/processing/timer/timeout.hpp>
#include <common/board.hpp>

#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

template<uint16_t Interval>
class CountThread : public modm::pt::Protothread
{
public:
    CountThread() : count(0) {}
    inline bool run() {
        PT_BEGIN();
        while (true)
        {
            timeout.restart(Interval);
            MODM_LOG_INFO << count << '\r' << modm::endl;
            count++;
            PT_WAIT_UNTIL(timeout.isExpired());
        }
        PT_END();
    }
private:
    modm::ShortTimeout timeout;
    uint32_t count;
};