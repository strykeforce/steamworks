#ifndef RIOUTILS_H
	#define RIOUTILS_H
	#include <vector>
	#include <unordered_map>
	#include "DigitalInput.h"
	#include "DigitalOutput.h"
	#include "CANTalon.h"
	#include "CodeUtils.h"
	#include "HostToObject.h"
	struct DigitalBoth
	{
		bool isInput;
		union
		{
			DigitalInput* input;
			DigitalOutput* output;
		};
	};
	struct CANTalonBundle
	{
		byte TalonID;
		CANTalon* TalonRef;
	};
	CANTalon* PossiblyCreateTalon(std::vector<JerrysGrapher_DeviceBundle>* JerrysGrapher_GraphableDevices,
		std::unordered_map<int,CANTalon*>* AllTalons, int ID);
	DigitalInput* PossiblyCreateDigitalInput(std::vector<JerrysGrapher_DeviceBundle>* JerrysGrapher_GraphableDevices,
		std::unordered_map<int,DigitalBoth>* AllDigitalBoths, int ID);
	DigitalOutput* PossiblyCreateDigitalOutput(std::vector<JerrysGrapher_DeviceBundle>* JerrysGrapher_GraphableDevices,
		std::unordered_map<int,DigitalBoth>* AllDigitalBoths, int ID);
#endif
