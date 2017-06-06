#ifndef OBJECT_TO_GRAPHER_H
#define OBJECT_TO_GRAPHER_H
#include "HostToObject.h"

#define JERRYSGRAPHER_MAX_NUMBER_OF_DEVICES 64
#define JERRYSGRAPHER_MAX_PACKET_SIZE 150
#define JERRYSGRAPHER_MAX_DATA_SIZE 8
#define JERRYSGRAPHER_MAX_NUMBER_OF_DATAPOINTS 16
#define JERRYSGRAPHER_DEFAULT_SPEED_MILLISECONDS 50

enum JerrysGrapher_Talon_PropertyID : byte {
  SetValue = 0,       // GetSetpoint()
  OutputVoltage,      // GetOutputVoltage()
  OutputCurrent,      // GetOutputCurrent()
  EncoderPosition,    // GetEncPosition()
  EncoderVelocity,    // GetEncVel()
  CTR_AbsPosition,    // GetPulseWidthPosition()
  ControlLoop_Error,  // GetClosedLoopError()
  IAccum,             // GetIaccum()
  SupplyVoltage,      // GetBusVoltage()
  ForwardHardLimit,   // IsFwdLimitSwitchClosed()
  ReverseHardLimit,   // IsRevLimitSwitchClosed()
  ForwardSoftLimit,   // GetForwardLimitOK()
  ReverseSoftLimit,   // GetReverseLimitOK()
  AnalogIn,           // GetAnalogIn()
};
enum JerrysGrapher_RoboRIO_PropertyID : byte {
  PacketCounter = 0,
  MillisecondsPerPacket,
  CPULoad,
};
enum JerrysGrapher_MessageID : byte {
  DeviceList_Request = 0,
  DeviceList_Response,
  UpdateSubscription,
  SubscriptionData,
  ChangeUpdateRate,
};
struct JerrysGrapher_DeviceList_Request  // Sent FROM Grapher (Begins
                                         // Commication)
{
  JerrysGrapher_MessageID id = JerrysGrapher_MessageID::DeviceList_Request;
};
struct JerrysGrapher_DeviceList_Response  // Sent TO Grapher
{
  JerrysGrapher_MessageID id = JerrysGrapher_MessageID::DeviceList_Response;
  byte NumberOfDevices;
  JerrysGrapher_DeviceType DeviceTypes[JERRYSGRAPHER_MAX_NUMBER_OF_DEVICES];
  byte DeviceIDs[JERRYSGRAPHER_MAX_NUMBER_OF_DEVICES];
};
struct JerrysGrapher_UpdateSubscription  // Sent FROM Grapher
{
  JerrysGrapher_MessageID id = JerrysGrapher_MessageID::UpdateSubscription;
  byte SubscriptionIndex;
  JerrysGrapher_DeviceType DeviceType;
  byte DeviceID;
  byte PropertyID;
};
struct JerrysGrapher_SubscriptionData  // Sent FROM Grapher
{
  JerrysGrapher_MessageID id = JerrysGrapher_MessageID::SubscriptionData;
  byte Seconds[sizeof(float)];
  byte Data[JERRYSGRAPHER_MAX_NUMBER_OF_DATAPOINTS]
           [JERRYSGRAPHER_MAX_DATA_SIZE];
};
struct JerrysGrapher_ChangeUpdateRate  // Sent FROM Grapher
{
  JerrysGrapher_MessageID id = JerrysGrapher_MessageID::SubscriptionData;
  byte MillisecondsToWait[sizeof(float)];
};
#endif
