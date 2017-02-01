#pragma once

namespace deadeye {
enum BoilerTargetParameters {
  kRange,
  kAngle,
  kSpeed,
};

const int boiler_target_offset = 76;
const int boiler_target_data_size = 4;

const int boiler_target_data[][3] = {
    {0, 1, 3},
    {2, 3, 4},
    {4, 5, 6},
    {6, 7, 8},
};
} /* deadeye */
