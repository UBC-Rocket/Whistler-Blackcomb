/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2014-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_CORTEX_SYSTICK_TIMER_HPP
#define MODM_CORTEX_SYSTICK_TIMER_HPP

#include <stdint.h>
#include <cmath>
#include <modm/architecture/interface/peripheral.hpp>
#include <modm/math/algorithm/prescaler.hpp>

namespace modm::platform
{

typedef void (*InterruptHandler)(void);

/**
 * @brief		SysTick Timer
 * @ingroup		modm_platform_clock_cortex
 */
class SysTickTimer
{
public:
	/**
	 * Initializes the SysTick Timer to generate periodic events.
	 *
	 *
	 * @warning	The SysTick Timer is used by default to increment
	 * 			modm::Clock, which is used by modm::Timeout and other
	 * 			similar processing classes.
	 * 			You must not increment the modm::Clock
	 * 			additionally somewhere else.
	 *
	 * @tparam	SystemClock
	 * 		the currently active system clock
	 * @tparam	rate
	 * 		the desired update rate of the modm::Clock
	 * @tparam	tolerance
	 * 		the allowed absolute tolerance for the resulting clock rate
	 */
	template< class SystemClock, percent_t tolerance=pct(0) >
	static void
	initialize()
	{
		constexpr auto result = Prescaler::from_range(
				SystemClock::Frequency, 1000, 1, (1ul << 24)-1);
		PeripheralDriver::assertBaudrateInTolerance< result.frequency, 1000, tolerance >();

		enable(result.index);
	}

	/**
	 * Disables SysTick Timer.
	 *
	 * @warning	If the SysTick Timer is disabled modm::Clock is not
	 * 			incremented automatically. Workflow classes which
	 * 			rely on modm::Clock will not work if modm::Clock
	 * 			is not incremented.
	 */
	static void
	disable();

	/**
	 * Passed method will be called periodically on each event.
	 * Previously passed interrupt handler will be detached.
	 */
	static void
	attachInterruptHandler(InterruptHandler handler);

	/**
	 * Detaches previously attached interrupt handler.
	 */
	static void
	detachInterruptHandler();

private:
	static void
	enable(uint32_t reload);
};

}

namespace modm::cortex {
	using SysTickTimer [[deprecated("Please use `modm::platform:SysTickTimer` instead")]] =
		::modm::platform::SysTickTimer;
}

#endif	//  MODM_STM32_CORTEX_TIMER_HPP