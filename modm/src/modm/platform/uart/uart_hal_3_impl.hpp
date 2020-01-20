/*
 * Copyright (c) 2013-2014, 2016, Kevin Läufer
 * Copyright (c) 2013-2017, Niklas Hauser
 * Copyright (c) 2017, Fabian Greif
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2018, Christopher Durand
 * Copyright (c) 2018, Lucas Mösch
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_UARTHAL_3_HPP
#	error 	"Don't include this file directly, use uart_hal_3.hpp instead!"
#endif
#include <modm/platform/clock/rcc.hpp>

// ----------------------------------------------------------------------------
void
modm::platform::UsartHal3::setParity(const Parity parity)
{
	const bool usartEnabled = (USART3->CR1 & USART_CR1_UE);
	if(usartEnabled) {
		disableOperation();
	}
	uint32_t flags = USART3->CR1;
	flags &= ~(USART_CR1_PCE | USART_CR1_PS | USART_CR1_M);
	flags |= static_cast<uint32_t>(parity);
	if (parity != Parity::Disabled) {
		// Parity Bit counts as 9th bit -> enable 9 data bits
		flags |= USART_CR1_M;
	}
	USART3->CR1 = flags;

	if(usartEnabled) {
		enableOperation();
	}
}

void
modm::platform::UsartHal3::enable()
{
	Rcc::enable<Peripheral::Usart3>();
	USART3->CR1 |= USART_CR1_UE;		// Uart Enable
}

void
modm::platform::UsartHal3::disable()
{
	// TX, RX, Uart, etc. Disable
	USART3->CR1 = 0;
	Rcc::disable<Peripheral::Usart3>();
}
template<class SystemClock, modm::baudrate_t baudrate,
		modm::platform::UsartHal3::OversamplingMode oversample>
void modm_always_inline
modm::platform::UsartHal3::initialize(Parity parity)
{
	initializeWithBrr(UartBaudrate::getBrr<SystemClock::Usart3, baudrate>(), parity, oversample);
}
template<class SystemClock, modm::baudrate_t baudrate>
void modm_always_inline
modm::platform::UsartHal3::initialize(Parity parity)
{
	initializeWithBrr(UartBaudrate::getBrr<SystemClock::Usart3, baudrate>(), parity,
					  UartBaudrate::getOversamplingMode(SystemClock::Usart3, baudrate));
}


void inline
modm::platform::UsartHal3::initializeWithBrr(uint16_t brr, Parity parity, OversamplingMode oversample)
{
	enable();
	// DIRTY HACK: disable and reenable uart to be able to set
	//             baud rate as well as parity
	USART3->CR1 &= ~USART_CR1_UE;	// Uart Disable
	// set baudrate
	USART3->BRR = brr;
	setParity(parity);
	setOversamplingMode(oversample);
	USART3->CR1 |=  USART_CR1_UE;	// Uart Reenable
}

void
modm::platform::UsartHal3::setOversamplingMode(OversamplingMode mode)
{
	const bool usartEnabled = (USART3->CR1 & USART_CR1_UE);
	if(usartEnabled) {
		disableOperation();
	}
	if(mode == OversamplingMode::By16) {
		USART3->CR1 &= ~static_cast<uint32_t>(OversamplingMode::By8);
	} else {
		USART3->CR1 |=  static_cast<uint32_t>(OversamplingMode::By8);
	}

	if(usartEnabled) {
		enableOperation();
	}
}
void
modm::platform::UsartHal3::setSpiClock(SpiClock clk)
{
	const bool usartEnabled = (USART3->CR1 & USART_CR1_UE);
	if(usartEnabled) {
		disableOperation();
	}
	if(clk == SpiClock::Disabled) {
		USART3->CR2 &= ~static_cast<uint32_t>(SpiClock::Enabled);
	} else {
		USART3->CR2 |=  static_cast<uint32_t>(SpiClock::Enabled);
	}

	if(usartEnabled) {
		enableOperation();
	}
}

void
modm::platform::UsartHal3::setSpiDataMode(SpiDataMode mode)
{
	const bool usartEnabled = (USART3->CR1 & USART_CR1_UE);
	if(usartEnabled) {
		disableOperation();
	}
	USART3->CR2 =
		(USART3->CR2 & ~static_cast<uint32_t>(SpiDataMode::Mode3))
		| static_cast<uint32_t>(mode);

	if(usartEnabled) {
		enableOperation();
	}
}

void
modm::platform::UsartHal3::setLastBitClockPulse(LastBitClockPulse pulse)
{
	const bool usartEnabled = (USART3->CR1 & USART_CR1_UE);
	if(usartEnabled) {
		disableOperation();
	}
	if(pulse == LastBitClockPulse::DoNotOutput) {
		USART3->CR2 &= ~static_cast<uint32_t>(LastBitClockPulse::Output);
	} else {
		USART3->CR2 |=  static_cast<uint32_t>(LastBitClockPulse::Output);
	}

	if(usartEnabled) {
		enableOperation();
	}
}
void
modm::platform::UsartHal3::write(uint8_t data)
{
	USART3->TDR = data;
}

void
modm::platform::UsartHal3::read(uint8_t &data)
{
	data = USART3->RDR;
}

void
modm::platform::UsartHal3::setTransmitterEnable(const bool enable)
{
	if (enable) {
		USART3->CR1 |=  USART_CR1_TE;
	} else {
		USART3->CR1 &= ~USART_CR1_TE;
	}
}

void
modm::platform::UsartHal3::setReceiverEnable(bool enable)
{
	if (enable) {
		USART3->CR1 |=  USART_CR1_RE;
	} else {
		USART3->CR1 &= ~USART_CR1_RE;
	}
}

void
modm::platform::UsartHal3::enableOperation()
{
	USART3->CR1 |= USART_CR1_UE;
}

void
modm::platform::UsartHal3::disableOperation()
{
	USART3->CR1 &= ~USART_CR1_UE;
}

bool
modm::platform::UsartHal3::isReceiveRegisterNotEmpty()
{
	return USART3->ISR & USART_ISR_RXNE;
}

bool
modm::platform::UsartHal3::isTransmitRegisterEmpty()
{
	return USART3->ISR & USART_ISR_TXE;
}

void
modm::platform::UsartHal3::enableInterruptVector(bool enable, uint32_t priority)
{
	if (enable) {
		// Set priority for the interrupt vector
		NVIC_SetPriority(USART3_IRQn, priority);

		// register IRQ at the NVIC
		NVIC_EnableIRQ(USART3_IRQn);
	}
	else {
		NVIC_DisableIRQ(USART3_IRQn);
	}
}

void
modm::platform::UsartHal3::enableInterrupt(Interrupt_t interrupt)
{
	USART3->CR1 |= interrupt.value;
}

void
modm::platform::UsartHal3::disableInterrupt(Interrupt_t interrupt)
{
	USART3->CR1 &= ~interrupt.value;
}

modm::platform::UsartHal3::InterruptFlag_t
modm::platform::UsartHal3::getInterruptFlags()
{
	return InterruptFlag_t( USART3->ISR );
}

void
modm::platform::UsartHal3::acknowledgeInterruptFlags(InterruptFlag_t flags)
{
	// Not all flags can be cleared by writing to this reg
#ifdef USART_ICR_NECF
#define USART_ICR_NCF USART_ICR_NECF
#endif
	const uint32_t mask = USART_ICR_PECF  | USART_ICR_FECF   |
		USART_ICR_NCF   | USART_ICR_ORECF | USART_ICR_IDLECF |
		USART_ICR_TCCF  | USART_ICR_CTSCF | USART_ICR_RTOCF  |
		USART_ICR_CMCF
#ifdef USART_ICR_LBDCF // F0x0 do not have LIN mode!
		| USART_ICR_LBDCF
#endif
#ifdef USART_ICR_EOBCF // F0x0 do not have Smartcard mode!
		| USART_ICR_EOBCF
#endif
#ifdef USART_ICR_WUCF
		| USART_ICR_WUCF
#endif
		;
	// Flags are cleared by writing a one to the flag position.
	// Writing a zero is (hopefully) ignored.
	USART3->ICR = (flags.value & mask);
}