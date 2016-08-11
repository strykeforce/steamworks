double tempvar1A = 0, tempvar1B = 0;
double tempvar2A = 0, tempvar2B = 0;
double tempvar3A = 0, tempvar3B = 0;
void SetP() { double P = query("New P Value"); ForEveryUsingTalon(SetP(P)); }
void SetI() { double I = query("New I Value"); ForEveryUsingTalon(SetI(I)); }
void SetD() { double D = query("New D Value"); ForEveryUsingTalon(SetD(D)); }
void SetF() { double F = query("New F Value"); ForEveryUsingTalon(SetF(F)); }
void SetIzone() { unsigned iz = query("New I Zone Value"); ForEveryUsingTalon(SetIzone(iz)); }
void SetCommandedValue_Q(double val) {ForEveryUsingTalon(Set(val));} void SetCommandedValue() { query2("New Commanded Value", SetCommandedValue_Q); }
void SetPeakCloseLoopOutForward() { tempvar1A = query("New Forward Peak Closed Loop Output");    ForEveryUsingTalon(ConfigPeakOutputVoltage(tempvar1A, tempvar1B)); }
void SetPeakCloseLoopOutReverse() { tempvar1B = query("New Reverse Peak Closed Loop Output");    ForEveryUsingTalon(ConfigPeakOutputVoltage(tempvar1A, tempvar1B)); }
void SetNominalCloseOutForward()  { tempvar2A = query("New Forward Nominal Closed Loop Output"); ForEveryUsingTalon(ConfigNominalOutputVoltage(tempvar2A, tempvar2B)); }
void SetNominalCloseOutReverse()  { tempvar2B = query("New Reverse Nominal Closed Loop Output"); ForEveryUsingTalon(ConfigNominalOutputVoltage(tempvar2A, tempvar2B)); }
void SetVoltageRampRate()      { double vrate = query("New Voltage Ramp Rate Value");   ForEveryUsingTalon(SetVoltageRampRate(vrate)); }
void SetSetCloseLoopRampRate() { double crate = query("New CloseLoop Ramp Rate Value"); ForEveryUsingTalon(SetCloseLoopRampRate(crate)); }
void SetAllowCloseLoopErr() { int allCloseLE = query((char*)"Set New Allowable Closed Loop Error"); ForEveryUsingTalon(SetAllowableClosedLoopErr(allCloseLE)); }
void Limit1(){ ForEveryUsingTalon(ConfigLimitMode(CANTalon::LimitMode::kLimitMode_SoftPositionLimits)); }
void Limit2(){ ForEveryUsingTalon(ConfigLimitMode(CANTalon::LimitMode::kLimitMode_SwitchInputsOnly)); }
void Limit3(){ ForEveryUsingTalon(ConfigLimitMode(CANTalon::LimitMode::kLimitMode_SrxDisableSwitchInputs)); }
void OpenFHLimit(){ ForEveryUsingTalon(ConfigFwdLimitSwitchNormallyOpen(true)); }
void CloseFHLimit(){ ForEveryUsingTalon(ConfigFwdLimitSwitchNormallyOpen(false)); }
void OpenRHLimit(){ ForEveryUsingTalon(ConfigRevLimitSwitchNormallyOpen(true)); }
void CloseRHLimit(){ ForEveryUsingTalon(ConfigRevLimitSwitchNormallyOpen(false)); }
void ConfigSoftLimitForward() { tempvar3A = query((char*)"Forward Soft Limit"); ForEveryUsingTalon(ConfigSoftPositionLimits(tempvar3A, tempvar3B)); }
void ConfigSoftLimitReverse() { tempvar3B = query((char*)"Reverse Soft Limit"); ForEveryUsingTalon(ConfigSoftPositionLimits(tempvar3A, tempvar3B)); }
void SetDT_QuadEncoder() { ForEveryUsingTalon(SetFeedbackDevice(CANTalon::FeedbackDevice::QuadEncoder)); }
void SetDT_AnalogPot() { ForEveryUsingTalon(SetFeedbackDevice(CANTalon::FeedbackDevice::AnalogPot)); }
void SetDT_AnalogEncoder() { ForEveryUsingTalon(SetFeedbackDevice(CANTalon::FeedbackDevice::AnalogEncoder)); }
void SetDT_CtreMagEncoder_Relative() { ForEveryUsingTalon(SetFeedbackDevice(CANTalon::FeedbackDevice::CtreMagEncoder_Relative)); }
void SetDT_CtreMagEncoder_Absolute() { ForEveryUsingTalon(SetFeedbackDevice(CANTalon::FeedbackDevice::CtreMagEncoder_Absolute)); }
void SetDT_PulseWidth() { ForEveryUsingTalon(SetFeedbackDevice(CANTalon::FeedbackDevice::PulseWidth)); }
void SetQPosition() { double set = query((char*)"New Encoder Value"); ForEveryUsingTalon(SetPosition(set)); }
void SetDirectionEF() { ForEveryUsingTalon(SetSensorDirection(false)); }
void SetDirectionER() { ForEveryUsingTalon(SetSensorDirection(true)); }
void SetDirectionOF() { ForEveryUsingTalon(SetInverted(false)); }
void SetDirectionOR() { ForEveryUsingTalon(SetInverted(true)); }
void SetBrakeCoastModeB() { ForEveryUsingTalon(ConfigNeutralMode(CANTalon::NeutralMode::kNeutralMode_Brake)); }
void SetBrakeCoastModeC() { ForEveryUsingTalon(ConfigNeutralMode(CANTalon::NeutralMode::kNeutralMode_Coast)); }
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
void SetStatusFrameRates_G() { double set = query((char*)"New Frame Rate (Milliseconds)"); ForEveryUsingTalon(SetStatusFrameRateMs(CANTalon::StatusFrameRate::StatusFrameRateGeneral, (int) set);) }
void SetStatusFrameRates_F() { double set = query((char*)"New Frame Rate (Milliseconds)"); ForEveryUsingTalon(SetStatusFrameRateMs(CANTalon::StatusFrameRate::StatusFrameRateFeedback, (int) set);) }
void SetStatusFrameRates_Q() { double set = query((char*)"New Frame Rate (Milliseconds)"); ForEveryUsingTalon(SetStatusFrameRateMs(CANTalon::StatusFrameRate::StatusFrameRateQuadEncoder, (int) set);) }
void SetStatusFrameRates_A() { double set = query((char*)"New Frame Rate (Milliseconds)"); ForEveryUsingTalon(SetStatusFrameRateMs(CANTalon::StatusFrameRate::StatusFrameRateAnalogTempVbat, (int) set);) }
void GetTalonRegistors()
{
	#define logrow(_string_, _func_)\
		printf(_string_); ForEveryUsingTalonMUTI(printf("%f\t", Talon->_func_);); printf("\n");
	printf("TalonID:   "); ForEveryUsingTalonMUTI(printf("%8i\t", TalonID);); printf("\n");
	logrow("P:         ", GetP())
	logrow("I:         ", GetI())
	logrow("D:         ", GetD())
	logrow("F:         ", GetF())
	logrow("I Zone:    ", GetIzone())
	logrow("Commanded: ", GetSetpoint())
	logrow("Fwd Soft:  ", GetForwardLimitOK())
	logrow("Rev Soft:  ", GetReverseLimitOK())
	logrow("Fwd Hard:  ", IsFwdLimitSwitchClosed())
	logrow("Rev Hard:  ", IsRevLimitSwitchClosed())
	logrow("SensorPos: ", GetPosition())
	logrow("CTR Pos:   ", GetPulseWidthPosition())
}





































