#pragma once

#include <memory>
#include "FlyCapture2.h"
#include "cpptoml/cpptoml.h"

std::shared_ptr<FlyCapture2::Camera> GetCamera(
    std::shared_ptr<cpptoml::table> config);
