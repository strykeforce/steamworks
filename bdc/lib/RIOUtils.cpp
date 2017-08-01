#include "RIOUtils.h"

CANTalon* PossiblyCreateTalon(std::vector<JerrysGrapher_DeviceBundle>* JerrysGrapher_GraphableDevices,
	std::unordered_map<int,CANTalon*>* AllTalons, int ID)
{
	CANTalon* t;
	if(AllTalons->operator[](ID) == NULL)
	{
		t = AllTalons->operator[](ID) = new CANTalon(ID);
		t->SetControlMode(CANTalon::ControlMode::kPercentVbus);
		t->ConfigLimitMode(CANTalon::LimitMode::kLimitMode_SrxDisableSwitchInputs);
		t->SetFeedbackDevice(CANTalon::FeedbackDevice::QuadEncoder);
		t->SetSafetyEnabled(false);
		t->Set(0);
		t->EnableControl();
		JerrysGrapher_DeviceBundle db;
		db.type = JerrysGrapher_DeviceType::Talon;
		db.ID = ID;
		db.objectPointer = t;
		JerrysGrapher_GraphableDevices->push_back(db);
	}
	else
	{
		t = AllTalons->operator[](ID);
	}
	return t;
}
DigitalInput* PossiblyCreateDigitalInput(std::vector<JerrysGrapher_DeviceBundle>* JerrysGrapher_GraphableDevices,
	std::unordered_map<int,DigitalBoth>* AllDigitalBoths, int ID)
{
	DigitalBoth* db = &(AllDigitalBoths->operator[](ID));
	if(!db->isInput)
	{
		delete(db->output); 
		db->output = NULL;
	}
	DigitalInput* t;
	if(db->input == NULL)
	{
		t = db->input = new DigitalInput(ID);
		db->isInput = true;
		JerrysGrapher_DeviceBundle bun;
		bun.type = JerrysGrapher_DeviceType::DIO;
		bun.ID = ID;
		bun.objectPointer = t;
		JerrysGrapher_GraphableDevices->push_back(bun);
	}
	else
	{
		t = db->input;
	}
	return t;
}

DigitalOutput* PossiblyCreateDigitalOutput(std::vector<JerrysGrapher_DeviceBundle>* JerrysGrapher_GraphableDevices,
	std::unordered_map<int,DigitalBoth>* AllDigitalBoths, int ID)
{
	DigitalBoth* db = &(AllDigitalBoths->operator[](ID));
	if(db->isInput)
	{
		delete(db->input); 
		db->input = NULL;
		JerrysGrapher_GraphableDevices->erase(JerrysGrapher_GraphableDevices->begin() + JerrysGrapher_IndexOfBundleWithID(JerrysGrapher_GraphableDevices, JerrysGrapher_DeviceType::DIO, ID));
	}
	DigitalOutput* t;
	if(db->output == NULL)
	{
		t = db->output = new DigitalOutput(ID);
		db->isInput = false;
	}
	else
	{
		t = db->output;
	}
	return t;
}




















