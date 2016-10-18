#ifndef DEADEYE_CONSTANTS_INC_
#define DEADEYE_CONSTANTS_INC_

const auto kNetworkBasePort = 5800;
const auto kNetworkHeatDataStreamPort = kNetworkBasePort + 1;
const auto kNetworkNetworkJoystickPort = kNetworkBasePort + 2;
const auto kNetworkBDCCommStatusPort = kNetworkBasePort + 3;
const auto kNetworkVisionCommPort = kNetworkBasePort + 4;
const auto kNetworkPropertySetterPort = kNetworkBasePort + 5;
const auto kNetworkDashboardPort = kNetworkBasePort + 6;

const auto kNetworkRobotAddress = "10.27.67.2";
const auto kNetworkNVIDIAAddress = "10.27.67.12";

const auto kFrameWidth = 640;
const auto kFrameHeight = 480;

#endif /* end of include guard: DEADEYE_CONSTANTS_INC_ */
