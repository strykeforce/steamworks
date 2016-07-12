#include "WPILib.h"
#include <cstdlib>
#include <pthread.h>
#include <thread>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <unordered_map>
#include "UDPHandler.cpp"
#include "JerrysGrapherInterface.h"

#define Query(__String__)\
	query((char*) __String__);

#define ForEveryUsingTalon(__Command__)            \
	for(int i = 0;i<TalonsCurrentlyUsing.size();i++)    \
	{                                          \
		TalonsCurrentlyUsing[i].TalonRef->__Command__;      \
	}                                          \

#define ForEveryUsingTalonMUTI(__Commands__)        \
	for(int i = 0;i<TalonsCurrentlyUsing.size();i++)     \
	{                                           \
		CANTalon* Talon = TalonsCurrentlyUsing[i].TalonRef;  \
		__Commands__;                       \
	}                                           \

std::unordered_map<int,CANTalon*> RU_AllTalons;
std::vector<CANTalonBundle> TalonsCurrentlyUsing;

typedef void (*VoidFunction) (void);

void menu(int length, char* options[], char* title, VoidFunction* funcs, bool loop);

double query(char* name)
{
	fprintf(stderr, "%s: ", name);
	double d;
	std::cin >> d;
	return d;
}
void inline Padding(int length, char c)
{
	for(int i = 0;i<length;i++) fprintf(stderr, "%c", c);
}

void ChangeTalons()
{
	while(true)
	{
		int ID = (int) query((char*) "Talon ID");
		CANTalon* t;
		if(RU_AllTalons[ID] == NULL)
		{
			fprintf(stderr, "New Talon #%i\n", ID);
			t = RU_AllTalons[ID] = new CANTalon(ID);
			t->SetControlMode(CANTalon::ControlMode::kPercentVbus);
			t->ConfigLimitMode(CANTalon::LimitMode::kLimitMode_SrxDisableSwitchInputs);
			t->SetFeedbackDevice(CANTalon::FeedbackDevice::QuadEncoder);
			t->SetSafetyEnabled(false);
			t->SetIzone(0);
			t->Set(0);
			t->EnableControl();
		}
		else
		{
			t = RU_AllTalons[ID];
		}
		uint32_t verison = t->GetFirmwareVersion();
		fprintf(stderr, "version == %X\n", verison);
		if(verison == 0 && false)
		{
			fprintf(stderr, "CANTalon not availible, try again.\n");
		}
		else
		{
			CANTalonBundle bun;
			bun.TalonID = ID;
			bun.TalonRef = t;
			TalonsCurrentlyUsing.push_back(bun);
			return;
		}
	}
}
void SingularTalon()
{
	TalonsCurrentlyUsing.clear();
	ChangeTalons();
}
void SetControlModePV() { ForEveryUsingTalon(SetControlMode(CANTalon::ControlMode::kPercentVbus)); }
void SetControlModeV()  { ForEveryUsingTalon(SetControlMode(CANTalon::ControlMode::kVoltage)); }
void SetControlModeP()  { ForEveryUsingTalon(SetControlMode(CANTalon::ControlMode::kPosition)); }
void SetControlModeSp() { ForEveryUsingTalon(SetControlMode(CANTalon::ControlMode::kSpeed)); }
void SetControlModeSl()
{
	int ID = (int) query((char*) "Master Talon ID");
	ForEveryUsingTalon(SetControlMode(CANTalon::ControlMode::kFollower));
	ForEveryUsingTalon(Set(ID));
}
void SetControlMode()
{
	char* FunctionNames[] =  {(char*) "Percent Voltage (-1 to 1)",
	                          (char*) "Position (Encoder Counts per tenth of second)",
	                          (char*) "Speed (Encoder Counts per Second, or Rotations per Second)",
	                          (char*) "Voltage (Volts)",
	                          (char*) "Slave"};
	VoidFunction FunctionPointers[] = {SetControlModePV, SetControlModeP, SetControlModeSp, SetControlModeV, SetControlModeSl};
	menu(5, FunctionNames, (char*)"Select New Control Mode", FunctionPointers, false);
}
void SetP() { double P = query((char*)"New P Value"); ForEveryUsingTalon(SetP(P));}
void SetI() { double I = query((char*)"New I Value"); ForEveryUsingTalon(SetI(I));}
void SetD() { double D = query((char*)"New D Value"); ForEveryUsingTalon(SetD(D));}
void SetF() { double F = query((char*)"New F Value"); ForEveryUsingTalon(SetF(F));}
void SetIzone() { unsigned iz = query((char*)"New I Zone Value"); ForEveryUsingTalon(SetIzone(iz));}
void SetPIDF()
{
	char* FunctionNames[] =  {(char*) "Set P",
	                          (char*) "Set I",
                                  (char*) "Set D",
	                          (char*) "Set F",
                                  (char*) "Set I Zone"};
	VoidFunction FunctionPointers[] = {SetP,
	                                   SetI,
	                                   SetD,
	                                   SetF,
	                                   SetIzone};
	menu(5, FunctionNames, (char*)"Set Which One?", FunctionPointers, true);
}
void SetVoltageRampRate()
{
	double vrate = query((char*)"New Voltage Ramp Rate Value");
	ForEveryUsingTalon(SetVoltageRampRate(vrate));
}
void SetAllowCloseLoopErr()
{
	int allCloseLE = query((char*)"Set New Allowable Closed Loop Error");
	ForEveryUsingTalon(SetAllowableClosedLoopErr(allCloseLE));
}
void SetPeakCloseLoopOut()
{
	double peakCloseLO1 = query((char*)"Set Forward Peak Closed Loop Output");
	double peakCloseLO2 = query((char*)"Set Reverse Peak Closed Loop Output");
	ForEveryUsingTalon(ConfigPeakOutputVoltage(peakCloseLO1, peakCloseLO2));
}
void SetNominalCloseOut()
{
	double nominalCloseLO1 = query((char*)"Set Forward Nominal Closed Loop Output");
	double nominalCloseLO2 = query((char*)"Set Reverse Nominal Closed Loop Output");
	ForEveryUsingTalon(ConfigNominalOutputVoltage(nominalCloseLO1, nominalCloseLO2));
}
void SetCloseLoopRampRate()
{
	double crate = query((char*)"New Closed Loop Ramp Rate");
	ForEveryUsingTalon(SetCloseLoopRampRate(crate));
}
void SetControlLoop()
{
	char* FunctionNames[] =  {(char*) "Set P, I, D, or I Zone",
	                          (char*) "Set Voltage Ramp Rate (Output)",
	                          (char*) "Set Allowable Closed Loop Error",
	                          (char*) "Set Peak Closed Loop Output",
				  (char*) "Set Nominal Closed Loop Output",
				  (char*) "Set Closed Loop Ramp Rate (Input)",};
	VoidFunction FunctionPointers[] = {SetPIDF,
	                                   SetVoltageRampRate,
	                                   SetAllowCloseLoopErr,
	                                   SetPeakCloseLoopOut,
	                                   SetNominalCloseOut,
				           SetCloseLoopRampRate};
	menu(6, FunctionNames, (char*)"Set Which One?", FunctionPointers, true);
}
void GetPID()
{
	for(int i = 0;i<TalonsCurrentlyUsing.size();i++)
	{
		fprintf(stderr, "Talon %i\n", TalonsCurrentlyUsing[i].TalonID);
		fprintf(stderr, "\tP:     %f\n", TalonsCurrentlyUsing[i].TalonRef->GetP());
		fprintf(stderr, "\tI:     %f\n", TalonsCurrentlyUsing[i].TalonRef->GetI());
		fprintf(stderr, "\tD:     %f\n", TalonsCurrentlyUsing[i].TalonRef->GetD());
		fprintf(stderr, "\tF:     %f\n", TalonsCurrentlyUsing[i].TalonRef->GetF());
		fprintf(stderr, "\tIZone: %i\n", TalonsCurrentlyUsing[i].TalonRef->GetIzone());
	}
}
void Set()
{
	double set = query((char*)"New Set Value");
	ForEveryUsingTalon(Set(set));
}
void SetPosition()
{
	double set = query((char*)"New Encoder Value");
	ForEveryUsingTalon(SetPosition(set));
	fprintf(stderr, "CANTalon->SetPosition(%f);\n", set);
}
void SetDirectionEF() { ForEveryUsingTalon(SetSensorDirection(false)); }
void SetDirectionER() { ForEveryUsingTalon(SetSensorDirection(true)); }
void SetDirectionE()
{
	char* FunctionNames[] =  {(char*) "Forward", (char*) "Reverse"};
	VoidFunction FunctionPointers[] = {SetDirectionEF, SetDirectionER};
	menu(2, FunctionNames, (char*)"Set Encoder Direction", FunctionPointers, false);
}
void SetDirectionOF() { ForEveryUsingTalon(SetInverted(false)); }
void SetDirectionOR() { ForEveryUsingTalon(SetInverted(true)); }
void SetDirectionO()
{
	char* FunctionNames[] =  {(char*) "Forward", (char*) "Reverse"};
	VoidFunction FunctionPointers[] = {SetDirectionOF, SetDirectionOR};
	menu(2, FunctionNames, (char*)"Set Output Direction", FunctionPointers, false);
}
void SetBrakeCoastModeB() { ForEveryUsingTalon(ConfigNeutralMode(CANTalon::NeutralMode::kNeutralMode_Brake)); }
void SetBrakeCoastModeC() { ForEveryUsingTalon(ConfigNeutralMode(CANTalon::NeutralMode::kNeutralMode_Coast)); }
void SetBrakeCoastMode()
{
	char* FunctionNames[] =  {(char*) "Brake", (char*) "Coast"};
	VoidFunction FunctionPointers[] = {SetBrakeCoastModeB, SetBrakeCoastModeC};
	menu(2, FunctionNames, (char*)"Set Brake/Coast Mode", FunctionPointers, false);
}
void Limit1(){ ForEveryUsingTalon(ConfigLimitMode(CANTalon::LimitMode::kLimitMode_SoftPositionLimits)); }
void Limit2(){ ForEveryUsingTalon(ConfigLimitMode(CANTalon::LimitMode::kLimitMode_SwitchInputsOnly)); }
void Limit3(){ ForEveryUsingTalon(ConfigLimitMode(CANTalon::LimitMode::kLimitMode_SrxDisableSwitchInputs)); }
void EDLimitSwitches()
{
	char* FunctionNames[] = {(char*) "Enable Hard and Soft Limits",
				 (char*) "Enable Hard limits only",
	                         (char*) "Disable all Limit Switches"};
	VoidFunction FunctionPointers[] = {Limit1, Limit2, Limit3};
	menu(3, FunctionNames, (char*)"Enable/Disable Hard Limit Switches", FunctionPointers, false);
}
void OpenFHLimit(){ ForEveryUsingTalon(ConfigFwdLimitSwitchNormallyOpen(true)); }
void CloseFHLimit(){ ForEveryUsingTalon(ConfigFwdLimitSwitchNormallyOpen(false)); }
void ConfigForwardHLimit()
{
	char* FunctionNames[] = {(char*) "Normally Open",
	                         (char*) "Normally Close"};
	VoidFunction FunctionPointers[] = {OpenFHLimit, CloseFHLimit};
	menu(2, FunctionNames, (char*)"Open/Close Forward Hard Limit Switch", FunctionPointers, false);
}
void OpenRHLimit(){ ForEveryUsingTalon(ConfigRevLimitSwitchNormallyOpen(true)); }
void CloseRHLimit(){ ForEveryUsingTalon(ConfigRevLimitSwitchNormallyOpen(false)); }
void ConfigReverseHLimit()
{
	char* FunctionNames[] = {(char*) "Normally Open",
	                         (char*) "Normally Close"};
	VoidFunction FunctionPointers[] = {OpenRHLimit, CloseRHLimit};
	menu(2, FunctionNames, (char*)"Open/Close Reverse Hard Limit Switch", FunctionPointers, false);
}
void ConfigSoftLimit()
{
	double fset = query((char*)"Forward Soft Limit");
	double rset = query((char*)"Reverse Soft Limit");
	ForEveryUsingTalon(ConfigSoftPositionLimits(fset, rset));
}

void ConfigureEncoder()
{
	char* FunctionNames[] =  {(char*) "Set Current Encoder Position",
	                          (char*) "Change Encoder Direction",
	                          (char*) "Change Output Direction",
	                          (char*) "Set Brake/Coast Mode",
	                          (char*) "Enable/Disable Limit Switches",
	                          (char*) "Configure Forward Hard Limit",
	                          (char*) "Configure Reverse Hard Limit",
	                          (char*) "Set Soft Limit Positions",};
	VoidFunction FunctionPointers[] = {SetPosition,
	                                   SetDirectionE,
	                                   SetDirectionO,
	                                   SetBrakeCoastMode,
	                                   EDLimitSwitches,
	                                   ConfigForwardHLimit,
	                                   ConfigReverseHLimit,
	                                   ConfigSoftLimit};
	menu(8, FunctionNames, (char*)"Select Action", FunctionPointers, true);
}
class Robot : public SampleRobot
{
	public:
	Robot()
	{
		StartStatusThread(&TalonsCurrentlyUsing);
	}
	void OperatorControl()
	{
		fprintf(stderr, "OperatorControl()\n");
		SingularTalon();
		
		char* FunctionNames[] =  {(char*) "Select One Single Talon",
		                          (char*) "Select Additional Talon",
		                          (char*) "Set Control Mode",
		                          (char*) "Set Control Loop Values",
		                          (char*) "Get Control Loop Values",
		                          (char*) "Set Commanded Value",
		                          (char*) "Configure Encoder/Talon"};
		VoidFunction FunctionPointers[] = {SingularTalon,
		                                   ChangeTalons,
		                                   SetControlMode,
		                                   SetControlLoop,
		                                   GetPID,
		                                   Set,
		                                   ConfigureEncoder};
		while(true)
		{
			menu(7, FunctionNames, (char*)"Select Action", FunctionPointers, true);
			fprintf(stderr, "You cannot leave root menu.\n");
			fprintf(stderr, "If you want to leave BDC_Comm, type in 'q', or 'e'.\n");
		}
	}
	void Disabled()
	{
		fprintf(stderr, "Please Enable.\n");
	}
};


void menu(int length, char* options[], char* title, VoidFunction* funcs, bool loop)
{
	do
	{
		#define SpacePadding 8
		int BannerLength = strlen(title) + SpacePadding * 2 + 7 + TalonsCurrentlyUsing.size();
		Padding(BannerLength, '-');
		fprintf(stderr, "----------\n");
		Padding(SpacePadding, ' ');
		fprintf(stderr, "%s (Talon ", title);
		for(int i = 0;i<TalonsCurrentlyUsing.size();i++)
		{
			fprintf(stderr, "%i", TalonsCurrentlyUsing[i].TalonID);
			if(i < TalonsCurrentlyUsing.size() - 1)
				fprintf(stderr, ",");
		}
		fprintf(stderr, ")\n");
		Padding(BannerLength, '-');
		fprintf(stderr, "----------\n");
		for(int i = 0;i<length;i++)
		{
			Padding(SpacePadding/2, ' ');
			fprintf(stderr, "%x - %s\n", i + 1, options[i]);
		}
		int OptionIndex;
		bool isVaild = false;
		while(!isVaild)
		{
			fprintf(stderr, ">");
			char c;
			std::cin >> c;
			if(c == 'b') return;
			if(c == 'e' || c == 'q') exit(0);
			OptionIndex = c - '0';
			isVaild = 1 <= OptionIndex && OptionIndex <= length;
			if(!isVaild)
			fprintf(stderr, "Please type in a Valid Number.\n");
		}
		funcs[OptionIndex-1]();
	}
	while(loop);
}
START_ROBOT_CLASS(Robot);
