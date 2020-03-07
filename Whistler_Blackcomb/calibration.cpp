#include "./includes/calibration.h"
#include "./includes/modbus.h"

void getNominalCalibrationT7(DeviceCalibrationT7 *devCal)
{
	int i = 0;

	devCal->HS[0].PSlope = 0.000315805780f;
	devCal->HS[0].NSlope = -0.000315805800f;
	devCal->HS[0].Center = 33523.0f;
	devCal->HS[0].Offset = -10.58695652200f;
	devCal->HS[1].PSlope = 0.0000315805780f;
	devCal->HS[1].NSlope = -0.0000315805800f;
	devCal->HS[1].Center = 33523.0f;
	devCal->HS[1].Offset = -1.058695652200f;
	devCal->HS[2].PSlope = 0.00000315805780f;
	devCal->HS[2].NSlope = -0.00000315805800f;
	devCal->HS[2].Center = 33523.0f;
	devCal->HS[2].Offset = -0.1058695652200f;
	devCal->HS[3].PSlope = 0.000000315805780f;
	devCal->HS[3].NSlope = -0.000000315805800f;
	devCal->HS[3].Center = 33523.0f;
	devCal->HS[3].Offset = -0.01058695652200f;

	for(i = 0; i < 4; i++)
		devCal->HR[i] = devCal->HS[i];

	devCal->DAC[0].Slope = 13200.0f;
	devCal->DAC[0].Offset = 0.0f;
	devCal->DAC[1].Slope = 13200.0f;
	devCal->DAC[1].Offset = 0.0f;

	devCal->Temp_Slope = -92.379f;
	devCal->Temp_Offset = 465.129f;

	devCal->ISource_10u = 0.000010f;
	devCal->ISource_200u = 0.000200f;

	devCal->I_Bias = 0;
}

int getCalibrationT7(EthernetClient * sock, DeviceCalibrationT7 *devCal)
{
	const unsigned int EFAdd_CalValues = 0x3C4000;
	const int FLASH_PTR_ADDRESS	= 61810;
	
	// 3 frames	of 13 values, one frame	of 2 values
	const int FLASH_READ_ADDRESS = 61812;
	const int FLASH_READ_NUM_REGS[4] = {26, 26, 26, 4};

	float calValue = 0.0;
	int calIndex = 0;
	unsigned char data[52];
	int i = 0;
	int j = 0;

	for(i = 0; i < 4; i++)
	{
		//Set the pointer. This	indicates which	part of the memory we want to read
		uint32ToBytes(EFAdd_CalValues + i * 13 * 4, data);
		if(writeMultipleRegistersTCP(sock, FLASH_PTR_ADDRESS, 2, data) < 0)
			return -1;

		//Read the calibration constants
		if(readMultipleRegistersTCP(sock, FLASH_READ_ADDRESS, FLASH_READ_NUM_REGS[i], data) < 0)
			return -1;

		for(j = 0; j < FLASH_READ_NUM_REGS[i]*2; j+=4)
		{
			bytesToFloat(&data[j], &calValue);
			((float *)devCal)[calIndex]	= calValue;
			calIndex++;
		}
	}
	return 0;
}

int ainBinToVoltsT7(const DeviceCalibrationT7 *devCal, const unsigned char *ainBytes, unsigned int gainIndex, float *volts)
{
	unsigned short rawAIN = 0;
	bytesToUint16(ainBytes, &rawAIN);

	if(gainIndex > 3)
	{
		printf("ainBinToVolts error: Invalid gainIndex %u\n", gainIndex);
		return -1;
	}

	if(*volts < devCal->HS[gainIndex].Center)
		*volts = (devCal->HS[gainIndex].Center - rawAIN) * devCal->HS[gainIndex].NSlope;
	else
		*volts = (rawAIN - devCal->HS[gainIndex].Center) * devCal->HS[gainIndex].PSlope;
	return 0;
}

void getNominalCalibrationT4(DeviceCalibrationT4 *devCal)
{
	devCal->HV[0].Slope = 3.235316E-04;
	devCal->HV[0].Offset = 	-10.532965;
	devCal->HV[1].Slope = 	3.236028E-04;
	devCal->HV[1].Offset = -10.534480;
	devCal->HV[2].Slope = 	3.235439E-04;
	devCal->HV[2].Offset = 	-10.530597;
	devCal->HV[3].Slope = 3.236133E-04;
	devCal->HV[3].Offset = 	-10.530210;
	devCal->LV.Slope = 	3.826692E-05;
	devCal->LV.Offset = 0.002484;
	devCal->SpecV.Slope = -3.839420E-05;
	devCal->SpecV.Offset = 2.507430;
	devCal->DAC[0].Slope = 1.310768E+04;
	devCal->DAC[0].Offset = 54.091066;
	devCal->DAC[1].Slope = 1.310767E+04;
	devCal->DAC[1].Offset = 54.044314;
	devCal->Temp_Slope = -9.260000E+01;
	devCal->Temp_Offset = 467.600000;
	devCal->I_Bias = 0.000000015;

}

int getCalibrationT4(EthernetClient * sock, DeviceCalibrationT4 *devCal)
{
	const unsigned int EFAdd_CalValues = 0x3C4000;
	const int FLASH_PTR_ADDRESS	= 61810;
	
	// 3 frames	of 13 values, one frame	of 2 values
	const int FLASH_READ_ADDRESS = 61812;
	const int FLASH_READ_NUM_REGS[4] = {26, 26, 26, 4};

	float calValue = 0.0;
	int calIndex = 0;
	unsigned char data[52];
	int i = 0;
	int j = 0;

	for(i = 0; i < 4; i++)
	{
		//Set the pointer. This	indicates which	part of the memory we want to read
		uint32ToBytes(EFAdd_CalValues + i * 13 * 4, data);
		if(writeMultipleRegistersTCP(sock, FLASH_PTR_ADDRESS, 2, data) < 0)
			return -1;

		//Read the calibration constants
		if(readMultipleRegistersTCP(sock, FLASH_READ_ADDRESS, FLASH_READ_NUM_REGS[i], data) < 0)
			return -1;

		for(j = 0; j < FLASH_READ_NUM_REGS[i]*2; j+=4)
		{
			bytesToFloat(&data[j], &calValue);
			((float *)devCal)[calIndex]	= calValue;
			calIndex++;
		}
	}
	return 0;
}

int ainBinToVoltsT4(const DeviceCalibrationT4 *devCal, const unsigned char *ainBytes, unsigned int ainNum, float *volts)
{
	unsigned short rawAIN = 0;
	bytesToUint16(ainBytes, &rawAIN);

	*volts=(rawAIN)*devCal->HV[ainNum].Slope+devCal->HV[ainNum].Offset;
	return 0;
}

