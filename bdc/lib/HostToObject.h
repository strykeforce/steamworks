#ifndef HOST_TO_OBJECT_H
	#define HOST_TO_OBJECT_H
	#include "CodeUtils.h"
	#include <vector>
	#include <unordered_map>

	enum JerrysGrapher_DeviceType : byte
	{
		Talon = 0,
		DIO,
		RoboRIO,
	};
	struct JerrysGrapher_DeviceBundle
	{
		JerrysGrapher_DeviceType type;
		byte ID;
		void* objectPointer;
	};

	void JerrysGrapher_StartStatusThread(std::vector<JerrysGrapher_DeviceBundle>* JerrysGrapher_GraphableDevices);

	int JerrysGrapher_IndexOfBundleWithID(std::vector<JerrysGrapher_DeviceBundle>* JerrysGrapher_GraphableDevices,JerrysGrapher_DeviceType target_device, int id);
#endif
