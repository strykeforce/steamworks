#include "camera.h"

#include <iostream>
#include "FlyCapture2.h"
#include "cpptoml/cpptoml.h"

using namespace FlyCapture2;

std::shared_ptr<Camera> GetCamera(std::shared_ptr<cpptoml::table> toml) {
  auto config = toml->get_table("CAMERA");
  if (!config) {
    throw std::invalid_argument("CAMERA config section missing");
  }
  Error error;
  BusManager busMgr;
  unsigned int numCameras;
  error = busMgr.GetNumOfCameras(&numCameras);
  if (error != PGRERROR_OK) {
    error.PrintErrorTrace();
  }

  if (numCameras < 1) {
    std::cout << "Camera not detected..." << std::endl;
  }

  PGRGuid guid;
  error = busMgr.GetCameraFromIndex(0, &guid);
  if (error != PGRERROR_OK) {
    error.PrintErrorTrace();
  }

  // Connect to a camera
  std::shared_ptr<Camera> camera = std::make_shared<Camera>();
  error = camera->Connect(&guid);
  if (error != PGRERROR_OK) {
    error.PrintErrorTrace();
  }

  auto width = config->get_as<int>("width");
  if (!width) {
    throw std::invalid_argument("CAMERA width setting missing");
  }
  auto height = config->get_as<int>("height");
  if (!height) {
    throw std::invalid_argument("CAMERA height setting missing");
  }

  Format7ImageSettings fmt7ImageSettings;
  fmt7ImageSettings.mode = MODE_0;
  fmt7ImageSettings.offsetX = 0;
  fmt7ImageSettings.offsetY = 0;
  fmt7ImageSettings.width = *width;
  fmt7ImageSettings.height = *height;
  fmt7ImageSettings.pixelFormat = PIXEL_FORMAT_RAW8;

  bool valid;
  Format7PacketInfo fmt7PacketInfo;

  // Validate the settings to make sure that they are valid
  error = camera->ValidateFormat7Settings(&fmt7ImageSettings, &valid,
                                          &fmt7PacketInfo);
  if (error != PGRERROR_OK) {
    error.PrintErrorTrace();
  }

  if (!valid) {
    // Settings are not valid
    std::cout << "Format7 settings are not valid" << std::endl;
  }

  // Set the settings to the camera
  error = camera->SetFormat7Configuration(
      &fmt7ImageSettings, fmt7PacketInfo.recommendedBytesPerPacket);
  if (error != PGRERROR_OK) {
    error.PrintErrorTrace();
  }

  auto exposure = config->get_as<double>("exposure");
  if (!exposure) {
    throw std::invalid_argument("CAMERA exposure setting missing");
  }

  Property prop;
  prop.type = AUTO_EXPOSURE;
  prop.onOff = true;            // ensure property ohn
  prop.autoManualMode = false;  // auto-adjust off
  prop.absControl = true;
  prop.absValue = *exposure;
  error = camera->SetProperty(&prop);

  return camera;
}
