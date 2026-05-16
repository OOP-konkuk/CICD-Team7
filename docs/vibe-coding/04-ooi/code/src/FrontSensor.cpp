#include "FrontSensor.h"

bool FrontSensor::detect() {
    // Hardware abstraction: reads actual front sensor state
    return detected;
}
