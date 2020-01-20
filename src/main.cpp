#include <modm/architecture/interface/delay.hpp>
#include <modm/debug/logger.hpp>

#include <common/board.hpp>
#include <common/blink_thread.hpp>
#include <common/count_thread.hpp>

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

int main(void) {
	initCommon();

	MODM_LOG_DEBUG << "main: debug logging here" << modm::endl;
	MODM_LOG_INFO << "main: info logging here" << modm::endl;
	MODM_LOG_WARNING << "main: warning logging here" << modm::endl;
	MODM_LOG_ERROR << "main: error logging here" << modm::endl;

	for (int i = 0; i < 10; i++) {
		LedD1::set();
		LedD2::reset();
		LedD3::reset();
		modm::delayMilliseconds(34);

		LedD1::reset();
		LedD2::set();
		LedD3::reset();
		modm::delayMilliseconds(33);

		LedD1::reset();
		LedD2::reset();
		LedD3::set();
		modm::delayMilliseconds(33);
	}
	LedD1::reset();
	LedD2::reset();
	LedD3::reset();

	BlinkThread<LedD1, 450, 50> blinkThread1;
	BlinkThread<LedD2, 950, 50> blinkThread2;
	BlinkThread<LedD3, 1450, 50> blinkThread3;
	CountThread<1000> countThread;
	while (1) {
		blinkThread1.run();
		blinkThread2.run();
		blinkThread3.run();
		countThread.run();
	}
	return 0;
}
