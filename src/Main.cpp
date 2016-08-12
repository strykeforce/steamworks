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
#include "TerminalEscapes.h"
#include "RIOUtils.h"

std::vector<JerrysGrapher_DeviceBundle> GraphableDevices;
std::unordered_map<int,CANTalon*> AllTalons;
std::unordered_map<int,DigitalBoth> AllDigitalBoths;

std::vector<CANTalonBundle> TalonsCurrentlyUsing;

#define ForEveryUsingTalon(__Command__)            \
	for(int i = 0;i<TalonsCurrentlyUsing.size();i++)    \
	{                                          \
		TalonsCurrentlyUsing[i].TalonRef->__Command__;      \
	}                                          \

#define ForEveryUsingTalonMUTI(__Commands__)                        \
	for(int i = 0;i<TalonsCurrentlyUsing.size();i++)            \
	{                                                           \
		int TalonID = TalonsCurrentlyUsing[i].TalonID;      \
		CANTalon* Talon = TalonsCurrentlyUsing[i].TalonRef; \
		__Commands__;                                       \
	}                                                           \

double query(char* name)
{
	printf("%s: ", name);
	char rawnumber[9];
	fgets(rawnumber, 9, stdin);
	return atof(rawnumber);
}

typedef void (*ParamDoubleFunction) (double);

void query2(char* name, ParamDoubleFunction func, bool repeat)
{
	char rawnumber[9];
	do
	{
		printf("%s (Use 'B' or 'b' to escape): ", name);
		fgets(rawnumber, 9, stdin);
		for(int i = 0;i<strlen(rawnumber);i++)
			if(rawnumber[i] == 'b' || rawnumber[i] == 'B')
				return;
		double val = atof(rawnumber);
		func(val);
	}
	while(repeat);
}

void query2(char* name, ParamDoubleFunction func)
{
	query2(name, func, true);
}

#include "OneLiners.cpp"

typedef void (*VoidFunction) (void);

void menu(int length, char* options[], char* title, VoidFunction* funcs, bool loop);

void inline Padding(int length, char c)
{
	for(int i = 0;i<length;i++) printf("%c", c);
}

void ChangeTalons()
{
	while(true)
	{
		int ID = (int) query((char*) "Talon ID [0]");
		CANTalon* t = PossiblyCreateTalon(&GraphableDevices, &AllTalons, ID);
		uint32_t verison = t->GetFirmwareVersion();
		if(verison == 0)
		{
			printf("WARNING: Talon does not exist!\n");
		}
		printf("version == %i\n", verison);
		CANTalonBundle bun;
		bun.TalonID = ID;
		bun.TalonRef = t;
		TalonsCurrentlyUsing.push_back(bun);
		return;
	}
}
void SingularTalon()
{
	TalonsCurrentlyUsing.clear();
	ChangeTalons();
}
void SetEncoderDeviceType()
{
	char* FunctionNames[] =  {"QuadEncoder", "AnalogPot", "AnalogEncoder", "CtreMagEncoder_Relative", "CtreMagEncoder_Absolute", "PulseWidth",};
	VoidFunction FunctionPointers[] = {SetDT_QuadEncoder, SetDT_AnalogPot, SetDT_AnalogEncoder, SetDT_CtreMagEncoder_Relative, SetDT_CtreMagEncoder_Absolute, SetDT_PulseWidth, };
	menu(6, FunctionNames, "Which Encoder Type?", FunctionPointers, false);
}
void SetDirectionE()
{
	char* FunctionNames[] =  {(char*) "Forward", (char*) "Reverse"};
	VoidFunction FunctionPointers[] = {SetDirectionEF, SetDirectionER};
	menu(2, FunctionNames, (char*)"Set Encoder Direction", FunctionPointers, false);
}
void SetDirectionO()
{
	char* FunctionNames[] =  {(char*) "Forward", (char*) "Reverse"};
	VoidFunction FunctionPointers[] = {SetDirectionOF, SetDirectionOR};
	menu(2, FunctionNames, (char*)"Set Output Direction", FunctionPointers, false);
}
void SetBrakeCoastMode()
{
	char* FunctionNames[] =  {(char*) "Brake", (char*) "Coast"};
	VoidFunction FunctionPointers[] = {SetBrakeCoastModeB, SetBrakeCoastModeC};
	menu(2, FunctionNames, (char*)"Set Brake/Coast Mode", FunctionPointers, false);
}
void EDLimitSwitches()
{
	char* FunctionNames[] = {(char*) "Enable Hard and Soft Limits",
				 (char*) "Enable Hard limits only",
	                         (char*) "Disable all Limit Switches"};
	VoidFunction FunctionPointers[] = {Limit1, Limit2, Limit3};
	menu(3, FunctionNames, (char*)"Enable/Disable Hard Limit Switches", FunctionPointers, false);
}
void ConfigForwardHLimit()
{
	char* FunctionNames[] = {(char*) "Normally Open",
	                         (char*) "Normally Close"};
	VoidFunction FunctionPointers[] = {OpenFHLimit, CloseFHLimit};
	menu(2, FunctionNames, (char*)"Open/Close Forward Hard Limit Switch", FunctionPointers, false);
}
void ConfigReverseHLimit()
{
	char* FunctionNames[] = {(char*) "Normally Open",
	                         (char*) "Normally Close"};
	VoidFunction FunctionPointers[] = {OpenRHLimit, CloseRHLimit};
	menu(2, FunctionNames, (char*)"Open/Close Reverse Hard Limit Switch", FunctionPointers, false);
}
void SetEncoderDirection()
{
	char* FunctionNames[] = {(char*) "Forward",
	                         (char*) "Reverse"};
	VoidFunction FunctionPointers[] = {SetDirectionEF, SetDirectionER};
	menu(2, FunctionNames, (char*)"Set Encoder Direction", FunctionPointers, false);
}
void SetOutputDirection()
{
	char* FunctionNames[] = {(char*) "Forward",
	                         (char*) "Reverse"};
	VoidFunction FunctionPointers[] = {SetDirectionOF, SetDirectionOR};
	menu(2, FunctionNames, (char*)"Set Output Direction", FunctionPointers, false);
}
void GetDIO_Pin()
{
	int ID = 0;
	while(!(0 <= (ID = (int) query("DigitalInput pin")) && ID <= 9));
	printf("ID == %i\n", ID);
	DigitalInput* t = PossiblyCreateDigitalInput(&GraphableDevices, &AllDigitalBoths, ID);
	printf("Value == %i\n", t->Get());
}
void SetDIO_Pin()
{
	int ID = 0;
	while(!(0 <= (ID = (int) query("DigitalOutput pin")) && ID <= 9));
	printf("ID == %i\n", ID);
	int value = (int) query("Value?");
	DigitalOutput* t = PossiblyCreateDigitalOutput(&qGraphableDevices, &AllDigitalBoths, ID);
	t->Set(value);
	printf("Value == %i\n", value);
}
void SetControlMode()
{
	char* FunctionNames[] =  {(char*) "Percent Voltage (-1 to 1)",
	                          (char*) "Position (Encoder Counts)",
	                          (char*) "Speed (Encoder Counts per Tenth of Second)",
	                          (char*) "Voltage (Volts)",
	                          (char*) "Slave"};
	VoidFunction FunctionPointers[] = {SetControlModePV, SetControlModeP, SetControlModeSp, SetControlModeV, SetControlModeSl};
	menu(5, FunctionNames, (char*)"Select New Control Mode", FunctionPointers, false);
}
void SetStatusFrameRates()
{
	char* FunctionNames[] =  {(char*) "General (Pin States, Control Mode, Throttle)",
	                          (char*) "Feedback (Current, Voltage)",
	                          (char*) "Quadrature Encoder (Encoder Position, Encoder Velocity)",
	                          (char*) "Analog Input / Temperature / Battery Voltage"};
	VoidFunction FunctionPointers[] = {SetStatusFrameRates_G, SetStatusFrameRates_F, SetStatusFrameRates_Q, SetStatusFrameRates_A};
	menu(4, FunctionNames, (char*)"Select Status Frame", FunctionPointers, false);
}

void Set()
{
	char* FunctionNames[] =
	{
		"P" ,"I" ,"D" ,"F", "I Zone" ,
		"Control Mode",
		"Forward Peak Closed Loop Output",
		"Reverse Peak Closed Loop Output",
		"Forward Nominal Closed Loop Output",
		"Reverse Nominal Closed Loop Output" ,
		"Voltage Ramp Rate",
		"CloseLoop Ramp Rate",
		"Allowable Closed Loop Error",
		"Enable/Disable Hard Limit Switches",
		"Normally Open/Close Forward Hard Limit Switch",
		"Normally Open/Close Reverse Hard Limit Switch",
		"Forward Soft Limit",
		"Reverse Soft Limit",
		"Sensor Selection",
		"Sensor Position",
		"Encoder Direction",
		"Output Direction",
		"DIO Pin",
		"Status Frame Rates"
	};
	VoidFunction FunctionPointers[] = 
	{
		SetP, SetI, SetD, SetF, SetIzone,
		SetControlMode,
		SetPeakCloseLoopOutForward,
		SetPeakCloseLoopOutReverse,
		SetNominalCloseOutForward,
		SetNominalCloseOutForward,
		SetVoltageRampRate,
		SetSetCloseLoopRampRate,
		SetAllowCloseLoopErr,
		EDLimitSwitches,
		ConfigForwardHLimit,
		ConfigReverseHLimit,
		ConfigSoftLimitForward,
		ConfigSoftLimitReverse,
		SetEncoderDeviceType,
		SetQPosition,
		SetEncoderDirection,
		SetOutputDirection,
		SetDIO_Pin,
		SetStatusFrameRates
	};
	menu(24, FunctionNames, "Set What?", FunctionPointers, true);
}
void Get()
{
	char* FunctionNames[] =
	{
		"All Talon Registors",
		"DIO Pin",
	};
	VoidFunction FunctionPointers[] =
	{
		GetTalonRegistors,
		GetDIO_Pin,
	};
	menu(2, FunctionNames, "Get What?", FunctionPointers, true);
}
class Robot : public SampleRobot
{
	public:
	Robot()
	{
		JerrysGrapher_StartStatusThread(&GraphableDevices);
		int nullfd = open("/dev/null", O_WRONLY | O_CREAT);
		dup2(nullfd, 2);
	}
	void OperatorControl()
	{
		Ter_Esc_SetForgroundColor(Terminal_Escape_Color::Default);
		Ter_Esc_ClearScreen();
		printf("\n");
		printf("Welcome to BDC_Comm v6.\n");
		SingularTalon();
		char* FunctionNames[] =  {"Select One Single Talon",
		                          "Select Additional Talon",
		                          "Set Commanded Value",
		                          "Set",
		                          "Get"};
		VoidFunction FunctionPointers[] = {SingularTalon,
		                                   ChangeTalons,
		                                   SetCommandedValue,
		                                   Set,
		                                   Get};
		while(true)
		{
			menu(5, FunctionNames, "Select Action", FunctionPointers, true);
			printf("You cannot leave root menu.\n");
			printf("If you want to leave BDC_Comm, type in 'q', or 'e'.\n");
		}
	}
	void Autonomous()
	{
		printf("Autonomous()\n");
		Ter_Esc_ClearScreen();
		char* art[] = {
			" BBBB      DDDDD        CCCCC           CCCCC                                 ",
			" B   BB    D    DD    CCC             CCC                                     ",
			" B    B    D     D    CC              CC                                      ",
			" BBBBB     D     DD   C               C           OOOO     MMMMM     MMMMMM   ",
			" B    B    D     D    CC              CC         O    O   M  M  M   M   M  M  ",
			" B   BB    D    DD    CCC             CCC        O    O   M  M  M   M   M  M  ",
			" BBBB      DDDDD        CCCCC           CCCCC     OOOO    M  M  M   M   M  M  "};
		
		while(DriverStation::GetInstance().IsAutonomous())
		{
			for(int i = 0;i<10;i++)
			{
				int row = (int) ((float) random() / RAND_MAX * 7);
				int col = (int) ((float) random() / RAND_MAX * strlen(art[row]));
				int color = (int) ((float) random() / RAND_MAX * 10);
				Ter_Esc_MoveCursorTo(col+4, row+4);
				Ter_Esc_SetForgroundColor((Terminal_Escape_Color)color);
				printf("%c", art[row][col]);
			}
			printf("\n");
			msleep(150);
		}
	}
	void Disabled()
	{
		printf("Please Enable.\n");
	}
};


void menu(int length, char* options[], char* title, VoidFunction* funcs, bool loop)
{
	top:
	do
	{
		//TER_ESC_CLEAR();
		#define SpacePadding 8
		int BannerLength = strlen(title) + SpacePadding * 2 + 7 + TalonsCurrentlyUsing.size();
		Padding(BannerLength, '-');
		printf("----------\n");
		Padding(SpacePadding, ' ');
		printf("%s (Talon ", title);
		for(int i = 0;i<TalonsCurrentlyUsing.size();i++)
		{
			printf("%i", TalonsCurrentlyUsing[i].TalonID);
			if(i < TalonsCurrentlyUsing.size() - 1)
				printf(",");
		}
		printf(")\n");
		Padding(BannerLength, '-');
		printf("----------\n");
		for(int i = 0;i<length;i++)
		{
			Padding(SpacePadding/2, ' ');
			printf("%3i - %s\n", i + 1, options[i]);
		}
		int OptionIndex;
		bool isVaild = false;
		while(!isVaild)
		{
			printf("> ");
			char selection[9];
			fgets(selection, 9, stdin);
			selection[strlen(selection) - 1] = '\0';
			if(strcmp(selection, "b") == 0) return;
			if(strcmp(selection, "c") == 0)
			{
				Ter_Esc_ClearScreen();
				goto top;
			}
			if(strcmp(selection, "e") == 0 ||
			   strcmp(selection, "q") == 0) exit(0);	
			OptionIndex = atoi(selection);
			isVaild = 1 <= OptionIndex && OptionIndex <= length;
			if(!isVaild)
			{
				printf("Please type in a Valid Number.\n");
			}
		}
		funcs[OptionIndex-1]();
	}
	while(loop);
}
START_ROBOT_CLASS(Robot);
