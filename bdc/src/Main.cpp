// Copyright 2016 StrykeForce2767

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "WPILib.h"
#include "ctrlib/CANTalon.h"

#include "RIOUtils.h"
#include "TerminalEscapes.h"

std::vector<JerrysGrapher_DeviceBundle> GraphableDevices;
std::unordered_map<int, CANTalon*> AllTalons;
std::unordered_map<int, DigitalBoth> AllDigitalBoths;

std::vector<CANTalonBundle> TalonsCurrentlyUsing;

auto pulse_ms = std::chrono::milliseconds(1000);

#define ForEveryUsingTalon(__Command__)                   \
  for (int i = 0; i < TalonsCurrentlyUsing.size(); i++) { \
    TalonsCurrentlyUsing[i].TalonRef->__Command__;        \
  }

#define ForEveryUsingTalonMUTI(__Commands__)              \
  for (int i = 0; i < TalonsCurrentlyUsing.size(); i++) { \
    int TalonID = TalonsCurrentlyUsing[i].TalonID;        \
    CANTalon* Talon = TalonsCurrentlyUsing[i].TalonRef;   \
    __Commands__;                                         \
  }

double query(char* name) {
  printf("%s: ", name);
  char rawnumber[9];
  fgets(rawnumber, 9, stdin);
  return atof(rawnumber);
}

typedef void (*ParamDoubleFunction)(double);

void query2(char* name, ParamDoubleFunction func, bool repeat) {
  char rawnumber[9];
  do {
    printf("%s (Use 'B' or 'b' to escape): ", name);
    fgets(rawnumber, 9, stdin);
    for (int i = 0; i < strlen(rawnumber); i++)
      if ((rawnumber[i] == 'b') || (rawnumber[i] == 'B')) return;
    double val = atof(rawnumber);
    func(val);
  } while (repeat);
}

void query2(char* name, ParamDoubleFunction func) { query2(name, func, true); }

double tempvar1A = 0, tempvar1B = 0;
double tempvar2A = 0, tempvar2B = 0;
double tempvar3A = 0, tempvar3B = 0;
void SetP() {
  double P = query("New P Value");
  ForEveryUsingTalon(SetP(P));
}
void SetI() {
  double I = query("New I Value");
  ForEveryUsingTalon(SetI(I));
}
void SetD() {
  double D = query("New D Value");
  ForEveryUsingTalon(SetD(D));
}
void SetF() {
  double F = query("New F Value");
  ForEveryUsingTalon(SetF(F));
}
void SetIzone() {
  unsigned iz = query("New I Zone Value");
  ForEveryUsingTalon(SetIzone(iz));
}

void SetCommandedValue_Q(double val) { ForEveryUsingTalon(Set(val)); }
void SetCommandedValue() { query2("New Commanded Value", SetCommandedValue_Q); }

void SetPulseLength() {
  double p = query("New pulse length (sec)");
  p = std::round(p * 1000);
  pulse_ms = std::chrono::milliseconds(static_cast<long>(p));
}

void PulseCommandedValue_Q(double val) {
  ForEveryUsingTalon(Set(val));
  std::this_thread::sleep_for(pulse_ms);
  ForEveryUsingTalon(Set(0.0));
}
void PulseCommandedValue() {
  query2("Pulse Commanded Value", PulseCommandedValue_Q);
}

void SetPeakCloseLoopOutForward() {
  tempvar1A = query("New Forward Peak Closed Loop Output");
  ForEveryUsingTalon(ConfigPeakOutputVoltage(tempvar1A, tempvar1B));
}
void SetPeakCloseLoopOutReverse() {
  tempvar1B = query("New Reverse Peak Closed Loop Output");
  ForEveryUsingTalon(ConfigPeakOutputVoltage(tempvar1A, tempvar1B));
}
void SetNominalCloseOutForward() {
  tempvar2A = query("New Forward Nominal Closed Loop Output");
  ForEveryUsingTalon(ConfigNominalOutputVoltage(tempvar2A, tempvar2B));
}
void SetNominalCloseOutReverse() {
  tempvar2B = query("New Reverse Nominal Closed Loop Output");
  ForEveryUsingTalon(ConfigNominalOutputVoltage(tempvar2A, tempvar2B));
}
void SetVoltageRampRate() {
  double vrate = query("New Voltage Ramp Rate Value");
  ForEveryUsingTalon(SetVoltageRampRate(vrate));
}
void SetSetCloseLoopRampRate() {
  double crate = query("New CloseLoop Ramp Rate Value");
  ForEveryUsingTalon(SetCloseLoopRampRate(crate));
}
void SetAllowCloseLoopErr() {
  int allCloseLE = query("Set New Allowable Closed Loop Error");
  ForEveryUsingTalon(SetAllowableClosedLoopErr(allCloseLE));
}
void Limit1() {
  ForEveryUsingTalon(
      ConfigLimitMode(CANTalon::LimitMode::kLimitMode_SoftPositionLimits));
}
void Limit2() {
  ForEveryUsingTalon(
      ConfigLimitMode(CANTalon::LimitMode::kLimitMode_SwitchInputsOnly));
}
void Limit3() {
  ForEveryUsingTalon(
      ConfigLimitMode(CANTalon::LimitMode::kLimitMode_SrxDisableSwitchInputs));
}
void OpenFHLimit() {
  ForEveryUsingTalon(ConfigFwdLimitSwitchNormallyOpen(true));
}
void CloseFHLimit() {
  ForEveryUsingTalon(ConfigFwdLimitSwitchNormallyOpen(false));
}
void OpenRHLimit() {
  ForEveryUsingTalon(ConfigRevLimitSwitchNormallyOpen(true));
}
void CloseRHLimit() {
  ForEveryUsingTalon(ConfigRevLimitSwitchNormallyOpen(false));
}
void ConfigSoftLimitForward() {
  tempvar3A = query("Forward Soft Limit");
  ForEveryUsingTalon(ConfigSoftPositionLimits(tempvar3A, tempvar3B));
}
void ConfigSoftLimitReverse() {
  tempvar3B = query("Reverse Soft Limit");
  ForEveryUsingTalon(ConfigSoftPositionLimits(tempvar3A, tempvar3B));
}
void SetDT_QuadEncoder() {
  ForEveryUsingTalon(SetFeedbackDevice(CANTalon::FeedbackDevice::QuadEncoder));
}
void SetDT_AnalogPot() {
  ForEveryUsingTalon(SetFeedbackDevice(CANTalon::FeedbackDevice::AnalogPot));
}
void SetDT_AnalogEncoder() {
  ForEveryUsingTalon(
      SetFeedbackDevice(CANTalon::FeedbackDevice::AnalogEncoder));
}
void SetDT_CtreMagEncoder_Relative() {
  ForEveryUsingTalon(
      SetFeedbackDevice(CANTalon::FeedbackDevice::CtreMagEncoder_Relative));
}
void SetDT_CtreMagEncoder_Absolute() {
  ForEveryUsingTalon(
      SetFeedbackDevice(CANTalon::FeedbackDevice::CtreMagEncoder_Absolute));
}
void SetDT_PulseWidth() {
  ForEveryUsingTalon(SetFeedbackDevice(CANTalon::FeedbackDevice::PulseWidth));
}
void SetQPosition() {
  double set = query("New Encoder Value");
  ForEveryUsingTalon(SetPosition(set));
}
void SetDirectionEF() { ForEveryUsingTalon(SetSensorDirection(false)); }
void SetDirectionER() { ForEveryUsingTalon(SetSensorDirection(true)); }
void SetDirectionOF() { ForEveryUsingTalon(SetInverted(false)); }
void SetDirectionOR() { ForEveryUsingTalon(SetInverted(true)); }
void SetBrakeCoastModeB() {
  ForEveryUsingTalon(
      ConfigNeutralMode(CANTalon::NeutralMode::kNeutralMode_Brake));
}
void SetBrakeCoastModeC() {
  ForEveryUsingTalon(
      ConfigNeutralMode(CANTalon::NeutralMode::kNeutralMode_Coast));
}
void SetControlModePV() {
  ForEveryUsingTalon(SetTalonControlMode(CANTalon::kThrottleMode));
}
void SetControlModeV() {
  ForEveryUsingTalon(SetTalonControlMode(CANTalon::kVoltageMode));
}
void SetControlModeP() {
  ForEveryUsingTalon(SetTalonControlMode(CANTalon::kPositionMode));
}
void SetControlModeSp() {
  ForEveryUsingTalon(SetTalonControlMode(CANTalon::kSpeedMode));
}
void SetControlModeCu() {
  ForEveryUsingTalon(SetTalonControlMode(CANTalon::kCurrentMode));
}

void SetControlModeMagic() {
  ForEveryUsingTalon(SetTalonControlMode(CANTalon::kMotionMagicMode));
}

void SetControlModeSl() {
  int ID = static_cast<int>(query("Master Talon ID"));
  ForEveryUsingTalon(SetTalonControlMode(CANTalon::kFollowerMode));
  ForEveryUsingTalon(Set(ID));
}
void SetStatusFrameRates_G() {
  int set = static_cast<int>(query("New Frame Rate (Milliseconds)"));
  ForEveryUsingTalon(
      SetStatusFrameRateMs(CANTalon::StatusFrameRate::StatusFrameRateGeneral,
                           set);)
}
void SetStatusFrameRates_F() {
  int set = static_cast<int>(query("New Frame Rate (Milliseconds)"));
  ForEveryUsingTalon(
      SetStatusFrameRateMs(CANTalon::StatusFrameRate::StatusFrameRateFeedback,
                           set);)
}
void SetStatusFrameRates_Q() {
  double set = query("New Frame Rate (Milliseconds)");
  ForEveryUsingTalon(
      SetStatusFrameRateMs(
          CANTalon::StatusFrameRate::StatusFrameRateQuadEncoder, set);)
      ForEveryUsingTalon(
          SetStatusFrameRateMs(
              CANTalon::StatusFrameRate::StatusFrameRatePulseWidthMeas, set);)
}
void SetStatusFrameRates_A() {
  int set = static_cast<int>(query("New Frame Rate (Milliseconds)"));
  ForEveryUsingTalon(
      SetStatusFrameRateMs(
          CANTalon::StatusFrameRate::StatusFrameRateAnalogTempVbat, set);)
}
void GetTalonRegistors() {
#define logrow(_string_, _func_)                          \
  printf("%s", _string_);                                 \
  ForEveryUsingTalonMUTI(printf("%f\t", Talon->_func_);); \
  printf("\n");
  // printf("TalonID:   ");

  ForEveryUsingTalonMUTI(printf("Talon ID: %8i\t", TalonID););
  printf("\n");
  logrow("P:         ", GetP()) logrow("I:         ", GetI())
      logrow("D:         ", GetD()) logrow("F:         ", GetF())
          logrow("I Zone:    ", GetIzone()) logrow("Commanded: ", GetSetpoint())
              logrow("Fwd Soft:  ", GetForwardLimitOK())
                  logrow("Rev Soft:  ", GetReverseLimitOK())
                      logrow("Fwd Hard:  ", IsFwdLimitSwitchClosed())
                          logrow("Rev Hard:  ", IsRevLimitSwitchClosed())
                              logrow("SensorPos: ", GetPosition())
                                  logrow("CTR Pos:   ", GetPulseWidthPosition())
}

typedef void (*VoidFunction)(void);

void menu(int length, char* options[], char* title, VoidFunction* funcs,
          bool loop);

void inline Padding(int length, char c) {
  for (int i = 0; i < length; i++) printf("%c", c);
}

void ChangeTalons() {
  while (true) {
    int ID = static_cast<int>(query("Talon ID [0]"));
    CANTalon* t = PossiblyCreateTalon(&GraphableDevices, &AllTalons, ID);
    uint32_t verison = t->GetFirmwareVersion();
    if (verison == 0) {
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
void SingularTalon() {
  TalonsCurrentlyUsing.clear();
  ChangeTalons();
}
void SetEncoderDeviceType() {
  char* FunctionNames[] = {"QuadEncoder",
                           "AnalogPot",
                           "AnalogEncoder",
                           "CtreMagEncoder_Relative",
                           "CtreMagEncoder_Absolute",
                           "PulseWidth"};
  VoidFunction FunctionPointers[] = {SetDT_QuadEncoder,
                                     SetDT_AnalogPot,
                                     SetDT_AnalogEncoder,
                                     SetDT_CtreMagEncoder_Relative,
                                     SetDT_CtreMagEncoder_Absolute,
                                     SetDT_PulseWidth};
  menu(6, FunctionNames, "Which Encoder Type?", FunctionPointers, false);
}
void SetDirectionE() {
  char* FunctionNames[] = {"Forward", "Reverse"};
  VoidFunction FunctionPointers[] = {SetDirectionEF, SetDirectionER};
  menu(2, FunctionNames, "Set Encoder Direction", FunctionPointers, false);
}
void SetDirectionO() {
  char* FunctionNames[] = {"Forward", "Reverse"};
  VoidFunction FunctionPointers[] = {SetDirectionOF, SetDirectionOR};
  menu(2, FunctionNames, "Set Output Direction", FunctionPointers, false);
}
void SetBrakeCoastMode() {
  char* FunctionNames[] = {"Brake", "Coast"};
  VoidFunction FunctionPointers[] = {SetBrakeCoastModeB, SetBrakeCoastModeC};
  menu(2, FunctionNames, "Set Brake/Coast Mode", FunctionPointers, false);
}
void EDLimitSwitches() {
  char* FunctionNames[] = {"Enable Hard and Soft Limits",
                           "Enable Hard limits only",
                           "Disable all Limit Switches"};
  VoidFunction FunctionPointers[] = {Limit1, Limit2, Limit3};
  menu(3, FunctionNames, "Enable/Disable Hard Limit Switches", FunctionPointers,
       false);
}
void ConfigForwardHLimit() {
  char* FunctionNames[] = {"Normally Open", "Normally Close"};
  VoidFunction FunctionPointers[] = {OpenFHLimit, CloseFHLimit};
  menu(2, FunctionNames, "Open/Close Forward Hard Limit Switch",
       FunctionPointers, false);
}
void ConfigReverseHLimit() {
  char* FunctionNames[] = {"Normally Open", "Normally Close"};
  VoidFunction FunctionPointers[] = {OpenRHLimit, CloseRHLimit};
  menu(2, FunctionNames, "Open/Close Reverse Hard Limit Switch",
       FunctionPointers, false);
}
void SetEncoderDirection() {
  char* FunctionNames[] = {"Forward", "Reverse"};
  VoidFunction FunctionPointers[] = {SetDirectionEF, SetDirectionER};
  menu(2, FunctionNames, "Set Encoder Direction", FunctionPointers, false);
}
void SetOutputDirection() {
  char* FunctionNames[] = {"Forward", "Reverse"};
  VoidFunction FunctionPointers[] = {SetDirectionOF, SetDirectionOR};
  menu(2, FunctionNames, "Set Output Direction", FunctionPointers, false);
}
void GetDIO_Pin() {
  int ID = 0;
  while (!(0 <= (ID = static_cast<int>(query("DigitalInput pin"))) && ID <= 9))
    ;
  printf("ID == %i\n", ID);
  DigitalInput* t =
      PossiblyCreateDigitalInput(&GraphableDevices, &AllDigitalBoths, ID);
  printf("Value == %i\n", t->Get());
}
void SetDIO_Pin() {
  int ID = 0;
  while (!(0 <= (ID = static_cast<int>(query("DigitalOutput pin"))) && ID <= 9))
    ;
  printf("ID == %i\n", ID);
  int value = static_cast<int>(query("Value?"));
  DigitalOutput* t =
      PossiblyCreateDigitalOutput(&GraphableDevices, &AllDigitalBoths, ID);
  t->Set(value);
  printf("Value == %i\n", value);
}
void SetTalonControlMode() {
  char* FunctionNames[] = {"Percent Voltage (-1 to 1)",
                           "Position (Encoder Counts)",
                           "Speed (Encoder Counts per Tenth of Second)",
                           "Voltage (Volts)",
                           "Slave",
                           "Current",
                           "Motion Magic"};
  VoidFunction FunctionPointers[] = {
      SetControlModePV, SetControlModeP,  SetControlModeSp,   SetControlModeV,
      SetControlModeSl, SetControlModeCu, SetControlModeMagic};
  menu(7, FunctionNames, "Select New Control Mode", FunctionPointers, false);
}
void SetStatusFrameRates() {
  char* FunctionNames[] = {
      "General (Pin States, Control Mode, Throttle)",
      "Feedback (Current, Voltage)",
      "Quadrature Encoder (Encoder Position, Encoder Velocity)",
      "Analog Input / Temperature / Battery Voltage"};
  VoidFunction FunctionPointers[] = {
      SetStatusFrameRates_G, SetStatusFrameRates_F, SetStatusFrameRates_Q,
      SetStatusFrameRates_A};
  menu(4, FunctionNames, "Select Status Frame", FunctionPointers, false);
}

void SetCurrentLimit() {
  uint32_t current_limit =
      static_cast<uint32_t>(query("New Current Limit (Amps)"));
  if (current_limit == 0) {
    // disable current limit for setpoint = 0
    ForEveryUsingTalon(EnableCurrentLimit(false));
    return;
  }
  ForEveryUsingTalon(SetCurrentLimit(current_limit));
  ForEveryUsingTalon(EnableCurrentLimit(true));
}

void SetMotionMagicCruiseVelocity() {
  double cruise_velocity = query("New Motion Magic Cruise Velocity (RPM)");
  ForEveryUsingTalon(SetMotionMagicCruiseVelocity(cruise_velocity));
}

void SetMotionMagicAcceleration() {
  double accel = query("New Motion Magic Acceleration (RPM/sec)");
  ForEveryUsingTalon(SetMotionMagicAcceleration(accel));
}

void Set() {
  char* FunctionNames[] = {
      "Control Mode",
      "P",
      "I",
      "D",
      "F",
      "I Zone",
      "Forward Peak Closed Loop Output",
      "Reverse Peak Closed Loop Output",
      "Forward Nominal Closed Loop Output",
      "Reverse Nominal Closed Loop Output",
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
      "Status Frame Rates",
      "Current Limit",
      "Pulse Length",
      "Motion Magic Cruise Velocity",
      "Motion Magic Acceleration",
  };
  VoidFunction FunctionPointers[] = {
      SetTalonControlMode,
      SetP,
      SetI,
      SetD,
      SetF,
      SetIzone,
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
      SetStatusFrameRates,
      SetCurrentLimit,
      SetPulseLength,
      SetMotionMagicCruiseVelocity,
      SetMotionMagicAcceleration,
  };
  menu(28, FunctionNames, "Set What?", FunctionPointers, true);
}
void Get() {
  char* FunctionNames[] = {
      "All Talon Registers", "DIO Pin",
  };
  VoidFunction FunctionPointers[] = {
      GetTalonRegistors, GetDIO_Pin,
  };
  menu(2, FunctionNames, "Get What?", FunctionPointers, true);
}
class Robot : public SampleRobot {
 public:
  Robot() {
    JerrysGrapher_StartStatusThread(&GraphableDevices);
    int nullfd = open("/dev/null", O_WRONLY | O_CREAT);
    dup2(nullfd, 2);
  }
  void OperatorControl() {
    SetForgroundColor(TColor::Default);
    ClearScreen();
    printf("\n");
    printf("Welcome to BDC_Comm v6.5\n");
    SingularTalon();
    char* FunctionNames[] = {"Select One Single Talon",
                             "Select Additional Talon",
                             "Set Commanded Value",
                             "Pulse Commanded Value",
                             "Set",
                             "Get"};
    VoidFunction FunctionPointers[] = {
        SingularTalon,       ChangeTalons, SetCommandedValue,
        PulseCommandedValue, Set,          Get};
    while (true) {
      menu(6, FunctionNames, "Select Action", FunctionPointers, true);
      printf("You cannot leave root menu.\n");
    }
  }
  void Autonomous() {
    printf("Autonomous()\n");
    ClearScreen();
    char* art[] = {
        "BBBB      DDDDD        CCCCC           CCCCC                          "
        "      ",
        "B   BB    D    DD    CCC             CCC                              "
        "      ",
        "B    B    D     D    CC              CC                               "
        "      ",
        "BBBBB     D     DD   C               C           OOOO     MMMMM     "
        "MMMMMM  ",
        "B    B    D     D    CC              CC         O    O   M  M  M   M  "
        " M  M ",
        "B   BB    D    DD    CCC             CCC        O    O   M  M  M   M  "
        " M  M ",
        "BBBB      DDDDD        CCCCC           CCCCC     OOOO    M  M  M   M  "
        " M  M "};
    while (DriverStation::GetInstance().IsAutonomous()) {
      for (int i = 0; i < 10; i++) {
        int row = static_cast<int>(Random() * 7);
        int col = static_cast<int>(Random() * strlen(art[row]));
        int color = static_cast<int>(Random() * 10);
        MoveCursorTo(col + 4, row + 4);
        SetForgroundColor((TColor)color);
        printf("%c", art[row][col]);
      }
      printf("\n");
      msleep(150);
    }
  }
  void Disabled() { printf("Please Enable.\n"); }
};

void menu(int length, char* options[], char* title, VoidFunction* funcs,
          bool loop) {
top:
  do {
#define SpacePadding 8
    int BannerLength =
        strlen(title) + SpacePadding * 2 + 7 + TalonsCurrentlyUsing.size();
    Padding(BannerLength, '-');
    printf("----------\n");
    Padding(SpacePadding, ' ');
    printf("%s (Talon ", title);
    for (int i = 0; i < TalonsCurrentlyUsing.size(); i++) {
      printf("%i", TalonsCurrentlyUsing[i].TalonID);
      if (i < TalonsCurrentlyUsing.size() - 1) {
        printf(",");
      }
    }
    printf(")\n");
    Padding(BannerLength, '-');
    printf("----------\n");
    for (int i = 0; i < length; i++) {
      Padding(SpacePadding / 2, ' ');
      printf("%3i - %s\n", i + 1, options[i]);
    }
    int OptionIndex;
    bool isVaild = false;
    while (!isVaild) {
      printf("> ");
      char selection[9];
      fgets(selection, 9, stdin);
      selection[strlen(selection) - 1] = '\0';
      if (strcmp(selection, "b") == 0) return;
      if (strcmp(selection, "c") == 0) {
        ClearScreen();
        goto top;
      }
      if (strcmp(selection, "e") == 0 || strcmp(selection, "q") == 0) exit(0);
      OptionIndex = atoi(selection);
      isVaild = 1 <= OptionIndex && OptionIndex <= length;
      if (!isVaild) {
        printf("Please type in a Valid Number.\n");
      }
    }
    funcs[OptionIndex - 1]();
  } while (loop);
}
START_ROBOT_CLASS(Robot);
