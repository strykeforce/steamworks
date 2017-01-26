#include <pthread.h>
#include <stdio.h>
#include "CANTalon.h"
#include "CodeUtils.h"
#include "DigitalInput.h"
#include "ObjectToGrapher.h"
#include "SocketHandler.h"

int JerrysGrapher_IndexOfBundleWithID(
    std::vector<JerrysGrapher_DeviceBundle>* JerrysGrapher_GraphableDevices,
    JerrysGrapher_DeviceType target_device, int id) {
  for (int i = 0; i < JerrysGrapher_GraphableDevices->size(); i++) {
    if (JerrysGrapher_GraphableDevices->operator[](i).type == target_device)
      if (JerrysGrapher_GraphableDevices->operator[](i).ID == id) return i;
  }
  return -1;
}

struct Subscription {
  JerrysGrapher_DeviceType DeviceType;
  byte DeviceID;
  byte PropertyID;
  void* Bundle;
};
pthread_t JerrysGrapherRIO_Thread_Handle;
int Past_Sum;
int Current_Sum;
Socket JerrysGrapher_RIO_out;
SocketAddress GrapherAddress;
void SendDeviceList(
    std::vector<JerrysGrapher_DeviceBundle>* JerrysGrapher_GraphableDevices) {
  JerrysGrapher_DeviceList_Response packet;
  packet.NumberOfDevices = Current_Sum;
  int i = 0;
  for (; i < JerrysGrapher_GraphableDevices->size(); i++) {
    packet.DeviceTypes[i] = JerrysGrapher_GraphableDevices->operator[](i).type;
    packet.DeviceIDs[i] = JerrysGrapher_GraphableDevices->operator[](i).ID;
  }
  SocketHandler::UDPSend(JerrysGrapher_RIO_out, &packet, sizeof(packet),
                         &GrapherAddress);
  Past_Sum = Current_Sum;
}
void* JerrysGrapherRIO_Thread(void* JerrysGrapher_GraphableDevicesRAW) {
  std::vector<JerrysGrapher_DeviceBundle>* JerrysGrapher_GraphableDevices =
      (std::vector<JerrysGrapher_DeviceBundle>*)
          JerrysGrapher_GraphableDevicesRAW;
  JerrysGrapher_RIO_out =
      SocketHandler::CreateUDPSocket(NetworkP_JerrysGrapherDataPort);
  SocketHandler::SetTimeout(JerrysGrapher_RIO_out, 1);
  struct timeval now, start;
  gettimeofday(&start, NULL);
  byte* PacketSpace = (byte*)malloc(JERRYSGRAPHER_MAX_PACKET_SIZE);
  int GraphableTalons_OldLength = -1;
  Subscription Subscriptions[JERRYSGRAPHER_MAX_NUMBER_OF_DATAPOINTS];
  for (int i = 0; i < JERRYSGRAPHER_MAX_NUMBER_OF_DATAPOINTS; i++) {
    Subscriptions[i].DeviceID = 255;
    Subscriptions[i].PropertyID = 255;
  }
  Past_Sum = -1;
  // while(1)
  float MillisecondsToWait = JERRYSGRAPHER_DEFAULT_SPEED_MILLISECONDS;
  {
    JerrysGrapher_DeviceBundle rio;
    rio.type = JerrysGrapher_DeviceType::RoboRIO;
    rio.ID = 0;
    rio.objectPointer = NULL;
    JerrysGrapher_GraphableDevices->push_back(rio);
  }
  for (int packetcounter = 0;; packetcounter++) {
    Current_Sum = JerrysGrapher_GraphableDevices->size();
    if (Past_Sum != Current_Sum) {
      SendDeviceList(JerrysGrapher_GraphableDevices);
    }
    int Recv_Status = SocketHandler::RecvFromWho(
        JerrysGrapher_RIO_out, PacketSpace, JERRYSGRAPHER_MAX_PACKET_SIZE,
        &GrapherAddress);
    if (Recv_Status > 0) {
      JerrysGrapher_MessageID* Header = (JerrysGrapher_MessageID*)PacketSpace;
      switch (*Header) {
        case JerrysGrapher_MessageID::DeviceList_Request: {
          SendDeviceList(JerrysGrapher_GraphableDevices);
          break;
        }
        case JerrysGrapher_MessageID::UpdateSubscription: {
          JerrysGrapher_UpdateSubscription* packet =
              (JerrysGrapher_UpdateSubscription*)PacketSpace;
          Subscriptions[packet->SubscriptionIndex].DeviceType =
              packet->DeviceType;
          Subscriptions[packet->SubscriptionIndex].DeviceID = packet->DeviceID;
          Subscriptions[packet->SubscriptionIndex].PropertyID =
              packet->PropertyID;
          int bundleindex = JerrysGrapher_IndexOfBundleWithID(
              JerrysGrapher_GraphableDevices, packet->DeviceType,
              packet->DeviceID);
          // printf("bundleindex == %i\n", bundleindex);
          Subscriptions[packet->SubscriptionIndex].Bundle =
              JerrysGrapher_GraphableDevices->operator[](bundleindex)
                  .objectPointer;
          // printf("Subscriptions[packet->SubscriptionIndex].Bundle == %p\n",
          // Subscriptions[packet->SubscriptionIndex].Bundle);
          break;
        }
        case JerrysGrapher_MessageID::ChangeUpdateRate: {
          JerrysGrapher_ChangeUpdateRate* packet =
              (JerrysGrapher_ChangeUpdateRate*)PacketSpace;
          memcpy(&MillisecondsToWait, packet->MillisecondsToWait,
                 sizeof(float));
          break;
        }
      }
    }
    JerrysGrapher_SubscriptionData packet;
    for (int i = 0; i < JERRYSGRAPHER_MAX_NUMBER_OF_DATAPOINTS; i++) {
      void* raw_device_ref = Subscriptions[i].Bundle;
      double val;
      if (Subscriptions[i].PropertyID == 255 ||
          (raw_device_ref == NULL &&
           Subscriptions[i].DeviceType != JerrysGrapher_DeviceType::RoboRIO)) {
        val = nan("nan");
      } else {
        switch (Subscriptions[i].DeviceType) {
          case JerrysGrapher_DeviceType::RoboRIO: {
            switch (Subscriptions[i].PropertyID) {
              case JerrysGrapher_RoboRIO_PropertyID::PacketCounter: {
                val = packetcounter;
                break;
              }
              case JerrysGrapher_RoboRIO_PropertyID::MillisecondsPerPacket: {
                val = MillisecondsToWait;
                break;
              }
              case JerrysGrapher_RoboRIO_PropertyID::CPULoad: {
                static unsigned long long TotalUser_Normal2, TotalUser_Niced2,
                    TotalSys_Proc2, TotalIdle2;
                FILE* f = fopen("/proc/stat", "r");
                unsigned long long TotalUser_Normal, TotalUser_Niced,
                    TotalSys_Proc, TotalIdle;
                fscanf(f, "cpu %llu %llu %llu %llu", &TotalUser_Normal,
                       &TotalUser_Niced, &TotalUser_Niced, &TotalIdle);
                fclose(f);
                double dom = (TotalUser_Normal - TotalUser_Normal2) +
                             (TotalUser_Niced - TotalUser_Niced2) +
                             (TotalSys_Proc - TotalSys_Proc2);
                double num = dom;
                dom += (TotalIdle - TotalIdle2);
                val = num / dom;
                TotalUser_Normal2 = TotalUser_Normal;
                TotalUser_Niced2 = TotalUser_Niced;
                TotalSys_Proc2 = TotalSys_Proc;
                TotalIdle2 = TotalIdle;
                break;
              }
              default: {
                val = cos(packetcounter / 64.0) * 10;
                break;
              }
            }
            break;
          }
          case JerrysGrapher_DeviceType::Talon: {
#define FAKE_VALUES 0
#if FAKE_VALUES
            val =
                sin(packetcounter / (50.0 + Subscriptions[i].PropertyID)) * 10;
#else
            CANTalon* talon = (CANTalon*)raw_device_ref;
            switch (Subscriptions[i].PropertyID) {
              case JerrysGrapher_Talon_PropertyID::SetValue: {
                val = talon->GetSetpoint();
                break;
              }
              case JerrysGrapher_Talon_PropertyID::OutputVoltage: {
                val = talon->GetOutputVoltage();
                break;
              }
              case JerrysGrapher_Talon_PropertyID::OutputCurrent: {
                val = talon->GetOutputCurrent();
                break;
              }
              case JerrysGrapher_Talon_PropertyID::EncoderPosition: {
                val = talon->GetEncPosition();
                break;
              }
              case JerrysGrapher_Talon_PropertyID::EncoderVelocity: {
                val = talon->GetEncVel();
                break;
              }
              case JerrysGrapher_Talon_PropertyID::CTR_AbsPosition: {
                val = talon->GetPulseWidthPosition() &
                      0xFFF;  // 0-4095, we don't care about wrap-around
                break;
              }
              case JerrysGrapher_Talon_PropertyID::ControlLoop_Error: {
                val = talon->GetClosedLoopError();
                break;
              }
              case JerrysGrapher_Talon_PropertyID::IAccum: {
                val = talon->GetIaccum();
                break;
              }
              case JerrysGrapher_Talon_PropertyID::SupplyVoltage: {
                val = talon->GetBusVoltage();
                break;
              }
              case JerrysGrapher_Talon_PropertyID::ForwardHardLimit: {
                val = talon->IsFwdLimitSwitchClosed();
                break;
              }
              case JerrysGrapher_Talon_PropertyID::ReverseHardLimit: {
                val = talon->IsRevLimitSwitchClosed();
                break;
              }
              case JerrysGrapher_Talon_PropertyID::ForwardSoftLimit: {
                val = talon->GetForwardLimitOK();
                break;
              }
              case JerrysGrapher_Talon_PropertyID::ReverseSoftLimit: {
                val = talon->GetReverseLimitOK();
                break;
              }
              case JerrysGrapher_Talon_PropertyID::AnalogIn: {
                val = talon->GetAnalogIn();
                break;
              }
              default: {
                val = cos(packetcounter / 64.0) * 10;
                break;
              }
            }
#endif
            break;
          }
          case JerrysGrapher_DeviceType::DIO: {
#if FAKE_VALUES
            val = (packetcounter / 100) < 50;
#else
            DigitalInput* di = (DigitalInput*)raw_device_ref;
            val = di->Get();
#endif
            break;
          }
        }
      }
      memcpy(packet.Data[i], &val, sizeof(val));
    }
    gettimeofday(&now, NULL);
    float realtime =
        (now.tv_sec - start.tv_sec) +
        (float)(now.tv_usec - start.tv_usec) / (1.0 * 1000 * 1000.0);
    memcpy(&(packet.Seconds), &realtime, sizeof(float));
    SocketHandler::UDPSend(JerrysGrapher_RIO_out, &packet, sizeof(packet),
                           &GrapherAddress);
    msleep(MillisecondsToWait);
  }
}
void JerrysGrapher_StartStatusThread(
    std::vector<JerrysGrapher_DeviceBundle>* JerrysGrapher_GraphableDevices) {
  pthread_create(&JerrysGrapherRIO_Thread_Handle, NULL, JerrysGrapherRIO_Thread,
                 JerrysGrapher_GraphableDevices);
}
