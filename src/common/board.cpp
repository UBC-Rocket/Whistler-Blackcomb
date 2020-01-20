#include "board.hpp"
#include <modm/architecture/interface/clock.hpp>
#include <modm/architecture/interface/can.hpp>
#include <modm/debug/logger.hpp>

modm::IODeviceWrapper<SerialDebug, modm::IOBuffer::BlockIfFull> debugLogger;
modm::log::Logger modm::log::debug(debugLogger);
modm::log::Logger modm::log::info(debugLogger);
modm::log::Logger modm::log::warning(debugLogger);
modm::log::Logger modm::log::error(debugLogger);

modm::IODevice& missionControlIO = debugLogger;

using namespace modm::literals;

bool ClockConfiguration::enable() {
	// modm::platform::ClockControl can be accessed as only ClockControl
	using namespace modm::platform;
	//ClockControl::enableExternalCrystal(); // 26 MHz
	Rcc::enableInternalClock(); //16 MHz
	Rcc::enablePll(
		ClockControl::PllSource::InternalClock,
		16,     // 16MHz / N=16 -> 1MHz   !!! Must be 1 MHz for PLLSAI !!!
		432,    // 1MHz * M=432 -> 432MHz
		2,      // 432MHz / P=2 -> 216MHz = F_cpu
		2       // 336MHz / Q=7 -> 216MHz (not used)
	);

	Rcc::setFlashLatency<Frequency>();
	Rcc::setApb1Prescaler(ClockControl::Apb1Prescaler::Div4);
	Rcc::setApb2Prescaler(ClockControl::Apb2Prescaler::Div2);

	Rcc::enableSystemClock(ClockControl::SystemClockSource::Pll);
	// update clock frequencies
	Rcc::updateCoreFrequency<Frequency>();

	return true;
}

void initCommon() {
	using namespace modm::platform;

	ClockConfiguration::enable();
	modm::cortex::SysTickTimer::initialize<ClockConfiguration>();

	LedD1::setOutput(modm::Gpio::Low);
	LedD2::setOutput(modm::Gpio::Low);
	LedD3::setOutput(modm::Gpio::Low);

	SerialDebug::connect<modm::platform::GpioD8::Tx, modm::platform::GpioD9::Rx>();
	SerialDebug::initialize<ClockConfiguration, 115200_Bd, modm::pct(1)>();

	// using Scl = GpioB6;
	// using Sda = GpioB7;

	// Scl::configure(modm::platform::Gpio::InputType::PullUp);
	// Sda::configure(modm::platform::Gpio::InputType::PullUp);
	// Scl::configure(modm::platform::Gpio::OutputType::OpenDrain);
	// Sda::configure(modm::platform::Gpio::OutputType::OpenDrain);

	// I2cMaster1::connect<Scl::Scl, Sda::Sda>(I2cMaster1::PullUps::Internal);
	// I2cMaster1::initialize<ClockConfiguration, 100_kHz>();

	// Can1::connect<GpioA11::Rx, GpioA12::Tx>(Gpio::InputType::PullUp);
	// Can1::initialize<ClockConfiguration, 250_kbps>(9);
	// Can1::setAutomaticRetransmission(true);
	// Can1::setMode(Can1::Mode::Normal);
	// CanFilter::setFilter(0, CanFilter::FIFO0,
	// CanFilter::StandardIdentifier(0),
	// CanFilter::StandardFilterMask(0));
}

// void stupidI2cBugFix() {
// 	using I2cMaster = modm::platform::I2cMaster1;
// 	using Scl = modm::platform::GpioB6;
// 	using Sda = modm::platform::GpioB7;
// 	I2C_TypeDef* I2CBase = I2C1;
// 	I2CBase->CR1 &= ~I2C_CR1_PE;
// 	Scl::setOutput(modm::Gpio::Low);
// 	Sda::setOutput(modm::Gpio::Low);
// 	Scl::configure(modm::platform::Gpio::OutputType::OpenDrain);
// 	Sda::configure(modm::platform::Gpio::OutputType::OpenDrain);
// 	while (Scl::read() != modm::Gpio::Low);
// 	while (Sda::read() != modm::Gpio::Low);
// 	Scl::set(modm::Gpio::High);
// 	Sda::set(modm::Gpio::High);
// 	while (Scl::read() != modm::Gpio::High);
// 	while (Sda::read() != modm::Gpio::High);
// 	I2cMaster::connect<Sda::Sda, Scl::Scl>(modm::I2cMaster::PullUps::Internal);
// 	I2cMaster::initialize<ClockConfiguration, 100_kHz>();
// };

